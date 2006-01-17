/^_Xconst /s/psheader\[\] = "\\/*psheader[] = {/
/^\/TeXDict/,$s/^\(.*\)\\n\\$/"\1",/
/^\(.*"\);/s//"\1, 0 };/