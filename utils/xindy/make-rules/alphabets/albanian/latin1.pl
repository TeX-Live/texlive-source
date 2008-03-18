#!/usr/bin/perl

$language = "Albanian";
$prefix = "sq";
$script = "latin";

$alphabet = [
['A',  ['a','A']],
                   [], # a with breve (romanian)
                   [], # a with circumflex (romanian)
                   [], # a with ogonek (polish)
['B',  ['b','B']],
                   [], # b with hook (hausa)
['C',  ['c','C']],
                   [], # ch (spanish/traditional)
                   [], # cs (hungarian)
                   [], # c with caron (many)
                   [], # c with acute (croatian, lower sorbian, polish)
                   [], # c with circumflex (esperanto)
['Ç',  ['ç','Ç']],
['D',  ['d','D']],
['Dh', ['dh','Dh','DH']],
                   [], # dz (hungarian)
                   [], # dzs (hungarian)
                   [], # d+z with caron (croatian)
                   [], # d+z with acute (upper sorbian)
                   [], # d with caron (slovak/large)
                   [], # d with stroke (croatian)
                   [], # d with hook (hausa)
                   [], # eth (icelandic)
['E',  ['e','E']],
                   [], # e with caron (lower/upper sorbian)
                   [], # e with circumflex (kurdish)
['Ë',  ['ë','Ë']],
                   [], # e with ogonek (polish)
['F',  ['f','F']],
['G',  ['g','G']],
['Gj', ['gj','Gj','GJ']],
                   [], # gy (hungarian)
                   [], # g with circumflex (esperanto)
                   [], # g with breve (turkish)
                   [], # g with cedilla/comma (latvian)
                   [], # postpalatal fricative (gypsy/northrussian)
['H',  ['h','H']],
                   [], # h with circumflex (esperanto)
                   [], # ch (many)
                   [], # dotless i (turkish)
['I',  ['i','I']],
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
['Ll', ['ll','Ll','LL']],
                   [], # ly (hungarian)
                   [], # l with caron (slovak/large)
                   [], # l with cedilla/comma (latvian)
                   [], # l with stroke (polish)
['M',  ['m','M']],
['N',  ['n','N']],
['Nj', ['nj','Nj','NJ']],
                   [], # ny (hungarian)
                   [], # n with caron (slovak/large)
                   [], # n with acute (lower/upper sorbian, polish)
                   [], # n with tilde (spanish/modern, spanish/traditional)
                   [], # n with cedilla/comma (latvian)
['O',  ['o','O']],
                   [], # o with acute (polish, upper sorbian)
                   [], # o with circumflex (vietnamese)
                   [], # o with horn (vietnamese)
                   [], # o with diaeresis (hungarian, turkish)
['P',  ['p','P']],
                   [], # ph (gypsy/northrussian)
['Q',  ['q','Q']],
['R',  ['r','R']],
['Rr', ['rr','Rr','RR']],
                   [], # r with caron (czech, slovak/large, upper sorbian)
                   [], # r with acute (lower sorbian)
                   [], # r with cedilla/comma (latvian)
['S',  ['s','S']],
['Sh', ['sh','Sh','SH']],
                   [], # sz (hungarian)
                   [], # s with caron (many)
                   [], # s with acute (lower sorbian, polish)
                   [], # s with circumflex (esperanto)
                   [], # s with comma below (romanian)
                   [], # s with cedilla (kurdish, turkish)
                   [], # z (estonian)
                   [], # z with caron (estonian)
['T',  ['t','T']],
['Th', ['th','Th','TH']],
                   [], # ty (hungarian)
                   [], # t with caron (slovak/large)
                   [], # t with comma below (romanian)
                   [], # c with acute (upper sorbian)
['U',  ['u','U']],
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
['Xh', ['xh','Xh','XH']],
['Y',  ['y','Y']],
                   [], # y preceded by apostrophe (hausa)
                   [], # yogh (english)
['Z',  ['z','Z']],
['Zh', ['zh','Zh','ZH']],
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
];

@special = ('?', '!', '.', 'letters', '-', '\'');

do 'make-rules.pl';
