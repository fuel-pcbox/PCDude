from os import *
VariantDir('build', 'src', duplicate=0)
env = Environment()

env.ParseConfig('pkg-config --cflags sfml-all')
env.ParseConfig('pkg-config --libs sfml-all')

SRCS = Split("""
build/apidefs.cpp
build/cputick.cpp
build/video.cpp

build/pc.cpp
""")  # build/mda.cpp

CMDOPTS = []

env.Program(
target = 'pc',
source = SRCS,
CCFLAGS=['-std=c++0x']+CMDOPTS
)

