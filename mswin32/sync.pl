require "newgetopt.pl";
use lib ("c:/source/texlive/Master/Tools");
use Cwd;
use FileUtils qw(dirname basename normalize walk_dir cleandir sort_uniq
		 copy remove_list push_uniq newer sync_dir canon_dir
		 is_absolute build_path);
use File::Copy qw(copy);

my @ignore = ( 
	   "libs/gifreader",
	   "libs/geturl",
	   "libs/geturl-new",
	   "libs/freetype2",
	   "libs/expat",
	   "libs/libttf",
	   "libs/T1",
	   "libs/unzip",
	   "texk/kpathsea-fp",
	   "texk/kpsexx",
	   "texk/tth",
	   "texk/windvi-0.68",
	   "utils/l2h-orig",
	   "utils/l2h-orig-orig",
	   "utils/latex2html",
	   "utils/noweb",
	   "utils/src2tex",
	   "utils/texinfo/info",
	   "texk.stable",
	   "texk/kpse-shared*",
	   "texk/cwebk",
	   "texk/msvc/factory",
	   "texk/kpse.*",
	   "texk/web2c/eomegadir/.*",
	   "texk/web2c/eomegadir-1.15/.*",
	   "texk/web2c/eomegadir-1.23/.*",
	   "CVS"
	  );

# $ignore_regexp = "(" . join ( '|', map { s/^/.\\/ ; s/\//\\/g; $_ } @ignore) . ")";
my $ignore_regexp = "(" . join ( '|', @ignore) . ")";

&main;

1;

sub restrict_win {
  my ($dir, $dst, @files) = @_;
  # under the texk/msvc dir, we don't want to copy all the files
  $dir =~ s/^\.//;
  $dir = &getcwd . $dir;
  $dir =~ s@\\@/@g;
  if ($dir =~ m@/msvc$@) {
    @files = grep { /(\.h|\.mak|\.sed|\.pl|\.pm|\.bat|\.inc|\.version)$/ } @files;
  } 
  # only .mak and .rc files under most directories
  elsif ($dir eq '.' || ($dir =~ m@/(texk|libs|utils)@ && $dir !~ m@$ignore_regexp@xo)) {
#    map { if ($dir =~ m@[/\\]$_@) { @files = (); return @files; } } @ignore;
    @files = grep { (-d "$dir/$_") || ($_ =~ m/(\.mak|\.rc)$/) } @files;
  }
  # don't bother with other directories
  else {
    @files = ();
  }
  return @files;
}

sub restrict_unix {
  my ($dir, $dst, @files) = @_;
  $dir =~ s/^\.//;
  $dir = &getcwd . $dir;
  $dir =~ s@\\@/@g;
  # Unix autoconf files
  if ($dir eq '.' || $dir =~ m@[/\\](texk|libs|utils)@ && $dir !~ m@$ignore_regexp@xo) {
    @files = grep { /(Makefile\.in|config|configure|configure\.in|config\.guess|config\.sub|c-auto\.in|\.mk)$/ || (-d "$dir/$_") } @files;
  }
  # don't bother with other directories
  else {
    @files = ();
  }
  return @files;
}

sub restrict_mswin32 {
  my ($dir, $dst, @files) = @_;

  @files = grep /(README|\.exclude|\*.pm|\.pl|\.bat|\.txt)$/, @files;

}

sub restrict_standard {
  my ($dir, $dst, @files) = @_;
  $dir =~ s/^\.//;
  $dir = &getcwd . $dir;
  $dir =~ s@\\@/@g;

  if ($dir =~ m@/CVS$@) {
    @files = ();
  }
  else {
    @files = grep { $_ !~ /^(\.nosearch|\.cvsignore)$/ } @files ;
  }

  return @files;
}

