#!/usr/bin/env python
from os import *

varss = Variables()
varss.Add('mda', 'Set to 1 to compile the MDA graphics card', 1)
varss.Add('cga', 'Set to 1 to compile the CGA graphics card', 0)
varss.Add('ega', 'Set to 1 to compile the EGA graphics card', 0)
varss.Add('hercules', 'Set to 1 to compile the Hercules graphics card', 0)

VariantDir('build', 'src', duplicate=0)
env = Environment(variables=varss)

env.ParseConfig('pkg-config --cflags sfml-all')
env.ParseConfig('pkg-config --libs sfml-all')

SRCS = Split("""
build/apidefs.cpp
build/cputick.cpp
build/video.cpp
build/pc.cpp
build/../jzon/Jzon.cpp
""") 

CMDOPTS = []
CMDDEFS = []

usemda = ARGUMENTS.get('mda', 1)
usecga = ARGUMENTS.get('cga', 0)
useega = ARGUMENTS.get('ega', 0)
usehercules = ARGUMENTS.get('hercules', 0)

if usemda>0:
	SRCS += ['build/mda.cpp']
	CMDDEFS += ['USE_MDA']
if usecga>0:
	SRCS += ['build/cga.cpp']
	CMDDEFS += ['USE_CGA']
if useega>0:
	SRCS += ['build/ega.cpp']
	CMDDEFS += ['USE_EGA']
if usehercules>0:
	SRCS += ['build/hercules.cpp']
	CMDDEFS += ['USE_HERCULES']

env.Program(
target = 'pc',
source = SRCS,
CCFLAGS=['-std=c++0x','-Ijzon']+CMDOPTS,
CPPDEFINES=CMDDEFS
)

