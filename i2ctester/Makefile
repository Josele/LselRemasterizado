
CC = gcc
CFLAGS = -g -o testillo -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lreadline -lm -lbcm2835
objs = \
        main.o\
	lsquaredc.o\
        i2c1.o
all: main


main: $(objs)


clean:	$(RM) *.o *~ main




