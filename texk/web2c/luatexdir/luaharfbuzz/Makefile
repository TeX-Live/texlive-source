DOCS_DIR := docs

spec:
	busted .

clean:
	rm -rf build *.so

lint:
	luacheck src spec examples

doc:
	ldoc -d ${DOCS_DIR}  .

.PHONY: clean lint spec doc
