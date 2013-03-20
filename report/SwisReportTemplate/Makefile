

TEMPFILES   = $(wildcard *.aux *.bbl *.blg *.dvi *.log *.toc *~ )
TEXSRCS     = $(wildcard *.tex )
PDFDOCS     = $(addsuffix .pdf,$(basename $(TEXSRCS)))

all : $(PDFDOCS)

%.pdf : %.tex
	pdflatex $<
	bibtex $*
	pdflatex $<
	pdflatex $<

clean :
ifneq ( $(TEMPFILES), '' )
	rm $(TEMPFILES)
endif

distclean : clean
	rm $(PDFDOCS)

