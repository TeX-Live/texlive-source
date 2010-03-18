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
  field051
  field052
  field053
  field054
  field055
  field056
  field057
  field058
  field059
  field060
  field061
  field062
  field063
  field064
  field065
  field066
  field067
  field068
  field069
  field070
  field071
  field072
  field073
  field074
  field075
  field076
  field077
  field078
  field079
  field080
  field081
  field082
  field083
  field084
  field085
  field086
  field087
  field088
  field089
  field090
  field091
  field092
  field093
  field094
  field095
  field096
  field097
  field098
  field099
  field100
  field101
  field102
  field103
  field104
  field105
  field106
  field107
  field108
  field109
  field110
  field111
  field112
  field113
  field114
  field115
  field116
  field117
  field118
  field119
  field120
  field121
  field122
  field123
  field124
  field125
  field126
  field127
  field128
  field129
  field130
  field131
  field132
  field133
  field134
  field135
  field136
  field137
  field138
  field139
  field140
  field141
  field142
  field143
  field144
  field145
  field146
  field147
  field148
  field149
  field150
  field151
  field152
  field153
  field154
  field155
  field156
  field157
  field158
  field159
  field160
  field161
  field162
  field163
  field164
  field165
  field166
  field167
  field168
  field169
  field170
  field171
  field172
  field173
  field174
  field175
  field176
  field177
  field178
  field179
  field180
  field181
  field182
  field183
  field184
  field185
  field186
  field187
  field188
  field189
  field190
  field191
  field192
  field193
  field194
  field195
  field196
  field197
  field198
  field199
  field200
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
