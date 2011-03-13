#!/usr/bin/perl

$language = "A general sorting order for Western European languages";
$prefix = "xy";
$script = "latin";

$alphabet = [
['A',  ['a','A'],['†','µ'],['Ö','∑'],['É','∂'],['∆','«'],['Ü','è'],
         ['Ñ','é']],
                   [], # a with breve (romanian)
                   [], # a with circumflex (romanian)
                   [], # a with ogonek (polish)
['B',  ['b','B']],
                   [], # b with hook (hausa)
['C',  ['c','C'],['á','Ä']],
                   [], # ch (spanish/traditional)
                   [], # cs (hungarian)
                   [], # c with caron (many)
                   [], # c with acute (croatian, lower sorbian, polish)
                   [], # c with circumflex (esperanto)
                   [], # c with cedilla (albanian, kurdish, turkish)
['D',  ['d','D']],
                   [], # dh (albanian)
                   [], # dz (hungarian)
                   [], # dzs (hungarian)
                   [], # d+z with caron (croatian)
                   [], # d+z with acute (upper sorbian)
                   [], # d with caron (slovak/large)
                   [], # d with stroke (croatian)
                   [], # d with hook (hausa)
                   [], # eth (icelandic)
['E',  ['e','E'],['Ç','ê'],['ä','‘'],['à','“'],['â','”']],
                   [], # e with caron (lower/upper sorbian)
                   [], # e with circumflex (kurdish)
                   [], # e with diaeresis (albanian)
                   [], # e with ogonek (polish)
['F',  ['f','F']],
['G',  ['g','G']],
                   [], # gj (albanian)
                   [], # gy (hungarian)
                   [], # g with circumflex (esperanto)
                   [], # g with breve (turkish)
                   [], # g with cedilla/comma (latvian)
                   [], # postpalatal fricative (gypsy/northrussian)
['H',  ['h','H']],
                   [], # h with circumflex (esperanto)
                   [], # ch (many)
                   [], # dotless i (turkish)
['I',  ['i','I'],['°','÷'],['ç','ﬁ'],['å','◊'],['ã','ÿ']],
                   [], # i with inverted breve below (gypsy/northrussian)
                   [], # i with circumflex (kurdish, romanian)
                   [], # i with diaeresis (gypsy/northrussian)
['J',  ['j','J']],
                   [], # j with circumflex (esperanto)
['K',  ['k','K']],
                   [], # kh (gypsy/northrussian)
                   [], # k with cedilla/comma (latvian)
                   [], # k with hook (hausa)
                   [], # x (gypsy/northrussian)
                   [], # l with stroke (lower/upper sorbian)
['L',  ['l','L']],
                   [], # lj (croatian)
                   [], # ll (albanian, spanish/traditional)
                   [], # ly (hungarian)
                   [], # l with cedilla/comma (latvian)
                   [], # l with stroke (polish)
                   [], # l with caron (slovak/large)
['M',  ['m','M']],
['N',  ['n','N'],['§','•']],
                   [], # nj (albanian, croatian)
                   [], # ny (hungarian)
                   [], # n with caron (slovak/large)
                   [], # n with acute (lower/upper sorbian, polish)
                   [], # n with tilde (spanish/modern, spanish/traditional)
                   [], # n with cedilla/comma (latvian)
['O',  ['o','O'],['¢','‡'],['ï','„'],['ì','‚'],['‰','Â'],['î','ô'],
         ['õ','ù']],
                   [], # o with acute (polish, upper sorbian)
                   [], # o with circumflex (vietnamese)
                   [], # o with horn (vietnamese)
                   [], # o with diaeresis (hungarian, turkish)
['P',  ['p','P']],
                   [], # ph (gypsy/northrussian)
['Q',  ['q','Q']],
['R',  ['r','R']],
                   [], # rr (albanian)
                   [], # r with caron (czech, slovak/large, upper sorbian)
                   [], # r with acute (lower sorbian)
                   [], # r with cedilla/comma (latvian)
['S',  ['s','S']],
                   [], # sh (albanian)
                   [], # sz (hungarian)
                   [], # s with caron (many)
                   [], # s with acute (lower sorbian, polish)
                   [], # s with circumflex (esperanto)
                   [], # s with comma below (romanian)
                   [], # s with cedilla (kurdish, turkish)
                   [], # z (estonian)
                   [], # z with caron (estonian)
['T',  ['t','T']],
                   [], # th (albanian)
                   [], # ty (hungarian)
                   [], # t with caron (slovak/large)
                   [], # t with comma below (romanian)
                   [], # c with acute (upper sorbian)
['U',  ['u','U'],['£','È'],['ó','Î'],['ñ','Í'],['Å','ö'],
	     ],
                   [], # u with breve (esperanto)
                   [], # u with circumflex (kurdish)
                   [], # u with horn (vietnamese)
                   [], # u with diaeresis (hungarian, turkish)
['V',  ['v','V']],
['W',  ['w','W']],
                   [], # o with tilde (estonian)
                   [], # a with diaeresis (estonian)
                   [], # o with diaeresis (estonian)
                   [], # u with diaeresis (estonian)
['X',  ['x','X']],
                   [], # xh (albanian)
['Y',  ['y','Y'],['Ï','Ì'],['ò']],
                   [], # y preceded by apostrophe (hausa)
                   [], # yogh (english)
['Z',  ['z','Z']],
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

$sortcase = 'Aa';
#$sortcase = 'aA';

$ligatures = [
[['·'], 'after', [['s','s']]],
[['ë','í'], 'after', [['a','e'], ['A','e']]],

];

@special = ('?', '!', '.', 'letters', '-', '\'');

do 'make-rules.pl';
