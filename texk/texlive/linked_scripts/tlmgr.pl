#!/usr/bin/env perl
# $Id: tlmgr.pl 8708 2008-06-13 11:56:01Z preining $
#
# Copyright 2008 Norbert Preining
# This file is licensed under the GNU General Public License version 2
# or any later version.
#
# TODO:
# - ordering or collections removal (see below for details)
# - (?) removal does not remove created format files from TEXMFSYSVAR
# - other features: dependency check?, ...?

my $svnrev = '$Revision: 8708 $';
$svnrev =~ m/: ([0-9]+) /;
my $tlmgrrevision = $1;

my $Master;

BEGIN {
  $^W = 1;
  $Master = `kpsewhich -var-value=SELFAUTOPARENT`;
  chomp($Master);
  unshift (@INC, "$Master/tlpkg");
}

use TeXLive::TLPOBJ;
use TeXLive::TLPDB;
use TeXLive::TLConfig;
use TeXLive::TLMedia;
use TeXLive::TLUtils;
use TeXLive::TLWinGoo;
use TeXLive::TLPostActions;
TeXLive::TLUtils->import( qw(kpsewhich member win32 merge_into) );
use Cwd qw/abs_path/;
use Pod::Usage;
use Getopt::Long qw(:config no_autoabbrev require_order);
use strict;

# used variables
# the TLMedia from which we install/update
my $tlmediasrc;
# the tlpdb of the install media
my $tlpdb;
# the local tlpdb
my $localtlpdb;
# a hash for saving the options saved into the local tlpdb
my %options;
# the location from where we install
my $location;

# option handling
my $opt_location;
my $opt_help = 0;
my $opt_version = 0;
my $opt_netarchive;
my $opt_diskarchive;
my $opt_gui = 0;
my $opt_gui_lang;

TeXLive::TLUtils::process_logging_options();

GetOptions("location=s" => \$opt_location,
           "netarchive=s" => \$opt_netarchive,
           "diskarchive=s" => \$opt_diskarchive,
           "gui" => \$opt_gui,
           "gui-lang=s" => \$opt_gui_lang,
           "version" => \$opt_version,
           "h|help|?" => \$opt_help) or pod2usage(2);

my $action = shift;

if (!defined($action) && $opt_gui) {
  $action = "gui";
}

if ($opt_version) {
  print "tlmgr revision $tlmgrrevision\n";
  exit(0);
}

if (!defined($action) && !$opt_help) {
  print "missing action\ntry 'tlmgr help' for more information.\n";
  exit 1;
}

if (defined($action) && ($action =~ m/^help/i)) {
  $opt_help = 1;
}

$NetArchive = $opt_netarchive if (defined($opt_netarchive));
$DiskArchive = $opt_diskarchive if (defined($opt_diskarchive));

if ($^O=~/^MSWin(32|64)$/i) {
  pod2usage(-exitstatus => 0, -verbose => 2, -noperldoc => 1) if $opt_help;
} else {
  pod2usage(-exitstatus => 0, -verbose => 2) if $opt_help;
}

my $loadmediasrcerror = "Cannot load TeX Live Database from ";
my %ret;

if ($action =~ m/^generate$/i) {
  merge_into(\%ret, action_generate());
} elsif ($action =~ m/^gui$/i) {
  action_gui();
} elsif ($action =~ m/^arch$/i) {
  merge_into(\%ret, action_arch());
} elsif ($action =~ m/^option$/i) {
  merge_into(\%ret, action_option());
  # should we do postinstall stuff?
  exit(0);
} elsif ($action =~ m/^list$/i) {
  merge_into(\%ret, action_list());
  # should we do postinstall stuff?
  exit(0);
} elsif ($action =~ m/^install$/i) {
  merge_into(\%ret, action_install());
} elsif ($action =~ m/^update$/i) {
  merge_into(\%ret, action_update());
} elsif ($action =~ m/^backup$/i) {
  merge_into(\%ret, action_backup());
} elsif ($action =~ m/^restore$/i) {
  merge_into(\%ret, action_restore());
} elsif ($action =~ m/^search$/i) {
  merge_into(\%ret, action_search());
  exit(0);
} elsif ($action =~ m/^show$/i) {
  merge_into(\%ret, action_show());
  exit(0);
} elsif ($action =~ m/^remove$/i) {
  merge_into(\%ret, action_remove());
} elsif (($action eq "paper") || ($action eq "dvips") || ($action eq "xdvi") ||
         ($action eq "pdftex") || ($action eq "dvipdfm") ||
         ($action eq "dvipdfmx")) {
  if ($opt_gui) {
    action_gui("config");
  }
  my $TEXMFMAIN = `kpsewhich -var-value=TEXMFMAIN`;
  chomp $TEXMFMAIN;
  system("texlua", "$TEXMFMAIN/scripts/texlive/texconf.tlu", "--prog", "tlmgr", "--sys", $action, @ARGV);
} elsif ($action =~ m/^uninstall$/i) {
  merge_into(\%ret, action_uninstall());
  exit(0);
} else {
  die "Unknown action: $action";
}


######################################

#
# checking the %ret hash
if (defined($ret{'mktexlsr'})) {
  print "running mktexlsr\n";
  system("mktexlsr");
}
my $TEXMFSYSVAR = `kpsewhich -var-value=TEXMFSYSVAR`;
chomp $TEXMFSYSVAR;
my $TEXMFLOCAL = `kpsewhich -var-value=TEXMFLOCAL`;
chomp $TEXMFLOCAL;
if (defined($ret{'map'})) {
  foreach my $m (@{$ret{'map'}}) {
    print "$m\n";
    system("updmap-sys --nomkmap --nohash --$m");
  }
  system("mktexlsr");
  system("updmap-sys");
}
if (defined($ret{'format'})) {
  print "regenerating fmtutil.cnf\n";
  TeXLive::TLUtils::create_fmtutil($localtlpdb,
    "$TEXMFSYSVAR/web2c/fmtutil.cnf",
    "$TEXMFLOCAL/web2c/fmtutil-local.cnf");
  # system("texconfig-sys", "generate", "fmtutil");
  # running fmtutil-sys only in case that at installation time the
  # option for building formats was selected
  if ($localtlpdb->option_create_formats) {
    print "running fmtutil-sys --missing\n";
    system("fmtutil-sys", "--missing");
  }
}
if (defined($ret{'language'})) {
  print "regenerating language.dat\n";
  TeXLive::TLUtils::create_language($localtlpdb,
    "$TEXMFSYSVAR/tex/generic/config/language.dat",
    "$TEXMFLOCAL/tex/generic/config/language-local.dat");
  # system("texconfig-sys", "generate", "language");
  #
  # win32 fmtutil needs language.dat WITHOUT full specification
  if (TeXLive::TLUtils::win32()) {
    print "running fmtutil-sys --byhyphen language.dat\n";
    system("fmtutil-sys", "--byhyphen", "language.dat");
  } else {
    print "running fmtutil-sys --byhyphen $TEXMFSYSVAR/tex/generic/config/language.dat\n";
    system("fmtutil-sys", "--byhyphen", "$TEXMFSYSVAR/tex/generic/config/language.dat");
  }
}

