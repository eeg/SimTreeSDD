all: SimTreeSDD-manual.pdf

SimTreeSDD-manual.pdf: SimTreeSDD-manual.tex codedoc.sty
	pdflatex SimTreeSDD-manual.tex
	pdflatex SimTreeSDD-manual.tex

clean:
	rm -f *.aux *.log *.toc
