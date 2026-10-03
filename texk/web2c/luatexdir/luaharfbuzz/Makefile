DOCS_DIR := docs
# Default compiler flags (override with: make CFLAGS="...").
CFLAGS ?= -Wall -Wextra -Wsign-compare -Werror

build:
	CFLAGS="$(CFLAGS)" luarocks --local make

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