exit(0);

#
# remove_package removes a single package with all files (including the
# tlpobj files) and the entry from the tlpdb.
#
sub remove_package {
  my ($pkg, $localtlpdb, $force) = @_;
  my $tlp = $localtlpdb->get_package($pkg);
  my %ret;
  if (!defined($tlp)) {
    print "Package $pkg not present, cannot remove it!\n";
  } else {
    if (!$force) {
      # do some checking
      my @needed = $localtlpdb->needed_by($pkg);
      if (@needed) {
        print "$pkg: not removed, referenced in @needed\n";
        return;
      }
    }
    if ($pkg eq "texlive.infra") {
      log("Not removing texlive.infra, it is essential!\n");
      return;
    }
    print "remove: $pkg\n";
    # we have to chdir to $localtlpdb->root
    my $Master = $localtlpdb->root;
    chdir ($Master) || die "chdir($Master) failed: $!";
    my @files = $tlp->all_files;
    # also remove the .tlpobj file
    push @files, "tlpkg/tlpobj/$pkg.tlpobj";
    # and the ones from src/doc splitting
    if (-r "tlpkg/tlpobj/$pkg.src.tlpobj") {
      push @files, "tlpkg/tlpobj/$pkg.src.tlpobj";
    }
    if (-r "tlpkg/tlpobj/$pkg.doc.tlpobj") {
      push @files, "tlpkg/tlpobj/$pkg.doc.tlpobj";
    }
    my @removals = &removed_dirs (@files);
    foreach my $entry (@files) {
      unlink $entry;
    }
    foreach my $entry (@removals) {
      rmdir $entry;
    }
    $localtlpdb->remove_package($pkg);
    merge_into(\%ret, $tlp->make_return_hash_from_executes("disable"));
    $ret{'mktexlsr'} = 1;
    # remove those bin dependencies .ARCH
    foreach my $d ($tlp->depends) {
      if ($d eq "$pkg.ARCH") {
        foreach my $a ($localtlpdb->available_architectures) {
          merge_into(\%ret,
                     &remove_package("$pkg.$a", $localtlpdb, $force));
        }
      }
    }
    # should we save at each removal???
    # advantage: the tlpdb actually reflects what is installed
    # disadvantage: removing a collection calls the save routine several times
    # still I consider it better that the tlpdb is in a consistent state
    $localtlpdb->save;
    # do the post removal actions
    if (defined($PostRemove{$pkg})) {
      info("running post remove action for $pkg\n");
      &{$PostRemove{$pkg}}($localtlpdb->root);
    }
  }
  return \%ret;
}

sub action_remove {
  if ($opt_gui) {
    action_gui("remove");
  }
  # we do the following:
  # - (not implemented) order collections such that those depending on
  #   other collections are first removed, and then those which only
  #   depend on packages. Otherwise
  #     remove collection-latex collection-latexrecommended
  #   will not succeed
  # - first loop over all cmd line args and consider only the collections
  # - for each to be removed collection:
  #   . check that no other collections/scheme asks for that collection
  #   . remove the collection
  #   . remove all dependencies
  # - for each normal package not already removed (via the above)
  #   . check that no collection/scheme still depends on this package
  #   . remove the package
  #
  my %ret;
  my $opt_nodepends = 0;
  my $opt_force = 0;
  my $opt_dry = 0;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("no-depends" => \$opt_nodepends,
             "force" => \$opt_force,
             "dry-run" => \$opt_dry) or pod2usage(2);
  my %already_removed;
  my @more_removal;
  init_local_db();
  foreach my $pkg (@ARGV) {
    my $tlp = $localtlpdb->get_package($pkg);
    next if defined($already_removed{$pkg});
    if (!defined($tlp)) {
      print "Package $pkg not present, cannot remove it!\n";
    } else {
      # in the first round we only remove collections, nothing else
      # but removing collections will remove all dependencies, too
      # save the information of which packages have already been removed
      # into %already_removed.
      if ($tlp->category eq "Collection") {
        # for collections we also remove all dependencies which are
        # not Collections or Schemes
        my %foo;
        if ($opt_dry) {
          print "remove: $pkg\n";
          # we need to set $foo to something positive otherwise
          # the rest will not be run in dry_run mode
          $foo{'mktexlsr'} = 1;
        } else {
          merge_into(\%foo, &remove_package($pkg, $localtlpdb, $opt_force));
        }
        if (keys %foo) {
          # removal was successful, so the return is at least 0x0001 mktexlsr
          # remove dependencies, too
          merge_into(\%ret, \%foo);
          $already_removed{$pkg} = 1;
          if (!$opt_nodepends) {
            foreach my $d ($tlp->depends) {
              my $tlpdd = $localtlpdb->get_package($d);
              if (defined($tlpdd)) {
                if ($tlpdd->category !~ m/$MetaCategoriesRegexp/) {
                  if ($opt_dry) {
                    print "remove: $d\n";
                  } else {
                    merge_into(\%ret, &remove_package($d, $localtlpdb, $opt_force));
                  }
                  $already_removed{$d} = 1;
                }
              }
            }
          }
        }
      } else {
        # save all the other packages into the @more_removal list to
        # be removed at the second state. Note that if a package has
        # already been removed due to a removal of a collection
        # it will be marked as such in %already_removed and not tried again
        push @more_removal, $pkg;
      }
    }
  }
  foreach my $pkg (@more_removal) {
    if (!defined($already_removed{$pkg})) {
      if ($opt_dry) {
        print "remove: $pkg\n";
      } else {
        my %foo;
        merge_into(\%foo, &remove_package($pkg, $localtlpdb, $opt_force));
        if (keys %foo) {
          # removal was successful
          merge_into(\%ret, \%foo);
          $already_removed{$pkg} = 1;
        }
      }
    }
  }
  if ($opt_dry) {
    # stop here, don't do any postinstall actions
    return;
  } else {
    $localtlpdb->save;
    my @foo = sort ( keys %already_removed);
    print "Actually removed packages: @foo\n";
    return(\%ret);
  }
}

