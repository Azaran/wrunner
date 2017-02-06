# This should work on Linux.  Modify as needed for other platforms.

BOINC_DIR = ../boinc-src
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib

ifeq ($(OS),Windows_NT)
	CXX = g++
else
	CXX = g++
endif

CXXFLAGS += -g \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \
    -L.

PROGS = wrunner

all: $(PROGS)

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f $(PROGS)
	/bin/rm -f *.o

distclean:
	/bin/rm -f $(PROGS) *.o libstdc++.a

resolver.o: resolver.cpp
	$(CXX) $(CXXFLAGS) -c resolver.cpp
	
parser.o: parser.cpp
	$(CXX) $(CXXFLAGS) -c parser.cpp -std=c++0x
	
socket.o: socket.cpp
	$(CXX) $(CXXFLAGS) -c socket.cpp -std=c++0x
		
control.o: control.cpp
	$(CXX) $(CXXFLAGS) -c control.cpp -std=c++0x
		
standalone.o: standalone.cpp
	$(CXX) $(CXXFLAGS) -c standalone.cpp -std=c++0x
	
simulation.o: simulation.cpp
	$(CXX) $(CXXFLAGS) -c simulation.cpp -std=c++0x

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -Wno-write-strings

wrunner: resolver.o parser.o socket.o control.o standalone.o simulation.o main.o $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) $(CXXFLAGS) -o wrunner resolver.o parser.o socket.o control.o standalone.o simulation.o main.o -pthread -lboinc_api -lboinc -lboost_system -lboost_thread -lzip -lz -static-libgcc -static-libstdc++ -static

