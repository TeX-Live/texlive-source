#!/usr/bin/perl
# (The MIT License)
#
# Copyright (c) 2022 Yegor Bugayenko
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

package bibcop;

use warnings;
use strict;

# Hash of incoming command line arguments.
my %args = map { $_ => 1 } @ARGV;

# If you want to add an extra check, just create a new procedure
# named as "check_*".

# Only these keys are allowed and only these types of items.
my %blessed = (
  'article' => ['doi', 'year', 'title', 'author', 'journal', 'volume', 'number', 'publisher?'],
  'inproceedings' => ['doi', 'booktitle', 'title', 'author', 'year', 'pages?', 'organization?', 'volume?'],
  'book' => ['doi', 'title', 'author', 'year', 'publisher'],
  'misc' => ['title', 'author', 'year', 'eprint?', 'archiveprefix?', 'primaryclass?', 'publisher?', 'organization?', 'doi?'],
);

# Check the presence of mandatory keys.
sub check_mandatory_keys {
  my (%item) = @_;
  my $type = $item{':type'};
  my $mandatory = $blessed{$type};
  foreach my $key (@$mandatory) {
    if ($key =~ /^.*\?$/) {
      next;
    }
    if (not(exists $item{$key})) {
      my $listed = listed_keys(%item);
      return "A mandatory '$key' key for '\@$type' is missing among $listed"
    }
  }
  if (exists $blessed{$type}) {
    my %required = map { $_ => 1 } @$mandatory;
    foreach my $key (keys %item) {
      if ($key =~ /^:/) {
        next;
      }
      if (not(exists $required{$key}) && not(exists $required{$key . '?'})) {
        return "The '$key' key is not suitable for '$type', use only these: (@$mandatory)"
      }
    }
  }
}

# Check that all major words are capitalized.
sub check_capitalization {
  my (%item) = @_;
  my %keys = map { $_ => 1 } qw/title booktitle journal/;
  my %minors = map { $_ => 1 } qw/in of at to by the a an and or as if up via yet nor but off on for into/;
  foreach my $key (keys %item) {
    if (not exists $keys{$key}) {
      next;
    }
    my $value = $item{$key};
    my @words = only_words($value);
    my $pos = 0;
    foreach my $word (@words) {
      if (not $word =~ /^[A-Za-z]/) {
        next;
      }
      $pos = $pos + 1;
      if (exists $minors{$word}) {
        next;
      }
      if (exists $minors{lc($word)} and $pos gt 1) {
        return "All minor words in the '$key' must be lower-cased, while '$word' (no.$pos) is not"
      }
      if ($word =~ /^[a-z].*/) {
        return "All major words in the '$key' must be capitalized, while '$word' (no.$pos) is not"
      }
    }
  }
}

# Check that the 'author' is formatted correctly.
sub check_author {
  my (%item) = @_;
  if (exists $item{'author'} and not $item{'author'} =~ /^\{.+\}$/) {
    my $author = clean_tex($item{'author'});
    if (not $author =~ /^[A-Z][^ ]+(,( [A-Z][^ ]+)+)?( and [A-Z][^ ]+(,( [A-Z][^ ]+)+)?)*( and others)?$/) {
      return "The format of the 'author' is wrong, use something like 'Knuth, Donald E. and Duane, Bibby'"
    }
    if ($author =~ /.*[A-Z]([ ,]|$).*/) {
      return "A shortened name must have a tailing dot, as in 'Knuth, Donald E.'"
    }
  }
}

# Check that titles don't have shortened words with a tailing dot.
sub check_shortenings {
  my (%item) = @_;
  my %keys = map { $_ => 1 } qw/title booktitle journal/;
  foreach my $key (keys %item) {
    if (not exists $keys{$key}) {
      next;
    }
    my $value = $item{$key};
    my @words = only_words($value);
    foreach my $word (@words) {
      if (not $word =~ /^[A-Za-z]/) {
        next;
      }
      if ($word =~ /^.*\.$/) {
        return "Do not shorten the words in the '$key', such as '$word'"
      }
    }
  }
}

# Check the right format of the 'title' and 'booktitle.'
sub check_titles {
  my (%item) = @_;
  my @keys = qw/title booktitle/;
  foreach my $key (@keys) {
    if (not exists($item{$key})) {
      next;
    }
    my $title = $item{$key};
    if (not $title =~ /^\{.+\}$/) {
      return "The '$key' must be wrapped in double curled brackets"
    }
  }
}

