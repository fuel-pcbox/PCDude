#!/usr/bin/env python
from os import *
import sys
import platform

varss = Variables()
varss.Add('debug', 'Set to 1 to compile with debug symbols', 1)
varss.Add('mda', 'Set to 1 to compile the MDA graphics card', 1)
varss.Add('cga', 'Set to 1 to compile the CGA graphics card', 0)
varss.Add('ega', 'Set to 1 to compile the EGA graphics card', 0)
varss.Add('hercules', 'Set to 1 to compile the Hercules graphics card', 0)
varss.Add('ncurses', 'Set to 1 to show debug information in an ncurses terminal', 1)

VariantDir('build', 'src', duplicate=0)
env = Environment()

p = platform.system()
isLinux = (p=='Linux')
isWinXX = (p=='Windows')

if not isWinXX:
	env.ParseConfig('pkg-config --cflags sfml-all')
	env.ParseConfig('pkg-config --libs sfml-all')
else:
	env.Append(LIBS=['sfml-graphics','sfml-audio','sfml-network','sfml-window','sfml-system'])

SRCS = Split("""
build/apidefs.cpp
build/cputick.cpp
build/video.cpp
build/pc.cpp
build/../jzon/Jzon.cpp
""") 

CMDOPTS = ['-Wall','-Wextra','-Wno-unused-parameter']
CMDDEFS = []

usemda = ARGUMENTS.get('mda', 1)
usecga = ARGUMENTS.get('cga', 1)
useega = ARGUMENTS.get('ega', 0)
usehercules = ARGUMENTS.get('hercules', 1)
dbg = ARGUMENTS.get('debug', 1)
usenc = ARGUMENTS.get('ncurses', (0 if isWinXX else 1) )

print(dbg)

if usemda==1:
	SRCS += ['build/mda.cpp']
	CMDDEFS += ['USE_MDA']
if usecga==1:
	SRCS += ['build/cga.cpp']
	CMDDEFS += ['USE_CGA']
if useega==1:
	SRCS += ['build/ega.cpp']
	CMDDEFS += ['USE_EGA']
if usehercules==1:
	SRCS += ['build/hercules.cpp']
	CMDDEFS += ['USE_HERCULES']
if usenc==1:
	CMDDEFS += ['USE_NCURSES']
	env.Append(LIBS=['ncurses'])
if dbg==1:
	CMDOPTS += ['-g']
else:
	CMDOPTS += ['-O2']

env.Program(
target = 'pc',
source = SRCS,
CCFLAGS=['-std=c++0x','-Ijzon']+CMDOPTS,
CPPDEFINES=CMDDEFS
)

