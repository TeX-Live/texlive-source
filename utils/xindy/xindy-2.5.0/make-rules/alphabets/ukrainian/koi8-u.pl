#!/usr/bin/perl

$language = "Ukrainian";
$prefix = "uk";
$script = "cyrillic";

$alphabet = [
['á',  ['Á','á']],
['â',  ['Â','â']],
['÷',  ['×','÷']],
['ç',  ['Ç','ç']],
['½',  ['­','½']],
['ä',  ['Ä','ä']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['å',  ['Å','å'],['£','³']],
                   [], # io (mongolian)
['´',  ['¤','´']],
['ö',  ['Ö','ö']],
['ú',  ['Ú','ú']],
                   [], # dze (macedonian)
['é',  ['É','é']],
['¶',  ['¦','¶']],
['·',  ['§','·']],
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
['ù',  ['Ù','ù']],
                   [], # soft sign (many)
                   [],
['ü',  ['Ü','ü']],
['à',  ['À','à']],
['ñ',  ['Ñ','ñ']],
['ø',  ['Ø','ø']],
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
