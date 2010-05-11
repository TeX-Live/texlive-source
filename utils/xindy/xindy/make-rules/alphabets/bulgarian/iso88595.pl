#!/usr/bin/perl

$language = "Bulgarian";
$prefix = "bg";
$script = "cyrillic";

$alphabet = [
['°',  ['Ð','°']],
['±',  ['Ñ','±']],
['²',  ['Ò','²']],
['³',  ['Ó','³']],
                   [], # ghe with upturn (ukrainian)
['´',  ['Ô','´']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['µ',  ['Õ','µ']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['¶',  ['Ö','¶']],
['·',  ['×','·']],
                   [], # dze (macedonian)
['¸',  ['Ø','¸']],
                   [], # belarusian-ukrainian i
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
                   [], # yeru (belarusian, russian)
['Ì',  ['ì','Ì']],
                   [],
                   [], # e (belarusian, russian)
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
