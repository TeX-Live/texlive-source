#!/usr/bin/perl

$language = "Ukrainian";
$prefix = "uk";
$script = "cyrillic";

$alphabet = [
['À',  ['à','À']],
['Á',  ['á','Á']],
['Â',  ['â','Â']],
['Ã',  ['ã','Ã']],
['¥',  ['´','¥']],
['Ä',  ['ä','Ä']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['Å',  ['å','Å'],['¸','¨']],
                   [], # io (mongolian)
['ª',  ['º','ª']],
['Æ',  ['æ','Æ']],
['Ç',  ['ç','Ç']],
                   [], # dze (macedonian)
['È',  ['è','È']],
['²',  ['³','²']],
['¯',  ['¿','¯']],
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
['Û',  ['û','Û']],
                   [], # soft sign (many)
                   [],
['Ý',  ['ý','Ý']],
['Þ',  ['þ','Þ']],
['ß',  ['ÿ','ß']],
['Ü',  ['ü','Ü']],
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
