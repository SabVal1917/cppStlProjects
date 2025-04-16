import os
from test_common import run_test, assert_not_exists, assert_exists


def simple(tmpdir):
    os.makedirs(tmpdir / "a" / "b" / "c" / "d" / "e", exist_ok=True)
    (tmpdir / "a" / "f").touch()
    (tmpdir / "a" / "b" / "f").touch()
    (tmpdir / "a" / "b" / "c" / "f").touch()
    (tmpdir / "a" / "b" / "c" / "d" / "f").touch()
    (tmpdir / "a" / "b" / "c" / "d" / "e" / "f").touch()

    def checker():
        assert_not_exists(tmpdir)

    return ["-r", tmpdir], checker


def multi_args(tmpdir):
    fls = [tmpdir / "a", tmpdir / "b", tmpdir / "c", tmpdir / "d", tmpdir / "e"]
    for f in fls:
        f.touch()

    def checker():
        assert_not_exists(*fls)
        assert_exists(tmpdir)

    return fls, checker


def delete_directory_only_with_r(tmpdir):
    return [tmpdir], None


run_test(simple)
run_test(multi_args)
run_test(delete_directory_only_with_r, fail=True)
