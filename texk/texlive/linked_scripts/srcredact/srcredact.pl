#!/usr/bin/env perl

use strict;

my $USAGE = <<END;
srcredact - a program for redacting the sources

Usage:

srcredact [OPTIONS] -e audience [full_file]

srcredact [OPTIONS] -u audience full_file [redacted_file]

srcredact -l [full_file]

srcredact -h|-v


srcredact is the program to extract ``redacted versions'' of the master
file (option -e) or to incorporate the changes in the redacted versions
into the master file (``unextract'', option -u).

The master file consists of chunks intended for different audiences.
Each audience has a name, e.g. "classified", "unclssified", "expert"
etc. Chunks are started and stopped by guard lines. Each guard line
has the format (for the default TeX syntax)

  %<name1|name2|...>

or

  %</name1|name2|...>

In the first cases the text following the guard is included for the
audiences "name1", "name2", .... In the second case it is excluded for
these audiences.

THere is a special audience "ALL": a wild card for all audiences. Thus
the idiom

  %</ALL>
  %<classified>

means that the chunk is excluded for all audiences but "classified"

Exactly one of the options -e (extract) or -u (unextract) must be present.
In the redact mode the non-option argument is the name of the full file.
If it is absent, or is "-", standard input is used. In the unextract mode
the first non-option argument

OPTIONS
 -c list of comment patterns
     Use the given pattern for comment lines to search for guards instead
     of the default "TeX" pattern. The recognized patterns are:

     c
           /<guard>/

     cpp
           //<guard>

     fortran
           C<guard>

     shell
           #<guard>

     TeX
           %<guard>

     The pattern names should be separated by commas, and the list may be
     enclosed in quotes to prevent shell expansion, e.g

       -c "TeX, c, shell"

 -d  Debug mode on.

 -e audience
     Extract the contents for the current audience into the file file.
     The cuurent audience is guessed from the file name, if the latter
     has the structure base-audience.extension, e.g.
     "report-unclassified.tex". The key -a overrides this guess and
     should be used if the file name does not follow this pattern. The
     file name "-" means the standard output.

 -h  Print help information and exit.

 -l  List all audiences set in the file (one per line) and exit.

 -u audience
     Take a redacted file intended for the audience (the second
     non-option argument) and incorporate the changes in it into the full
     file (the first non-option argument). If the second argument is
     missing, standard input is used instead. As usual, "-" also means
     standard input. Note that only one of the two file arguments in this
     case can be standard input.

 -v  Print version information and exit.

 -w on|off|1|0|true|false
     If "on", 1 or "true" (the deafult), implicitly wrap the full
     document into the guards

      %<ALL>
      ...
      %</ALL>
END


    my $COPYRIGHT= <<END;
srcredact - a program for redacting the sources

Copyright (C) 2015 Boris Veytsman.  Version 1.0

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA
END


######################################################
#   Reading arguments
######################################################

use Getopt::Std;
use File::Temp qw/tempdir/;

our %opts;
getopts ('c:de:hlu:vw:', \%opts) or do {
    print STDERR $USAGE;
    exit(2);
};

if ($opts{h}) {
    die ($USAGE);
}

if ($opts{v}){
    die ($COPYRIGHT);
}

my $DEBUG=0;
if ($opts{d}) {
    $DEBUG=1;
}


my @commentPatterns;
if ($opts{c})  {
    @commentPatterns=GetCommentPatterns($opts{c});
} else {
    @commentPatterns=GetCommentPatterns('TeX');
}

# We want exactly one of the modes -l, -e, -u:
my $audience;
my $mode;
my $modesFound=0;
if ($opts{l}) {
    $mode = "list";
    $modesFound++;
}
if ($opts{e}) {
    $mode='extract';
    $audience = $opts{e};
    $modesFound++;
}
if ($opts{u}) {
    $mode = "unextract";
    $audience = $opts{u};
    $modesFound++;
}

if ($modesFound != 1) {
    print STDERR
	"Exactly one of the options -l, -e, -u must be present.\n\n$USAGE";
    exit (2);
}