sub action_show {
  if ($opt_gui) {
    action_gui("config");
  }
  init_local_db();
  foreach my $pkg (@ARGV) {
    my $tlmediatlpdb;
    my $tlp = $localtlpdb->get_package($pkg);
    my $installed = 0;
    if (!$tlp) {
      if (!$tlmediatlpdb) {
        $tlmediasrc = TeXLive::TLMedia->new($location);
        die($loadmediasrcerror . $location) unless defined($tlmediasrc);
        $tlmediatlpdb = $tlmediasrc->tlpdb;
      }
      $tlp = $tlmediatlpdb->get_package($pkg);
    } else {
      $installed = 1;
    }
    if ($tlp) {
      print "Package:   ", $tlp->name, "\n";
      print "Category:  ", $tlp->category, "\n";
      print "ShortDesc: ", $tlp->shortdesc, "\n" if ($tlp->shortdesc);
      print "LongDesc:  ", $tlp->longdesc, "\n" if ($tlp->longdesc);
      print "Installed: ", ($installed?"Yes\n":"No\n");
      print "\n";
    } else {
      printf STDERR "Cannot find $pkg\n";
    }
  }
  return;
}

sub action_search {
  my $opt_global = 0;
  my $opt_file = 0;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("global" => \$opt_global,
             "file"   => \$opt_file) or pod2usage(2);
  my $r = shift @ARGV;
  my $ret = "";
  my $tlpdb;
  init_local_db();
  if ($opt_global) {
    $tlmediasrc =  TeXLive::TLMedia->new($location);
    die($loadmediasrcerror . $location) unless defined($tlmediasrc);
    $tlpdb = $tlmediasrc->tlpdb;
  } else {
    $tlpdb = $localtlpdb;
  }
  foreach my $pkg ($tlpdb->list_packages) {
    if ($opt_file) {
      my @ret = grep(m;$r;, $tlpdb->get_package($pkg)->all_files);
      if (@ret) {
        print "$pkg:\n";
        foreach (@ret) {
          print "\t$_\n";
        }
      }
    } else {
      next if ($pkg =~ m/\./);
      my $t = $tlpdb->get_package($pkg)->shortdesc;
      $t |= "";
      my $lt = $tlpdb->get_package($pkg)->longdesc;
      $lt |= "";
      if (($pkg =~ m/$r/) || ($t =~ m/$r/) || ($lt =~ m/$r/)) {
        $ret .= " $pkg - $t\n";
      }
    }
  }
  print "$ret";
  return;
}

sub action_restore {
  # tlmgr restore --backupdir dir 
  #   lists all packages with all revisions
  # tlmgr restore --backupdir dir pkg
  #   lists all revisions of pkg
  # tlmgr restore --backupdir dir pkg rev
  #   restores pkg to revision rev
  my $opt_backupdir;
  my $opt_force;
  my $opt_dry = 0;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("backupdir=s" => \$opt_backupdir,
             "dry-run"     => \$opt_dry,
             "force"       => \$opt_force) or pod2usage(2);
  if (!defined($opt_backupdir)) {
    die "restore needs --backupdir\n";
  }
  # initialize the hash(packages) of hash(revisions)
  my %backups;
  opendir (DIR, $opt_backupdir) || die "opendir($opt_backupdir) failed: $!";
  my @dirents = readdir (DIR);
  closedir (DIR) || warn "closedir($opt_backupdir) failed: $!";
  for my $dirent (@dirents) {
    next if (-d $dirent);
    next if ($dirent !~ m/^(.*)\.r([0-9]+)\.tar\.lzma$/);
    $backups{$1}->{$2} = 1;
  }
  my $ret;
  my ($pkg, $rev) = @ARGV;
  if (!defined($pkg)) {
    if (keys %backups) {
      print "Available backups:\n";
      foreach my $p (sort keys %backups) {
        print "$p: ";
        my @rs = sort (keys %{$backups{$p}});
        print "@rs\n";
      }
    } else {
      print "No backups available in $opt_backupdir\n";
    }
    exit 0;
  }
  if (!defined($rev)) {
    print "Available backups for $pkg: ";
    my @rs = sort (keys %{$backups{$pkg}});
    print "@rs\n";
    exit 0;
  }
  # we did arrive here, so we try to restore ...
  if (defined($backups{$pkg}->{$rev})) {
    if (!$opt_force) {
      print "Do you really want to restore $pkg to revision $rev (y/N): ";
      my $yesno = <STDIN>;
      if ($yesno !~ m/^y(es)?$/i) {
        print "Ok, cancelling the restore!\n";
        exit(0);
      }
    }
    print "Restoring $pkg, $rev from $opt_backupdir/${pkg}_r${rev}.tar.lzma\n";
    if (!$opt_dry) {
      init_local_db();
      # first remove the package, then reinstall it
      # this way we get rid of useless files
      # force the deinstallation since we will reinstall it
      $opt_backupdir = abs_path($opt_backupdir);
      merge_into(\%ret, &remove_package($pkg, $localtlpdb, 1));
      TeXLive::TLMedia->_install_package("$opt_backupdir/${pkg}_r${rev}.tar.lzma" , [] ,$localtlpdb);
      # now we have to read the .tlpobj file and add it to the DB
      my $tlpobj = TeXLive::TLPOBJ->new;
      $tlpobj->from_file($localtlpdb->root . "/tlpkg/tlpobj/$pkg.tlpobj");
      $localtlpdb->add_tlpobj($tlpobj);
      $ret = $localtlpdb->get_package($pkg)->make_return_hash_from_executes("enable");
      $localtlpdb->save;
    }
  } else {
    print "revision $rev for $pkg is not present in $opt_backupdir\n";
  }
  return $ret;
}

