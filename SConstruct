# Usage:
# scons => compiles with video subsystem
# scons --no-video => compiles without video subsystem
from os import *
VariantDir('build', 'src', duplicate=0)
env = Environment()

env.ParseConfig('sdl-config --cflags')
env.ParseConfig('sdl-config --libs')

SRCS = Split("""
build/apidefs.cpp
build/cputick.cpp
build/video.cpp
build/pc.cpp
""")

CMDOPTS = [,]

if not GetOption('no-video'):
	CMDOPTS += '-DVIDEO'

env.Program(
target = 'pc',
source = SRCS,
CCFLAGS=['-std=c++0x']+CMDOPTS
)

