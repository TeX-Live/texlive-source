#
# Perforce interface for TeXLive
# Author: F. Popineau
# Date  : 04/11/98
# Time-stamp: <03/03/16 12:22:48 popineau>
#
# Usage: 
#        p4update <option>
#
#

use lib 'c:/source/TeXLive/Master/Tools';

require "newgetopt.pl";

use FileUtils qw(dirname basename normalize walk_dir cleandir sort_uniq 
		 copy remove_list push_uniq newer sync_dir build_path);

use Cwd;

$ENV{P4PORT} = "xemtex.ese-metz.fr:1666";
$ENV{P4CLIENT} = "ansible";

%root = ( 
	 "xemtex" =>"c:/source/xemtex",
	 "texlive" => "c:/source/TeXLive"
	);
%p4port = ( 
	 "xemtex" =>"xemtex.ese-metz.fr:1666",
	 "texlive" => "sun.dante.de:1666"
	);
%p4client = (
	   "xemtex" => "ansible",
	   "texlive" => "ansible"
	  );

&main;
0;

sub main {

  unless (&NGetOpt (
		    "project=s", # Name of the project, changes the P4PORT and P4CLIENT variables
		    "subdir=s", # Subdir to restrict the sync/update operation
		    "dry",	# Do nothing, just show what commands would be run
		    "sync",	# Sync local depot with remote files
		    "update",	# Push your local changes to the remote depot
		    "submit=s"	# Run 'p4 submit' in addition to update
		   )) {
    print STDERR "Try `$0 --help'";
    exit 1;
  }

  if (! $opt_project) {
    $opt_project = "texlive";
  }

  my $rootprj = $root{$opt_project};
  my $p4clientprj = $p4client{$opt_project};
  my $p4portprj = $p4port{$opt_project};
  my $p4rootprj = "//$p4clientprj";
  $ENV{"P4CLIENT"} = $p4clientprj;
  $ENV{"P4PORT"} = $p4portprj;
  my $subdir = "";
  my $dir = "";


  if ($opt_subdir ne "") {
    $subdir = $opt_subdir;
  }

  $dir = build_path($rootprj, $subdir);

  if (! -d $dir) {
    print STDERR "$0: $dir is not a directory, exiting...\n";
    exit 1;

  }

  print "P4PORT = " . $ENV{"P4PORT"} . "\nP4CLIENT = " . $ENV{"P4CLIENT"} . "\n";

  $cwd = &getcwd;

  if ($opt_sync) {
    $dir = normalize($dir, "/");
    $dir =~ s/^$rootprj/$p4rootprj/;
    $dir = $dir . "/...";
    if ($opt_dry) {
      print STDERR "Executing \"p4 sync $dir\"\n";
    }
    else {
      system("p4 sync $dir");
    }
  }
  elsif ($opt_update) {
    if ($opt_dry) {
      print STDERR "Changing directory for $dir\n";
      print STDERR "Executing \"dir /s /b /a-d | p4 -x - add\"\n";
      print STDERR "Changing directory for $cwd\n";
    }
    else {
      chdir($dir);
      system("dir /s /b /a-d | p4 -x - add");
      chdir ($cwd);
    }
    $dir = normalize($dir, "/");
    $dir =~ s/^$rootprj/$p4rootprj/;
    $dir = $dir . "/...";
    
    if ($opt_dry) {
      print STDERR "Executing \"p4 diff -sd $dir | p4 -x - delete\"\n";
    print STDERR "Executing \"p4 diff -se $dir | p4 -x - edit\"\n";
    }
    else {
      system("p4 diff -sd $dir | p4 -x - delete");
      system("p4 diff -se $dir | p4 -x - edit");
    }
    if ($opt_submit) {
      $opt_submit = $dir if ($opt_submit !~ m/-c/);
      if ($opt_dry) {
	print STDERR "Executing \"p4 submit $opt_submit\"\n";
      }
      else {
	system("p4 submit $opt_submit");
      }
    }
  }
  elsif ($opt_submit) {
    $dir = normalize($dir, "/");
    $dir =~ s/^$rootprj/$p4rootprj/;
    $dir = $dir . "/...";
    $opt_submit = $dir if ($opt_submit !~ m/-c/);
    if ($opt_dry) {
      print STDERR "Executing \"p4 submit $opt_submit\"\n";
    }
    else {
      system("p4 submit $opt_submit");
    }
  }
}
