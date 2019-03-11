" Vim syntax file
" Language:	Gabc gregorian chant notation
" Last Change:	2016 Feb 11

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match gabcAttributeName /^[^:]*:/
syn match gabcAttributeEnd ";"
syn match gabcNoteError "." contained
syn match gabcBasicNote "[a-np]" contained
syn match gabcBasicNote "[A-NP][012]\?" contained
syn match gabcNoteShape "[\<\>~xy#vVwWqQR\-Ss\.+]" contained
syn match gabcNoteShape "_[0-5]*" contained
syn match gabcNoteShape "[oO'\.][01]\?" contained
syn match gabcNoteShape "r[0-8]\?" contained
syn match gabcClef "[cf]b\?[1-5]" contained
syn match gabcTextMarkup "</\?e>" contained
syn match gabcTextMarkup "</\?b>" contained
syn match gabcTextMarkup "</\?i>" contained
syn match gabcTextMarkup "</\?c>" contained
syn match gabcTextMarkup "</\?ul>" contained
syn match gabcTextMarkup "</\?tt>" contained
syn match gabcTextMarkup "</\?sc>" contained
syn match gabcTextMarkup "</\?eu>" contained
syn match gabcTextOrNoteMarkup "</\?nlba>" contained
syn match gabcTextCenter "[{}]" contained
syn match gabcFuse "@" contained
syn match gabcFuseEnd "\]" contained
syn match gabcBar ":?\?" contained
syn match gabcBar ";[1-8]\?" contained
syn match gabcBar ",[0-8]\?" contained
syn match gabcBar "[`^]0\?" contained
syn match gabcSpace "[! ]" contained
syn match gabcSpace "/0\?" contained
syn match gabcSpace "z[-+0]\?" contained
syn match gabcSpace "Z[-+]\?" contained
syn match gabcSpace "/\[[^\]]*\]" contained extend
syn match gabcCommand "@\@<!\[[^\]]*\]" contained extend
syn match gabcCommand "[{}]" contained extend
syn match gabcBracket "\[\[" contained
syn match gabcBracket "\]\]" contained

syn region gabcComment start="%" end="$" keepend extend
syn region gabcAlt matchgroup=gabcTextMarkup start="<alt>" end="</alt>"
            \ contained
syn region gabcSpecial matchgroup=gabcTextMarkup start="<sp>" end="</sp>"
            \ contained
syn region gabcVerbatim matchgroup=gabcTextMarkup start="<v>" end="</v>"
            \ contained
syn region gabcNabc matchgroup=gabcNabcCut start="|" end="[|)]" keepend
syn cluster gabcFusible contains=gabcBasicNote,gabcNoteShape,gabcBar,gabcSpace,
            \gabcComment,gabcCommand,gabcNoteError,gabcFuse,
            \gabcTextOrNoteMarkup,gabcClef,gabcNabc,gabcBracket
syn region gabcFuseGroup matchGroup=gabcFuseGroup start="@\[" end="\]"
            \ contains=@gabcFusible,gabcFuseEnd contained keepend
syn region gabcNotes matchgroup=gabcNote start="(" end=")"
            \ contains=@gabcFusible,gabcFuseGroup contained keepend
syn region gabcTranslation matchgroup=gabcTextMarkup start="\[" end="\]"
            \ contained extend
syn region gabcText start="^\(%%\)\@=" end="\%$"
            \ contains=gabcNotes,gabcTextMarkup,gabcTextOrNoteMarkup,
            \gabcTextCenter,gabcTranslation,gabcComment,gabcAlt,gabcSpecial,
            \gabcVerbatim

" Define the default highlighting.
hi def link gabcAttributeName       Statement
hi def link gabcAttributeEnd        Statement
hi def link gabcText                Constant
hi def link gabcTranslation         Constant
hi def link gabcComment             Comment
hi def link gabcTextMarkup          Delimiter
hi def link gabcTextOrNoteMarkup    Delimiter
hi def link gabcTextCenter          Delimiter
hi def link gabcSpecial             Constant
hi def link gabcVerbatim            Constant
hi def link gabcAlt                 Constant
hi def link gabcClef                Statement
hi def link gabcBasicNote           Statement
hi def link gabcNoteShape           PreProc
hi def link gabcCommand             Type
hi def link gabcBar                 Special
hi def link gabcSpace               Special
hi def link gabcNabcCut             Delimiter
hi def link gabcBracket             Delimiter
hi def link gabcFuse                Constant
hi def link gabcFuseGroup           Constant
hi def link gabcFuseEnd             Constant
hi def link gabcNoteError           Error

let b:current_syntax = "gabc"
