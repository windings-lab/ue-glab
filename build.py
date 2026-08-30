#!/usr/bin/env python3
"""Build the engine for GLab, in a chosen configuration.

This exists to compile the *engine* side of a configuration once, so Rider only
ever has to rebuild the two project modules. It wraps
Engine/Build/BatchFiles/Linux/Build.sh and renders UBT's [N/M] action counter as
a progress bar with an ETA -- a cold engine build runs for hours and UBT goes
quiet for long stretches, so otherwise a working build is hard to tell from a
hung one.

  ./build.py                 engine for Development
  ./build.py DebugGame       engine for DebugGame
  ./build.py --help          options
  ./build.py --explain       what the targets and configurations actually do
"""
from __future__ import annotations

import argparse
import os
import re
import shutil
import signal
import subprocess
import sys
import time
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent
UE_ROOT = Path(os.environ["UE_ROOT"])
BUILD_SH = UE_ROOT / "Engine/Build/BatchFiles/Linux/Build.sh"

CONFIGS = ["Debug", "DebugGame", "Development", "Test", "Shipping"]

# UBT prints e.g. "[213/964] Compile Module.SlateCore.3.cpp"
ACTION_RE = re.compile(r"^\[(\d+)/(\d+)\]\s*(.*)")

EXPLAIN = """
WHY A PROJECT TARGET AND NOT UnrealEditor

  Building the engine here means building GLabEditor, not the engine's own
  UnrealEditor target. Two reasons:

    1. Engine/Source/UnrealEditor.Target.cs sets bBuildAllModules = true,
       which compiles modules for all 895 engine plugins. That cannot be
       overridden from BuildConfiguration.xml -- XmlConfig is applied in the
       TargetRules base constructor (TargetRules.cs:3365), before the derived
       constructor assigns true.

    2. It would build the wrong set anyway. GameplayAbilities has
       EnabledByDefault: false, so UnrealEditor does not include it.

  GLabEditor builds exactly the engine and plugin set GLab.uproject asks for:
  the ~187 default-enabled plugins plus GameplayAbilities, GameFeatures and
  GLab_GameAbilitySystem. Because the target uses TargetBuildEnvironment.Shared
  it writes engine binaries to Engine/Binaries/Linux, which is the expensive
  work you want cached.

  It also compiles GLab and GLab_GameAbilitySystemRuntime. That is two modules
  and a few seconds; Rider overwrites them on your first iteration. Not worth
  engineering around.

CONFIGURATIONS
  The only real difference is which modules get compiler optimization. From
  UEBuildModuleCPP.ShouldEnableOptimization (UEBuildModuleCPP.cs:2628):

      Configuration != Debug && (Configuration != DebugGame || bIsEngineModule)

  Debug        Optimization OFF everywhere, engine included. Full debugging
               anywhere, but the editor is painfully slow and the entire
               engine compiles unoptimized. Only reach for this when the bug
               is inside engine code and DebugGame is not enough.

  DebugGame    Optimization ON for engine modules, OFF for your modules
               (GLab, GLab_GameAbilitySystemRuntime). Engine at full speed,
               your code fully debuggable: no inlined frames, no optimized-out
               locals, breakpoints land where you put them.
               ---> This is the one for debugging your GAS code. <---

  Development  Optimization ON everywhere. Full editor tooling, asserts and
               logging still on. Day-to-day default, and the fastest of the
               three to build.

  Test         Shipping-like but keeps stats/profiling. Game target only.
  Shipping     Fully optimized, logging/console stripped. Game target only.

  Each configuration is a separate build product with its own binaries, so
  switching between them does not invalidate the others -- but each one needs
  its own engine build. That is what this script is for.

  Whatever you pick here must match what you select in Rider, or Rider will
  rebuild the engine side itself.

NOTES
  Close Rider before a cold build. UBT picks parallelism ONCE at startup from
  free memory (1.5 GB per action). Rider holds ~8.6 GB, which caps this machine
  at 8 of 12 actions. Irrelevant for incremental builds, where it never binds.

  Debug info, core count and the artifact cache are set globally in
  ~/.config/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml and apply to
  every configuration here.
"""


