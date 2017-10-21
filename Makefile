CXX=g++
CPPFLAGS=-pthread -lpython2.7 -std=c++11 `pkg-config --cflags --libs libnotify`
WFLAGS=-W -Wall -Wextra

all: 
	$(CXX) src/*.cpp $(CPPFLAGS) $(WFLAGS) -o exe

clear:
	rm exe
	rm ./src/Python/*.pyc