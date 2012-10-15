
#ifndef VIDEOCARDS_H_INC
#define VIDEOCARDS_H_INC

#include "includes.h"

namespace mda
{
void Register();
void Unregister();
void render(sf::RenderWindow& rw, int xoff, int yoff);
};

namespace HGC
{
void Register();
void Unregister();
extern void render(sf::RenderWindow& rw, int xoff, int yoff);
};

namespace CGA
{
void Register();
void Unregister();
extern void render(sf::RenderWindow& rw, int xoff, int yoff);
};

#endif

