#
# Build tool for Web2C & supp
# Author: F. Popineau
# Date  : 04/11/98
# Time-stamp: <04/03/19 00:52:22 popineau>
#
# Usage: 
#        build <option>
#              [--static]         builds/cleans up statically
#              [--debug]          builds/cleans up in debug mode
#              [--depend]         updates dependencies
#              [--clean]          cleans up 
#              [--distclean]      removes everything that can be rebuild (needs to configure again)
#              [--dir <dir>]      only in <dir> (relative to top !)
#              [--install]        installs what has been built (incompaible with clean up targets)
#              [--from-scratch]   full build, removes everything from the destination tree.
#              [--log <file>]     logs everything onto <file>
#
#        Hope these options are clear enough. This is a wrapper to nmake calls.
#

BEGIN {
push @INC, 'c:/source/TeXLive/Master/Tools';
}

require "newgetopt.pl";
use lib qw (. ./msvc);
use FileUtils qw(canon_dir cleandir make_link newpath 
		  normalize substitute_var_val dirname
		  rec_rmdir sync_dir walk_dir start_redirection
		  stop_redirection sync_dir);
use Tpm;
use Cwd;
use Time::gmtime;

# list if functions to be called before nmake is run
@prepare_func_list = ( );

$fpTeXVersion = "0.7";
$FtpDir = "c:/InetPub/ftp/fptex/$fpTeXVersion";

$Gm = gmtime();
$Date = sprintf("%02d-%02d-%02d", $Gm->year() + 1900, $Gm->mon(), $Gm->wday());
# root directory
$topdir = &dirname(cwd());
$win32_dir = "$topdir/mswin32";
$msvc_dir = "$topdir/texk/msvc";
$tpmdir = "$win32_dir/factory/texmf-dist/tpm";
$zipsrc_name = "$FtpDir/sources/fptex-$fpTeXVersion-$Date.zip";
$destdir="c:/Program Files/TeXLive";
$logfile = "";

# the directories for which we build standalone packages
# (aka: no DLL)
@src_standalone = ( "libs/libgnuw32", "libs/libgsw32", "libs/geturl", 
		    "libs/zlib", "libs/libpng", "libs/jpeg", "libs/xpdf",
		    "utils/bzip2", "utils/gzip", "utils/jpeg2ps",
		    "texk/kpathsea", "texk/contrib",
		    "texk/web2c",
		    "texk/windvi", "texk/dvipsk",
		    "texk/odvipsk", "texk/owindvi",
		    "texk/gsftopk", "texk/tex4htk",
		    "texk/dvipdfm", "texk/dvipdfmx",
		    "utils/lcdf-typetools"
		   );

# list of directories in $(prefix) that will be created by nmake
# and that will be removed before anything if asking to install `from_scratch'
@fptex_dirs = ( "bin", "include", "lib", "setupw32" );

# directories that should not end up in the zip archive
# of sources
@zipsrc_exclude_dirs = (
			"/(dynamic|dynamic-debug|static|static-debug|Release|Debug)\$",
			"/win32/(diff|setupw32|factory|ttfinst|worked-on)\$",
			"/win32/(deutsch|english|fptex.old|french|labels|slovak|tds)\$",
			"/utils/rxp-1.2\$",
			"/texk/(ltx2rtf-old|windvi-0.68|texdoctk-0.3.0|texdoctk-0.4.0)\$"
		       );

@zipsrc_exclude_patterns = (
 "\.(obj|dll|exe|opt)\$",
 "~\$"
);


&main;

1;

sub help
{
    print "Usage: build [options]\n";
    print "\n";
    print "    --dir <path>     Apply to the subdir <path> (relative);\n";
    print "    --log <file>     Redirect stdin and stderr in <file>\n";
    print "    --static         Build statically linked versions;\n";
    print "    --debug          Build debug versions;\n";
    print "    --sync           Sync up with master;\n";
    print "    --standalone     Build standalone packages (ie: static, nodebug);\n";
    print "    --zipsrc(=clean) Build a source archive, (possibly cleaning the tree before);\n";
    print "\n";		       
    print "    Targets :\n";   
    print "      <none>         Compiles;\n";
    print "    --clean          Clean up last build (only compilation);\n";
    print "    --distclean      Clean up last build (everything, must configure again);\n";
    print "    --build          Build everything;\n";
    print "    --install        Install everything build;\n";
    print "    These targets are exclusive !\n";
    print "\n";
    print "    --from_scratch   Use with --install and --build only,\n";
    print "                     will remove everything from <prefix> directory,\n";
    print "    --prefix         Used with --install only,\n";
    print "                     will change temporarily the <prefix> directory,\n"; 
    print "\n";
    print "    Support commands:\n";
    print "    --help           displays this help text and exits\n";
    print "    --verbose        some more messages\n";
}

