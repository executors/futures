BUILDDIR = build

all: remote

setup:
	mkdir -p $(BUILDDIR)/

remote: setup
	find . -name "*.bs" -type f | sed 's/\.bs$$//' | xargs -I{} -t -n 1 sh -c "curl https://api.csswg.org/bikeshed/ -F file=@{}.bs -F force=1 > {}.html"
	mv *.html $(BUILDDIR)/

local: setup
	for t in `ls *.bs`; do bikeshed -f spec $${t}; done
	mv *.html $(BUILDDIR)/

clean:
	rm $(BUILDDIR)/*
	rmdir $(BUILDDIR)

