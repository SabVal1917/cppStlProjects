#!/usr/bin/env python3
from random import randint
import re
import sys
from math import sin

count = int(sys.argv[1])

def generate_test(number):
    def give_float_number():
        a = randint(-100000000000, 100000000000)
        a /= 1000000.0
        return a
        
    name = str(number).zfill(3)
    
    dat = open(name + ".dat", "w")
    ans = open(name + ".ans", "w")

    n = randint(1, 10000)

    _a = [give_float_number() for i in range(n)]
    _b = [give_float_number() for i in range(n)]
    res = 0
    for i in range(n):
        res+=_a[i]*_b[i]
    
    print(n, file=dat)
    print(" ".join(["{0:.8f}".format(i) for i in _a]), file=dat)
    print(" ".join(["{0:.8f}".format(i) for i in _b]), file=dat)
    
    print("{0:.8f}".format(res), file=ans)

    dat.close()
    ans.close()
    
for i in range(1, count + 1):
    generate_test(i)
