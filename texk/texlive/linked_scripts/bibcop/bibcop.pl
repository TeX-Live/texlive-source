#!/usr/bin/perl
# (The MIT License)
#
# Copyright (c) 2022-2024 Yegor Bugayenko
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# 2024-01-17 0.0.19
package bibcop;

use warnings;
use strict;
use File::Basename;

# Hash of incoming command line arguments.
my %args = map { $_ => 1 } @ARGV;

# If you want to add an extra check, just create a new procedure
# named as "check_*".

# Only these tags are allowed and only these types of entries.
my %blessed = (
  'article' => ['doi', 'year', 'title', 'author', 'journal', 'volume', 'number', 'publisher?', 'pages?'],
  'inproceedings' => ['doi', 'booktitle', 'title', 'author', 'year', 'pages?', 'organization?', 'volume?'],
  'book' => ['title', 'author', 'year', 'publisher', 'doi?'],
  'misc' => ['title', 'author', 'year', 'eprint?', 'archiveprefix?', 'primaryclass?', 'publisher?', 'organization?', 'doi?', 'url?'],
);

# See https://research.arizona.edu/faq/what-do-you-mean-when-you-say-use-title-case-proposalproject-titles
my %minors = map { $_ => 1 } qw/in of at to by the a an and or as if up via yet nor but off on for into/;

# Check the presence of mandatory tags.
sub check_mandatory_tags {
  if (exists $args{'--no:tags'}) {
    return;
  }
  my (%entry) = @_;
  my $type = $entry{':type'};
  my $mandatory = $blessed{$type};
  foreach my $tag (@$mandatory) {
    if ($tag =~ /^.*\?$/) {
      next;
    }
    if (not(exists $entry{$tag})) {
      my $listed = listed_tags(%entry);
      if ($tag eq 'doi' and exists $args{'--no:doi'}) {
        next;
      }
      return "A mandatory '$tag' tag for '\@$type' is missing among $listed"
    }
  }
  if (exists $blessed{$type}) {
    my %required = map { $_ => 1 } @$mandatory;
    foreach my $tag (keys %entry) {
      if ($tag =~ /^:/) {
        next;
      }
      if (not(exists $required{$tag}) && not(exists $required{$tag . '?'})) {
        return "The '$tag' tag is not suitable for '$type', use only these: (@$mandatory)"
      }
    }
  }
}

# Check that all major words are capitalized.
sub check_capitalization {
  if (exists $args{'--no:caps'}) {
    return;
  }
  my (%entry) = @_;
  my %tags = map { $_ => 1 } qw/title booktitle journal publisher organization/;
  foreach my $tag (keys %entry) {
    if (not exists $tags{$tag}) {
      next;
    }
    my $tailed = qr/^.+(:|\?)$/;
    my $value = $entry{$tag};
    my @words = only_words($value);
    my $pos = 0;
    foreach my $word (@words) {
      $pos = $pos + 1;
      if (not $word =~ /^[A-Za-z]/) {
        next;
      }
      if (exists $minors{$word}) {
        if ($pos eq 1) {
          return "The minor word in the '$tag' must be upper-cased since it is the first one"
        }
        if (not $words[$pos - 2] =~ $tailed) {
          next;
        }
        return "The minor word in the '$tag' must be upper-cased, because it follows the colon"
      }
      if (exists $minors{lc($word)}) {
        if ($pos eq 1) {
          next;
        }
        if ($words[$pos - 2] =~ $tailed) {
          next;
        }
        return "All minor words in the '$tag' must be lower-cased, while @{[as_position($pos)]} word '$word' is not"
      }
      if ($word =~ /^[a-z].*/) {
        return "All major words in the '$tag' must be capitalized, while @{[as_position($pos)]} word '$word' is not"
      }
    }
  }
}