sub read_config_var
{
    local($file, $varname) = @_;
    local($val);
    open VER, "<$file";
    while (<VER>) {
      if ($_ =~ m/^$varname\s*=\s*(.*)$/) {
	$val = $1;
      }
    }
    close VER;
    return $val;
}

sub make_absolute
{
  local ($rel) = @_;
  local ($current, $abs);
  $current = cwd();
  chdir $rel;
  $abs = cwd();
  print "current = $current, rel = $rel, abs = $abs\n";
  chdir $current;
  return $abs;
}

sub from_scratch
{
  local ($d, $prefix, $l, $highest, $srcdir, $dstdir);

  $prefix = &read_config_var("$msvc_dir/paths.mak", "prefix");
  $prefix = &canon_dir($prefix, '\\');
  print "Prefix directory is $prefix .\n" if $opt_verbose;

  # Removing every fpTeX thing from prefix directory
  foreach $d ( @fptex_dirs ) {
    if (-d "$prefix\\$d") {
      &cleandir("$prefix\\$d");
      rmdir "$prefix\\$d";
    }
  }

  # Add an option to sync up with the TeXLive texmf tree
}

sub mask_env {
  $ENV{"SystemDrive"} = "foo";
  $ENV{"TEXMFCNF"} = "foo";
  $ENV{"TEXMFMAIN"} = "foo";
#  $ENV{"TEXMF"} = "foo";
  $ENV{"LATEX2HTML"} = "foo";
  $ENV{"GROFF_TMAC_PATH"}="foo";
  $ENV{"GROFF_FONT_PATH"}="foo";
}

sub ziplist {
  my ($dir, @l) = @_;
  $dir =~ s@\\@/@g;
  map {
    if (! -d "$dir/$_") {
      for $pattern (@zipsrc_exclude_patterns) {
	if ("$dir/$_" =~ m@$pattern@i) {
	  goto suivant;
	}
      }
      print STDERR "$dir/$_\n" if ($opt_verbose);
      print ZIP "$dir/$_\n";
    }
  suivant:
  } @l;
}

sub zipprune {
  my ($dir) = @_;
  $dir =~ s@\\@/@g;
  map {
    if ($dir =~ m@$_@) {
      print STDERR "Pruning $dir...\n" if ($opt_verbose);
      return 1;
    }
  } @zipsrc_exclude_dirs;
  return 0;
}

sub do_zipsrc {
  unlink $zipsrc_name if (-f $zipsrc_name);
  open ZIP, "| zip -9\@or " . $zipsrc_name . " > nul";
  &walk_dir($topdir, \&ziplist, \&zipprune);
  close ZIP;
}

sub do_standalone {
  local($cmd, $cwd);

  # make the static binaries needed
  print STDERR "Installing in $prefix\n";

  &mask_env;

  &start_redirection($logfile);

  my $devel_flag = " DEVELOPMENT=1 " if ($opt_development);
  for $d (@src_standalone) {
    $d =~ s/texk/texk.development/ if ($opt_development);
    my $cwd = &getcwd;
    print "Changing directory to $topdir/${d}\n" if $opt_verbose;
    chdir("$topdir/${d}");
    $cmd = "nmake -f win32.mak NODEBUG=1 STATIC=1 ${devel_flag} all";
    print "Running $cmd\n" if $opt_verbose;
    system($cmd);
    $cmd = "nmake -f win32.mak NODEBUG=1 STATIC=1 ${devel_flag} install";
    print "Running $cmd\n" if $opt_verbose;
    system($cmd);
    chdir($cwd)
  }

  &stop_redirection($logfile);
}

sub do_sync {

  $cmd = "p4 sync";
  system($cmd);

  &sync_dir("$masterdir/texmf-dist", "$destdir/texmf-dist");
  &sync_dir("$masterdir/texmf", "$destdir/texmf");
  &sync_dir("$masterdir/texmf/web2c", "$destdir/texmf-var/web2c");

}

