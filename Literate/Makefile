DCOMPILER=gdc

Literate.html: Literate.md
	bin/lit Literate.md

fullbuild: Literate.html $(sources)
	$(DCOMPILER) $(sources)  -o bin/lit 
	bin/lit Literate.md

build: $(sources)
	$(DCOMPILER) $(sources) -o bin/lit

sources = src/globals.d \
		  src/main.d \
		  src/parser.d \
		  src/tangler.d \
		  src/util.d \
		  src/weaver.d \
		  src/dmarkdown/html.d \
		  src/dmarkdown/markdown.d \
		  src/dmarkdown/package.d \
		  src/dmarkdown/string.d