# Check that the 'author' is formatted correctly.
sub check_author {
  my (%entry) = @_;
  if (not exists $entry{'author'}) {
    return;
  }
  if ($entry{'author'} =~ /^\{.+\}$/) {
    return;
  }
  my $author = clean_tex($entry{'author'});
  my @authors = split(/\s+and\s+/, $author);
  my $pos = 0;
  for my $a (@authors) {
    $pos += 1;
    if ($a eq 'others') {
      next;
    }
    if (index($a, ' ') != -1 and index($a, ',') == -1) {
      return "The last name should go first, all other names must follow, after a comma in @{[as_position($pos)]} 'author', as in 'Knuth, Donald E.'";
    }
    my $npos = 0;
    for my $name (split(/[ ,]+/, $a)) {
      $npos += 1;
      if (index($name, '{') != -1) {
        next;
      }
      if ($name =~ /^[A-Z]\.$/) {
        next;
      }
      if ($name =~ /^[A-Z][^.]+$/) {
        next
      }
      if ($name =~ /^[A-Z]$/) {
        return "A shortened name must have a tailing dot in @{[as_position($pos)]} 'author', as in 'Knuth, Donald E.'";
      }
      return "In @{[as_position($pos)]} 'author' @{[as_position($npos)]} name looks suspicious ($name), use something like 'Knuth, Donald E. and Duane, Bibby'";
    }
  }
}

# Check that titles don't have shortened words with a tailing dot.
sub check_shortenings {
  my (%entry) = @_;
  my %tags = map { $_ => 1 } qw/title booktitle journal/;
  foreach my $tag (keys %entry) {
    if (not exists $tags{$tag}) {
      next;
    }
    my $value = $entry{$tag};
    my @words = only_words($value);
    foreach my $word (@words) {
      if (not $word =~ /^[A-Za-z]/) {
        next;
      }
      if ($word =~ /^.*\.$/) {
        return "Do not shorten the words in the '$tag', such as '$word'"
      }
    }
  }
}

# Check the right format of the 'title' and 'booktitle.'
sub check_wrapping {
  if (exists $args{'--no:wraps'}) {
    return;
  }
  my (%entry) = @_;
  my @tags = qw/title booktitle/;
  foreach my $tag (@tags) {
    if (not exists($entry{$tag})) {
      next;
    }
    my $title = $entry{$tag};
    if (not $title =~ /^\{.+\}$/) {
      return "The '$tag' must be wrapped in double curled brackets"
    }
  }
}

# Check the right format of the tags for arXiv.
# See https://arxiv.org/help/arxiv_identifier
sub check_arXiv {
  my (%entry) = @_;
  if (exists($entry{'archiveprefix'})) {
    if (not exists $entry{'eprint'}) {
      return "The 'eprint' is mandatory when 'archiveprefix' is there"
    }
    if (not $entry{'eprint'} =~ /^[0-9]{4}\.[0-9]{4,5}(v[0-9]+)?$/) {
      return "The 'eprint' must have two integers separated by a dot"
    }
    my $eprint = $entry{'eprint'};
    my ($head, $tail) = split(/\./, $eprint);
    my $year = substr($head, 0, 2);
    my $month = substr($head, 2);
    if ($month > 12) {
      return "The month '$month' of the 'eprint' is wrong, it can't be bigger than 12"
    }
    if (not exists $entry{'primaryclass'}) {
      return "The 'primaryclass' is mandatory when 'archiveprefix' is there"
    }
    if (not $entry{'primaryclass'} =~ /^[a-z]{2,}\.[A-Z]{2}$/) {
      return "The 'primaryclass' must have two parts, like 'cs.PL'"
    }
  }
}

# Check that organization is not mentioned in the booktitle.
sub check_org_in_booktitle {
  if (exists $args{'--no:org'}) {
    return;
  }
  my (%entry) = @_;
  my @orgs = ( 'ACM', 'IEEE' );
  if (exists($entry{'booktitle'})) {
    my $title = $entry{'booktitle'};
    foreach my $o (@orgs) {
      if ($title =~ /^.*\Q$o\E.*$/) {
        return "The '$o' organization must not be mentioned in the booktitle, use 'publisher' tag instead"
      }
    }
    if ($title =~ /^.*(ACM|IEEE).*$/) {
      return "Don't mention the"
    }
  }
}

