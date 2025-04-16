#!/bin/env python3

tests_count = 10

import random

print(tests_count)

def print_test(max_seq_len, max_abs_num):
  seq_len = random.randint(0, max_seq_len)
  print(seq_len)
  for j in range(seq_len):
    num = random.randint(-max_abs_num, max_abs_num)
    print(num, end='')
    if (j+1 != seq_len):
      print(end=' ')
    else:
      print()

for i in range(tests_count):
  print_test(20, 100)