# Check that no values have tailing dots.
# Check that there are no spaces before commans.
sub check_typography {
  my (%item) = @_;
  foreach my $key (keys %item) {
    if ($key =~ /^:.*/) {
      next;
    }
    my $value = $item{$key};
    if ($value =~ /.*\.$/ and $key ne 'author') {
      return "The '$key' must not end with a dot"
    }
    if ($value =~ /.* ,.*/) {
      return "In the '$key', do not put a space before the comma"
    }
  }
}

# Check the year is not mentioned in titles.
sub check_year_in_titles {
  my (%item) = @_;
  my @keys = qw/title booktitle journal/;
  foreach my $key (@keys) {
    if (not exists($item{$key})) {
      next;
    }
    my @words = only_words($item{$key});
    foreach my $word (@words) {
      if ($word =~ /^[1-9][0-9]{3}$/) {
        return "The '$key' must not contain the year $word, it is enough to have the 'year' key"
      }
    }
  }
}

# Check the right format of the 'booktitle' in the 'inproceedings' item.
sub check_booktile_of_inproceedings {
  my (%item) = @_;
  my $key = 'inproceedings';
  if ($item{':type'} eq $key) {
    if (exists $item{'booktitle'}) {
      my @words = only_words($item{'booktitle'});
      if (lc($words[0]) ne 'proceedings' or lc($words[1]) ne 'of' or lc($words[2]) ne 'the') {
        return "The '$key' must start with 'Proceedings of the ...'"
      }
    }
  }
}

# Check the right format of the 'doi.'
sub check_doi {
  my (%item) = @_;
  if (exists $item{'doi'}) {
    my $doi = $item{'doi'};
    if (not $item{'doi'} =~ /^[0-9a-zA-Z.]+\/[0-9a-zA-Z._\-]+$/) {
      return "The format of the 'doi' is wrong"
    }
  }
}

# Check the right format of the 'year.'
sub check_year {
  my (%item) = @_;
  if (exists $item{'year'}) {
    my $year = $item{'year'};
    if (not $item{'year'} =~ /^[0-9]{3,4}$/) {
      return "The format of the 'year' is wrong"
    }
  }
}

# Check the right format of the 'month.'
sub check_month {
  my (%item) = @_;
  if (exists $item{'month'}) {
    my $month = $item{'month'};
    if (not $item{'month'} =~ /^[1-9]|10|11|12$/) {
      return "The format of the 'month' is wrong"
    }
  }
}

# Check the right format of the 'volume.'
sub check_volume {
  my (%item) = @_;
  if (exists $item{'volume'}) {
    my $volume = $item{'volume'};
    if (not $item{'volume'} =~ /^[1-9][0-9]*$/) {
      return "The format of the 'volume' is wrong"
    }
  }
}

# Check the right format of the 'number.'
sub check_number {
  my (%item) = @_;
  if (exists $item{'number'}) {
    my $number = $item{'number'};
    if (not $item{'number'} =~ /^[1-9][0-9]*$/) {
      return "The format of the 'number' is wrong"
    }
  }
}

# Check the right format of the 'pages.'
sub check_pages {
  my (%item) = @_;
  if (exists $item{'pages'}) {
    my $pages = $item{'pages'};
    if (not $item{'pages'} =~ /^[1-9][0-9]*--[1-9][0-9]*|[1-9][0-9]*$/) {
      return "The format of the 'pages' is wrong"
    }
    my @parts = split(/--/, $pages);
    if (@parts+0 eq 2) {
      if ($parts[0] gt $parts[1]) {
        return "The 'pages' are in the wrong order"
      }
    }
  }
}

# Check one item.
sub process_item {
  my (%item) = @_;
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
    my $err = $check->(%item);
    if ($err ne '') {
      push(@errors, $err);
    }
  }
  return @errors;
}

