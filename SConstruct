#!/usr/bin/env python
from os import *
VariantDir('build', 'src', duplicate=0)
env = Environment()

env.ParseConfig('pkg-config --cflags sfml-all')
env.ParseConfig('pkg-config --libs sfml-all')

SRCS = Split("""
build/apidefs.cpp
build/cputick.cpp
build/video.cpp
build/mda.cpp
build/pc.cpp
""") 

CMDOPTS = []

env.Program(
target = 'pc',
source = SRCS,
CCFLAGS=['-std=c++0x']+CMDOPTS
)

