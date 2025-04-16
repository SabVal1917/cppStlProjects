import sys
import os
import pathlib
import tempfile
import subprocess
import random
import string


def randomword(length):
   letters = string.ascii_lowercase
   return ''.join(random.choice(letters) for i in range(length))


def run_test_bash(n):
    with tempfile.TemporaryDirectory() as tmpdir:
        out_files = {str(pathlib.Path(tmpdir) / randomword(10)): random.randint(1, 10) for i in range(n)}
        commands = ["echo -n " + "a" * count + " 1>" + filename for filename, count in out_files.items()]
        command_string = ";".join(commands)
        env = os.environ.copy()
        proc = subprocess.Popen([sys.argv[1], "bash", "-c", command_string], stdout=subprocess.PIPE)
        proc_stdout, err = proc.communicate()
        actual = proc_stdout.decode()
        actual_counters = {s.split(':')[0]: int(s.split(':')[1]) for s in actual.split()}
        assert out_files == actual_counters

run_test_bash(1)
run_test_bash(10)
