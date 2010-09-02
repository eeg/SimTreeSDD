CFLAGS = -Wall -O2 -g -std=gnu99
LDFLAGS = -lm
CC = gcc

targets = SimTreeSDD TreeLike SDD-MCMC PruneTest

SimTreeSDD_obj = SimTreeSDD.o build.o build_2regions.o keyvalue.o label.o nodes.o input_sim.o output.o randist.o report.o

TreeLike_obj = TreeLike.o read_tree.o input_like.o keyvalue.o label.o likelihood.o nodes.o output_like.o output.o simplex.o report.o extra.o reconstruct.o

Get2Like_obj = Get2Like.o build.o buildcheck.o extra.o input_sim.o label.o likelihood-sdd.o nodes.o randist.o read_tree.o report.o output.o output_like.o keyvalue.o

SDD-MCMC_obj = SDD-MCMC.o build.o buildcheck.o extra.o input_like.o input_mcmc.o input_sim.o keyvalue.o label.o likelihood-sdd.o nodes.o output.o output_like.o randist.o read_tree.o report.o

PruneTest_obj = PruneTest.o read_tree.o label.o nodes.o output.o report.o extra.o copy.o build.o randist.o

all: ${targets}

depend:
	gcc -E -MM *.c > .depend

install:
	cp ${targets} /usr/local/bin/

SimTreeSDD: ${SimTreeSDD_obj}
	${CC} ${LDFLAGS} ${SimTreeSDD_obj} -o SimTreeSDD

TreeLike: ${TreeLike_obj}
	${CC} ${LDFLAGS} ${TreeLike_obj} -o TreeLike

Get2Like: ${Get2Like_obj}
	${CC} ${LDFLAGS} ${Get2Like_obj} -o Get2Like

SDD-MCMC: ${SDD-MCMC_obj}
	${CC} ${LDFLAGS} ${SDD-MCMC_obj} -o SDD-MCMC

PruneTest: ${PruneTest_obj}
	${CC} ${LDFLAGS} ${PruneTest_obj} -o PruneTest

clean:
	rm *.o ${targets}
