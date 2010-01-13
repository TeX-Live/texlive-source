#!/usr/bin/perl

$language = "Serbian";
$prefix = "sr";
$script = "cyrillic";

$alphabet = [
['á',  ['Á','á'] ],
['â',  ['Â','â']],
['÷',  ['×','÷']],
['ç',  ['Ç','ç']],
                   [], # ghe with upturn (ukrainian)
['ä',  ['Ä','ä']],
['±',  ['¡','±']],
                   [], # gje (macedonian)
['å',  ['Å','å'] ],
                   [], # io (mongolian)
                   [], # ukrainian ie
['ö',  ['Ö','ö']],
['ú',  ['Ú','ú']],
                   [], # dze (macedonian)
['é',  ['É','é'] ],
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
['ï',  ['Ï','ï'] ],
                   [], # barred o (mongolian)
['ð',  ['Ð','ð']],
['ò',  ['Ò','ò'] ],
['ó',  ['Ó','ó']],
['ô',  ['Ô','ô']],
['»',  ['«','»']],
                   [], # kje (macedonian)
['õ',  ['Õ','õ'] ],
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
                   [], # soft sign (ukrainian)
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
