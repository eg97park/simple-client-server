TARGET=../bin/ts
CPPFLAGS+=-Wall -O0 -g
LDLIBS+=-pthread
#LDLIBS+=-lws2_32 # for mingw

all: $(TARGET)

$(TARGET): ts.o ../mingw_net.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f $(TARGET) *.o
