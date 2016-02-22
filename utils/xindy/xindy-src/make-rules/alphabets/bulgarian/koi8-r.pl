#!/usr/bin/perl

$language = "Bulgarian";
$prefix = "bg";
$script = "cyrillic";

$alphabet = [
['á',  ['Á','á']],
['â',  ['Â','â']],
['÷',  ['×','÷']],
['ç',  ['Ç','ç']],
                   [], # ghe with upturn (ukrainian)
['ä',  ['Ä','ä']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['å',  ['Å','å']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['ö',  ['Ö','ö']],
['ú',  ['Ú','ú']],
                   [], # dze (macedonian)
['é',  ['É','é']],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
['ê',  ['Ê','ê']],
                   [], # je (macedonian, serbian)
['ë',  ['Ë','ë']],
['ì',  ['Ì','ì']],
                   [], # lje (macedonian, serbian)
['í',  ['Í','í']],
['î',  ['Î','î']],
                   [], # nje (macedonian, serbian)
['ï',  ['Ï','ï']],
                   [], # barred o (mongolian)
['ð',  ['Ð','ð']],
['ò',  ['Ò','ò']],
['ó',  ['Ó','ó']],
['ô',  ['Ô','ô']],
                   [], # tshe (serbian)
                   [], # kje (macedonian)
['õ',  ['Õ','õ']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['æ',  ['Æ','æ']],
['è',  ['È','è']],
['ã',  ['Ã','ã']],
['þ',  ['Þ','þ']],
                   [], # dzhe (macedonian, serbian)
['û',  ['Û','û']],
['ý',  ['Ý','ý']],
['ÿ',  ['ß','ÿ']],
                   [], # yeru (belarusian, russian)
['ø',  ['Ø','ø']],
                   [],
                   [], # e (belarusian, russian)
['à',  ['À','à']],
['ñ',  ['Ñ','ñ']],
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
