#!/usr/bin/env python

import pdb

def fibonacci(max):
    """docstring for fibonacci"""
    s = 1
    t = 1
    while s < max:
        yield s
        w = s + t
        s = t
        t = w

for n in fibonacci(10000):
  print n
