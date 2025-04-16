import os
import sys
import pathlib
import subprocess
import tempfile


def expect_answer(exe, lhs_path, rhs_path, expected):
    proc = subprocess.run([exe, lhs_path, rhs_path], stdout=subprocess.PIPE)

    if proc.returncode != 0:
        print(f"Solution exited with non-zero exit code {proc.returncode}", file=sys.stderr)
        exit(1)

    try:
        actual = proc.stdout.decode().strip()
    except UnicodeDecodeError:
        print(f"Couldn't decode solution output", file=sys.stderr)
        exit(1)

    assert actual == expected, f"{repr(actual)} != {repr(expected)}"


def expect_failure(exe, *args):
    proc = subprocess.run([exe, *args], stdout=subprocess.PIPE)

    if proc.returncode == 0:
        print(f"Solution exited with zero exit code, but required to fail", file=sys.stderr)
        exit(1)


def touch_test_files():
    pathlib.Path("file").touch()
    pathlib.Path("file2").touch()
    pathlib.Path("hardlink-file").hardlink_to("file")
    pathlib.Path("symlink-file").symlink_to("file")
    pathlib.Path("symlink-symlink-file").symlink_to("file")
    pathlib.Path("symlink-hardlink-file").symlink_to("hardlink-file")

    pathlib.Path("dir").mkdir()
    pathlib.Path("dir2").mkdir()
    pathlib.Path("symlink-dir").symlink_to("dir")
    pathlib.Path("symlink-symlink-dir").symlink_to("symlink-dir")

    pathlib.Path("dangling-symlink").symlink_to("/path/to/nowhere")

    pathlib.Path("begin-cycle").symlink_to("end-cycle")
    pathlib.Path("end-cycle").symlink_to("begin-cycle")


if __name__ == "__main__":
    exe = pathlib.Path(sys.argv[1]).resolve()

    with tempfile.TemporaryDirectory() as tmpdir:
        os.chdir(tmpdir)
        touch_test_files()

        expect_answer(exe, "file", "file", "yes")
        expect_answer(exe, "file", pathlib.Path("file").resolve(), "yes")
        expect_answer(exe, pathlib.Path("file").resolve(), pathlib.Path("file").resolve(), "yes")
        expect_answer(exe, "dir/./../dir/../file", "file", "yes")

        expect_answer(exe, "file", "hardlink-file", "yes")
        expect_answer(exe, "file", "symlink-file", "yes")
        expect_answer(exe, "file", "symlink-symlink-file", "yes")
        expect_answer(exe, "file", "symlink-hardlink-file", "yes")
        expect_answer(exe, "symlink-file", "symlink-file", "yes")
        expect_answer(exe, "symlink-file", "symlink-symlink-file", "yes")
        expect_answer(exe, "symlink-file", "symlink-hardlink-file", "yes")

        expect_answer(exe, ".", ".", "yes")
        expect_answer(exe, "dir", "symlink-dir", "yes")
        expect_answer(exe, "dir", "symlink-symlink-dir", "yes")
        expect_answer(exe, "symlink-dir", "symlink-symlink-dir", "yes")

        expect_answer(exe, "file", "file2", "no")
        expect_answer(exe, "dir", "dir2", "no")
        expect_answer(exe, ".", "..", "no")
        expect_answer(exe, "file", "dir", "no")

        expect_answer(exe, "file", "/path/to/nowhere", "no")
        expect_answer(exe, "file", "dangling-symlink", "no")
        expect_answer(exe, "dir", "/path/to/nowhere", "no")
        expect_answer(exe, "dir", "dangling-symlink", "no")
        expect_answer(exe, "/path/to/nowhere", "/path/to/nowhere", "no")
        expect_answer(exe, "dangling-symlink", "/path/to/nowhere", "no")
        expect_answer(exe, "dangling-symlink", "dangling-symlink", "no")
        expect_answer(exe, "file", "begin-cycle", "no")
        expect_answer(exe, "dangling-symlink", "begin-cycle", "no")
        expect_answer(exe, "begin-cycle", "begin-cycle", "no")
        expect_answer(exe, "begin-cycle", "end-cycle", "no")

        expect_failure(exe, "x", "y", "z")
        expect_failure(exe, "x")
        expect_failure(exe)
