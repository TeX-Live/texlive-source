ENTRY {
  field001
  field002
  field003
  field004
  field005
  field006
  field007
  field008
  field009
  field010
  field011
  field012
  field013
  field014
  field015
  field016
  field017
  field018
  field019
  field020
  field021
  field022
  field023
  field024
  field025
  field026
  field027
  field028
  field029
  field030
  field031
  field032
  field033
  field034
  field035
  field036
  field037
  field038
  field039
  field040
  field041
  field042
  field043
  field044
  field045
  field046
  field047
  field048
  field049
  field050
}{}{}

FUNCTION {output_entry} {
  cite$ " " * write$
}

FUNCTION {type} {output_entry}

READ

FUNCTION {set_sortkey} {
  cite$ 'sort.key$ :=
}

ITERATE {set_sortkey}

SORT

FUNCTION {output_beg} {
  "\section*{Test result}" write$ newline$
  "Processing... " write$
}

FUNCTION {output_end} {
  newline$
  "\endinput" write$ newline$
}

EXECUTE {output_beg}

ITERATE {call.type$}

EXECUTE {output_end}
