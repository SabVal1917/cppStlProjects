import tempfile
import subprocess
import random
import os
import sys


def assert_reads_chunk(f, chunk):
    actual = b""
    while True:
        data = f.read(4096)
        if len(data) == 0:
            break
        actual += data
        if len(actual) == len(chunk):
            break
    assert chunk == actual


def run_test():
    fd, name = tempfile.mkstemp()
    os.write(fd, b"test")
    with subprocess.Popen([sys.argv[1], name], stdout=subprocess.PIPE, shell=False, bufsize=0) as proc:
        assert_reads_chunk(proc.stdout, b"test")
        for _ in range(200):
            chunk = b"a" * random.randint(1, 200)
            os.write(fd, chunk)
            assert_reads_chunk(proc.stdout, chunk)
        proc.terminate()
    if proc.returncode != -15:
        print(f"Solution exited with non-zero exit code {proc.returncode}", file=sys.stderr)
        exit(1)


run_test()
