#!/usr/bin/env perl
# $Id: texfot,v 1.25 2014/10/10 17:52:53 karl Exp $
# Invoke a TeX command, filtering all but interesting terminal output;
# do not look at the log or check any output files.
# Exit status is that of the subprogram.
# Tee the complete (unfiltered) output to (by default) /tmp/fot.
# 
# Public domain.  Originally written 2014 by Karl Berry.

my $ident = '$Id: texfot,v 1.25 2014/10/10 17:52:53 karl Exp $';
(my $prg = $0) =~ s,^.*/,,;
select STDERR; $| = 1;  # no buffering
select STDOUT; $| = 1;

use IPC::Open3; # control what happens with stderr from the child.

# require_order because we don't want getopt to permute anything;
# arguments to the tex invocation must remain in order, not be handled here.
use Getopt::Long qw(:config require_order);
use Pod::Usage;

my $opt_debug = 0;
my @opt_ignore = ();
my $opt_interactive = 0;
my $opt_quiet = 0;
my $opt_tee = ($ENV{"TMPDIR"} || "/tmp") . "/fot";
my $opt_version = 0;
my $opt_help = 0;

exit (&main ());


# 
sub main {
  my $ret = GetOptions (
    "debug!"       => \$opt_debug,
    "ignore=s"     => \@opt_ignore,
    "interactive!" => \$opt_interactive,
    "quiet!"       => \$opt_quiet,
    "tee=s"        => \$opt_tee,
    "version"      => \$opt_version,
    "help|?"       => \$opt_help) || pod2usage (2);

  # --help, --version
  pod2usage ("-exitstatus" => 0, "-verbose" => 2)
    if $opt_help;
  #
  if ($opt_version) {
    print "$ident\n";
    return 0;
  }
  
  die "$prg: missing TeX command, i.e., a non-option argument\n"
      . "Try --help if you need it."
    if ! @ARGV;

  # guess we're going to run something.  typically no interaction.
  close (STDIN) unless $opt_interactive;

  local *FOTTMP;
  $FOTTMP = ">$opt_tee";
  open (FOTTMP) || die "$prg: aborting, open($FOTTMP) failed: $!";

  # We want to grab stderr, otherwise it gets merged with stdout, not
  # necessarily at line breaks, hence can lose useful messages.
  print "$0: invoking: @ARGV\n" unless $opt_quiet;
  local *TEXOUT, *TEXERR;
  my $pid = open3 (undef, \*TEXOUT, \*TEXERR, @ARGV)
            || die "$prg: fork(TeX) failed: $! [cmd=@ARGV]\n";
  
  # For what it's worth, some other approaches to stderr I investigated:
  # "safe pipe open" in perlipc
  # http://www.perlmonks.org/?node_id=757524
  # http://stackoverflow.com/questions/3486575/direct-stderr-when-opening-pipe-in-perl

  # It's not ideal to read all of stdout and then all of stderr, would
  # be better to intermix them in the original order of child output, but
  # it's simpler than other ways of avoiding possible deadlock (select,
  # sysread, etc.).
  &debug ("processing stdout from child");
  &process_output (\*TEXOUT, "");
  
  &debug ("processing stderr from child");
  &process_output (\*TEXERR, "[stderr] ");
  
  # Be sure everything is drained.
  waitpid ($pid, 0) || die "$prog: waitpid($pid) failed: $!\n";
  my $child_exit_status = $? >> 8;
  &debug ("child exit status = $exit_status\n");
  return $child_exit_status;
}



