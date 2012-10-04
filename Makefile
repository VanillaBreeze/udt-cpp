C++ = cc
CCFLAGS = -Wall -Wno-multichar -finline-functions -O0 -std=c++0x -DDEBUG -I../ -Icdebug
LDFLAGS = -ludt -lstdc++

OBJS = socket.o epoll.o

%.o: %.cpp
	$(C++) $(CCFLAGS) --analyze $< -c
	$(C++) $(CCFLAGS) $< -c

libudtcpp.a: $(OBJS)
	ar -r $@ $^

all: libudtcpp.a
	

clean:
	rm *.o *.a
