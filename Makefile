BUILDDIR = build

all: setup html
	
setup:
	mkdir -p $(BUILDDIR)

html:
	find . -name "*.bs" -type f | sed 's/\.bs$$//' | xargs -I{} -t -n 1 sh -c "curl https://api.csswg.org/bikeshed/ -F file=@{}.bs -F force=1 > {}.html"
	mv *.html $(BUILDDIR)/

