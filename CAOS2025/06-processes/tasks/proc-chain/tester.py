import subprocess
from os import remove

BINARY_NAME = "chainy"
TEST_FILE_NAME = "content"
SINGLE_COMMANDS = ["whoami", "pwd", "ls", "ls -l", "ls -la"]
PAIR_COMMANDS = ["ls | wc", "ls -la | wc", "ls -la | wc -c", "pwd | cut -d/ -f2"]
LONG_COMMANDS = ["ls -la | wc | wc | wc | wc | wc | wc -c", f"cat {BINARY_NAME} | grep -q a | wc -l | wc -c"]


def test_commands(commands):
    for command in commands:
        result = subprocess.check_output(f'./{BINARY_NAME} "{command}"', shell=True)
        expected_result = subprocess.check_output(command, shell=True)

        if result != expected_result:
            print(f"Output missmatch for the following command: {command}")
            exit(1)


def test_buffer_overflow():
    with open(TEST_FILE_NAME, "wt") as file:
        pretty_long_string = "Aa\nAA\naaAa\nAaaA\nAAa\na" * (2 << 18)
        file.write(pretty_long_string)

    test_commands([f"cat {TEST_FILE_NAME} | wc -c", f"cat {TEST_FILE_NAME} | grep -q a | grep -q A | wc -c | wc -c"])

    remove(TEST_FILE_NAME)
    

def main():
    test_commands(SINGLE_COMMANDS)
    test_commands(PAIR_COMMANDS)
    test_commands(LONG_COMMANDS)

    test_buffer_overflow()


if __name__ == "__main__":
    main()