my $wrap=1;
if (exists $opts{w}) {
    if ($opts{w} eq '0' || $opts{w} =~ m/^off$/i ||
	$opts{w} =~ m/^false$/i) {
	$wrap=0;
    } elsif ($opts{w} eq '1' || $opts{w} =~ m/^on$/i ||
	$opts{w} =~ m/^true$/i) {
	$wrap=0;
    } else {
	print STDERR "Unknwon value for -w option: $opts{w}.\n" .
	    "Must be 0|1|on|off|true|false\n";
	exit (2);
    }
}

if ($DEBUG) {
    print STDERR "Wrapping in <ALL> is $wrap\n";
}

# Opening files
my $fullFH;
my $newFH;

# In 'l' or 'e' mode we need one or zero non-option arguments
if (($mode eq 'list') || ($mode eq 'extract')) {
    if (scalar(@ARGV) == 0) {
	if ($DEBUG) {
	    print STDERR "Usign stdin for full file\n";
	}
	$fullFH=*STDIN;
    } elsif (scalar(@ARGV) == 1) {
	my $full = shift @ARGV;
	if ($full eq '-') {
	    if ($DEBUG) {
		print STDERR "Usign stdin for full file\n";
	    }
	    $fullFH = *STDIN;
	} else {
	    if ($DEBUG) {
		print STDERR "Usign $full for full file\n";
	    }
	    open ($fullFH, "<", $full) or do {
		print STDERR "Cannot open file $full\n";
		exit (2);
	    };
	}
    } else {
	print STDERR 
	    "Options -e and -l require one or zero non-option argument\n\n"; 
	print STDERR $USAGE;
	exit(2);
    }
}

# In 'u' mode we need one or two arguments
if ($mode eq 'unextract') {
    if ((scalar(@ARGV) < 1) || (scalar(@ARGV) > 2)) {
	print STDERR 
	    "Option -u requires one or two non-option argument\n\n"; 
	print STDERR $USAGE;
	exit(2);
    }

    my $full = shift @ARGV;
    if ($full eq '-') {
	if ($DEBUG) {
	    print STDERR "Usign stdin for full file\n";
	}
	$fullFH = *STDIN;
    } else {
	if ($DEBUG) {
	    print STDERR "Usign $full for full file\n";
	}
	open ($fullFH, "<", $full) or do {
	    print STDERR "Cannot open file $full\n";
	    exit (2);
	};
    }
    
    if (scalar(@ARGV) == 0) {
	if ($DEBUG) {
	    print STDERR "Usign stdin for redacted file\n";
	}
	$newFH=*STDIN;
    } else {
	my $redacted = shift @ARGV;
	if ($redacted eq '-') {
	    if ($DEBUG) {
		print STDERR "Usign stdin for redacted file\n";
	    }
	    $newFH = *STDIN;
	} else {
	    if ($DEBUG) {
		print STDERR "Usign $redacted for redacted file\n";
	    }
	    open ($newFH, "<", $redacted) or do {
		print STDERR "Cannot open file $redacted\n";
		exit (2);
	    };
	}
    }
}
	


# And the real work
if ($mode eq 'list') {
    if ($DEBUG) {
	print STDERR "List mode\n";
    }
    my @audiences=ListAudiences($fullFH, \@commentPatterns);
    print join("\n", @audiences), "\n";
    exit (0);
}

if ($mode eq 'extract') {
    if ($DEBUG) {
	print STDERR "Extract mode\n";
    }
    ExtractText($fullFH, \*STDOUT, $audience,
		\@commentPatterns, $wrap);
    exit (0);
}

if ($mode eq 'unextract') {
    if ($DEBUG) {
	print STDERR "Unextract mode\n";
    }
    
    my $exitCode = UnextractText($fullFH, $newFH, \*STDOUT,
				 $audience, \@commentPatterns, $wrap);

    exit ($exitCode);

}    

##################################################
# Extracting comment patterns
##################################################

sub GetCommentPatterns {

    my %knownPatterns = ( 'c' => '^/\*<([^>]*)>\*/',
			  'cpp' => '^\/\/<([^>]*)>',
			  fortran => '^C<([^>]*)>',
			  shell => '^#<([^>]*)>',
			  tex => '^%<([^>]*)>'
			  );
			 


    my $input = shift;
    if ($DEBUG) {
	print STDERR "Got pattern string $input\n";
    }
    $input =~ s/^\s*//;
    $input =~ s/\s*$//;
    my @languages = split /[\s,]\s*/, $input;
    my @patterns;

    foreach my $lang (@languages) {
	$lang =~ s/^(.*)$/\L$1\E/;
	if ($DEBUG) {
	    print STDERR "Processing language $lang... ";
	}
	if (exists $knownPatterns{$lang}) {
	    push @patterns, $knownPatterns{$lang};
	    if ($DEBUG) {
		print STDERR "found pattern $knownPatterns{$lang}\n";
	    }
	} else {
	    print STDERR 
		"Unknown comment language $lang.  The supported languages are ",
		join(", ", keys %knownPatterns), "\n";
	    exit(2);
	}
    }
    
    return @patterns;
}

