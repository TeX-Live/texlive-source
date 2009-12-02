#!/usr/bin/perl

$language = "Slovak";
$variant = "small";
$prefix = "sk";
$script = "latin";

$alphabet = [
['A',    ['a','A'], ['á','Á'], ['ä','Ä']],
                   [], # a with breve (romanian)
                   [], # a with circumflex (romanian)
                   [], # a with ogonek (polish)
['B',    ['b','B']],
                   [], # b with hook (hausa)
['C',    ['c','C'], ['è','È']],
                   [], # ch (spanish/traditional)
                   [], # cs (hungarian)
                   [], # c with caron (many)
                   [], # c with acute (croatian, lower sorbian, polish)
                   [], # c with circumflex (esperanto)
                   [], # c with cedilla (albanian, kurdish, turkish)
['D',    ['d','D'], ['ï','Ï']],
                   [], # dh (albanian)
                   [], # dz (hungarian)
                   [], # dzs (hungarian)
                   [], # d+z with caron (croatian)
                   [], # d+z with acute (upper sorbian)
                   [], # d with caron (slovak/large)
                   [], # d with stroke (croatian)
                   [], # d with hook (hausa)
                   [], # eth (icelandic)
['E',    ['e','E'], ['é','É']],
                   [], # e with caron (lower/upper sorbian)
                   [], # e with circumflex (kurdish)
                   [], # e with diaeresis (albanian)
                   [], # e with ogonek (polish)
['F',    ['f','F']],
['G',    ['g','G']],
                   [], # gj (albanian)
                   [], # gy (hungarian)
                   [], # g with circumflex (esperanto)
                   [], # g with breve (turkish)
                   [], # g with cedilla/comma (latvian)
                   [], # postpalatal fricative (gypsy/northrussian)
['H',    ['h','H']],
                   [], # h with circumflex (esperanto)
['Ch',   ['ch','Ch','CH']],
                   [], # dotless i (turkish)
['I',    ['i','I'], ['í','Í']],
                   [], # i with inverted breve below (gypsy/northrussian)
                   [], # i with circumflex (kurdish, romanian)
                   [], # i with diaeresis (gypsy/northrussian)
['J',    ['j','J']],
                   [], # j with circumflex (esperanto)
['K',    ['k','K']],
                   [], # kh (gypsy/northrussian)
                   [], # k with cedilla/comma (latvian)
                   [], # k with hook (hausa)
                   [], # x (gypsy/northrussian)
                   [], # l with stroke (lower/upper sorbian)
['L',    ['l','L'], ['å','Å'], ['µ','¥']],
                   [], # lj (croatian)
                   [], # ll (albanian, spanish/traditional)
                   [], # ly (hungarian)
                   [], # l with cedilla/comma (latvian)
                   [], # l with stroke (polish)
                   [], # l with caron (slovak/large)
['M',    ['m','M']],
['N',    ['n','N'], ['ò','Ò']],
                   [], # nj (albanian, croatian)
                   [], # ny (hungarian)
                   [], # n with caron (slovak/large)
                   [], # n with acute (lower/upper sorbian, polish)
                   [], # n with tilde (spanish/modern, spanish/traditional)
                   [], # n with cedilla/comma (latvian)
['O',    ['o','O'], ['ó','Ó'], ['ô','Ô']],
                   [], # o with acute (polish, upper sorbian)
                   [], # o with circumflex (vietnamese)
                   [], # o with horn (vietnamese)
                   [], # o with diaeresis (hungarian, turkish)
['P',    ['p','P']],
                   [], # ph (gypsy/northrussian)
['Q',    ['q','Q']],
['R',    ['r','R'], ['à','À'], ['ø','Ø']],
                   [], # rr (albanian)
                   [], # r with caron (czech, slovak/large, upper sorbian)
                   [], # r with acute (lower sorbian)
                   [], # r with cedilla/comma (latvian)
['S',    ['s','S'], ['¹','©']],
                   [], # sh (albanian)
                   [], # sz (hungarian)
                   [], # s with caron (many)
                   [], # s with acute (lower sorbian, polish)
                   [], # s with circumflex (esperanto)
                   [], # s with comma below (romanian)
                   [], # s with cedilla (kurdish, turkish)
                   [], # z (estonian)
                   [], # z with caron (estonian)
['T',    ['t','T'], ['»','«']],
                   [], # th (albanian)
                   [], # ty (hungarian)
                   [], # t with caron (slovak/large)
                   [], # t with comma below (romanian)
                   [], # c with acute (upper sorbian)
['U',    ['u','U'], ['ú','Ú']],
                   [], # u with breve (esperanto)
                   [], # u with circumflex (kurdish)
                   [], # u with horn (vietnamese)
                   [], # u with diaeresis (hungarian, turkish)
['V',    ['v','V']],
['W',    ['w','W']],
                   [], # o with tilde (estonian)
                   [], # a with diaeresis (estonian)
                   [], # o with diaeresis (estonian)
                   [], # u with diaeresis (estonian)
['X',    ['x','X']],
                   [], # xh (albanian)
['Y',    ['y','Y'], ['ý','Ý']],
                   [], # y preceded by apostrophe (hausa)
                   [], # yogh (english)
['Z',    ['z','Z'], ['¾','®']],
                   [], # zh (albanian)
                   [], # zs (hungarian)
                   [], # z with caron (many)
                   [], # z with acute (lower sorbian, polish)
                   [], # z with dot above (polish)
                   [], # thorn (icelandic)
                   [], # wynn (english)
                   [], # ligature ae (danish, icelandic, norwegian)
                   [], # o with stroke (danish, norwegian)
                   [], # a with ring above (danish, norwegian, swedish)
                   [], # a with diaeresis (finnish, swedish)
                   [], # o with diaeresis (finnish, swedish)
                   [], # a with ring above (icelandic)
];

$ligatures = [
[['ß'], 'after', [['s','s']]],
];

@special = ('?', '!', '.', 'letters', '-', '\'');

$sortcase = "Aa";
#$sortcase = "aA";

do 'make-rules.pl';
