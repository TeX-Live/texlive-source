/^zmakescripts ( q , delta ) /a\
#pragma optimize ("g", on)
/^zmakeleftright ( q , style , maxd , maxh ) / a\
#pragma optimize ("g", on)
/^zmakeop ( q ) / a\
#pragma optimize ("g", on)
/^zmakeord ( q ) / a\
#pragma optimize ("g", on)
/^zbegintokenlist ( halfword p , quarterword t )/ i\
#pragma optimize ("g", off)
/^backinput ( void ) / i\
#pragma optimize ("g", on)
/^getnext ( void ) / i\
#pragma optimize ("g", off)
/^firmuptheline ( void ) / i\
#pragma optimize ("g", on)