sub action_backup {
  init_local_db();
  my $opt_dry = 0;
  my $opt_all = 0;
  my $opt_backupdir;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("all" => \$opt_all,
             "backupdir=s" => \$opt_backupdir,
            ) or pod2usage(2);
  my %ret;
  my @todo;
  die ("tlmgr backup needs an obligatory argument --backupdir") 
    unless (defined($opt_backupdir));
  if ($opt_all) {
    @todo = $localtlpdb->list_packages;
  } else {
    @todo = @ARGV;
  }
  if (!@todo) {
    printf "tlmgr backup takes either a list of packages or --all\n";
  }
  foreach my $pkg (@todo) {
    $opt_backupdir = abs_path($opt_backupdir);
    if (! -d $opt_backupdir) {
      printf STDERR "backupdir argument must be an existing directory!\n";
    } else {
      my $tlp = $localtlpdb->get_package($pkg);
      $tlp->make_container("lzma", $localtlpdb->root, 
                           $opt_backupdir, "${pkg}.r" . $tlp->revision);
    }
  }
  return(\%ret);
}

  
sub action_update {
  if ($opt_gui) {
    action_gui("update");
  }
  init_local_db();
  # initialize the TLMedia from $location
  my $opt_nodepends = 0;
  my $opt_dry = 0;
  my $opt_all = 0;
  my $opt_list = 0;
  my $opt_backupdir;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("no-depends" => \$opt_nodepends,
             "all" => \$opt_all,
             "list" => \$opt_list,
             "backupdir=s" => \$opt_backupdir,
             "dry-run" => \$opt_dry) or pod2usage(2);
  my %ret;
  $tlmediasrc = TeXLive::TLMedia->new($location);
  die($loadmediasrcerror . $location) unless defined($tlmediasrc);
  my $mediatlpdb = $tlmediasrc->tlpdb;
  my @todo;
  if ($opt_all || $opt_list) {
    @todo = $localtlpdb->list_packages;
  } else {
    @todo = @ARGV;
  }
  if (!@todo) {
    printf "tlmgr update takes either a list of packages or --all\n";
  }
  foreach my $pkg (@todo) {
    next if ($pkg =~ m/^00texlive/);
    # it looks like that can actually be done!!!
    # It gives several warnings but afterwards the files are changed. Strange
    #if (win32() && (($pkg eq "texlive.infra") || ($pkg eq "bin-texlive"))) {
    #  info("We cannot upgrade $pkg on win32, since we are running it!\n");
    #  next;
    #}
    my $tlp = $localtlpdb->get_package($pkg);
    if (!defined($tlp)) {
      printf STDERR "Strange, $pkg cannot be found!\n";
      next;
    }
    my $rev = $tlp->revision;
    my $mediatlp = $mediatlpdb->get_package($pkg);
    if (!defined($mediatlp)) {
      print "$pkg cannot be found in $location\n";
      next;
    }
    my $mediarev = $mediatlp->revision;
    if ($rev < $mediarev) {
      if ($opt_list) {
        print "$pkg: local: $rev, source: $mediarev\n";
      } elsif ($opt_dry) {
        print "Installing $pkg\n";
      } else {
        # first remove the package, then reinstall it
        # this way we get rid of useless files
        # force the deinstallation since we will reinstall it
        if ($opt_backupdir) {
          $opt_backupdir = abs_path($opt_backupdir);
          if (! -d $opt_backupdir) {
            printf STDERR "backupdir argument must be an existing directory!\n";
          } else {
            my $tlp = $localtlpdb->get_package($pkg);
            $tlp->make_container("lzma", $localtlpdb->root, 
                                 $opt_backupdir, "${pkg}.r" . $tlp->revision);
          }
        }
        print "update: $pkg (first remove old, then install new)\n";
        merge_into(\%ret, &remove_package($pkg, $localtlpdb, 1));
        merge_into(\%ret, $tlmediasrc->install_package($pkg, $localtlpdb, $opt_nodepends, 0));
        print "update: $pkg done\n";
      }
    } elsif ($rev > $mediarev) {
      print "$pkg: revision in $location is less then local revision, not updating!\n";
      next;
    }
  }
  return(\%ret);
}

sub action_install {
  if ($opt_gui) {
    action_gui("install");
  }
  init_local_db();
  # initialize the TLMedia from $location
  my $opt_nodepends = 0;
  my $opt_dry = 0;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("no-depends" => \$opt_nodepends,
             "dry-run" => \$opt_dry) or pod2usage(2);
  my %ret;
  $tlmediasrc = TeXLive::TLMedia->new($location);
  die($loadmediasrcerror . $location) unless defined($tlmediasrc);
  foreach my $pkg (@ARGV) {
    if ($opt_dry) {
      print "install: $pkg\n";
    } else {
      # install the packages and run postinstall actions (that is the 0)
      merge_into(\%ret, $tlmediasrc->install_package($pkg, $localtlpdb, $opt_nodepends, 0));
    }
  }
  if ($opt_dry) {
    # stop here, don't do any postinstall actions
    return(0);
  }
  return(\%ret);
}

sub action_list {
  init_local_db();
  my $what = shift @ARGV;
  $what || ($what = "");
  $tlmediasrc = TeXLive::TLMedia->new($location);
  die($loadmediasrcerror . $location) unless defined($tlmediasrc);
  my @whattolist;
  if ($what =~ m/^collection/i) {
    @whattolist = $tlmediasrc->tlpdb->collections;
  } elsif ($what =~ m/^scheme/i) {
    @whattolist = $tlmediasrc->tlpdb->schemes;
  } else {
    @whattolist = $tlmediasrc->tlpdb->list_packages;
  }
  foreach (@whattolist) {
    if (defined($localtlpdb->get_package($_))) {
      print "i ";
    } else {
      print "  ";
    }
    my $foo = $tlmediasrc->tlpdb->get_package($_)->shortdesc;
    print "$_: ", defined($foo) ? $foo : "(shortdesc missing)" , "\n";
  }
  return;
}

