#!/usr/bin/perl

$language = "Macedonian";
$prefix = "mk";
$script = "cyrillic";

$alphabet = [
['°',  ['Ð','°']],
['±',  ['Ñ','±']],
['²',  ['Ò','²']],
['³',  ['Ó','³']],
                   [], # ghe with upturn (ukrainian)
['´',  ['Ô','´']],
                   [], # dje (serbian)
['£',  ['ó','£']],
['µ',  ['Õ','µ']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['¶',  ['Ö','¶']],
['·',  ['×','·']],
['¥',  ['õ','¥']],
['¸',  ['Ø','¸']],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
                   [], # short i (many)
['¨',  ['ø','¨']],
['º',  ['Ú','º']],
['»',  ['Û','»']],
['©',  ['ù','©']],
['¼',  ['Ü','¼']],
['½',  ['Ý','½']],
['ª',  ['ú','ª']],
['¾',  ['Þ','¾']],
                   [], # barred o (mongolian)
['¿',  ['ß','¿']],
['À',  ['à','À']],
['Á',  ['á','Á']],
['Â',  ['â','Â']],
                   [], # tshe (serbian)
['¬',  ['ü','¬']],
['Ã',  ['ã','Ã']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['Ä',  ['ä','Ä']],
['Å',  ['å','Å']],
['Æ',  ['æ','Æ']],
['Ç',  ['ç','Ç']],
['¯',  ['ÿ','¯']],
['È',  ['è','È']],
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
