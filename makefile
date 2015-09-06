CXX=clang++-3.5
CXXFLAGS= 
#-O3  -I"." -I"../Nx/include" -DNX32
CPPFLAGS= -std=c++11 -Wno-ignored-attributes -ferror-limit=1 -Wno-dangling-else -Wno-undefined-bool-conversion
#-DNX_CALL_CONV
BIN= ./maker/maker
LINKOBJ= ./maker/main.o ./io/file_system.o ./io/string_file.o ./general/system.o ./string/str_class.o ./string/tokenizer.o ./string/token_parser.o ./string/string_converter.o ./general/thread_system.o ./general/exception.o ./io/log.o
#./general/root.o 
LIBS= -L"/usr/X11R6/lib/" -lX11 -lpthread

all: $(BIN)
	yo1 $(BIN) $(CXXFLAGS)

#new: clean $(BIN)
#	yo2 $(BIN) --new $(CXXFLAGS)

clean:
	rm -f $(LINKOBJ)
	

#first: $(BIN)
#	yo4 $(BIN) --first $(CXXFLAGS)

$(BIN): $(LINKOBJ)
#	yo5 $(LINKOBJ) -o $(BIN) $(LIBS)


#*.o: *.cpp
#	yo6 -c $*.cpp -o sklfdsfn $*.o $(CXXFLAGS) $(CPPFLAGS)
