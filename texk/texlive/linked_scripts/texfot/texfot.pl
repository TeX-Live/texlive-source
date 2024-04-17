#!/usr/bin/env perl
# $Id: texfot,v 1.53 2024/04/17 16:52:12 karl Exp $
# Invoke a TeX command, filtering all but interesting terminal output;
# do not look at the log or check any output files.
# Exit status is that of the subprogram.
# Tee the complete (unfiltered) standard output and standard error to
# (by default) /tmp/fot.$UID.
# 
# Public domain.  Originally written 2014 by Karl Berry.

my $ident = '$Id: texfot,v 1.53 2024/04/17 16:52:12 karl Exp $';
(my $prg = $0) =~ s,^.*/,,;
select STDERR; $| = 1;  # no buffering
select STDOUT; $| = 1;

use IPC::Open3; # control what happens with stderr from the child.
use IO::File;   # use new_tmpfile for that stderr.

# require_order because we don't want getopt to permute anything;
# arguments to the tex invocation must remain in order, not handled by us.
use Getopt::Long qw(:config require_order);
use Pod::Usage;

my @opt_accept = ();
my $opt_debug = 0;
my @opt_ignore = ();
my $opt_interactive = 0;
my $opt_quiet = 0;
my $opt_stderr = 1;
my $opt_tee = ($ENV{"TMPDIR"} || $ENV{"TMP"} || "/tmp") . "/fot.$>";
my $opt_version = 0;
my $opt_help = 0;

exit (&main ());