sub main {

  local ($changed_prefix);

  unless (&NGetOpt ("static", "debug", 
		    "clean", "distclean:s" ,
		    "depend","from_scratch",
		    "build:s", "install:s",
		    "zipsrc:s",
		    "standalone", "sync",
		    "development",
		    "dir=s", "log=s", "prefix=s",
		    "verbose", "help"))
    {
	print STDERR "Try `configure --help'";
	exit 1;
    }

  if ($opt_help) {
    &help;
    exit 0;
  }

  $prefix = &read_config_var("$msvc_dir/paths.mak", "prefix");

  if ($opt_log) {
    $logfile = "$win32_dir\\$opt_log";
    if (! $opt_log =~ /\.log$/) {
      $opt_log .= ".log";
    }
  }

#    $topdir .= "/source";
#    if ($opt_development) {
#      $topdir .= ".development";
#    }

#    $win32_dir = eval($win32_dir);
#    $tpmdir = eval($tpmdir);

  if ($opt_standalone) {
    &do_standalone;
    exit 0;
  }

  if (defined($opt_zipsrc) && $opt_zipsrc ne "clean") {
    &do_zipsrc;
    exit 0;
  }
  if (defined($opt_zipsrc) && $opt_zipsrc eq "clean") {
    $opt_distclean = 1;
  }

  # build the command line
  $prog = "nmake -f win32.mak ";
  $prog .= "verbose=on " if ($opt_verbose);
  if ($opt_debug eq '') {
    $prog .= "NODEBUG=1 ";
  }
  if ($opt_static) {
    $prog .= "STATIC=1 ";
  }

  if ($opt_development) {
    $prog .= "DEVELOPMENT=1 ";
  }

  if ($opt_depend) {
    $prog .= "MAINT=1 ";
    $target = "depend ";
  }
  if (defined($opt_build)) {
    if ($target ne '') {
      print "Conflicting targets : $target and build\n";
      return 1;
    }
    $target = "all ";
    if ($opt_from_scratch) {
	push @prepare_func_list , ( \&from_scratch );
    }
    $target = "XEMTEX=1 $target" if ($opt_build =~ m/xemtex/i);
    $target = "NETPBM=1 $target" if ($opt_build =~ m/^netpbm$/i);
    $target = "GNU=1 $target" if ($opt_build =~ m/^gnu$/i);
    print "target = $target\n";
  }
  if (defined($opt_install)) {
    if ($target ne '') {
      print "Conflicting targets : $target and install\n";
      return 1;
    }
    $target = "install ";
    if ($opt_from_scratch) {
	push @prepare_func_list , ( \&from_scratch );
    }
    $target = "XEMTEX=1 $target" if ($opt_install =~ m/xemtex/i);
    $target = "NETPBM=1 $target" if ($opt_install =~ m/^netpbm$/i);
    $target = "GNU=1 $target" if ($opt_install =~ m/^gnu$/i);
    print "target = $target\n";
  }
  if ($opt_clean) {
    if ($target ne '') {
      print "Conflicting targets : $target and clean\n";
      return 1;
    }
    $target = "clean ";
  }
  if (defined($opt_distclean)) {
    if ($target ne '') {
      print "Conflicting targets : $target and distclean\n";
      return 1;
    }
    $prog .= "MAINT=1 ";
    $target = "distclean ";
    $target = "XEMTEX=1 $target" if ($opt_distclean =~ m/xemtex/i);
    $target = "NETPBM=1 $target" if ($opt_distclean =~ m/^netpbm$/i);
    $target = "GNU=1 $target" if ($opt_distclean =~ m/^gnu$/i);
    print "target = $target\n";
  }

  if ($target eq '') {
      print "No target specified.\nEnd.\n";
      exit 0;
  }

  if ($opt_prefix) {
    if ($target ne "install ") {
      print STDERR "$0: --prefix option ignored, wrong target.\n";
    }
    elsif (! -d $opt_prefix) {
      print STDERR "$0: $opt_prefix is not a directory, aborting.\n";
      exit 1;
    }
    else {
      $opt_prefix = &normalize($opt_prefix);
      if ($prefix ne $opt_prefix) {
	print STDERR "Replacing prefix $prefix with $opt_prefix.\n";
	@old_prefix = &substitute_var_val("$msvc_dir/paths.mak", 
					  "prefix", "$opt_prefix");
	$changed_prefix = 1;
	$old_prefix = $prefix;
	$prefix = $opt_prefix;
      }
    }
  }

  print STDERR "Installing in $prefix\n";

  &mask_env;

  $prog .= $target;

  $buildir = $topdir;

  if ($opt_dir) {
    $buildir .= "\\$opt_dir";
  }

  print STDERR "buildir = $buildir\n";

  if (-d $buildir) {
	chdir $buildir;
  }
  else {
	print "Non-existent dir: $opt_dir\n";
	return 1;
  }

  # start redirection if asked
  &start_redirection($logfile);

  if ($opt_sync) {
    &do_sync;
  }

  # Prepare functions
  for $f (@prepare_func_list) {
    if ($f) {
      &{$f};
    }
  }

  # Call the program
  #`$prog`;
  print STDOUT "Executing $prog in " . &getcwd . "\n";
  system($prog);

  # stop redirection
  stop_redirection($logfile);

  chdir $topdir;

  if ($#old_prefix > -1) {
    &substitute_var_val("$msvc_dir/paths.mk", 
			"prefix", shift @old_prefix);
  }

  if ($opt_distclean && $opt_zipsrc) {
    &do_zipsrc;
  }

  print "This is the end!\n";
}
