#!/usr/bin/perl

$language = "Macedonian";
$prefix = "mk";
$script = "cyrillic";

$alphabet = [
['á',  ['Á','á']],
['â',  ['Â','â']],
['÷',  ['×','÷']],
['ç',  ['Ç','ç']],
                   [], # ghe with upturn (ukrainian)
['ä',  ['Ä','ä']],
                   [], # dje (serbian)
['²',  ['¢','²']],
['å',  ['Å','å']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['ö',  ['Ö','ö']],
['ú',  ['Ú','ú']],
['µ',  ['¥','µ']],
['é',  ['É','é']],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
                   [], # short i (many)
['¸',  ['¨','¸']],
['ë',  ['Ë','ë']],
['ì',  ['Ì','ì']],
['¹',  ['©','¹']],
['í',  ['Í','í']],
['î',  ['Î','î']],
['º',  ['ª','º']],
['ï',  ['Ï','ï']],
                   [], # barred o (mongolian)
['ð',  ['Ð','ð']],
['ò',  ['Ò','ò']],
['ó',  ['Ó','ó']],
['ô',  ['Ô','ô']],
                   [], # tshe (serbian)
['¼',  ['¬','¼']],
['õ',  ['Õ','õ']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['æ',  ['Æ','æ']],
['è',  ['È','è']],
['ã',  ['Ã','ã']],
['þ',  ['Þ','þ']],
['¿',  ['¯','¿']],
['û',  ['Û','û']],
                   [], # shcha (many)
                   [], # hard sign (bulgarian, russian)
                   [], # yeru (belarusian, russian)
                   [], # soft sign (many)
                   [],
                   [], # e (belarusian, russian)
                   [], # yu (many)
                   [], # ya (many)
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
