" Vim syntax file
" Language:	Gabc gregorian chant notation
" Maintainer:	Elie Roux <elie.roux@telecom-bretagne.eu>
" Last Change:	2008 Nov 29

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match gabcAttributeName /^[^:]*:/
syn match gabcAttributeNameb ";"
syn match gabcTextMarkup "</\?\w>" contained
syn match gabcTranslation "\[[^\[\(]*\]\?" contained
syn match gabcBasicNote "[a-mA-M]" contained
syn match gabcAlteration "[\<\>~xXyYvVoOwWqQ\-Ss\._'1234]" contained
syn match gabcSpace "[`,;!: /zZ]" contained
syn region gabcNotes matchgroup=gabcNote start="(" end=")" contains=gabcBasicNote,gabcAlteration,gabcSpace contained
syn region gabcText start="%%" end="%%zz" contains=gabcNotes,gabcTextMarkup,gabcTranslation


" Define the default highlighting.
hi def link gabcAttributeName   Comment
hi def link gabcAttributeNameb  Comment
hi def link gabcText            Comment
hi def link gabcTextMarkup      PreProc
hi def link gabcBasicNote       Statement
hi def link gabcNote            Type
hi def link gabcAlteration      Type
hi def link gabcSpace           Special

let b:current_syntax = "gabc"
