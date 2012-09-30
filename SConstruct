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

env.Program(
target = 'pc',
source = SRCS,
CCFLAGS=['-std=c++0x','-DVIDEO']
)