def discover_targets() -> list[str]:
    """Targets from the project's Source/*.Target.cs. Editor first: that is
    the one worth building, since it carries the engine modules."""
    names = [p.name[: -len(".Target.cs")]
             for p in (PROJECT_DIR / "Source").glob("*.Target.cs")]
    return sorted(names, key=lambda n: (not n.endswith("Editor"), n))


def find_uproject() -> Path:
    found = list(PROJECT_DIR.glob("*.uproject"))
    if not found:
        sys.exit(f"no .uproject in {PROJECT_DIR}")
    return found[0]


def fmt_duration(seconds: float) -> str:
    seconds = int(seconds)
    if seconds >= 3600:
        return f"{seconds // 3600}h{(seconds % 3600) // 60:02d}m"
    if seconds >= 60:
        return f"{seconds // 60}m{seconds % 60:02d}s"
    return f"{seconds}s"


class Progress:
    """Renders UBT's action counter as a bar. Falls back to plain passthrough
    when stdout is not a terminal."""

    def __init__(self, enabled: bool):
        self.enabled = enabled and sys.stdout.isatty()
        self.start = time.monotonic()
        self.active = False

    def update(self, done: int, total: int, desc: str) -> None:
        if not self.enabled:
            print(f"[{done}/{total}] {desc}", flush=True)
            return

        width = min(shutil.get_terminal_size((80, 24)).columns, 100)
        elapsed = time.monotonic() - self.start
        rate = done / elapsed if elapsed > 0 else 0
        eta = fmt_duration((total - done) / rate) if rate > 0 else "--"

        bar_w = max(10, width - 46)
        filled = int(bar_w * done / total) if total else 0
        bar = "#" * filled + "." * (bar_w - filled)
        pct = (100 * done // total) if total else 0

        sys.stdout.write(
            "\r\033[K" + f"[{bar}] {done:>4}/{total} {pct:>3}%  ETA {eta}"[:width])
        sys.stdout.flush()
        self.active = True

    def note(self, text: str) -> None:
        """Print a line without clobbering the bar."""
        if self.active:
            sys.stdout.write("\r\033[K")
            self.active = False
        print(text, flush=True)

    def finish(self) -> None:
        if self.active:
            sys.stdout.write("\r\033[K")
            self.active = False


def session_members(sid: int) -> list[int]:
    """Every live pid in session `sid`.

    Signalling the process group is not enough: UBT calls setpgid, so it lands
    in a different group from the Build.sh we spawned. Killing that group
    leaves UBT running, reparented to init, still holding its global mutex --
    the next build then fails with ConflictingInstance. The session is the
    boundary that actually holds, because nothing in the build calls setsid.
    Field 6 of /proc/<pid>/stat is the session id; comm (field 2) can contain
    spaces and parentheses, so parse after the final ')'.
    """
    found = []
    for entry in os.listdir("/proc"):
        if not entry.isdigit():
            continue
        try:
            with open(f"/proc/{entry}/stat", "rb") as handle:
                data = handle.read()
        except OSError:
            continue
        tail = data.rpartition(b")")[2].split()
        if len(tail) >= 4 and int(tail[3]) == sid:
            found.append(int(entry))
    return found


def shutdown(proc: subprocess.Popen, progress: Progress) -> None:
    """Stop UBT and everything it spawned: clang, UbaAgent, the lot.

    Waiting on the direct child is not enough. UBT's own exit says nothing
    about its descendants, and anything it backgrounded inherits SIG_IGN for
    SIGINT, so a compile can outlive the build and keep hammering the CPU.

    SIGINT first, so UBT can release its global mutex and tear down UBA
    workers; a hard kill leaves the mutex held. Escalate only if it will not
    go, and keep going until the session is genuinely empty."""
    progress.finish()
    print("\nInterrupt: stopping UBT (Ctrl-C again to force)...", flush=True)

    # start_new_session=True made the child a session leader, so sid == pid.
    sid = proc.pid

    for sig, patience in ((signal.SIGINT, 15.0),
                          (signal.SIGTERM, 5.0),
                          (signal.SIGKILL, 5.0)):
        for pid in session_members(sid):
            try:
                os.kill(pid, sig)
            except (ProcessLookupError, PermissionError):
                pass

        deadline = time.monotonic() + patience
        while time.monotonic() < deadline:
            proc.poll()  # reap the direct child so it stops counting as alive
            if not session_members(sid):
                proc.wait()
                return
            time.sleep(0.2)

    proc.wait()


def run(cmd: list[str], quiet: bool) -> int:
    progress = Progress(enabled=not quiet)

    # Own process group: the terminal's Ctrl-C reaches only us, so forwarding
    # is explicit and UBT cannot be torn down behind our back mid-shutdown.
    proc = subprocess.Popen(
        cmd, cwd=str(UE_ROOT), stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT, text=True, bufsize=1, errors="replace",
        start_new_session=True,
    )

    interrupted = False
    try:
        assert proc.stdout is not None
        for line in proc.stdout:
            line = line.rstrip("\n")
            match = ACTION_RE.match(line)
            if match:
                progress.update(int(match.group(1)), int(match.group(2)),
                                match.group(3))
            elif line.strip():
                progress.note(line)
        return proc.wait()
    except KeyboardInterrupt:
        interrupted = True
        try:
            shutdown(proc, progress)
        except KeyboardInterrupt:
            for pid in session_members(proc.pid):
                try:
                    os.kill(pid, signal.SIGKILL)
                except (ProcessLookupError, PermissionError):
                    pass
            proc.wait()
        return 130
    finally:
        progress.finish()
        if interrupted:
            print("Build interrupted. Compiled objects are kept, so "
                  "restarting resumes where it stopped.", flush=True)


def main() -> int:
    targets = discover_targets()
    default_target = targets[0] if targets else "GLabEditor"

    parser = argparse.ArgumentParser(
        prog="build.py",
        description=f"Build the engine for GLab against {UE_ROOT}.",
        epilog="Use --explain for the reasoning behind targets and configs.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "config", nargs="?", default="Development", choices=CONFIGS,
        help="build configuration (default: Development)")
    parser.add_argument(
        "target", nargs="?", default=default_target, choices=targets or None,
        help=f"build target (default: {default_target})")
    parser.add_argument("-c", "--clean", action="store_true",
                        help="clean build products instead of building")
    parser.add_argument("-r", "--rebuild", action="store_true",
                        help="clean, then build")
    parser.add_argument("-n", "--dry-run", action="store_true",
                        help="print the UBT command without running it")
    parser.add_argument("-q", "--quiet", action="store_true",
                        help="no progress bar; pass UBT output through unchanged")
    parser.add_argument("--explain", action="store_true",
                        help="explain the targets and configurations, then exit")

    # Anything after a literal "--" is for UBT verbatim. Pull it out first:
    # argparse would otherwise try to read "-verbose" as the config positional.
    argv = sys.argv[1:]
    if "--" in argv:
        split = argv.index("--")
        argv, passthrough = argv[:split], argv[split + 1:]
    else:
        passthrough = []

    # Unrecognised flags also go to UBT (e.g. -verbose, -DisablePlugin=X).
    # argparse.REMAINDER would be the obvious choice for this but it swallows
    # our own short flags once a positional has been seen: "build.py DebugGame
    # -n" would forward -n to UBT instead of setting --dry-run.
    args, unknown = parser.parse_known_args(argv)
    extra = unknown + passthrough

    if args.explain:
        print(EXPLAIN.strip())
        return 0

    if not BUILD_SH.exists():
        sys.exit(f"engine build script not found: {BUILD_SH}")

    if args.target.endswith("Editor") and args.config in ("Test", "Shipping"):
        sys.exit(f"{args.config} is not valid for an editor target; "
                 f"use a game target instead.")

    uproject = find_uproject()

    phases: list[tuple[str, list[str]]] = []
    if args.clean or args.rebuild:
        phases.append(("clean", ["-Mode=Clean"]))
    if not args.clean:
        phases.append(("build", []))

    overall = time.monotonic()
    for name, mode_args in phases:
        cmd = [str(BUILD_SH), args.target, "Linux", args.config,
               f"-project={uproject}", *mode_args, *extra]

        print(f"==> {name}  {args.target} | Linux | {args.config}")
        if args.dry_run:
            print("    " + " ".join(cmd))
            continue

        rc = run(cmd, quiet=args.quiet or name == "clean")
        if rc != 0:
            print(f"\n{name} FAILED (exit {rc})", file=sys.stderr)
            return rc

    if not args.dry_run:
        print(f"\nDone in {fmt_duration(time.monotonic() - overall)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
