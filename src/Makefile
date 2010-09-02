CFLAGS = -Wall -g -O2 -std=gnu99
LDFLAGS = -lm
CC = gcc

targets = SimTreeSDD TreeLike SDD-MCMC PruneTree NodeAges CompositePruneTree CountTips CountTips3 AvgBranchLength EstimateBeta PruneTree2 PD_TTD

SimTreeSDD_obj = SimTreeSDD.o build.o build_2regions.o keyvalue.o label.o nodes.o input_sim.o output.o randist.o report.o

TreeLike_obj = TreeLike.o read_tree.o input_like.o keyvalue.o label.o likelihood.o nodes.o output_like.o output.o simplex.o report.o extra.o reconstruct.o

Get2Like_obj = Get2Like.o build.o buildcheck.o extra.o input_sim.o label.o likelihood-sdd.o nodes.o randist.o read_tree.o report.o output.o output_like.o keyvalue.o

SDD-MCMC_obj = SDD-MCMC.o build.o buildcheck.o extra.o input_like.o input_mcmc.o input_sim.o keyvalue.o label.o likelihood-sdd.o nodes.o output.o output_like.o randist.o read_tree.o report.o

PruneTree_obj = PruneTree.o read_tree.o label.o nodes.o output.o report.o extra.o copy.o build.o randist.o prune.o

PruneTree2_obj = PruneTree2.o read_tree.o label.o nodes.o output.o report.o extra.o copy.o build.o randist.o prune.o

CompositePruneTree_obj = CompositePruneTree.o read_tree.o label.o nodes.o output.o report.o extra.o copy.o build.o randist.o prune.o

NodeAges_obj = NodeAges.o read_tree.o label.o nodes.o output.o report.o extra.o copy.o build.o randist.o prune.o

CountTips_obj = CountTips.o read_tree.o label.o nodes.o report.o extra.o

CountTips3_obj = CountTips3.o read_tree.o label.o nodes.o report.o extra.o

AvgBranchLength_obj = AvgBranchLength.o read_tree.o label.o nodes.o report.o extra.o

EstimateBeta_obj = EstimateBeta.o read_tree.o label.o nodes.o report.o extra.o

PD_TTD_obj = PD_TTD.o read_tree.o label.o nodes.o output.o report.o extra.o copy.o build.o randist.o prune.o


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

PruneTree: ${PruneTree_obj}
	${CC} ${LDFLAGS} ${PruneTree_obj} -o PruneTree

PruneTree2: ${PruneTree2_obj}
	${CC} ${LDFLAGS} ${PruneTree2_obj} -o PruneTree2

CompositePruneTree: ${CompositePruneTree_obj}
	${CC} ${LDFLAGS} ${CompositePruneTree_obj} -o CompositePruneTree

NodeAges: ${NodeAges_obj}
	${CC} ${LDFLAGS} ${NodeAges_obj} -o NodeAges

CountTips: ${CountTips_obj}
	${CC} ${LDFLAGS} ${CountTips_obj} -o CountTips

CountTips3: ${CountTips3_obj}
	${CC} ${LDFLAGS} ${CountTips3_obj} -o CountTips3

AvgBranchLength: ${AvgBranchLength_obj}
	${CC} ${LDFLAGS} ${AvgBranchLength_obj} -o AvgBranchLength

EstimateBeta: ${EstimateBeta_obj}
	${CC} ${LDFLAGS} ${EstimateBeta_obj} -o EstimateBeta 

PD_TTD: ${PD_TTD_obj}
	${CC} ${LDFLAGS} ${PD_TTD_obj} -o PD_TTD

clean:
	rm *.o ${targets}