# Check that no values have tailing dots.
# Check that there are no spaces before commans.
sub check_typography {
  my (%entry) = @_;
  my %symbols = (
    '.' => 'dot',
    ',' => 'comma',
    ';' => 'semi-colon',
    ':' => 'colon',
    '!' => 'exclamation mark',
    '?' => 'question mark',
    '-' => 'dash',
    '--' => 'double dash',
    '---' => 'triple dash',
    '(' => 'opening bracket',
    ')' => 'closing bracket',
    '[' => 'opening square bracket',
    ']' => 'closing square bracket',
  );
  my @spaces_around = ( '---' );
  my @no_spaces_around = ( '--', '-' );
  my @no_space_before = ( '.', ',', ';', ':', '?', '!', ')', ']' );
  my @no_space_after = ( '(', '[' );
  my @space_before = ( '(', '[' );
  my @space_after = ( ')', ']' );
  my @bad_tails = ( '.', ',', ';', ':', '-' );
  foreach my $tag (keys %entry) {
    if ($tag =~ /^:.*/) {
      next;
    }
    if ($tag eq 'doi') {
      next;
    }
    my $value = $entry{$tag};
    foreach my $s (@bad_tails) {
      if ($s eq '.' and $tag eq 'author') {
        next;
      }
      if ($value =~ /^.*\Q$s\E$/) {
        return "The '$tag' must not end with a $symbols{$s}"
      }
    }
    foreach my $s (@no_space_before) {
      if ($value =~ /^.*\s\Q$s\E.*$/) {
        return "In the '$tag', do not put a space before the $symbols{$s}"
      }
    }
    foreach my $s (@no_space_after) {
      if ($value =~ /^.*\Q$s\E\s.*$/) {
        return "In the '$tag', do not put a space after the $symbols{$s}"
      }
    }
    foreach my $s (@space_before) {
      if ($value =~ /^.*[^\{\s\\]\Q$s\E.*$/) {
        return "In the '$tag', put a space before the $symbols{$s}"
      }
    }
    foreach my $s (@space_after) {
      my $p = join('', @no_space_before);
      if ($value =~ /^.*\Q$s\E[^\}\s\Q$p\E].*$/) {
        return "In the '$tag', put a space after the $symbols{$s}"
      }
    }
    foreach my $s (@spaces_around) {
      if ($value =~ /^.*[^\s]\Q$s\E.*$/ or $value =~ /^.*\Q$s\E[^\s].*$/) {
        return "In the '$tag', put spaces around the $symbols{$s}"
      }
    }
    foreach my $s (@no_spaces_around) {
      if ($value =~ /^.*\s\Q$s\E\s.*$/) {
        return "In the '$tag', don't put spaces around the $symbols{$s}"
      }
    }
  }
}

# Check that the type is small letters.
sub check_type_capitalization {
  my (%entry) = @_;
  my $type = $entry{':type'};
  if (not $type =~ /^[a-z]+$/) {
    return "The type '$type' must be lower-cased"
  }
}

# Check that no values have non-ASCII symbols.
sub check_ascii {
  my (%entry) = @_;
  foreach my $tag (keys %entry) {
    if ($tag =~ /^:.*/) {
      next;
    }
    my $value = $entry{$tag};
    for my $pos (0..length($value)-1) {
      my $char = substr($value, $pos, 1);
      my $ord = ord($char);
      if ($ord == 9 || $ord == 10 || $ord == 13) {
        next;
      }
      if ($ord < 20) {
        return "In the '$tag', don't use control symbol '0x" . (sprintf '%04x', $ord) . "'"
      }
      if ($ord > 0x7f) {
        return "In the '$tag', don't use Unicode symbol '0x" . (sprintf '%04x', $ord) . "'"
      }
    }
  }
}

# Check the year is not mentioned in titles.
sub check_year_in_titles {
  my (%entry) = @_;
  my @tags = qw/title booktitle journal/;
  foreach my $tag (@tags) {
    if (not exists($entry{$tag})) {
      next;
    }
    my @words = only_words($entry{$tag});
    foreach my $word (@words) {
      if ($word =~ /^[1-9][0-9]{3}$/) {
        return "The '$tag' must not contain the year $word, it is enough to have the 'year' tag"
      }
    }
  }
}

# Check the right format of the 'booktitle' in the 'inproceedings' entry.
sub check_booktile_of_inproceedings {
  if (exists $args{'--no:inproc'}) {
    return;
  }
  my (%entry) = @_;
  my $tag = 'inproceedings';
  if ($entry{':type'} eq $tag) {
    if (exists $entry{'booktitle'}) {
      my @words = only_words($entry{'booktitle'});
      if (lc($words[0]) ne 'proceedings' or lc($words[1]) ne 'of' or lc($words[2]) ne 'the') {
        return "The '$tag' must start with 'Proceedings of the ...'"
      }
    }
  }
}

