#!/usr/bin/env perl

# This file is a part of TeX package EasyDTX, available at
# https://ctan.org/pkg/easydtx and https://github.com/sasozivanovic/easydtx.
#
# Copyright (c) 2023- Saso Zivanovic <saso.zivanovic@guest.arnes.si>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# The files belonging to this work and covered by LPPL are listed in
# <texmf>/doc/support/easydtx/FILES.

use strict;
use Getopt::Long;

my $usage = <<END;
EasyDTX: convert .edtx into .dtx
Usage: edtx2dtx filename.edtx > filename.dtx
END

my $VERSION = '0.1.0';
my ($help,  $print_version);
GetOptions(
    "help|h|?"  => \$help,
    "version|V"  => \$print_version,
    ) or die $usage;
if ($help) {print($usage); exit 0}
if ($print_version) { print("edtx2dtx $VERSION\n"); exit 0 }
die $usage unless @ARGV == 1;

sub begin_macrocode { print("%    \\begin{macrocode}\n"); }
sub end_macrocode   { print("%    \\end{macrocode}\n");   }

sub process_edtx {
    my $indoc = 1;
    while (<>) {
	if (/^%    \\end{macrocode}/) { # trailer starts here
	    end_macrocode unless ($indoc);
	    last;
	} elsif (/^%    \\begin{macrocode}/) {
	    die "Nested \\begin{macrocode}";
	} elsif (/^ *(%%+)/) { # code: multiple comments
	    begin_macrocode if ($indoc);
	    print;
	    $indoc = 0;
	} elsif (/^%(<[^>]*>)(.*)$/) { # code: unindented guard
	    begin_macrocode if ($indoc);
	    print;
	    $indoc = 0;
	} elsif (/^( *)%(<[^>]*>) *(.*)$/) { # code: indented guard
	    begin_macrocode if ($indoc);
	    print("%$2$1$3\n");
	    $indoc = 0;
	} elsif (/^% *(.*)$/) { # doc: unindented comment
	    end_macrocode unless ($indoc);
	    print;
	    $indoc = 1;
	} elsif (/^( ?)( *)% *(.*)$/) { # doc: indented comment
	    end_macrocode unless ($indoc);
	    print "%$2$3\n";
	    $indoc = 1;
	} else { # code
	    begin_macrocode if ($indoc);
	    print;
	    $indoc = 0;
	}
    }
}

my $edtx = @ARGV[0];
my $dtx = $edtx;
$dtx =~ s/\.edtx$/.dtx/;

my $first = 1;
while (<>) {
    if ($first and s/$edtx +(.*)/$dtx (generated from $edtx by edtx2dtx)/) {
	print;
	$first = 0;
    } elsif (/^%    \\begin{macrocode}/) {
	process_edtx;
    } elsif (/^%    \\end{macrocode}/) {
	die "\\end{macrocode} without the opening \\begin{macrocode}!";
    } else {
	print;
    }
}


