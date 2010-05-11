#!/usr/bin/perl

$language = "Klingon";
$variant = "Standard Okrand Latin transliteration";
$prefix = "k1";
$script = "latin";

$alphabet = [
['a',  ['a']],
['b',  ['b']],
['ch',  ['ch']],
['D',  ['D']],
['e',  ['e']],
['gh',  ['gh']],
['H',  ['H']],
['I',  ['I']],
['j',  ['j']],
['l',  ['l']],
['m',  ['m']],
['n',  ['n']],
['ng',  ['ng']],
['o',  ['o']],
['p',  ['p']],
['q',  ['q']],
['Q',  ['Q']],
['r',  ['r']],
['S',  ['S']],
['t',  ['t']],
['tlh',  ['tlh']],
['u',  ['u']],
['v',  ['v']],
['w',  ['w']],
['y',  ['y']],
['\'',  ['\'']]
];

$sortcase = 'Aa';
#$sortcase = 'aA';

$ligatures = [
];

@special = ('?', '!', '.', 'letters', '-', '{', '}');

do 'make-rules.pl';
