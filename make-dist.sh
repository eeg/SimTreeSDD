#!/bin/bash          

PACKAGE=SimTreeSDD
VERSION=$(date +%Y%m%d)
ARCHIVE=packaged/$PACKAGE-$VERSION

if [ -a $ARCHIVE ]
then
	echo "aborting:" $ARCHIVE "already exists" 
else
	git checkout-index COPYING misc/* src/* --prefix=$ARCHIVE/
	echo -e  "$PACKAGE-$VERSION\n\n$(cat README)" > $ARCHIVE/README
	cp doc/SimTreeSDD-manual.pdf $ARCHIVE/misc/
	tar czvf $ARCHIVE.tar.gz $ARCHIVE/
fi
