
#ifndef VIDEO_H_INC
#define VIDEO_H_INC

#ifdef API_H_INC
#error Video.h MUST be included before api.h
#endif

#include "includes.h"
#ifdef VIDEO
#include <SDL/SDL.h>
#endif
#include <tr1/memory>

using std::tr1::shared_ptr;

#ifdef VIDEO
typedef SDL_Surface* Surface;
typedef SDL_Event Event;
#else
typedef void* Surface;
typedef u8 Event;
#endif

class vEvent
{
public:
	vEvent() {}
	virtual ~vEvent() {}
	virtual std::string type() const
	{
		return "NONE";
	}
};

#define MAKE_VEVENT( NAME )  public:  \
	vev ## NAME (){} \
	virtual ~vev ## NAME (){} \
	virtual std::string type() const {return #NAME ;}


class vevQuit : public vEvent
{
	MAKE_VEVENT(Quit)
};

class VideoException : public std::exception
{
public:
	explicit VideoException(const std::string& m):mm(m) {} ~VideoException() throw() {} std::string mm;
	const char* what()
	{
		return mm.c_str();
	}
};

class Video_t
{
public:
	Video_t() : screen(nullptr) {}
	virtual ~Video_t()
	{
		Quit();
	}

	inline void SDLAssert(int a,std::string message)
	{
		if(a!=0)
		{
			throw new VideoException(message);
		}
	}

	Surface screen;
	Event e;
	shared_ptr<vEvent> ev;
	void Init();
	void CreateScreen(int w, int h);
	bool PollEvent();
	void RefreshScreen();
	void Quit();


};

extern Video_t Video;

#endif

