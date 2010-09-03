#--------------------------------------------------
# Functions for dealing with the .ttn files produced by SimTreeSDD.
#
# Note that ttn node numbering is in downpass order, which is different than
# ape's numbering.  Also, ttn tip numbering starts with 0 rather than with 1.
# This is dealt with by making the ttn tip and node numbers into ape tip.labels
# and node.labels.
#-------------------------------------------------- 

require(ape)

read.ttn <- function(treefile, nodes=TRUE, check=TRUE)
{
    treestr <- readLines(treefile, n=1)
    tree <- read.tree(text=treestr)
    ntips <- Ntip(tree)
    ntipsnodes <- Nnode(tree) + ntips

    all.states <- read.table(treefile, skip=1)
    tip.states <- all.states[1:ntips, 2]
    names(tip.states) <- all.states[1:ntips, 1]

    if (nodes)
    {
         node.states <- all.states[(ntips+1) : ntipsnodes, 2]
         names(node.states) <- all.states[(ntips+1) : ntipsnodes, 1]
    }

    if (check) # no node checking yet
    {
        ntips2 <- length(tree$tip.label)
        if (ntips != ntips2)
        {
            warning(paste("number of states (", ntips, ") is not equal to \
                         number of tips (", ntips2, ")", sep=""), immediate.=T)
        }
        print(paste("read tree with", ntips2, "tips"))

        i <- which(!(names(tip.states[1:ntips]) %in% tree$tip.label))
        if (length(i) != 0)
        {
            warning("some tip and state labels do not match", immediate.=T)
            print(names(tip.states)[i])
            j <- which(!(tree$tip.label %in% names(tip.states)))
            print(tree$tip.label[j])
        }
    }

    if (nodes)
    {
         return(list(tree = tree, tip.states = tip.states, node.states = node.states))
    } else
    {
         return(list(tree = tree, states = tip.states))
    }
}


write.ttn <- function(ttn, filename, nodes=FALSE)
{
    write.tree(ttn$tree, file=filename)

    if (nodes)
    {
         write.table(ttn$tip.states, file=filename, append=TRUE, quote=FALSE, sep="\t", 
                     row.names=TRUE, col.names=FALSE)
         write.table(ttn$node.states, file=filename, append=TRUE, quote=FALSE, sep="\t", 
                     row.names=TRUE, col.names=FALSE)
    } else
    {
         write.table(ttn$states, file=filename, append=TRUE, quote=FALSE, sep="\t", 
                     row.names=TRUE, col.names=FALSE)
    }
}


plot.ttn <- function(ttn, nodes=TRUE)
{
     state.colors <- c("black", "white", "gray")

     tree <- ttn$tree
     plot(tree, label.offset=0.2)

     if (nodes)
     {
          tipstates <- ttn$tip.states + 1
          nodestates <- ttn$node.states + 1
     } else
     {
          tipstates <- ttn$states + 1
     }

     tip.colors <- sapply(tipstates[tree$tip.label], function(x) state.colors[x])
     tiplabels(bg=tip.colors, pch=22, cex=2)

     if (nodes)
     {
          node.colors <- sapply(nodestates[tree$node.label], function(x) state.colors[x])
          nodelabels(bg=node.colors, pch=22, cex=2)
     }
}