sub action_option {
  if ($opt_gui) {
    action_gui("config");
  }
  my $what = shift @ARGV;
  $what = "show" unless defined($what);
  init_local_db();
  if ($what =~ m/^location$/i) {
    # changes the default location
    my $loc = shift @ARGV;
    if ($loc) {
      print "Setting default installation source to $loc!\n";
      $localtlpdb->option_location($loc);
      $localtlpdb->save;
    } else {
      print "Default installation source: ", $localtlpdb->option_location, "\n";
    }
  } elsif ($what =~ m/^docfiles$/i) {
    # changes the default docfiles
    my $loc = shift @ARGV;
    if (defined($loc)) {
      print "Defaulting to", ($loc ? "" : " not"), " install documentation files!\n";
      $localtlpdb->option_install_docfiles($loc);
      $localtlpdb->save;
    } else {
      print "Install documentation files: ", $localtlpdb->option_install_docfiles, "\n";
    }
  } elsif ($what =~ m/^srcfiles$/i) {
    # changes the default srcfiles
    my $loc = shift @ARGV;
    if (defined($loc)) {
      print "Defaulting to", ($loc ? "" : " not"), " install source files!\n";
      $localtlpdb->option_install_srcfiles($loc);
      $localtlpdb->save;
    } else {
      print "Install source files: ", $localtlpdb->option_install_srcfiles, "\n";
    }
  } elsif ($what =~ m/^formats$/i) {
    # changes the default formats
    my $loc = shift @ARGV;
    if (defined($loc)) {
      print "Defaulting to", ($loc ? "" : " not"), " generate format files on installation!\n";
      $localtlpdb->option_create_formats($loc);
      $localtlpdb->save;
    } else {
      print "Create formats on installation: ", $localtlpdb->option_create_formats, "\n";
    }
  } elsif ($what =~ m/^show$/i) {
    print "Default installation source:    ", $localtlpdb->option_location, "\n";
    print "Create formats on installation: ", ($localtlpdb->option_create_formats ? "yes": "no"), "\n";
    print "Install documentation files:    ", ($localtlpdb->option_install_docfiles ? "yes": "no"), "\n";
    print "Install source files:           ", ($localtlpdb->option_install_srcfiles ? "yes": "no"), "\n";
  } else {
    warn "Setting other options currently not supported, please edit texlive.tlpdb!";
  }
  return;
}

sub action_arch {
  if ($^O=~/^MSWin(32|64)$/i) {
    printf STDERR "action `arch' not supported on Windows\n";
  }
  if ($opt_gui) {
    action_gui("arch");
  }
  my $what = shift @ARGV;
  my %ret;
  my $opt_dry = 0;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("dry-run" => \$opt_dry) or pod2usage(2);
  init_local_db();
  $what || ($what = "list");
  if ($what =~ m/^list$/i) {
    # list the available architectures
    # initialize the TLMedia from $location
    $tlmediasrc = TeXLive::TLMedia->new($location);
    die($loadmediasrcerror . $location) unless defined($tlmediasrc);
    my $mediatlpdb = $tlmediasrc->tlpdb;
    my @already_installed_arch = $localtlpdb->available_architectures;
    print "Available architectures:\n";
    foreach my $a ($mediatlpdb->available_architectures) {
      if (member($a,@already_installed_arch)) {
        print "(i) $a\n";
      } else {
        print "    $a\n";
      }
    }
    print "Already installed architectures are marked with (i)\n";
    print "You can add new architectures with tlmgr arch add arch1 arch2\n";
    exit(0);
  } elsif ($what =~ m/^add$/i) {
    $tlmediasrc = TeXLive::TLMedia->new($location);
    die($loadmediasrcerror . $location) unless defined($tlmediasrc);
    my $mediatlpdb = $tlmediasrc->tlpdb;
    my @already_installed_arch = $localtlpdb->available_architectures;
    my @available_arch = $mediatlpdb->available_architectures;
    my @todoarchs;
    foreach my $a (@ARGV) {
      if (TeXLive::TLUtils::member($a, @already_installed_arch)) {
        print "Arch $a is already installed\n";
        next;
      }
      if (!TeXLive::TLUtils::member($a, @available_arch)) {
        print "Arch $a not available, use 'tlmgr available_archs'!\n";
        next;
      }
      push @todoarchs, $a;
    }
    foreach my $pkg ($localtlpdb->list_packages) {
      next if ($pkg =~ m/^00texlive/);
      my $tlp = $localtlpdb->get_package($pkg);
      foreach my $dep ($tlp->depends) {
        if ($dep =~ m/^(.*)\.ARCH$/) {
          # we have to install something
          foreach my $a (@todoarchs) {
            if ($opt_dry) {
              print "Installing $pkg.$a\n";
            } else {
              merge_into(\%ret, $tlmediasrc->install_package("$pkg.$a", $localtlpdb, 0, 0));
            }
          }
        }
      }
    }
    if (TeXLive::TLUtils::member('win32', @todoarchs)) {
      # install the necessary win32 stuff
      merge_into (\%ret, $tlmediasrc->install_package("bin-tlperl.win32", $localtlpdb, 1, 0));
      merge_into (\%ret, $tlmediasrc->install_package("bin-tlgs.win32", $localtlpdb, 1, 0));
      merge_into (\%ret, $tlmediasrc->install_package("bin-tlpsv.win32", $localtlpdb, 1, 0));
    }
    # update the option_archs list of installed archs
    my @larchs = $localtlpdb->option_available_architectures;
    push @larchs, @todoarchs;
    $localtlpdb->option_available_architectures(@larchs);
    $localtlpdb->save;
  } else {
    die "Unknown option for arch: $what";
  }
  return(\%ret);
}

