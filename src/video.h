
#ifndef VIDEO_H_INC
#define VIDEO_H_INC

#ifdef API_H_INC
#error Video.h MUST be included before api.h
#endif

#include "includes.h"

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
	Video_t() {}
	virtual ~Video_t()
	{
	}

	sf::RenderWindow win;
	sf::Event ev;
	std::function<std::tuple<int,int>()> gfxCardGetDisplaySize; // returns (w,h) of the graphics display.
	std::function<void(sf::RenderWindow&,int,int)> gfxCardRender; // void X(window, x offset, y offset)

	void Init();
	bool PollEvent();
	void RefreshScreen();

	sf::Font fntMono, fntMonoBold, fntSans;
	sfg::Desktop gui;

};

extern Video_t Video;

#endif

