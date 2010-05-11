#!/usr/bin/perl

$language = "Russian";
$prefix = "ru";
$script = "cyrillic";

$alphabet = [
['°',  ['Ð','°'] ],
['±',  ['Ñ','±']],
['²',  ['Ò','²']],
['³',  ['Ó','³']],
                   [], # ghe with upturn (ukrainian)
['´',  ['Ô','´']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['µ',  ['Õ','µ'],  ['ñ','¡'] ],
                   [], # io (mongolian)
                   [], # ukrainian ie
['¶',  ['Ö','¶']],
['·',  ['×','·']],
                   [], # dze (macedonian)
['¸',  ['Ø','¸']],
[],
                   [], # yi (ukrainian)
['¹',  ['Ù','¹']],
                   [], # je (macedonian, serbian)
['º',  ['Ú','º']],
['»',  ['Û','»']],
                   [], # lje (macedonian, serbian)
['¼',  ['Ü','¼']],
['½',  ['Ý','½']],
                   [], # nje (macedonian, serbian)
['¾',  ['Þ','¾']],
                   [], # barred o (mongolian)
['¿',  ['ß','¿']],
['À',  ['à','À']],
['Á',  ['á','Á']],
['Â',  ['â','Â']],
                   [], # tshe (serbian)
                   [], # kje (macedonian)
['Ã',  ['ã','Ã']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['Ä',  ['ä','Ä']],
['Å',  ['å','Å']],
['Æ',  ['æ','Æ']],
['Ç',  ['ç','Ç']],
                   [], # dzhe (macedonian, serbian)
['È',  ['è','È']],
['É',  ['é','É']],
['Ê',  ['ê','Ê']],
['Ë',  ['ë','Ë']],
['Ì',  ['ì','Ì']],
[],
['Í',  ['í','Í']],
['Î',  ['î','Î']],
['Ï',  ['ï','Ï']],
                   [],
                   [],
[],
[]
];

$sortcase = 'Aa';
#$sortcase = 'aA';

$ligatures = [
];

@special = ('?', '!', '.', 'letters', '-', '\'');

do 'make-rules.pl';
