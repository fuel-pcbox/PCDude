
#ifndef INCLUDES_H_INC
#define INCLUDES_H_INC

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <tr1/memory>
#include <tr1/functional>
#include <tr1/tuple>
#include <Jzon.h>

// #include <unistd.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFGUI/SFGUI.hpp>

#ifdef USE_NCURSES
#include <ncurses.h>
#else
#define wprintw(x,...) printf(__VA_ARGS__)
#define mvwprintw(x,y,z,...) printf(__VA_ARGS__)
#define printw(...) printf(__VA_ARGS__)
#endif

using std::shared_ptr;

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t s8;
typedef int16_t s16;

#endif

