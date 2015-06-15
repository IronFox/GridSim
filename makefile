CPP= g++
CXXFLAGS= -O3  -I"." -I"../Nx/include" -DNX32
#-DNX_CALL_CONV
BIN= ./maker/maker
LINKOBJ= ./maker/main.o ./io/file_system.o ./io/string_file.o ./general/system.o ./string/str_class.o ./string/tokenizer.o ./string/token_parser.o ./string/string_converter.o ./general/thread_system.o ./general/exception.o ./io/log.o
#./general/root.o 
LIBS= -L"/usr/X11R6/lib/" -lX11 -lpthread

all: $(BIN)
	$(BIN) $(CXXFLAGS)

new: clean $(BIN)
	$(BIN) --new $(CXXFLAGS)

clean:
	rm -f $(LINKOBJ)
	

first: $(BIN)
	$(BIN) --first $(CXXFLAGS)

$(BIN): $(LINKOBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)


*.o: *.cpp
	$(CPP) -c $*.cpp -o $*.o $(CXXFLAGS)