sub main {
  
  unless (&NGetOpt ("dry", "mirror", "sources", "texmf", "restrict")) {
    print STDERR "Try `$0 --help'";
    exit 1;
  }

  $opt_verbose = 1;

  if ($opt_sources) {
    local ($f1, $f2);
    $f1 = "c:/source/fptex/libs";
    $f2 = "c:/source/texlive/Build/source/TeX/libs";
    &sync_dir($f1, $f2, \&restrict_win, "", $opt_dry != 0, $opt_mirror != 0, 1);
    &sync_dir($f2, $f1, \&restrict_unix, "", $opt_dry != 0, $opt_mirror != 0, 1);
    $f2 = "c:/source/texlive/Build/source.development/TeX/libs";
    &sync_dir($f1, $f2, \&restrict_win, "", $opt_dry != 0, $opt_mirror != 0, 1);
    &sync_dir($f2, $f1, \&restrict_unix, "", $opt_dry != 0, $opt_mirror != 0, 1);

    $f1 = "c:/source/fptex/utils";
    $f2 = "c:/source/texlive/Build/source/TeX/utils";
    &sync_dir($f1, $f2, \&restrict_win, "", $opt_dry != 0, $opt_mirror != 0, 1);
    &sync_dir($f2, $f1, \&restrict_unix, "", $opt_dry != 0, $opt_mirror != 0, 1);
    $f2 = "c:/source/texlive/Build/source.development/TeX/utils";
    &sync_dir($f1, $f2, \&restrict_win, "", $opt_dry != 0, $opt_mirror != 0, 1);
    &sync_dir($f2, $f1, \&restrict_unix, "", $opt_dry != 0, $opt_mirror != 0, 1);

    $f1 = "c:/source/fptex/texk";
    $f2 = "c:/source/texlive/Build/source/TeX/texk";
    &sync_dir($f1, $f2, \&restrict_win, "", $opt_dry != 0, $opt_mirror != 0, 1);
    &sync_dir($f2, $f1, \&restrict_unix, "", $opt_dry != 0, $opt_mirror != 0, 1);

    $f1 = "c:/source/fptex/texk.development";
    $f2 = "c:/source/texlive/Build/source.development/TeX/texk";
    &sync_dir($f1, $f2, \&restrict_win, "", $opt_dry != 0, $opt_mirror != 0, 1);
    &sync_dir($f2, $f1, \&restrict_unix, "", $opt_dry != 0, $opt_mirror != 0, 1);

    $f1 = "c:/source/fptex/mswin32";
    $f2 = "c:/source/texlive/Build/source/TeX/mswin32";
    &sync_dir($f1, $f2, \&restrict_mswin32, "", $opt_dry != 0, $opt_mirror != 0, 1);

    $f1 = "c:/source/fptex/mswin32";
    $f2 = "c:/source/texlive/Build/source.development/TeX/mswin32";
    &sync_dir($f1, $f2, \&restrict_mswin32, "", $opt_dry != 0, $opt_mirror != 0, 1);

  }
  elsif ($opt_texmf) {
    local ($f1, $f2) = ("c:/source/texlive/Master/texmf",
			"c:/Program Files/TeXLive/texmf");
    my $restrict = ($opt_restrict ? \&restrict_standard : "");
    &sync_dir($f1, $f2, $restrict , "", $opt_dry != 0, $opt_mirror != 0, 0);    
    ($f1, $f2) = ("c:/source/texlive/Master/texmf-dist",
		  "c:/Program Files/TeXLive/texmf-dist");
    &sync_dir($f1, $f2, $restrict, "", $opt_dry != 0, $opt_mirror != 0, 0);    
    ($f1, $f2) = ("c:/source/texlive/Master/texmf-doc",
		  "c:/Program Files/TeXLive/texmf-doc");
    &sync_dir($f1, $f2, $restrict, "", $opt_dry != 0, $opt_mirror != 0, 0);    
  }
  else {
    local ($f1, $f2) = @ARGV;

    die "$f1 is not a directory!\n" if (! -d $f1);
    die "$f2 is not a directory!\n" if (! -d $f2);

    $f1 = &canon_dir( &build_path(&getcwd, "$f1")) if (! &is_absolute($f1));
    $f2 = &canon_dir( &build_path(&getcwd, "$f2")) if (! &is_absolute($f2));
    
    if ($opt_restrict) {
      &sync_dir($f1, $f2, \&restrict_standard, "", $opt_dry != 0, $opt_mirror != 0, 0);
    }
    else {
      &sync_dir($f1, $f2, "", "", $opt_dry != 0, $opt_mirror != 0, 0);
    }
  }
}