# Check the right format of the 'doi.'
sub check_doi {
  if (exists $args{'--no:doi'}) {
    return;
  }
  my (%entry) = @_;
  if (exists $entry{'doi'}) {
    my $doi = $entry{'doi'};
    if (not $doi =~ /^[0-9a-zA-Z.]+\/[0-9a-zA-Z._\-)(><:;]+$/) {
      return "The format of the 'doi' is wrong"
    }
  }
}

# Check the right format of the 'year.'
sub check_year {
  my (%entry) = @_;
  if (exists $entry{'year'}) {
    my $year = $entry{'year'};
    if ($year =~ /^\{.+\}$/) {
      return;
    }
    if (not $year =~ /^[0-9]+$/) {
      return "The format of the 'year' is wrong, may only contain numbers or must be wrapped in curly braces"
    }
    if (not $year =~ /^[0-9]{4}$/) {
      return "Exactly four digits must be present in the 'year', or it must be wrapped in curly braces"
    }
  }
}

# Check the right format of the 'month.'
sub check_month {
  my (%entry) = @_;
  if (exists $entry{'month'}) {
    my $month = $entry{'month'};
    if (not $month =~ /^[1-9]|10|11|12$/) {
      return "The format of the 'month' is wrong"
    }
  }
}

# Check the right format of the 'volume.'
sub check_volume {
  my (%entry) = @_;
  if (exists $entry{'volume'}) {
    my $volume = $entry{'volume'};
    if (not $volume =~ /^[1-9][0-9]*$/) {
      return "The format of the 'volume' is wrong"
    }
  }
}

# Check the right format of the 'number.'
sub check_number {
  my (%entry) = @_;
  if (exists $entry{'number'}) {
    my $number = $entry{'number'};
    if (not $number =~ /^[1-9][0-9]*$/) {
      return "The format of the 'number' is wrong"
    }
  }
}

# Check the right format of the 'pages.'
sub check_pages {
  my (%entry) = @_;
  if (exists $entry{'pages'}) {
    my $pages = $entry{'pages'};
    if (not $pages =~ /^([1-9][0-9]*--[1-9][0-9]*|[1-9][0-9]*)$/) {
      return "The format of the 'pages' is wrong"
    }
    my @parts = split(/--/, $pages);
    if (@parts+0 eq 2) {
      if ($parts[0] eq $parts[1]) {
        return "The 'pages' mentions the same page twice, just use it once"
      }
      if ($parts[0] > $parts[1]) {
        return "The 'pages' are in the wrong order, since $parts[0] is greater than $parts[1]"
      }
    }
  }
}

