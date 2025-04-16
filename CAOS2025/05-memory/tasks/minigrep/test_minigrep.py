import tempfile
import pathlib
import sys
import re
import os
import subprocess


def run_test(fn, fail=False):
    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = pathlib.Path(tmpdir)
        extra_args, checker = fn(tmpdir)
        args = [sys.argv[1]] + extra_args
        with subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="utf-8") as proc:
            stdout, stderr = proc.communicate()
        if checker is not None:
            checker(proc.returncode, stderr, stdout)


def gen_answer(tmpdir, regexp):
    regexp = re.compile(regexp)
    ans = []
    for dirpath, _, fnames in os.walk(tmpdir):
        for fname in fnames:
            fname = os.path.join(dirpath, fname)
            with open(fname) as f:
                for lineno, l in enumerate(f):
                    l = l.rstrip('\n')
                    if next(regexp.finditer(l), None) is None:
                        continue
                    ans.append(f"{fname}:{lineno + 1}: {l}")
    return ans


def check_ans(tmpdir, stdout):
    lines = stdout.split('\n')
    if len(lines) > 0 and lines[-1] == '':
        lines = lines[:-1]
    lines.sort()
    ans_lines = gen_answer(tmpdir, 'const')
    ans_lines.sort()
    assert lines == ans_lines, f"\nexpected:\n{ans_lines}\n\nactual:\n{lines}"


def check_match_with_empty_lines(tmpdir):
    for i in range(10):
        fname = tmpdir / ("file_" + str(i))
        line = "\n" * 20 + " " * i + "const" + "\n" * 20
        with open(fname, "w") as f:
            f.write(line)

    def checker(returncode, stderr, stdout):
        assert returncode == 0, f"{stderr} {stdout}"
        check_ans(tmpdir, stdout)

    return ['const', tmpdir], checker


def check_match_not_all(tmpdir):
    for i in range(10):
        fname = tmpdir / ("file_" + str(i))
        line = " " * 4 + "const" * (i % 2) + "\n"
        with open(fname, "w") as f:
            for j in range(10):
                f.write(line)

    def checker(returncode, stderr, stdout):
        assert returncode == 0, f"{stderr} {stdout}"
        check_ans(tmpdir, stdout)

    return ['const', tmpdir], checker


def check_invalid_pattern(tmpdir):
    def checker(returncode, stderr, stdout):
        assert returncode != 0, f"{stderr} {stdout}"
        assert 'missing )' in stderr

    return ['([a-z]+', tmpdir], checker


run_test(check_match_with_empty_lines)
run_test(check_match_not_all)
run_test(check_invalid_pattern)