sub action_generate {
  my $what = shift @ARGV;
  my $localconf = "";
  my $dest = "";
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("localcfg=s" => \$localconf, "dest=s" => \$dest,) or pod2usage(2);
  init_local_db();

  if ($what =~ m/^language$/i) {
    $dest || ($dest = kpsewhich ("TEXMFSYSVAR") . "/tex/generic/config/language.dat");
    $localconf || ($localconf = kpsewhich ("TEXMFLOCAL") . "/tex/generic/config/language-local.dat");
    debug("$0: writing language data to $dest\n");
    TeXLive::TLUtils::create_language ($localtlpdb, $dest, $localconf);

  } elsif ($what =~ m/^fmtutil$/i) {
    $dest || ($dest = kpsewhich("TEXMFSYSVAR") . "/web2c/fmtutil.cnf");
    $localconf || ($localconf = kpsewhich("TEXMFLOCAL") . "/web2c/fmtutil-local.cnf");
    debug("$0: writing fmtutil.cnf data to $dest\n");
    TeXLive::TLUtils::create_fmtutil($localtlpdb, $dest, $localconf);

  } elsif ($what =~ m/^updmap$/i) {
    $dest || ($dest = kpsewhich("TEXMFSYSVAR") . "/web2c/updmap.cfg");
    $localconf || ($localconf = kpsewhich("TEXMFLOCAL") . "/web2c/updmap-local.cfg");
    debug("$0: writing new updmap.cfg to $dest\n");
    TeXLive::TLUtils::create_updmap ($localtlpdb, $dest, $localconf);

  } else {
    die "$0: Unknown option for generate: $what (try --help if you need it)\n";
  }

  return;
}

sub init_local_db {
  $localtlpdb = TeXLive::TLPDB->new ("root" => "$Master");
  die("cannot find tlpdb!") unless (defined($localtlpdb));
  # setup the programs, for win32 we need the shipped wget/lzma etc, so we
  # pass the location of these files to setup_programs.
  setup_programs("$Master/tlpkg/installer", $localtlpdb->option_platform);
  # let cmd line options override the settings in localtlpdb
  my $loc = $localtlpdb->option_location;
  if (defined($loc)) {
    $location = $loc;
  }
  if (defined($opt_location)) {
    $location = $opt_location;
  }
  if (!defined($location)) {
    die("No installation source found, nor in the texlive.tlpdb nor on the cmd line.\nPlease specify one!");
  }
}

sub action_gui {
  my ($screen) = @_;
  my $perlbin = "perl";
  my $perlbinquote = "perl";
  my $perlscript = "$Master/texmf/scripts/texlive/tlmgrgui/tlmgrgui.pl";
  if ($^O=~/^MSWin(32|64)$/i) {
    $ENV{'PERL5LIB'} = "$Master/tlpkg/tlperl/lib";
    $perlbin = "$Master/tlpkg/tlperl/bin/perl.exe";
    $perlbinquote = "\"$Master/tlpkg/tlperl/bin/perl.exe\"";
    $perlbinquote =~ s!/!\\!g;
    $perlscript = "\"$perlscript\"";
    $perlscript =~ s!/!\\!g;
  }
  my @cmdline;
  push @cmdline, $perlbinquote, $perlscript;
  push @cmdline, "--lang", "$opt_gui_lang"
    if (defined($opt_gui_lang));
  push @cmdline, "--location", "$opt_location"
    if (defined($opt_location));
  push @cmdline, "--netarchive", "$opt_netarchive"
    if (defined($opt_netarchive));
  push @cmdline, "--diskarchive", "$opt_diskarchive"
    if (defined($opt_diskarchive));
  if (defined($screen)) {
    push @cmdline, "--screen", $screen;
  }
  exec $perlbin @cmdline;
  die("Cannot execute the GUI script\n");
}

