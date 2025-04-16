import sys
import os
import pathlib
import tempfile
import subprocess


def run_test_raw(s, expected, fail=False):
    with tempfile.TemporaryDirectory() as tmpdir:
        inp = pathlib.Path(tmpdir) / "in.txt"
        out = pathlib.Path(tmpdir) / "out.txt"
        with open(inp, "wb") as f:
            f.write(s)
        env = os.environ.copy()
        # env["LD_PRELOAD"] = "./libc_restricter.so"
        proc = subprocess.run([sys.argv[1], inp, out], env=env)
        if proc.returncode != 0 and not fail:
            print(f"Solution exited with non-zero exit code {proc.returncode}", file=sys.stderr)
            exit(1)
        if proc.returncode == 0 and fail:
            print(f"Solution exited with zero exit code, but required to fail", file=sys.stderr)
            exit(1)
        if not fail:
            with open(out, "rb") as f:
                actual = f.read()
                assert actual == expected, f"{repr(actual)} != {repr(expected)}"


def run_test(s):
    s = expected = s.encode("utf-8")
    run_test_raw(s, expected)


run_test("я люблю АКОС")
run_test("ура " * 1000)
run_test_raw(bytes([0b11000000, 0b10110010]), bytes([0x32]))
run_test_raw(bytes([0b11100000, 0b10000000, 0b10110010]), bytes([0x32]))
run_test_raw(bytes([0b11110000, 0b10000000, 0b10000000, 0b10110010]), bytes([0x32]))
run_test_raw(bytes([0b11111000, 0b10000000, 0b10000000, 0b10000000, 0b10110010]), bytes([0x32]))
run_test_raw(bytes([0b11111100, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10110010]), bytes([0x32]))
run_test_raw(bytes([0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10110010]), bytes([0x32]), fail=True)
