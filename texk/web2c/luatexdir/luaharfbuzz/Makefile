DOCS_DIR := docs

build:
	luarocks --local make

spec: test

test:
	busted --no-coverage .

clean:
	rm -rf build *.so

lint:
	luacheck src spec examples

doc:
	ldoc -d ${DOCS_DIR}  .

.PHONY: build clean lint test spec doc