# Parse the incoming .bib file and return an array
# of hash-maps, where each one is a bibitem.
sub bibitems {
  my ($bib) = @_;
  my @items;
  my $s = 'top';
  my %item;
  my $acc = '';
  my $key = '';
  my $lineno = 0;
  my $nest = 0;
  my $escape = 0;
  for my $pos (0..length($bib)-1) {
    my $char = substr($bib, $pos, 1);
    if ($char eq ' ') {
      # ignore the white space
    } elsif ($char eq "\n") {
      # ignore the EOL
      $lineno = $lineno + 1;
    } elsif ($char eq '@' and $s eq 'top') {
      %item = ();
      $s = 'start';
      $acc = '';
    } elsif ($char =~ /[a-z]/ and $s eq 'start') {
      # @article
    } elsif ($char eq '{' and $s eq 'start') {
      $item{':type'} = substr($acc, 1);
      $acc = '';
      $s = 'body';
    } elsif ($char =~ /[a-zA-Z]/ and $s eq 'body') {
      $acc = '';
      $s = 'key';
    } elsif ($char =~ /[a-zA-Z0-9_]/ and $s eq 'key') {
      # reading the key
    } elsif ($char =~ /[a-zA-Z0-9]/ and $s eq 'value') {
      # reading the value without quotes or brackets
    } elsif ($char eq ',' and $s eq 'key') {
      $item{':name'} = $acc;
      $s = 'body';
    } elsif ($char eq '=' and $s eq 'key') {
      $key = $acc;
      $s = 'value';
      $acc = '';
    } elsif ($char eq ',' and $s eq 'value') {
      if (not exists $item{lc($key)}) {
        my $tex = substr($acc, 1);
        $tex =~ s/\s//g;
        $item{lc($key)} = $tex;
      }
      $s = 'body';
    } elsif ($char eq '}' and $s eq 'body') {
      push(@items, { %item });
      $s = 'top';
    } elsif ($char eq '}' and $s eq 'value') {
      if (not exists $item{lc($key)}) {
        my $tex = substr($acc, 1);
        $tex =~ s/\s//g;
        $item{lc($key)} = $tex;
      }
      push(@items, { %item });
      $s = 'top';
    } elsif ($char eq '}' and $s eq 'key') {
      $item{':name'} = $acc;
      push(@items, { %item });
      $s = 'top';
    } elsif ($char eq '"' and $s eq 'value') {
      $s = 'quote';
      $acc = '';
    } elsif ($char eq '"' and $s eq 'quote') {
      $item{lc($key)} = substr($acc, 1);
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
          $item{lc($key)} = substr($acc, 1);
          $s = 'value';
        }
      }
      $escape = 0;
    } else {
      print "\\PackageWarningNoLine{bibcop}{It is impossible to parse the .bib file, because I do not know what to do with '$char' at line #$lineno (s=$s)}\n";
      last;
    }
    if ($char eq ' ' and not($s =~ /quote|brackets/)) {
      next;
    }
    $acc = $acc . $char;
  }
  return @items;
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

# Take a bibitem and print all its keys as a comma-separated string.
sub listed_keys {
  my (%item) = @_;
  my @list;
  foreach my $key (keys %item) {
    if ($key =~ /^:.*/) {
      next;
    }
    push(@list, $key);
  }
  my @sorted = sort @list;
  return '(' . join(', ', @sorted) . ')';
}

# Print ERROR message to the console and die.
sub error {
  my ($txt) = @_;
  print $txt . "\n";
  exit 1;
}

# Print DEBUG message to the console.
sub debug {
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
    print "\\PackageWarningNoLine{bibcop}{$txt}\n";
  } else {
    print $txt . "\n";
  }
}

if (@ARGV+0 eq 0 or exists $args{'--help'}) {
  debug("Bibcop is a Style Checker of .bib Files\n" .
    "Usage: bibcop [<options>] <.bib file path>\n" .
    "  --version Print the current version of the tool and exit\n" .
    "  --help    Print this help screen\n" .
    "  --fix     Fix the errors and print a new version of the .bib file to the console\n" .
    "  --latex   Report errors in LaTeX format using \\PackageWarningNoLine command");
} elsif (exists $args{'--version'}) {
  debug('0.0.3');
} else {
  my ($file) = grep { not($_ =~ /^--.*$/) } @ARGV;
  open(my $fh, '<', $file);
  my $bib; { local $/; $bib = <$fh>; }
  my @items = bibitems($bib);
  if (exists $args{'--fix'}) {
    for my $i (0..(@items+0 - 1)) {
      my %item = %{ $items[$i] };
      my $type = $item{':type'};
      if (not exists $blessed{$type}) {
        error("I don't know what to do with \@$type type of bibitem");
      }
      my $keys = $blessed{$item{':type'}};
      my %allowed = map { $_ => 1 } @$keys;
      my @lines;
      foreach my $key (keys %item) {
        if ($key =~ /^:/) {
          next;
        }
        if (not exists $allowed{$key} and not exists $allowed{$key . '?'}) {
          next;
        }
        my $value = clean_tex($item{$key});
        if ($key =~ /title|booktitle|journal/) {
          $value = '{' . $value . '}';
        }
        push(@lines, "  $key = {$value},");
      }
      debug("\@$type\{$item{':name'},");
      my @sorted = sort @lines;
      foreach my $line (@sorted) {
        debug($line);
      }
      debug("}\n");
    }
  } else {
    debug((@items+0) . ' bibitems found in ' . $file);
    for my $i (0..(@items+0 - 1)) {
      my %item = %{ $items[$i] };
      debug("Checking $item{':name'} (#$i)...");
      foreach my $err (process_item(%item)) {
        warning("$err, in the '$item{':name'}' bibitem");
      }
    }
  }
}

1;