sub action_uninstall {
  if (win32()) {
    printf STDERR "Please use \"Add/Remove Programs\" from the Control Panel to removing TeX Live!\n";
    exit 1;
  }
  if ($opt_gui) {
    action_gui("uninstall");
  }
  my $force = 0;
  Getopt::Long::Configure(qw(no_pass_through));
  GetOptions("force" => \$force) or pod2usage(2);
  if (!$force) {
    print("If you answer yes here the whole TeX Live installation will be removed!\n");
    print "Remove TeX Live (y/N): ";
    my $yesno = <STDIN>;
    if ($yesno !~ m/^y(es)?$/i) {
      print "Ok, cancelling the removal!\n";
      exit(0);
    }
  }
  print ("Ok, removing the whole installation:\n");
  init_local_db;
  my $tlpdb = $localtlpdb;
  my $opt_symlinks = 0;
  my ($sys_bin,$sys_info,$sys_man);
  my ($texdir,$texmflocal,$texmfhome,$texmfsysvar);
  if ($tlpdb) {
    $sys_bin = $tlpdb->option_sys_bin;
    $sys_man = $tlpdb->option_sys_man;
    $sys_info= $tlpdb->option_sys_info;
    $opt_symlinks = $tlpdb->option_create_symlinks;
    $texdir = $Master;
    $texmfhome = `kpsewhich -var-value=TEXMFHOME`; chomp($texmfhome);
    $texmfsysvar = `kpsewhich -var-value=TEXMFSYSVAR`; chomp($texmfsysvar);
    $texmflocal = `kpsewhich -var-value=TEXMFLOCAL`; chomp($texmflocal);
  } else {
    die("Cannot find tlpdb in $Master!");
  }
  # remove the links missing
  if ($opt_symlinks) {
    my @files;
    if ((-d "$sys_bin") && (-w "$sys_bin")) {
      my $plat_bindir;
      if (-l "$sys_bin/pdftex") {
        my $fullpath = readlink("$sys_bin/pdftex");
        if ($fullpath =~ m;^$Master/bin/(.*)/[^/]*$;) {
          $plat_bindir = $1;
        }
      } else {
        warn "$sys_bin/pdftex not present or not a link, not removing any link of binaries!\n";
      }
      if ($plat_bindir) {
        @files = `ls "$Master/bin/$plat_bindir"`;
        chomp(@files);
        foreach my $f (@files) {
          next if (! -r "$sys_bin/$f");
          if ((-l "$sys_bin/$f") &&
              (readlink("$sys_bin/$f") =~ m;^$Master/bin/$plat_bindir/;)) {
            unlink("$sys_bin/$f");
          } else {
            warn ("not removing $sys_bin/$f, not a link or wrong destination!\n");
          }
        }
      }
      `rmdir "$sys_bin" 2>/dev/null`;
    } else {
      warn "destination of bin symlink $sys_bin not writable, no removal of links of bin files done!\n";
    }
    # info files
    if (-w  $sys_info) {
      @files = `ls "$Master/texmf/doc/info"`;
      chomp(@files);
      foreach my $f (@files) {
        next if (! -r "$sys_info/$f");
        if ((-l "$sys_info/$f") &&
            (readlink("$sys_info/$f") =~ m;^$Master/texmf/doc/info/;)) {
          unlink("$sys_info/$f");
        } else {
          warn ("not removing $sys_info/$f, not a link or wrong destination!\n");
        }
      }
      `rmdir "$sys_info" 2>/dev/null`;
    } else {
      warn "destination of info symlink $sys_info not writable, no removal of links of info files done!\n";
    }
    # man files
    if (-w  $sys_man) {
      my $foo = `(cd "$Master/texmf/doc/man" && echo *)`;
      my @mans = split ' ', $foo;
      chomp(@mans);
      foreach my $m (@mans) {
        my $mandir = "$Master/texmf/doc/man/$m";
        next unless -d $mandir;
        next unless -d "$sys_man/$m";
        @files = `ls "$mandir"`;
        chomp(@files);
        foreach my $f (@files) {
          next if (! -r "$sys_man/$m/$f");
          if ((-l "$sys_man/$m/$f") &&
              (readlink("$sys_man/$m/$f") =~ m;^$Master/texmf/doc/man/$m/;)) {
            unlink("$sys_man/$m/$f");
          } else {
            warn ("not removing $sys_man/$m/$f, not a link or wrong destination!\n");
          }
        }
        # ignore errors, it might be not empty
        `rmdir "$sys_man/$m" 2>/dev/null`;
      }
      `rmdir "$sys_man" 2>/dev/null`;
    } else {
      warn "destination of man symlink $sys_man not writable, no removal of links of man files done!\n";
    }
  }
  #
  # now do remove the rest
  system("rm", "-rf", "$texdir/texmf-dist");
  system("rm", "-rf", "$texdir/texmf-doc");
  system("rm", "-rf", "$texdir/texmf-var");
  system("rm", "-rf", "$texdir/texmf");
  system("rm", "-rf", "$texdir/tlpkg");
  system("rm", "-rf", "$texdir/bin");
  if (-d "$texdir/temp") {
    system("rmdir", "--ignore-fail-on-non-empty", "$texdir/temp");
  }
  unlink("$texdir/install-tl.log");
  # should we do that????
  system("rm", "-rf", "$texdir/texmf-config");
  system("rmdir", "--ignore-fail-on-non-empty", "$texdir");
}


#
# return all the directories from which all content will be removed
#
# idea:
# - create a hashes by_dir listing all files that should be removed
#   by directory, i.e., key = dir, value is list of files
# - for each of the dirs (keys of by_dir and ordered deepest first)
#   check that all actually contained files are removed
#   and all the contained dirs are in the removal list. If this is the
#   case put that directory into the removal list
# - return this removal list
#
sub removed_dirs
{
  my (@files) = @_;
  my %removed_dirs;
  my %by_dir;

  # construct hash of all directories mentioned, values are lists of the
  # files/dirs in that directory.
  for my $f (@files) {
    # what should we do with not existing entries????
    next if (! -r "$f");
    my $abs_f = abs_path ($f);
    if (!$abs_f) {
      warn ("oops, no abs_path($f) from " . `pwd`);
      next;
    }
    (my $d = $abs_f) =~ s,/[^/]*$,,;
    my @a = exists $by_dir{$d} ? @{$by_dir{$d}} : ();
    push (@a, $abs_f);
    $by_dir{$d} = \@a;
  }

  # for each of our directories, see if we are removing everything in
  # the directory.  if so, return the directory; else return the
  # individual files.
  for my $d (reverse sort keys %by_dir) {
    opendir (DIR, $d) || die "opendir($d) failed: $!";
    my @dirents = readdir (DIR);
    closedir (DIR) || warn "closedir($d) failed: $!";

    # initialize test hash with all the files we saw in this dir.
    # (These idioms are due to "Finding Elements in One Array and Not
    # Another" in the Perl Cookbook.)
    my %seen;
    my @rmfiles = @{$by_dir{$d}};
    @seen{@rmfiles} = ();

    # see if everything is the same.
    my $cleandir = 1;
    for my $dirent (@dirents) {
      next if $dirent =~ /^\.(\.|svn)?$/;  # ignore . .. .svn
      my $item = "$d/$dirent";  # prepend directory for comparison
      if (
           ((-d $item) && (defined($removed_dirs{$item})))
           ||
           (exists $seen{$item})
         ) {
        # do nothing
      } else {
        $cleandir = 0;
        last;
      }
    }
    if ($cleandir) {
      $removed_dirs{$d} = 1;
    }
  }
  return keys %removed_dirs;
}

__END__

=head1 NAME

tlmgr - the TeX Live Manager

=head1 SYNOPSIS

tlmgr [I<options>] I<action> [I<option>]... [I<operand>]...

=head1 DESCRIPTION

B<tlmgr> manages an existing TeX Live installation, both packages and
options.

=head1 OPTIONS

The following options have to be given I<before> you specify the action.

=over 8

=item B<--location> I<location>

Specifies the location from which packages should be installed or
updated, overriding the location found in the installation's TLPDB.

=item B<--gui>

Instead of starting the GUI via the B<gui> action, you can also give this
option together with an action from above and will be brought directly
into the respective screen of the GUI. So calling

  tlmgr --gui update

will bring you directly into the update screen.

=item B<--gui-lang>

Normally the GUI tries to deduce your language from the environment
(on Windows via the registry, on Unix via LC_MESSAGES). If that fails 
you can select a different language by giving this option a two-letter
language code.

=item B<--netarchive> I<dir>

Overrides the default settings for netarchive. Should be used with care.

=item B<--diskarchive> I<dir>

