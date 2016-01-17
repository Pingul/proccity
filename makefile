CPPFLAGS = -Wall -I/opt/local/include/ -I/opt/local/include/freetype2 -std=c++11
FRAMEWORKS = -L/opt/local/lib/ -framework OpenGL -framework Cocoa -lglfw -lGLEW -lfreetype
DEPS = glUtilities.hpp camera.hpp buildings.hpp geometryUtils.hpp texture.hpp glText.hpp drawable.hpp

main: main.o glUtilities.o camera.o buildings.cpp geometryUtils.cpp texture.cpp glText.cpp drawable.cpp
	clang++ $(CPPFLAGS) $(FRAMEWORKS) -o main $^

%.o: %.cpp $(DEPS)
	clang++ -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f *.o main