##################################################
# List audiences
##################################################

sub ListAudiences {
    my $fh=shift;
    my $patterns=shift;
    my %foundAudiences = ('ALL' => 1);
    while (<$fh>) {
	my @result = FindAudiences ($_, $patterns);
	if (scalar @result) {
	    shift @result;
	    foreach my $audience (@result) {
		$foundAudiences{$audience} = 1;
	    }
	}
    }
    return (sort keys %foundAudiences);
}

##################################################
# Check whether the line is a comment
##################################################
# Return 0 to delete, 1 to add, and the list of
# audiences
sub FindAudiences {
    my $line = shift;
    my $patterns=shift;
    foreach my $pattern (@{$patterns}) {
	if ($line =~ m/$pattern/) {
	    if ($DEBUG) {
		print STDERR "Line $. is a guard: $line";
	    }
	    my $guards = $1;
	    my @result;
	    if (substr($guards,0,1) eq '*') {
		push @result, 1;
	    } elsif (substr($guards,0,1) eq '/') {
		push @result, 0;
	    } else {
		die "Wrong guard line $line\n";
	    }
	    @result = (@result, split(/\|/, substr($guards,1)));
	    return @result;
	}
    }
    return ();
}

##################################################
# Extracting text for the given audience
##################################################
sub ExtractText {
    my $fullFH=shift;
    my $outFH=shift;
    my $audience = shift;
    my $patterns = shift;
    my $state = shift;

    if ($DEBUG) {
	print STDERR "Initial state $state\n";
    }

    while (<$fullFH>) {
	my @result = FindAudiences ($_, $patterns);
	if (scalar @result) {
	    my $newstate = shift @result;
	    foreach my $tryaudience (@result) {
		if (($tryaudience eq 'ALL') || 
		    ($audience eq $tryaudience)) {
		    $state = $newstate;
		    if ($DEBUG) {
			print STDERR "Changing state to $state\n";
		    }
		    last;
		}
	    }
	} else { # Normal line
	    if ($state) {
		print $outFH $_;
	    }
	}
    }
}

################################################################
#  Merge files
################################################################

sub UnextractText {
    my $fullFH=shift;
    my $newFH=shift;
    my $outFH = shift;
    my $audience = shift;
    my $patterns = shift;
    my $state = shift;

    # Creating tempdir.  We leave the tempdir in place in 
    # the debug mode
    my $tmpdir = tempdir (CLEANUP => !$DEBUG);
    if ($DEBUG) {
	print STDERR "Creating directory  $tmpdir\n";
    }

    open (FULL, ">$tmpdir/full") or do {	
	print STDERR "Cannot open temporary file $tmpdir/full\n";
	return(2);
    };

    print FULL <$fullFH>;
    close FULL;
    
    open (FULL, "<$tmpdir/full") or do {	
	print STDERR "Cannot open temporary file $tmpdir/full\n";
	return(2);
    };

    open (EXTRACTED, ">$tmpdir/extracted") or do {	
	print STDERR "Cannot open temporary file $tmpdir/extracted\n";
	return(2);
    };

    if ($DEBUG) {
	print STDERR "Extracting text\n";
    }

    ExtractText(\*FULL, \*EXTRACTED, $audience,
		$patterns, $wrap);

    close FULL;
    close EXTRACTED;

    open (NEW, ">$tmpdir/new") or do {	
	print STDERR "Cannot open temporary file $tmpdir/new\n";
	return(2);
    };

    print NEW <$newFH>;
    close NEW;


    if ($DEBUG) {
	print STDERR "Merging text\n";
    }

    print $outFH `diff3 -m $tmpdir/full $tmpdir/extracted $tmpdir/new`;

    return ${^CHILD_ERROR_NATIVE};

}
