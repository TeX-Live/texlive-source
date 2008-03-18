#!/usr/bin/perl

$language = "Bulgarian";
$prefix = "bg";
$script = "cyrillic";

$alphabet = [
['À',  ['à','À']],
['Á',  ['á','Á']],
['Â',  ['â','Â']],
['Ã',  ['ã','Ã']],
                   [], # ghe with upturn (ukrainian)
['Ä',  ['ä','Ä']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['Å',  ['å','Å']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['Æ',  ['æ','Æ']],
['Ç',  ['ç','Ç']],
                   [], # dze (macedonian)
['È',  ['è','È']],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
['É',  ['é','É']],
                   [], # je (macedonian, serbian)
['Ê',  ['ê','Ê']],
['Ë',  ['ë','Ë']],
                   [], # lje (macedonian, serbian)
['Ì',  ['ì','Ì']],
['Í',  ['í','Í']],
                   [], # nje (macedonian, serbian)
['Î',  ['î','Î']],
                   [], # barred o (mongolian)
['Ï',  ['ï','Ï']],
['Ð',  ['ð','Ð']],
['Ñ',  ['ñ','Ñ']],
['Ò',  ['ò','Ò']],
                   [], # tshe (serbian)
                   [], # kje (macedonian)
['Ó',  ['ó','Ó']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['Ô',  ['ô','Ô']],
['Õ',  ['õ','Õ']],
['Ö',  ['ö','Ö']],
['×',  ['÷','×']],
                   [], # dzhe (macedonian, serbian)
['Ø',  ['ø','Ø']],
['Ù',  ['ù','Ù']],
['Ú',  ['ú','Ú']],
                   [], # yeru (belarusian, russian)
['Ü',  ['ü','Ü']],
                   [],
                   [], # e (belarusian, russian)
['Þ',  ['þ','Þ']],
['ß',  ['ÿ','ß']],
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
