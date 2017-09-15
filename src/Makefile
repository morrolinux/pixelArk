OBJ = main.o init.o fileIO.o controller.o
CXXFLAGS=-Wall 
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

pixelArk: $(OBJ)
	g++ -o pixelArk $(OBJ) $(LIBS)

debug: CXXFLAGS += -g -DDEBUG_MODE
debug: pixelArk

-include dependencies

.PHONY: depend clean cleanall

depend:
	g++ -MM *.cc > dependencies
clean:
	rm -f *.o
cleanall:
	rm -f pixelArk *.o *~

