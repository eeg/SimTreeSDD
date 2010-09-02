CFLAGS = -Wall -g -O3
LDFLAGS = -lm
CC = gcc

targets = SimTreeSDD

SimTreeSDD_obj = SimTreeSDD.o build_common.o build_2states.o build_2regions.o keyvalue.o label.o nodes.o input_sim.o output.o randist.o report.o

all: ${targets}

depend:
	gcc -E -MM *.c > .depend

#install:
#	cp ${targets} /usr/local/bin/

SimTreeSDD: ${SimTreeSDD_obj}
	${CC} ${LDFLAGS} ${SimTreeSDD_obj} -o SimTreeSDD
	mv ${targets} ../

clean:
#	rm *.o ${targets}
	rm *.o