# Read filehandle $FH; print lines that we want to stdout, prefixed by
# $PREFIX.  If $PREFIX is null, omit lines by default; if $PREFIX is
# non-null, print lines by default.
# 
sub process_output {
  my ($fh,$prefix) = @_;
  
  my $print_next = 0;
  LINE: while (<$fh>) {
    my $line = $_;
    print FOTTMP $line; # tee everything

    warn "\n" if $opt_debug; # get blank line without texfot: prefix
    &debug ("looking at line: $_");
    &debug ("checking if have print_next (is $print_next)\n");
    if ($print_next) {
      &debug ("  printing next ($print_next)\n");
      print $prefix;
      print $line;
      $print_next = 0;
      next;
    }
    
    &debug ("checking ignores\n");
    next if /^(
      LaTeX\ Warning:\ You\ have\ requested\ package
     |LaTeX\ Font\ Warning:\ Some\ font\ shapes
     |LaTeX\ Font\ Warning:\ Size\ substitutions
     |Package\ caption\ Warning:\ Unsupported\ document\ class
     |Package\ frenchb\.ldf\ Warning:\ (Figures|The\ definition)
     |Reloading\ Xunicode\ for\ encoding  # spurious ***
     |This\ is.*epsf\.tex                 # so what
     |pdfTeX\ warning:.*inclusion:\ fou   #nd PDF version ...
     |pdfTeX\ warning:.*inclusion:\ mul   #tiple pdfs with page group
     |libpng\ warning:\ iCCP:\ Not\ recognizing
    )/x;
    
    # don't anchor user ignores, leave it up to them.
    for my $user_ignore (@opt_ignore) {
      &debug ("checking user ignore '$user_ignore'\n");
      next LINE if /${user_ignore}/;
    }

    &debug ("checking for print_next\n");
    if (/^(
      .*?:[0-9]+:        # usual file:lineno: form
     |!                  # usual ! form
     |.*pdfTeX\ warning  # pdftex complaints often cross lines
     |LaTeX\ Font\ Warning:\ Font\ shape
     |>\ [^<]            # from \show..., but not "> <img.whatever"
     |removed\ on\ input\ line  # hyperref
    )/x) {
      &debug ("  found print_next ($1)\n");
      print $prefix;
      print $line;
      $print_next = 1;
      next;
    }

    &debug ("checking for showing\n");
    if (/^(
      This\ is
     |Output\ written
     |No\ pages\ of\ output
     |(Und|Ov)erfull
     |(LaTeX|Package).*(Error|Warning)
     |.*Citation.*undefined
     |.*\ Error           # as in \Url Error ->...
     |Missing\ character: # good to show (need \tracinglostchars=1)
     |\*\*\*\s            # *** from some packages or subprograms
     |l\.[0-9]+\          # line number marking
     |all\ text\ was\ ignored\ after\ line
     |.*Fatal\ error
     |.*for\ symbol.*on\ input\ line
    )/x) {
      &debug ("  matched for showing ($1)\n");
      print $prefix;
      print $line;
      next;
    }

    &debug ("done with all checks\n");
    
    if ($prefix) {
      &debug ("prefix (stderr), showing line by default: $_");
      print $prefix;
      print $line;
    } else {
      &debug ("no prefix (stdout), ignoring line by default: $_");
    }
  }
}


sub debug { warn ("$prg: ", @_) if $opt_debug; }


__END__

=head1 NAME

texfot - run TeX, filtering online transcript for interesting messages

=head1 SYNOPSIS

texfot [I<option>]... I<texcmd> [I<texarg>...]

=head1 DESCRIPTION

C<texfot> invokes I<texcmd> with the given I<texarg> arguments,
filtering the online output for ``interesting'' messages.  Its exit
value is that of I<texcmd>.  Examples:

  # Sample basic invocation:
  texfot pdflatex file.tex
  
  # Ordinarily all output is copied to /tmp/fot before filtering;
  # omit that:
  texfot --tee=/dev/null file.tex
  
  # Example of more complex engine invocation:
  texfot lualatex --recorder '\nonstopmode\input file'

Aside from its own options, described below, C<texfot> just runs the
given command with the given arguments (same approach to command line
syntax as C<env>, C<nice>, C<time>, C<timeout>, etc.).  Thus, C<texfot>
works with any engine and any command line options.

C<texfot> does not look at the log file or any other possible output
file(s); it only looks at the standard output and standard error from
the command.  stdout is processed first, then stderr.  Lines from stderr
have an identifying prefix.  C<texfot> writes all accepted lines to its
stdout.

The messages shown are intended to be those which likely need action by
the author: error messages, overfull and underfull boxes, undefined
citations, missing characters from fonts, etc.

=head1 FLOW OF OPERATION

Here is the order in which lines of output are checked:

=over 4

=item 1.

If the ``next line'' needs to be printed (see below), print it.

=item 2.

Otherwise, if the line matches the built-in list of regexps to ignore,
or any user-supplied list of regexps to ignore (given with C<--ignore>,
see below), in that order, ignore it.

=item 3.

Otherwise, if the line matches the list of regexps for which the next
line (two lines in all) should be shown, set the ``next line'' flag for
the next time around the loop and show this line.  Examples are the
usual C<!> and C<filename:lineno:> error messages, which are generally
followed by a line with specific detail about the error.

=item 4.

Otherwise, if the line matches the list of regexps to show, show it.

=item 5.

