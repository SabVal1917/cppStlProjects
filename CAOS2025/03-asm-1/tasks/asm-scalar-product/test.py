#!/bin/env python3

import subprocess
import sys

tests_count = 20
eps = 1e-4

test_passed = 0
test_failed = 0

dat_files = ["tests_data/{:03d}.dat".format(x) for x in range(1, tests_count+1)]
ans_files = ["tests_data/{:03d}.ans".format(x) for x in range(1, tests_count+1)]

for test_num in range(tests_count):
  file_data = open(dat_files[test_num], "r")
  file_ans = open(ans_files[test_num], "r")

  test_result = subprocess.run("./main", shell=True, check=True,
                               stdin=file_data, stdout=subprocess.PIPE)

  expected_ans = float(file_ans.read())
  actual_ans = float(test_result.stdout.decode('utf-8'))

  inaccuracy = abs((expected_ans - actual_ans) / expected_ans)
  if (inaccuracy < eps):
    test_passed += 1
  else:
    test_failed += 1

  file_data.close()
  file_ans.close()

print("Passed {} tests".format(test_passed))
print("Failed {} tests".format(test_failed))

if (test_failed > 0):
  sys.exit(1)
