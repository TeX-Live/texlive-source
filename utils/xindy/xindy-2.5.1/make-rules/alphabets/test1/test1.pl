#!/usr/bin/perl

# a string describing the language (to be exact, the sorting order)
$language = "Test1";
$prefix = "xx";
$script = "latin";

# Technically speaking, $alphabet is (a reference to) an array of arrays of
# arrays. Sounds complicated? Don't worry! Explanation follows:

# Every line describes one letter of the alphabet (in all its variants).
# The first string is the name of the letter; this appears in the heading of
# letter groups (when defined with the proper markup). Currently the maximum
# number of letters is limited to 95. A future expansion up to 223 letters
# should be no problem.

# Next follows a sequence of arrays, delimited by commas. Each of these arrays
# describes one variant of the letter with different diacritical marks
# (accents). The order of those describes the sorting order if two words
# appear which differ only in the diacritical variant of this letter.
# Currently the maximum supported number of diacritical variants of one letter
# is 93.

# Each of these arrays contains first the lowercase variant of the letter,
# followed by uppercase variant(s). You might wonder: How can there be other
# than one uppercase variant? Consider the letter combination `ch': Uppercase
# variants here are: `Ch' and `CH'. Also, in some character sets there might
# not exist an uppercase variant of a letter, e.g. the letter `ÿ' in the
# ISO-8859-1 character set. In this case we just leave it out.

# The sum of the number of uppercase and lowercase variants of one diacritical
# version of a letter should be 10 or less. (In case of `ch' it is 3:
# `ch', `Ch' and `CH')

# There can be empty arrays [] which are called slots. They are used for
# mixing alphabets of different languages.

# In order to merge several letters into on letter group, give them the same
# letter-group name. (see 'c' and 'ch').

$alphabet = [
['A',    ['a','A'], ['á','Á']],
                   [], # a with breve (romanian)
                   [], # a with circumflex (romanian)
                   [], # a with ogonek (polish)
['B',    ['b','B']],
['C',    ['c','C']],
['C',    ['ch','Ch','CH']],
                   [], # cs (hungarian)
                   [], # c with caron (many)
                   [], # c with acute (croatian, lower sorbian, polish)
                   [], # c with circumflex (esperanto)
                   [], # c with cedilla (albanian, kurdish, turkish)
['D',    ['d','D']],
                   [], # dh (albanian)
                   [], # d+z with caron (croatian)
                   [], # d+z with acute (upper sorbian)
                   [], # d with caron (slovak/large)
                   [], # d with stroke (croatian)
                   [], # eth (icelandic)
['E',    ['e','E'], ['é','É']],
                   [], # e with caron (lower/upper sorbian)
                   [], # e with circumflex (kurdish)
                   [], # e with diaeresis (albanian)
                   [], # e with ogonek (polish)
['F',    ['f','F']],
['G',    ['g','G']],
                   [], # gj (albanian)
                   [], # g with circumflex (esperanto)
                   [], # g with breve (turkish)
['H',    ['h','H']],
                   [], # h with circumflex (esperanto)
                   [], # ch (many)
                   [], # dotless i (turkish)
['I',    ['i','I'], ['í','Í']],
                   [], # i with circumflex (kurdish, romanian)
['J',    ['j','J']],
                   [], # j with circumflex (esperanto)
['K',    ['k','K']],
                   [], # l with stroke (lower/upper sorbian)
['L',    ['l','L']],
                   [], # lj (croatian)
['Ll',   ['ll','Ll','LL']],
                   [], # l with stroke (polish)
                   [], # l with caron (slovak/large)
['M',    ['m','M']],
['N',    ['n','N']],
                   [], # nj (albanian, croatian)
                   [], # ny (hungarian)
['Ñ',    ['ñ','Ñ']],
                   [], # n with acute (lower/upper sorbian, polish)
                   [], # n with tilde (spanish/modern, spanish/traditional)
['O',    ['o','O'], ['ó','Ó']],
                   [], # o with acute (polish, upper sorbian)
                   [], # o with diaeresis (hungarian, turkish)
['P',    ['p','P']],
['Q',    ['q','Q']],
['R',    ['r','R']],
                   [], # rr (albanian)
                   [], # r with caron (czech, slovak/large, upper sorbian)
                   [], # r with acute (lower sorbian)
['S',    ['s','S']],
                   [], # sh (albanian)
                   [], # sz (hungarian)
                   [], # s with caron (many)
                   [], # s with acute (lower sorbian, polish)
                   [], # s with circumflex (esperanto)
                   [], # s with comma below (romanian)
                   [], # s with cedilla (kurdish, turkish)
['T',    ['t','T']],
                   [], # th (albanian)
                   [], # t with caron (slovak/large)
                   [], # t with comma below (romanian)
                   [], # c with acute (upper sorbian)
['U',    ['u','U'], [], ['ú','Ú'], ['ü','Ü']],
                   [], # u with breve (esperanto)
                   [], # u with circumflex (kurdish)
                   [], # u with diaeresis (hungarian, turkish)
['V',    ['v','V']],
['W',    ['w','W']],
['X',    ['x','X']],
                   [], # xh (albanian)
['Y',    ['y','Y']],
['Z',    ['z','Z']],
                   [], # zh (albanian)
                   [], # zs (hungarian)
                   [], # z with caron (many)
                   [], # z with acute (lower sorbian, polish)
                   [], # z with dot above (polish)
                   [], # thorn (icelandic)
                   [], # ligature ae (danish, icelandic, norwegian)
                   [], # o with stroke (danish, norwegian)
                   [], # a with ring above (danish, norwegian, swedish)
                   [], # a with diaeresis (finnish, swedish)
                   []  # o with diaeresis (finnish, swedish)
];

# The next should be pretty easy:
# It means: 'ß' is a ligature which is sorted like the letter sequence `ss'
# but in case two words differs only there, the word with 'ß' comes after the
# one with 'ss' (e.g. Masse, Maße.)

# The same with Ä/ä, only this time with uppercase/lowercase variants.
# The order of the lines in $ligatures does not matter.

$ligatures = [
[['ß'], 'after', [['s','s']]],
[['Ä','ä'], 'before', [['A','e'], ['a','e']]],
[['Ö','ö'], 'before', [['O','e'], ['o','e']]]
];

# `special' are those characters which are normally ignored in the sorting
# process, but e.g. to sort the words "coop" and "co-op" we must also define
# an order here.

@special = ('?', '!', '.', 'letters', '-', '\'', '\\/');

# first lower or upper case?

$sortcase = "Aa";
#$sortcase = "aA";

#@letter_group_names = ('A','B','C','Ch','D','E','F','G','H','I','J','K',
#'L','Ll','M','N','Ñ','O','P','Q','R','S','T','U','V','W','X','Y','Z');

do 'make-rules.pl';