# Check unescaped symbols.
sub check_unescaped_symbols {
  my (%entry) = @_;
  my @symbols = ( '&' );
  foreach my $tag (keys %entry) {
    if ($tag =~ /^:.*/) {
      next;
    }
    my $value = $entry{$tag};
    foreach my $s (@symbols) {
      if ($value =~ /^(.*[^\\{]|)\Q$s\E.*$/) {
        return "The '$tag' contains a dangerous unescaped symbol (wrap it in curled brackets if you really need it)"
      }
    }
  }
}

# Check one entry.
sub process_entry {
  my (%entry) = @_;
  my @checks;
  foreach my $entry (keys %bibcop::) {
    if ($entry =~ /^check_/) {
      push(@checks, $entry);
    }
  }
  my @sorted = sort @checks;
  my @errors;
  foreach my $check (@sorted) {
    no strict 'refs';
    my $err = $check->(%entry);
    if (defined $err and $err ne '') {
      push(@errors, $err);
    }
  }
  return @errors;
}

# Fix one entry.
sub entry_fix {
  my (%entry) = @_;
  if (not exists $entry{':type'}) {
    error("I don't know what to do with an entry without a type");
  }
  my $type = $entry{':type'};
  if (not exists $blessed{$type}) {
    error("I don't know what to do with \@$type type of BibTeX entry");
  }
  if (not exists $entry{':name'}) {
    error("I don't know what to do with an entry without a name");
  }
  my $tags = $blessed{$type};
  my %allowed = map { $_ => 1 } @$tags;
  my @lines;
  foreach my $tag (keys %entry) {
    if ($tag =~ /^:/) {
      next;
    }
    if (not exists $allowed{$tag} and not exists $allowed{$tag . '?'}) {
      next;
    }
    my $value = clean_tex($entry{$tag});
    my $fixer = "fix_$tag";
    my $fixed = $value;
    if (defined &{$fixer}) {
      no strict 'refs';
      $value = $fixer->($value);
    }
    if ($tag =~ /title|booktitle|journal/) {
      $value = '{' . $value . '}';
    }
    if (not $value eq '') {
      push(@lines, "  $tag = {$value},");
    }
  }
  my $fixed = "\@$type\{$entry{':name'},\n";
  my @sorted = sort @lines;
  foreach my $line (@sorted) {
    $fixed = $fixed . $line . "\n";
  }
  $fixed = $fixed . "}\n\n";
  return $fixed;
}

sub fix_author {
  my ($value) = @_;
  my @authors = split(/\s+and\s+/, $value);
  foreach my $author (@authors) {
    $author =~ s/^\s+|\s+$//g;
    $author =~ s/ ([A-Z])($| )/ $1.$2/g;
  }
  return join(' and ', @authors);
}

sub fix_number {
  my ($value) = @_;
  $value =~ s/^0+//g;
  return $value;
}

sub fix_capitalization {
  my ($value) = @_;
  my @words = split(/\s+/, $value);
  my $pos = 0;
  foreach my $word (@words) {
    $pos += 1;
    if (not $word =~ /^[A-Za-z]/) {
      next;
    }
    if (exists $minors{$word}) {
      next;
    }
    if (exists $minors{lc($word)} and $pos gt 1) {
      $word = lc($word);
      next;
    }
    if ($word =~ /^[a-z].*/) {
      $word =~ s/^([a-z])/\U$1/g;
    }
    if (index($word, '-') != -1) {
      $word =~ s/-([a-z])/-\U$1/g;
    }
  }
  return join(' ', @words);
}

sub fix_title {
  my ($value) = @_;
  $value = fix_capitalization($value);
  return $value;
}

sub fix_pages {
  my ($value) = @_;
  if ($value =~ /^[1-9][0-9]*$/) {
    return $value;
  }
  if ($value eq '') {
    return $value;
  }
  my ($left, $right) = split(/---|--|-|–|—|\s/, $value);
  $left //= $right;
  if ($left eq '') {
    $left = $right;
  }
  $right //= $left;
  if ($right eq '') {
    $right = $left;
  }
  $left =~ s/^0+//g;
  $right =~ s/^0+//g;
  if ($left !~ /^[0-9]*$/ or $right !~ /^[0-9]*$/) {
    return $value;
  }
  if ($left + 0 gt $right + 0) {
    my $tmp = $left;
    $left = $right;
    $right = $tmp;
  }
  if ($left eq $right) {
    return $left;
  }
  return $left . '--' . $right;
}

sub fix_booktitle {
  my ($value) = @_;
  $value = fix_capitalization($value);
  if (index($value, 'Proceedings ') != 0) {
    $value = 'Proceedings of the ' . $value;
  }
  return $value;
}

sub fix_journal {
  my ($value) = @_;
  $value = fix_capitalization($value);
  return $value;
}

sub fix_publisher {
  my ($value) = @_;
  $value = fix_capitalization($value);
  return $value;
}

sub fix_organization {
  my ($value) = @_;
  $value = fix_capitalization($value);
  return $value;
}

# Parse the incoming .bib file and return an array
# of hash-maps, where each one is a bibentry.
sub entries {
  my ($bib) = @_;
  my @entries;
  my $s = 'top';
  my %entry;
  my $acc = '';
  my $tag = '';
  my $interrupted = ''; # where the comment interrupted the proceeding (state name)
  my $lineno = 0;
  my $nest = 0;
  my $escape = 0;
  for my $pos (0..length($bib)-1) {
    my $char = substr($bib, $pos, 1);
    if ($char eq ' ') {
      # ignore the white space
    } elsif ($char eq '%' and not($s eq 'quote')) {
      if ($pos eq 0 or substr($bib, $pos - 1, 1) ne '\\') {
        $interrupted = $s;
        $s = 'comment';
      }
    } elsif ($char eq "\n") {
      # ignore the EOL
      $lineno = $lineno + 1;
      if ($s eq 'comment') {
        $s = $interrupted;
      }
    } elsif ($s eq 'comment') {
      # ignore the comment
    } elsif ($s eq 'top') {
      if ($char eq '@') {
        %entry = ();
        $s = 'start';
        $acc = '';
      } else {
        warning("Each BibTeX entry must start with '\@', what is '$char' at line no.$lineno?");
        last;
      }
    } elsif ($char =~ /[a-zA-Z]/ and $s eq 'start') {
      # @article
    } elsif ($char eq '{' and $s eq 'start') {
      $entry{':type'} = substr($acc, 1);
      $acc = '';
      $s = 'body';
    } elsif ($char =~ /[a-zA-Z0-9]/ and $s eq 'body') {
      $acc = '';
      $s = 'tag';
    } elsif ($char =~ /[a-zA-Z0-9_\.\-\/:]/ and $s eq 'tag') {
      # reading the tag
    } elsif ($char =~ /[a-zA-Z0-9]/ and $s eq 'value') {
      # reading the value without quotes or brackets
    } elsif ($char eq ',' and $s eq 'tag') {
      $entry{':name'} = $acc;
      $s = 'body';
    } elsif ($char eq '=' and $s eq 'tag') {
      my $t = lc($acc);
      if (exists $entry{$t}) {
        warning("The tag '$t' is seen more than once at line no.$lineno");
      }
      $tag = $t;
      $s = 'value';
      $acc = '';
    } elsif ($char eq ',' and $s eq 'value') {
      if (not exists $entry{$tag}) {
        my $tex = substr($acc, 1);
        $tex =~ s/\s//g;
        $entry{$tag} = $tex;
      }
      $s = 'body';
    } elsif ($char eq '}' and $s eq 'body') {
      push(@entries, { %entry });
      $s = 'top';
    } elsif ($char eq '}' and $s eq 'value') {
      if (not exists $entry{$tag}) {
        my $tex = substr($acc, 1);
        $tex =~ s/\s//g;
        $entry{$tag} = $tex;
      }
      push(@entries, { %entry });
      $s = 'top';
    } elsif ($char eq '}' and $s eq 'tag') {
      $entry{':name'} = $acc;
      push(@entries, { %entry });
      $s = 'top';
    } elsif ($char eq '"' and $s eq 'value') {
      $s = 'quote';
      $acc = '';
    } elsif ($char eq '"' and $s eq 'quote') {
      $entry{$tag} = substr($acc, 1);
      $s = 'value';
    } elsif ($s eq 'quote') {
      # nothing
    } elsif ($char eq '{' and $s eq 'value') {
      $nest = 1;
      $s = 'brackets';
      $acc = '';
    } elsif ($s eq 'brackets') {
      if ($char eq '\\') {
        $escape = 1;
      } elsif ($char eq '{' and $escape ne 1) {
        $nest = $nest + 1;
      } elsif ($char eq '}' and $escape ne 1) {
        $nest = $nest - 1;
        if ($nest eq 0) {
          $entry{$tag} = substr($acc, 1);
          $s = 'value';
        }
      }
      $escape = 0;
    } else {
      warning("It is impossible to parse the .bib file, because I do not know what to do with '$char' at line no.$lineno (s=$s)");
      last;
    }
    if ($char eq ' ' and not($s =~ /quote|brackets/)) {
      next;
    }
    $acc = $acc . $char;
  }
  return @entries;
}

# Takes the text and returns only list of words seen there.
sub only_words {
  my ($tex) = @_;
  return split(/[ \-]/, clean_tex($tex));
}

# Take a TeX string and return a cleaner one, without redundant spaces, brackets, etc.
sub clean_tex {
  my ($tex) = @_;
  $tex =~ s/\s+/ /g;
  $tex =~ s/^\s+//g;
  $tex =~ s/\s+$//g;
  while ($tex =~ s/^\{(.+)\}$/$1/g) {};
  return $tex;
}

# Turn a number into a position, like 1 -> 1st, 2 -> 2nd, 3 -> 3rd, 4 -> 4th, and so on.
sub as_position {
  my ($i) = @_;
  my $txt;
  if ($i == 1) {
    $txt = '1st';
  } elsif ($i == 2) {
    $txt = '2nd';
  } elsif ($i == 3) {
    $txt = '3rd';
  } else {
    $txt = "${i}th";
  }
  return "the $txt";
}

# Take a bibentry and print all its tags as a comma-separated string.
sub listed_tags {
  my (%entry) = @_;
  my @list;
  foreach my $tag (keys %entry) {
    if ($tag =~ /^:.*/) {
      next;
    }
    push(@list, $tag);
  }
  my @sorted = sort @list;
  return '(' . join(', ', @sorted) . ')';
}

# Make sure the text can safely be rendered in TeX.
sub escape_tex {
  my ($tex) = @_;
  $tex =~ s/[^a-zA-Z0-9-.,+)(:;@ '"]/?/g;
  return $tex;
}

# Print ERROR message to the console and die.
sub error {
  my ($txt) = @_;
  if (exists $args{'--latex'}) {
    print "\\PackageError{bibcop}{" . escape_tex($txt). "}{}\n";
  } else {
    print STDERR $txt . "\n";
  }
  fail();
}

# Print DEBUG message to the console.
sub debug {
  my ($txt) = @_;
  if (exists $args{'--verbose'}) {
    if (exists $args{'--latex'}) {
      print "\\message{bibcop: " . escape_tex($txt) . "^^J}\n";
    } else {
      print $txt . "\n";
    }
  }
}

# Print INFO message to the console.
sub info {
  my ($txt) = @_;
  if (exists $args{'--latex'}) {
    print '% ';
  }
  print $txt . "\n";
}

# Print INFO message to the console.
sub warning {
  my ($txt) = @_;
  if (exists $args{'--latex'}) {
    print "\\PackageWarningNoLine{bibcop}{" . escape_tex($txt) . "}\n";
  } else {
    print $txt . "\n";
  }
}

sub fail {
  if (exists $args{'--latex'}) {
    exit(0);
  } else {
    exit(1);
  }
}

my $script = basename($0);
$script =~ s/\.pl$//g;
if (not $script eq 'bibcop') {
  goto END;
}

if (@ARGV+0 eq 0 or exists $args{'--help'} or exists $args{'-?'}) {
  info("Bibcop is a Style Checker of BibTeX Files\n\n" .
    "Usage:\n" .
    "  bibcop [<options>] <.bib file path>\n\n" .
    "Options:\n" .
    "  -v, --version   Print the current version of the tool and exit\n" .
    "  -?, --help      Print this help screen\n" .
    "      --fix       Fix the errors and print a new version of the .bib file to the console\n" .
    "  -i, --in-place  When used together with --fix, modifies the file in place, doesn't print it to the console\n" .
    "      --verbose   Print supplementary debugging information\n" .
    "      --no:XXX    Disable one of the following checks (e.g. --no:wraps):\n" .
    "                    tags    Only some tags are allowed, while some of them are mandatory\n" .
    "                    caps    All major words in titles and booktitles must be capitalized\n" .
    "                    wraps   Double curly braces are required around titles and booktitles\n" .
    "                    doi     The presence of the 'doi' tag is mandatory in all entries\n" .
    "                    inproc  The booktitle of \@inproceedings must start with 'Proceedings of the'\n" .
    "                    org     The booktitle may not mention ACM or IEEE\n" .
    "      --latex     Report errors in LaTeX format using \\PackageWarningNoLine command\n\n" .
    "If any issues, report to GitHub: https://github.com/yegor256/bibcop");
} elsif (exists $args{'--version'} or exists $args{'-v'}) {
  info('0.0.19 2024-01-17');
} else {
  my ($file) = grep { not($_ =~ /^-.*$/) } @ARGV;
  if (not $file) {
    error('File name must be specified');
  }
  open(my $fh, '<', $file) or error('Cannot open file: ' . $file);
  my $bib; { local $/; $bib = <$fh>; }
  my @entries = entries($bib);
  if (exists $args{'--fix'}) {
    my $fixed = '';
    for my $i (0..(@entries+0 - 1)) {
      my %entry = %{ $entries[$i] };
      $fixed = $fixed . entry_fix(%entry);
    }
    if (exists $args{'-i'} or exists $args{'--in-place'}) {
      open(my $out, '>', $file) or error('Cannot open file for writing: ' . $file);
      print $out $fixed;
      close($out);
    } else {
      info($fixed);
    }
  } else {
    debug((@entries+0) . ' entries found in ' . $file);
    my $found = 0;
    for my $i (0..(@entries+0 - 1)) {
      my %entry = %{ $entries[$i] };
      debug("Checking $entry{':name'} (no.$i)...");
      foreach my $err (process_entry(%entry)) {
        warning("$err, in the '$entry{':name'}' entry");
        $found += 1;
      }
    }
    if ($found gt 0) {
      debug("$found problem(s) found");
      fail();
    }
  }
}

END:

# In order to finish it with success:
1;
