#!/usr/bin/perl

$language = "Serbian";
$prefix = "sr";
$script = "cyrillic";

$alphabet = [
['À',  ['à','À'] ],
['Á',  ['á','Á']],
['Â',  ['â','Â']],
['Ã',  ['ã','Ã']],
                   [], # ghe with upturn (ukrainian)
['Ä',  ['ä','Ä']],
['€',  ['','€']],
                   [], # gje (macedonian)
['Å',  ['å','Å'] ],
                   [], # io (mongolian)
                   [], # ukrainian ie
['Æ',  ['æ','Æ']],
['Ç',  ['ç','Ç']],
                   [], # dze (macedonian)
['È',  ['è','È'] ],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
                   [], # short i (many)
['£',  ['¼','£']],
['Ê',  ['ê','Ê']],
['Ë',  ['ë','Ë']],
['Š',  ['š','Š']],
['Ì',  ['ì','Ì']],
['Í',  ['í','Í']],
['Œ',  ['œ','Œ']],
['Î',  ['î','Î'] ],
                   [], # barred o (mongolian)
['Ï',  ['ï','Ï']],
['Ğ',  ['ğ','Ğ'] ],
['Ñ',  ['ñ','Ñ']],
['Ò',  ['ò','Ò']],
['',  ['','']],
                   [], # kje (macedonian)
['Ó',  ['ó','Ó'] ],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['Ô',  ['ô','Ô']],
['Õ',  ['õ','Õ']],
['Ö',  ['ö','Ö']],
['×',  ['÷','×']],
['',  ['Ÿ','']],
['Ø',  ['ø','Ø']],
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
