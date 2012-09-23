all:
	g++ pc.cpp -DVIDEO -o pc --std=gnu++0x -lSDL
	
graphics-debug:
	g++ pc.cpp -DVIDEO -o pc --std=gnu++0x -g -lSDL
	
cpu-debug:
	g++ pc.cpp -o pc --std=gnu++0x -g