Overrides the default settings for diskarchive. Should be used with care.

=item B<--version>

Echos the tlmgr.pl script's revision number and exits.

=back

The standard options B<--help/-h/-?> and B<--debug> are also accepted.

=head1 ACTIONS

=over 8

=item B<help>

Gives this help page.

=item B<gui>

Start the graphical user interface.

=item B<install [I<option>]... I<pkg>...>

Install all I<pkg>s given on the command line. By default this installs
all packages that the given I<pkg>s are dependent on, also.  Options:

=over 8

=item B<--no-depends>

Do not install dependencies.  Bydefault, installing a package ensures
that all dependencies of this package are fulfilled.

=item B<--dry-run>

Nothing is actually installed; instead, the actions to be performed are
written to the terminal.

=back


=item B<update [I<option>] [pkg [pkg ...]]>

Updates the packages given as arguments to the latest version available
at the installation source. Options:

=over 8

=item B<--list>

List only which packages could be updated.

=item B<--all>

Update all package.

=item B<--dry-run>

Nothing is actually installed; instead, the actions to be performed are
written to the terminal.

=item B<--backupdir directory>

If this option is given it must specify an existing directory where a 
snapshot of the current package as installed will be saved before 
the packages is updated. This way one can go back in case an update
turned out as not working. See the action B<restore> for details.

=back

=item B<backup --backupdir dir [--all] [pkg [pkg]]>

Makes a backup of the given packages (or all packages with B<--all>) to
the directory specified with B<--backupdir> (must exist and be a writable).

Warning: Making a backup of I<all> packages will need quite some space and
time.

Options:

=over 8

=item B<--backupdir directory>

This is an obligatory argument and has to specify a directory where
backups are to be found.

=item B<--all>

Make a backup of all packages in the texlive.tlpdb

=back


=item B<restore --backupdir dir [pkg [rev]]>

If no B<pkg> and B[rev] is given list all packages the available 
backup revisions.

With B<pkg> given but without B<rev> lists all available backup revisions
of B<pkg>.

With both B<pkg> an B<rev> given tries to restore the package from its
backup.

The option B<--backupdir dir> is obligatory and has to specify a directory
with backups.

Options:

=over 8

=item B<--dry-run>

Nothing is actually restored; instead, the actions to be performed are
written to the terminal.

=item B<--backupdir directory>

This is an obligatory argument and has to specify a directory where
backups are to be found.

=back


=item B<remove [I<option>]... I<pkg>...>

Remove all I<pkg>s given on the command line.  Removing a collection
will remove all package dependencies (but not collection dependencies)
in that collection, unless C<--no-depends> is specified.  However, when
removing a package, dependencies are never removed.

Removing a package which is referenced as a dependency in another
collection or scheme is disallowed, unless C<--force> is specified.

Options:

=over 8

=item B<--no-depends>

Do not remove dependent packages.

=item B<--force>

By default, when removing a package or collection would invalidate a
dependency of another collection/scheme, the package will not be removed
and a warning issued.  With this option, the package will be removed
unconditionally.  Use with care.

=item B<--dry-run>

Nothing is actually installed; instead, the actions to be performed are
written to the terminal.

=back


=item B<option [show]>

=item B<option I<key> [I<value>]>

The first form shows the settings currently saved in the TLPDB.

In the second form, if I<value> is missing the setting for I<key> is
displayed.  If I<value> is present, I<key> is set to I<value>.

Possible values for I<key> are:
C<location> (default installation source),
C<formats> (create formats at installation time),
C<docfiles> (install documentation files),
C<srcfiles> (install source files).

=item B<paper a4>

=item B<paper letter>

=item B<[xdvi|dvips|pdftex|dvipdfm|dvipdfmx] paper [help|papersize]>

Configures the system wide paper settings, either for all programs in
one go, or just for the specified program.


=item B<arch list>

Prints the names of the systems available at the default install location.

=item B<arch add I<arch>...>

Add executables for the specified I<arch>es to the installation.
Options:

=over 8

=item B<--dry-run>

Nothing is actually installed; instead, the actions to be performed are
written to the terminal.

=back


=item B<search [I<option>...] I<what>>

By default searches the names, short and long descriptions of all
locally installed packages for the given argument (interpreted as
regexp).  Options:

=over 8

=item B<--file>

List all filenames containing I<what>.

=item B<--global>

Search the TeX Live Database of the installation medium, instead of the
local installation.

=back

=item B<show I<pkg>...>

Shows information about I<pkg>: the name, category, installation status,
short and long description.  Searches in the remote installation source
for the package if it is not locally installed.


=item B<list [collections|schemes]>

With no argument, lists all packages available at the default install
location, prefixing those already installed with C<i >.

With an argument lists only collections or schemes, as requested.

=item B<uninstall>

Uninstalls the entire TeX Live installation.  Options:

=over 8

=item B<--force>

Do not ask for confirmation, remove immediately.

=back


=item B<generate language>

=item B<generate fmtutil>

=item B<generate updmap>

B<WARNING:> This action overwrites any user changes made in
the respective files, by regenerating them from scratch.

This is especially questionable with updmap, because the result will be
to disable all maps which have been manually installed (via C<updmap-sys
--enable>), e.g., for proprietary or local fonts.  Be sure that's what
you want to do.

This action regenerates any of the three config files C<language.dat>,
C<fmtutil.cnf>, and C<updmap.cfg> from the information present in the
local TLPDB. If the files C<language-local.dat>, C<fmtutil-local.cnf>,
or C<updmap-local.cfg> are present under C<TEXMFLOCAL> in the respective
directories, their contents will be simply merged into the final files,
with no error checking of any kind.

Options:

=over 8

=item B<--dest> I<output file>

specifies the output file (defaults to the respective location in
C<TEXMFSYSVAR>).

=item B<--localcfg> I<local conf file>

specifies the (optional) local additions (defaults to the respective
location in C<TEXMFSYSVAR>).

=back

=back


=head1 AUTHORS AND COPYRIGHT

This script and its documentation were written for the TeX Live
distribution (L<http://tug.org/texlive>) and both are licensed under the
GNU General Public License Version 2 or later.

=cut

### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim:set tabstop=2 expandtab: #
