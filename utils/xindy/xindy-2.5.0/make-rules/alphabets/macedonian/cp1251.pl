#!/usr/bin/perl

$language = "Macedonian";
$prefix = "mk";
$script = "cyrillic";

$alphabet = [
['À',  ['à','À']],
['Á',  ['á','Á']],
['Â',  ['â','Â']],
['Ã',  ['ã','Ã']],
                   [], # ghe with upturn (ukrainian)
['Ä',  ['ä','Ä']],
                   [], # dje (serbian)
['',  ['ƒ','']],
['Å',  ['å','Å']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['Æ',  ['æ','Æ']],
['Ç',  ['ç','Ç']],
['½',  ['¾','½']],
['È',  ['è','È']],
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
['Î',  ['î','Î']],
                   [], # barred o (mongolian)
['Ï',  ['ï','Ï']],
['Ğ',  ['ğ','Ğ']],
['Ñ',  ['ñ','Ñ']],
['Ò',  ['ò','Ò']],
                   [], # tshe (serbian)
['',  ['','']],
['Ó',  ['ó','Ó']],
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
