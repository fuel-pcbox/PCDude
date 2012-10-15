
#ifndef VIDEOCARDS_H_INC
#define VIDEOCARDS_H_INC

namespace mda
{
#ifdef USE_MDA
void Register();
void Unregister();
#else
void Register(){}
void Unregister(){}
#endif
}

namespace hercules
{
#ifdef USE_HERCULES
void Register();
void Unregister();
#else
void Register(){}
void Unregister(){}
#endif
}

#endif