Otherwise, the default: if the line came from stdout, ignore it; if the
line came from stderr, print it (to stdout).  (This distinction is made
because TeX engines write relatively few messages to stderr, and it's
not unlikely that any such should be considered.

=back

Once a particular check matches, the program moves on to process the
next line.

Don't hesitate to peruse the source to the script, which is essentially
a straightforward loop matching against the different lists as above.
You can see the exact regexps being matched in the different categories
in the source.

Incidentally, although nothing in this basic operation is specific to
TeX engines, all the regular expressions included in the program are
specific to TeX.  So in practice the program isn't useful except with
TeX engines, although it would be easy enough to adapt it (if there was
anything else as verbose as TeX to make that useful).

=head1 OPTIONS

The following are the options to C<texfot> itself (not the TeX engine
being invoked; consult the TeX documentation or the engine's C<--help>
output for that).

The first non-option terminates C<texfot>'s option parsing, and the
remainder of the command line is invoked as the TeX command, without
further parsing.  For example, C<texfot --debug tex
--debug> will output debugging information from both C<texfot> and
C<tex>.

Options may start with either - or --, and may be unambiguously
abbreviated.  It is best to use the full option name in scripts, though,
to avoid possible collisions with new options in the future.

=over 4

=item C<--debug>

=item C<--no-debug>

Output (or not) what is being done on standard error.  Off by default.

=item C<--ignore> I<regexp>

Ignore lines in the TeX output matching (Perl) I<regexp>.  Can be
repeated.  Adds to the default set of ignore regexps rather than
replacing.  These regexps are not automatically anchored (or otherwise
altered), simply used as-is.

=item C<--interactive>

=item C<--no-interactive>

By default, standard input to the TeX process is closed so that TeX's
interactive mode (waiting for input upon error, the C<*> prompt, etc.)
never happens.  Giving C<--interactive> allows interaction to happen.

=item C<--quiet>

=item C<--no-quiet>

By default, the TeX command being invoked is reported on standard output.
C<--quiet> omits that reporting.

=item C<--tee> I<file>

By default, the output being filtered is C<tee>-ed, before filtering, to
C<$TMPDIR/fot> (C</tmp/fot> if C<TMPDIR> is not set), to make it easy to
check the full output when the filtering seems suspect.  This option
allows specifying a different file.  Use S<C<--tee /dev/null>> if you don't
want the original output at all.

=item C<--version>

Output version information and exit successfully.

=item C<--help>

Display this help and exit successfully.

=back

=head1 RATIONALE

I wrote this because, in my work as a TUGboat editor
(L<http://tug.org/TUGboat>, submissions welcome, by the way), I end up
running and rerunning many papers, many times each.  It was too easy to
lose warnings I needed to see in the mass of unvarying and uninteresting
output from TeX, such as all the style files being read and all the
fonts being used.  I wanted to see all and only those messages which
actually needed some action by me.

I found some other programs of a similar nature, the C<silence> LaTeX
package, and plenty of other (La)TeX wrappers, but it seemed none of
them did what I wanted.  Either they read the log file (I wanted the
online output only), or they output more or less than I wanted, or they
required invoking TeX differently (I wanted to keep my build process
exactly the same, definitely including the TeX invocation, which can get
complicated).  Hence I wrote this.

Here are some keywords if you want to explore other options:
texloganalyser, pydflatex, logfilter, latexmk, rubber, arara, and
searching for C<log> at L<http://ctan.org/search>.

C<texfot> is written in Perl, and runs on Unix, and does not work on
Windows.  (If by some chance anyone wants to use this program on
Windows, please make your own fork; I'm not interested in supporting
it.)

The name comes from the C<trip.fot> and C<trap.fot> files that are part
of Knuth's trip and trap torture tests, which record the online output
from the programs.  I am not sure what "fot" stands for in trip and
trap, but I can pretend that it stands for "filter online transcript" in
the present S<case :).>

=head1 AUTHORS AND COPYRIGHT

This script and its documentation were written by Karl Berry and both
are released to the public domain.  Email C<karl@freefriends.org> with
bug reports.  There is no home page beyond the package on CTAN:
L<http://www.ctan.org/pkg/texfot>.

=cut

# doesn't survive forks, have to use shell to rediret.

#  if ($pid == 0) { # child
#    # Unfortunately, the stderr lines are typically concatenated onto
#    # however much of the TeX output line has happened.  Should somehow
#    # prepend a newline to stderr lines.
#    #open (STDERR, ">&", \*STDOUT) || die "$prg: open(STDERR) failed: $!";
#    #warn "tying stderr";
#    open (E, "-|", "echo-stderr", "fromerr") || die "xopen fil";
#    close (E) || die "xclo fil";
#    #exec (@ARGV, "2>&1") || die "$prg: exec(TeX) failed: $! [cmd=@ARGV]\n";
#    die "$prg: after exec, shouldn't happen: $!";
#  }