# 
sub main {
  my $ret = GetOptions (
    "accept=s"     => \@opt_accept,
    "debug!"       => \$opt_debug,
    "ignore=s"     => \@opt_ignore,
    "interactive!" => \$opt_interactive,
    "quiet!"       => \$opt_quiet,
    "stderr!"	   => \$opt_stderr,
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

  # We need to separate stderr from stdout.  Otherwise they are randomly
  # merged, not always at line breaks, hence we can lose useful messages.
  print "$0: invoking: @ARGV\n" unless $opt_quiet;

  # In order to avoid deadlock when there is lots of stuff on stderr,
  # we must write it to a temporary file
  # http://perldoc.perl.org/perlfaq8.html#How-can-I-capture-STDERR-from-an-external-command 
  local *TEXERR = IO::File->new_tmpfile
  || die "IO::File->new_tmpfile failed: $!";
  
  # But we can process stdout as it comes.
  local *TEXOUT;
  my $pid = open3 (undef, \*TEXOUT, ">&TEXERR", @ARGV)
            || die "$prg: fork(TeX) failed: $! [cmd=@ARGV]\n";
  &debug ("open3() returned pid $pid [cmd=@ARGV]");
  
  # It's not ideal to read all of stdout and then all of stderr; it would
  # be better to intermix them in the original order of child output.
  # But this is simpler than other ways of avoiding possible deadlock (such
  # as select, sysread, etc.).
  &debug ("processing stdout from child");
  &process_output (\*TEXOUT, "");
  
  # Be sure everything is drained.
  &debug ("starting waitpid() for $pid")  ;
  waitpid ($pid, 0) || die "$prg: waitpid($pid) failed: $!\n";
  my $child_exit_status = $? >> 8;
  &debug ("child exit status = $child_exit_status\n");

  &debug ("processing stderr from child");
  seek (TEXERR, 0, 0) || warn "seek(stderr) failed: $!";
  &process_output (\*TEXERR, "[stderr] ");
  close (TEXERR) || warn "close(stderr tmpfile) failed: $!";
  
  return $child_exit_status;
}



# Read filehandle $FH, printing lines that we want to stdout,
# prefixed by $PREFIX.
# If $PREFIX is null (happens for processing stdout), omit lines by default;
# if $PREFIX is non-null (processing stderr), print lines by default.
# 
sub process_output {
  my ($fh,$prefix) = @_;
  
  my $print_next = 0;
  LINE: while (<$fh>) {
    my $line = $_;
    print FOTTMP $line; # tee everything

    warn "\n" if $opt_debug; # get blank line without texfot: prefix
    &debug ("looking at line: $_");
    
    &debug ("checking if have print_next (= $print_next)\n");
    if ($print_next) {
      &debug ("  printing next ($print_next)\n");
      print $prefix;
      print $line;
      $print_next = 0;
      next;
    }
    
    # don't anchor user accept patterns, leave it up to them.
    for my $user_accept (@opt_accept) {
      &debug ("checking user accept '$user_accept'\n");
      if (/${user_accept}/) {
        &debug ("  found user accept ($user_accept)\n");
        print $prefix;
        print $line;
        next;
      }
    }

    &debug ("checking ignores\n");
    next if /^(
      LaTeX\ Warning:\ You\ have\ requested\ package
     |LaTeX\ Font\ Warning:\ Some\ font\ shapes
     |LaTeX\ Font\ Warning:\ Size\ substitutions
     |Module\ luamplib\ Warning:\ This\ is\ MetaPost
     |Package\ auxhook\ Warning:\ Cannot\ patch
     |Package\ biditools\ Warning:\ Patching
     |Package\ caption\ Warning:\ Un(supported|known)\ document\ class
     |Package\ fixltx2e\ Warning:\ fixltx2e\ is\ not\ required
     |Package\ frenchb?\.ldf\ Warning:\ (Figures|The\ definition)
     |Package\ layouts\ Warning:\ Layout\ scale
     |\*\*\*\ Reloading\ Xunicode\ for\ encoding  # spurious ***
     |This\ is\ `?(epsf\.tex|.*\.sty|TAP) # so what
     |pdfTeX\ warning:.*inclusion:\ fou   #nd PDF version ...
     |pdfTeX\ warning:.*inclusion:\ mul   #tiple pdfs with page group
     |libpng\ warning:\ iCCP:\ Not\ recognizing
     |!\ $
    )/x;
    
    # don't anchor user ignore patterns either.
    for my $user_ignore (@opt_ignore) {
      &debug ("checking user ignore '$user_ignore'\n");
      next LINE if /${user_ignore}/;
    }

    &debug ("checking for print_next pattern\n");
    if (/^(
      .*?:[0-9]+:        # usual file:lineno: form
     |!                  # usual ! form
     |>\ [^<]            # from \show..., but not "> <img.whatever"
     |.*pdfTeX\ warning  # pdftex complaints often cross lines
     |LaTeX\ Font\ Warning:\ Font\ shape
     |Package\ hyperref\ Warning:\ Token\ not\ allowed
     |removed\ on\ input\ line  # hyperref
     |Runaway\ argument
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
     |\(.*end\ occurred\ inside\ a\ group
     |(Und|Ov)erfull
     |(LaTeX|Package|Class|Module).*(Error|Warning)
     |.*Citation.*undefined
     |.*\ Error           # as in \Url Error ->...
     |Missing\ character: # good to show (need \tracinglostchars=1)
     |\\endL.*problem     # XeTeX?
     |\*\*\*\s            # *** from some packages or subprograms
     |l\.[0-9]+\          # line number marking
     |all\ text\ was\ ignored\ after\ line
     |.*Fatal\ error
     |.*for\ symbol.*on\ input\ line
     |\#\#
    )/x) {
      &debug ("  matched for showing ($1)\n");
      print $prefix;
      print $line;
      next;
    }

    &debug ("done with all checks\n");
    
    if ($prefix && $opt_stderr) {
      &debug ("prefix (stderr), showing line by default: $_");
      print $prefix;
      print $line;
    } else {
      &debug ("no prefix (stdout) or no stderr, ignoring line by default: $_");
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
filtering the online output for ``interesting'' messages. Its exit
value is that of I<texcmd>. Examples:

  # Sample basic invocation:
  texfot pdflatex file.tex
  
  # Ordinarily the full output is copied to /tmp/fot.$UID before
  # filtering, but that can be omitted, or the filename changed:
  texfot --tee=/dev/null lualatex file.tex
  
  # Example of more complex engine invocation:
  texfot xelatex --recorder '\nonstopmode\input file'

Here is an example of what the output looks like (in its entirety) on an
error-free run:

  /path/to/texfot: invoking: pdflatex hello.ltx
  This is pdfTeX, Version 3.141592653-2.6-1.40.24 (TeX Live 2022) (preloaded format=pdflatex)
  Output written on hello.pdf (1 page, 94415 bytes).

Aside from its own options, described below, C<texfot> just runs the
given command with the given arguments (same approach to command line
syntax as C<env>, C<nice>, C<timeout>, etc.). Thus, C<texfot> works with
any engine and any command line options.

C<texfot> does not look at the log file or any other possible output
file(s); it only looks at the standard output and standard error from
the command. stdout is processed first, then stderr. C<texfot> writes
all accepted lines to its stdout.

The messages shown are intended to be those which likely need action by
the author: error messages, overfull and underfull boxes, undefined
citations, missing characters from fonts, etc.

=head1 FLOW OF OPERATION

Here is the order in which lines of output are checked:

=over 4

=item 1.

If the ``next line'' needs to be printed (see below), print it.

=item 2.

Otherwise, if the line matches any user-supplied list of regexps to
accept (given with C<--accept>, see below), in that order, print it.

=item 3.

Otherwise, if the line matches the built-in list of regexps to ignore,
or any user-supplied list of regexps to ignore (given with C<--ignore>,
see below), in that order, ignore it.

=item 4.

Otherwise, if the line matches the list of regexps for which the next
line (two lines in all) should be shown, show this line and set the
``next line'' flag for the next time around the loop. Examples are the
common C<!> and C<filename:lineno:> error messages, which are generally
followed by a line with specific detail about the error.

=item 5.

Otherwise, if the line matches the list of regexps to show, show it.

=item 6.

Otherwise, the default: if the line came from stdout, ignore it; if the
line came from stderr, print it (to stdout), with the prefix
S<C<[stderr] >>. This distinction is made because TeX engines write
relatively few messages to stderr, and it's likely that any such should
be considered.

=back

Once a particular check matches, the program moves on to process the
next line.

C<texfot> matches exclusively line-by-line; however, TeX itself folds
output lines, typically at column 79. This means matches might fail
because the text being matched was split over two lines. To work around
this, you can effectively turn off TeX's folding by setting the
C<max_print_line> parameter to a large number, either in the environment
or on the command:

  # When errors are missed due to TeX's folding of lines:
  texfot pdftex --cnf-line max_print_line=999 file.tex
  
  # Equivalently:
  env max_print_line=999 texfot pdftex file.tex

Don't hesitate to peruse the source to the script, which is essentially
a straightforward loop matching against the different lists as above.
You can see the exact regexps being matched in the different categories
in the source.

Incidentally, although nothing in this basic operation is specific to
TeX engines, all the regular expressions included in the program are
specific to TeX. So in practice the program isn't useful except with TeX
engines, although it would be easy enough to adapt it (if there was
anything else as verbose as TeX to make that useful).

=head1 OPTIONS

The following are the options to C<texfot> itself (not the TeX engine
being invoked; consult the engine documentation or C<--help> output for
that).

The first non-option terminates C<texfot>'s option parsing, and the
remainder of the command line is invoked as the TeX command, without
further parsing. For example, C<texfot --debug tex
--debug> will output debugging information from both C<texfot> and
C<tex>. TeX engines, unlike many standard programs, require that options
be specified before the input filename or text.

Options may start with either - or --, and may be unambiguously
abbreviated. It is best to use the full option name in scripts, though,
to avoid possible collisions with new options in the future.

=over 4

=item C<--accept> I<regexp>

Accept lines in the TeX output matching (Perl) I<regexp>. Can be
repeated. This list is checked first, so any and all matches will be
shown, regardless of other options. These regexps are not automatically
anchored (or otherwise altered), simply used as-is.

=item C<--debug>

=item C<--no-debug>

Output (or not) what the program is doing to standard error; off by default.

=item C<--ignore> I<regexp>

Ignore lines in the TeX output matching (Perl) I<regexp>. Can be
repeated. Adds to the default set of ignore regexps rather than
replacing. Like the acceptance regexps, these are not automatically
anchored (or otherwise altered).

=item C<--interactive>

=item C<--no-interactive>

By default, standard input to the TeX process is closed so that TeX's
interactive mode (waiting for input upon error, the C<*> prompt, etc.)
is never entered. Giving C<--interactive> allows interaction to happen.

=item C<--quiet>

=item C<--no-quiet>

By default, the TeX command being invoked is reported on standard
output; C<--quiet> omits that reporting. To get a completely silent run,
redirect standard output: S<C<texfot ... E<gt>/dev/null>>. (The only
messages to standard error should be errors from C<texfot> itself, so it
shouldn't be necessary to redirect that, but of course that could be
done as well.)

=item C<--stderr>

=item C<--no-stderr>

The default is for C<texfot> to report everything written to stderr by
the TeX command (on stdout). C<--no-stderr> omits that reporting. (Some
programs, C<dvisvgm> is one, can be rather verbose on stderr.)

=item C<--tee> I<file>

By default, the output being filtered is C<tee>-ed, before filtering, to
make it easy to check the full output in case of problems.

The default I<file> is C<$TMPDIR/fot.>I<uid>; if C<TMPDIR> is not set,
C<TMP> is used if set; if neither is set, the default directory is
C</tmp>. For example: C</tmp/fot.1001>. The I<uid> suffix is the
effective userid of the process, appended for basic avoidance of
collisions between different users on the same system.

This option allows specifying a different file. Use S<C<--tee
/dev/null>> to discard the original output.

=item C<--version>

Output version information and exit successfully.

=item C<--help>

Display this help and exit successfully.

=back

=head1 RATIONALE

I wrote this because, in my work as a TUGboat editor
(L<https://tug.org/TUGboat>, article submissions always welcome!), I run
and rerun many documents, many times each. It was easy to lose warnings
I needed to see in the mass of unvarying and uninteresting output from
TeX, such as style files being read and fonts being used. I wanted to
see all and only those messages which needed some action by me.

I found some other programs of a similar nature, the LaTeX package
C<silence>, and plenty of other (La)TeX wrappers, but it seemed none of
them did what I wanted. Either they read the log file (I wanted to look
at only the online output), or they output more, or less, than I wanted,
or they required invoking TeX differently (I wanted to keep my build
process exactly the same, most critically the TeX invocation, which can
get complicated). Hence I wrote this little script.

Here are some keywords if you want to explore other options:
texloganalyser, pydflatex, logfilter, latexmk, rubber, arara, and
searching for C<log> at L<https://ctan.org/search>.

C<texfot> is written in Perl, and runs on Unix. It may work on Windows
if Perl and other software is installed, but I don't use Windows and
don't support C<texfot> there.

The name comes from the C<trip.fot> and C<trap.fot> files that are part
of Knuth's trip and trap torture tests, which record the online output
from the programs. I am not sure what "fot" stands for in trip and trap,
but I can pretend that it stands for "filter online transcript" in the
present S<case :).>

=head1 AUTHORS AND COPYRIGHT

This script and its documentation were written by Karl Berry and both
are released to the public domain. Email C<karl@freefriends.org> with
bug reports. It has no home page beyond the package page on CTAN:
L<https://ctan.org/pkg/texfot>.

  $Id: texfot,v 1.53 2024/04/17 16:52:12 karl Exp $

=cut
