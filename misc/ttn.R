require(ape)
#--------------------------------------------------
# functions for dealing with the .ttn files produced by SimTreeSDD 
# (or assembled by hand)
#-------------------------------------------------- 

read.ttn <- function(treefile, nodes=TRUE, check=TRUE)
{
    treestr <- readLines(treefile, n=1)
    tree <- read.tree(text=treestr)

    all.states <- read.table(treefile, skip=1)

    if (nodes)
    {
        ntips <- (length(all.states[,1]) + 1) / 2
        states <- all.states[,2]
        names(states) <- all.states[,1]
        tree$node.label <- names(states[ntips+1:length(states)])
    } else
    {
        ntips <- length(all.states[,1])
        states <- all.states[1:ntips,2]
        names(states) <- all.states[1:ntips,1]
    }

    if (check)
    {
        ntips2 <- length(tree$tip.label)
        if (ntips != ntips2)
        {
            warning(paste("number of states (", ntips, ") is not equal to \
                         number of tips (", ntips2, ")", sep=""), immediate.=T)
        }
        print(paste("read tree with", ntips2, "tips"))

        i <- which(!(names(states[1:ntips]) %in% tree$tip.label))
        if (length(i) != 0)
        {
            warning("some tip and state labels do not match", immediate.=T)
            print(names(states)[i])
            j <- which(!(tree$tip.label %in% names(states)))
            print(tree$tip.label[j])
        }
    }

    return(list(tree = tree, states = states))
}


write.ttn <- function(ttn, file, nodes=FALSE)
{
    write.tree(ttn$tree, file=file)
    write.table(ttn$states, file=file, append=TRUE, quote=FALSE, sep="\t", 
                row.names=TRUE, col.names=FALSE)
}
