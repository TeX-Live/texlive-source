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

# 2024-01-11 0.18.0
package eolang;

use warnings;
use strict;
use File::Basename;

# Hash of incoming command line arguments.
my %args = map { $_ => 1 } @ARGV;

# Read file content.
sub readfile {
  my ($path) = @_;
  open(my $h, '<', $path) or die('Cannot open file: ' . $path);
  my $content; { local $/; $content = <$h>; }
  return $content;
}

# Save content to file.
sub savefile {
  my ($path, $content) = @_;
  open(my $f, '>', $path) or error('Cannot open file for writing: ' . $path);
  print $f $content;
  close($f);
}

# Print INFO message to the console.
sub info {
  my ($txt) = @_;
  print $txt . "\n";
}

# Print DEBUG message to the console.
sub debug {
  my ($txt) = @_;
  if (exists $args{'--verbose'}) {
    print $txt . "\n";
  }
}

# Print ERROR message to the console.
sub error {
  my ($txt) = @_;
  print STDERR $txt . "\n";
}

if (@ARGV+0 eq 0 or exists $args{'--help'} or exists $args{'-?'}) {
  info("This script helps embedding \\phiquation and \\phiq into .tex document\n\n" .
    "Usage:\n" .
    "  eolang [<options>] <.tex file path>\n\n" .
    "Options:\n" .
    "  -v, --version        Print the current version of the tool and exit\n" .
    "  -?, --help           Print this help screen\n" .
    "      --verbose        Print all possible debugging information\n" .
    "      --tmpdir=path    Temp directory with .tex files ('_eolang' by default)\n\n" .
    "If any issues, report to GitHub: https://github.com/yegor256/bibcop");
} elsif (exists $args{'--version'} or exists $args{'-v'}) {
  info('0.18.0 2024-01-11');
} else {
  my ($src, $target) = grep { not($_ =~ /^-.*$/) } @ARGV;
  if (not $src) {
    error('Source file name must be specified');
    exit(1);
  }
  debug('Source: ' . $src);
  my $job = basename($src);
  $job =~ s/\.[^.]+$//;
  debug('Job name: ' . $job);
  my $tex = readfile($src);
  my $tmpdir = dirname($src) . '/_eolang/' . $job;
  debug('EO tmpdir: ' . $tmpdir);
  foreach my $f (glob($tmpdir . '/*-phiq.tex')) {
    my $id = basename($f);
    $id =~ s/\.[^.]+$//;
    $id =~ s/-phiq$//;
    my $phiq = readfile($f);
    $phiq =~ s/^\s+|\s+$//g;
    my $search = quotemeta($phiq);
    $search =~ s/(\\\\[a-zA-Z]+)\\ /$1\\ ?/g;
    $search = '\\\\phiq\\s*\\{\\s*' . $search . '\\s*\\}|\\$\\s*' . $search . '\\s*\\$';
    my $re = '\input{' . $tmpdir . '/' . $id . '-phiq-post.tex' . "}";
    my $count = 0;
    while (1) {
      my $applied = $tex =~ s/${search}/${re}/g;
      if (!$applied) {
        if ($count eq 0) {
          debug("Neither \\phiq{$phiq} nor \$$phiq\$ found, suggested by $f");
        }
        last;
      }
      debug('\\phiq ' . $id . '( ' . $phiq . ' ) -> ' . $re);
      $count += 1;
    }
  }
  my @kinds = ('sodg', 'phiquation', 'phiquation*');
  for my $kind (@kinds) {
    my $k = $kind;
    $k =~ s/\*$//;
    foreach my $f (glob($tmpdir . '/*-' . $k . '.tex')) {
      my $id = basename($f);
      $id =~ s/\.[^.]+$//;
      $id =~ s/-${k}$//;
      my $search = quotemeta(readfile($f));
      $search = '\\\\begin\\s*\\{\\s*' . quotemeta($kind) . '\\s*\\}\\n' . $search . '\\\\end\\s*\\{\\s*' . quotemeta($kind) . '\\s*\\}\\n';
      my $re = '\input{' . $tmpdir . '/' . $id . '-' . $k . '-post.tex' . "}\% '$kind' replaced\n\n";
      my $count = 0;
      while (1) {
        my $applied = $tex =~ s/${search}/${re}/g;
        if (!$applied) {
          if ($count eq 0) {
            debug("Didn't find \\begin{$kind} suggested by $f");
          }
          last;
        }
        debug('\\begin{' . $kind . '} ' . $id . ' -> ' . $re);
        $count += 1;
      }
    }
  }
  if (not $target) {
    error('Target file name must be specified');
    exit(1);
  }
  debug('Target: ' . $target);
  savefile($target, $tex);
  info("New TeX file save to: ". $target);
}

# In order to finish it with success:
1;
