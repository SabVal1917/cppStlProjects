import os
from test_common import run_test, assert_exists, assert_mode


def simple(tmpdir):
    dest = tmpdir / "a"
    def checker():
        assert_exists(dest)
    return [dest], checker


def with_parents(tmpdir):
    dest = tmpdir / "a" / "b" / "c" / "d" / "e"
    def checker():
        assert_exists(dest)
    return ["-p", dest], checker


def with_no_parents(tmpdir):
    dest = tmpdir / "a" / "b" / "c" / "d" / "e"
    return [dest], None


def with_mode(tmpdir):
    dest = tmpdir / "a" / "b" / "c" / "d" / "e"
    def checker():
        assert_exists(dest)
        assert_mode(0o040, dest)
    return ["-p", "--mode=040", dest], checker


def with_umasked_mode(tmpdir):
    dest = tmpdir / "a" / "b" / "c" / "d" / "e"
    def checker():
        assert_exists(dest)
        assert_mode(0o777, dest)
    return ["-p", "--mode=755", dest], checker


run_test(simple)
run_test(with_parents)
run_test(with_no_parents, fail=True)
run_test(with_mode)
