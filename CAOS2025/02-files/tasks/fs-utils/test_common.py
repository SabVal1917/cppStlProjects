import tempfile
import pathlib
import sys
import subprocess


def assert_exists(*args):
    for path in args:
        if not path.exists():
            print("Required file does not exists")
            exit(1)


def assert_not_exists(*args):
    for path in args:
        if path.exists():
            print("File exists, but should not")
            exit(1)


def assert_mode(expected, *args):
    for path in args:
        actual = path.stat().st_mode & 0x1ff
        if actual != expected:
            print("File mode ({:o}) doesn't match expected ({:o})".format(expected, actual))
            exit(1)


def run_test(fn, fail=False):
    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = pathlib.Path(tmpdir)
        extra_args, checker = fn(tmpdir)
        args = [sys.argv[1]] + extra_args
        proc = subprocess.run(args)
        if checker is not None:
            checker()
        if proc.returncode != 0 and not fail:
            print(f"Solution exited with non-zero exit code {proc.returncode}", file=sys.stderr)
            exit(1)
        if proc.returncode == 0 and fail:
            print(f"Solution exited with zero exit code, but required to fail", file=sys.stderr)
            exit(1)
