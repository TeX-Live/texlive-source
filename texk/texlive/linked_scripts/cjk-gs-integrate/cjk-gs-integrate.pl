#!/usr/bin/env perl
#
# cjk-gs-integrate - setup ghostscript for CID/TTF CJK fonts
#
# Copyright 2015-2017 by Norbert Preining
# Copyright 2016-2017 by Japanese TeX Development Community
#
# Based on research and work by Yusuke Kuroki, Bruno Voisin, Munehiro Yamamoto
# and the TeX Q&A wiki page
#
# This file is licensed under GPL version 3 or any later version.
# For copyright statements see end of file.
#
# For development see
#  https://github.com/texjporg/cjk-gs-support
#
# TODO:
# - how to deal with MacTeX pre-shipped configuration files?
# - interoperability with updmap-config-kanji
# - input from CK about font priorities
#

$^W = 1;
use Getopt::Long qw(:config no_autoabbrev ignore_case_always);
use File::Basename;
use File::Path qw(make_path);
use Cwd 'abs_path';
use strict;

(my $prg = basename($0)) =~ s/\.pl$//;
my $version = '20170309.0';

if (win32()) {
  print_error("Sorry, currently not supported on Windows!\n");
  exit(1);
}

my %encode_list = (
  Japan => [ qw/
    2004-H
    2004-V
    78-EUC-H
    78-EUC-V
    78-H
    78-RKSJ-H
    78-RKSJ-V
    78-V
    78ms-RKSJ-H
    78ms-RKSJ-V
    83pv-RKSJ-H
    90ms-RKSJ-H
    90ms-RKSJ-V
    90msp-RKSJ-H
    90msp-RKSJ-V
    90pv-RKSJ-H
    90pv-RKSJ-V
    Add-H
    Add-RKSJ-H
    Add-RKSJ-V
    Add-V
    Adobe-Japan1-0
    Adobe-Japan1-1
    Adobe-Japan1-2
    Adobe-Japan1-3
    Adobe-Japan1-4
    Adobe-Japan1-5
    Adobe-Japan1-6
    EUC-H
    EUC-V
    Ext-H
    Ext-RKSJ-H
    Ext-RKSJ-V
    Ext-V
    H
    Hankaku
    Hiragana
    Identity-H
    Identity-V
    Katakana
    NWP-H
    NWP-V
    RKSJ-H
    RKSJ-V
    Roman
    UniJIS-UCS2-H
    UniJIS-UCS2-HW-H
    UniJIS-UCS2-HW-V
    UniJIS-UCS2-V
    UniJIS-UTF16-H
    UniJIS-UTF16-V
    UniJIS-UTF32-H
    UniJIS-UTF32-V
    UniJIS-UTF8-H
    UniJIS-UTF8-V
    UniJIS2004-UTF16-H
    UniJIS2004-UTF16-V
    UniJIS2004-UTF32-H
    UniJIS2004-UTF32-V
    UniJIS2004-UTF8-H
    UniJIS2004-UTF8-V
    UniJISPro-UCS2-HW-V
    UniJISPro-UCS2-V
    UniJISPro-UTF8-V
    UniJISX0213-UTF32-H
    UniJISX0213-UTF32-V
    UniJISX02132004-UTF32-H
    UniJISX02132004-UTF32-V
    V
    WP-Symbol/ ],
  GB => [ qw/
    Adobe-GB1-0
    Adobe-GB1-1
    Adobe-GB1-2
    Adobe-GB1-3
    Adobe-GB1-4
    Adobe-GB1-5
    GB-EUC-H
    GB-EUC-V
    GB-H
    GB-RKSJ-H
    GB-V
    GBK-EUC-H
    GBK-EUC-V
    GBK2K-H
    GBK2K-V
    GBKp-EUC-H
    GBKp-EUC-V
    GBT-EUC-H
    GBT-EUC-V
    GBT-H
    GBT-RKSJ-H
    GBT-V
    GBTpc-EUC-H
    GBTpc-EUC-V
    GBpc-EUC-H
    GBpc-EUC-V
    Identity-H
    Identity-V
    UniGB-UCS2-H
    UniGB-UCS2-V
    UniGB-UTF16-H
    UniGB-UTF16-V
    UniGB-UTF32-H
    UniGB-UTF32-V
    UniGB-UTF8-H
    UniGB-UTF8-V/ ],
  CNS => [ qw/
    Adobe-CNS1-0
    Adobe-CNS1-1
    Adobe-CNS1-2
    Adobe-CNS1-3
    Adobe-CNS1-4
    Adobe-CNS1-5
    Adobe-CNS1-6
    B5-H
    B5-V
    B5pc-H
    B5pc-V
    CNS-EUC-H
    CNS-EUC-V
    CNS1-H
    CNS1-V
    CNS2-H
    CNS2-V
    ETHK-B5-H
    ETHK-B5-V
    ETen-B5-H
    ETen-B5-V
    ETenms-B5-H
    ETenms-B5-V
    HKdla-B5-H
    HKdla-B5-V
    HKdlb-B5-H
    HKdlb-B5-V
    HKgccs-B5-H
    HKgccs-B5-V
    HKm314-B5-H
    HKm314-B5-V
    HKm471-B5-H
    HKm471-B5-V
    HKscs-B5-H
    HKscs-B5-V
    Identity-H
    Identity-V
    UniCNS-UCS2-H
    UniCNS-UCS2-V
    UniCNS-UTF16-H
    UniCNS-UTF16-V
    UniCNS-UTF32-H
    UniCNS-UTF32-V
    UniCNS-UTF8-H
    UniCNS-UTF8-V/ ],
  Korea => [ qw/
    Adobe-Korea1-0
    Adobe-Korea1-1
    Adobe-Korea1-2
    Identity-H
    Identity-V
    KSC-EUC-H
    KSC-EUC-V
    KSC-H
    KSC-Johab-H
    KSC-Johab-V
    KSC-RKSJ-H
    KSC-V
    KSCms-UHC-H
    KSCms-UHC-HW-H
    KSCms-UHC-HW-V
    KSCms-UHC-V
    KSCpc-EUC-H
    KSCpc-EUC-V
    UniKS-UCS2-H
    UniKS-UCS2-V
    UniKS-UTF16-H
    UniKS-UTF16-V
    UniKS-UTF32-H
    UniKS-UTF32-V
    UniKS-UTF8-H
    UniKS-UTF8-V/ ] );

#
# location where links to fonts in texmf are created, relative to TEXMF
my $otf_pathpart = "fonts/opentype/cjk-gs-integrate";
my $ttf_pathpart = "fonts/truetype/cjk-gs-integrate";


my $dry_run = 0;
my $opt_help = 0;
my $opt_quiet = 0;
my $opt_debug = 0;
my $opt_listaliases = 0;
my $opt_listallaliases = 0;
my $opt_listfonts = 0;
my $opt_remove = 0;
my $opt_info = 0;
my $opt_fontdef;
my $opt_output;
my @opt_aliases;
my $opt_only_aliases = 0;
my $opt_machine = 0;
my $opt_filelist;
my $opt_force = 0;
my $opt_texmflink;
my $opt_markdown = 0;

if (! GetOptions(
        "n|dry-run"   => \$dry_run,
        "info"        => \$opt_info,
        "list-aliases" => \$opt_listaliases,
        "list-all-aliases" => \$opt_listallaliases,
        "list-fonts"  => \$opt_listfonts,
        "link-texmf:s" => \$opt_texmflink,
        "remove"       => \$opt_remove,
        "only-aliases" => \$opt_only_aliases,
        "machine-readable" => \$opt_machine,
        "force"       => \$opt_force,
        "filelist=s"  => \$opt_filelist,
        "markdown"    => \$opt_markdown,
        "o|output=s"  => \$opt_output,
        "h|help"      => \$opt_help,
        "q|quiet"     => \$opt_quiet,
        "d|debug+"    => \$opt_debug,
        "f|fontdef=s" => \$opt_fontdef,
        "a|alias=s"   => \@opt_aliases,
        "v|version"   => sub { print &version(); exit(0); }, ) ) {
  die "Try \"$0 --help\" for more information.\n";
}

sub win32 { return ($^O=~/^MSWin(32|64)$/i); }
my $nul = (win32() ? 'nul' : '/dev/null') ;
my $sep = (win32() ? ';' : ':');
my %fontdb;
my %aliases;
my %user_aliases;

if ($opt_help) {
  Usage();
  exit 0;
}

if ($opt_debug) {
  require Data::Dumper;
  $Data::Dumper::Indent = 1;
}

if (defined($opt_texmflink)) {
  my $foo;
  if ($opt_texmflink eq '') {
    # option was passed but didn't receive a value
    #  -> use TEXMFLOCAL
    chomp( $foo = `kpsewhich -var-value=TEXMFLOCAL`);
  } else {
    # option was passed with an argument
    #  -> use it
    $foo = $opt_texmflink;
  }
  $opt_texmflink = $foo;
}

main(@ARGV);

#
# only sub definitions from here on
#
sub main {
  print_info("reading font database ...\n");
  read_font_database();
  determine_nonotf_link_name(); # see comments there
  if (!$opt_listallaliases) {
    print_info("checking for files ...\n");
    check_for_files();
  } else {
    make_all_available();
  }
  compute_aliases();
  if ($opt_info) {
    $opt_listfonts = 1;
    $opt_listaliases = 1;
  }
  if ($opt_listfonts) {
    info_found_fonts();
  }
  if ($opt_listaliases || $opt_listallaliases) {
    print "List of ", ($opt_listaliases ? "all" : "available"), " aliases and their options (in decreasing priority):\n" unless $opt_machine;
    my (@jal, @kal, @tal, @sal);
    for my $al (sort keys %aliases) {
      my $cl;
      my @ks = sort { $a <=> $b} keys(%{$aliases{$al}});
      my $foo = '';
      $foo = "$al:\n" unless $opt_machine;
      for my $p (@ks) {
        my $t = $aliases{$al}{$p};
        my $fn = ($opt_listallaliases ? "-" : $fontdb{$t}{'target'} );
        # should always be the same ;-)
        $cl = $fontdb{$t}{'class'};
        if (!$opt_listallaliases && ($fontdb{$t}{'type'} eq 'TTC' || $fontdb{$t}{'type'} eq 'OTC')) {
          $fn .= "($fontdb{$t}{'subfont'})";
        }
        if ($opt_machine) {
          $foo .= "$al:$p:$aliases{$al}{$p}:$fn\n";
        } else {
          $foo .= "\t($p) $aliases{$al}{$p} ($fn)\n";
        }
      }
      if ($cl eq 'Japan') {
        push @jal, $foo;
      } elsif ($cl eq 'Korea') {
        push @kal, $foo;
      } elsif ($cl eq 'GB') {
        push @sal, $foo;
      } elsif ($cl eq 'CNS') {
        push @tal, $foo;
      } else {
        print STDERR "unknown class $cl for $al\n";
      }
    }
    if ($opt_machine) {
      print @jal if @jal;
      print @kal if @kal;
      print @tal if @tal;
      print @sal if @sal;
    } else {
      print "Aliases for Japanese fonts:\n", @jal, "\n" if @jal;
      print "Aliases for Korean fonts:\n", @kal, "\n" if @kal;
      print "Aliases for Traditional Chinese fonts:\n", @tal, "\n" if @tal;
      print "Aliases for Simplified Chinese fonts:\n", @sal, "\n" if @sal;
    }
  }
  exit(0) if ($opt_listfonts || $opt_listaliases || $opt_listallaliases);

  if (! $opt_output) {
    print_info("searching for GhostScript resource\n");
    my $gsres = find_gs_resource();
    if (!$gsres) {
      print_error("Cannot find GhostScript, terminating!\n");
      exit(1);
    } else {
      $opt_output = $gsres;
    }
  }
  if (! -d $opt_output) {
    $dry_run || mkdir($opt_output) || 
      die ("Cannot create directory $opt_output: $!");
  }
  print_info("output is going to $opt_output\n");
  if (!$opt_only_aliases) {
    print_info(($opt_remove ? "removing" : "generating") . " font snippets and link CID fonts ...\n");
    do_otf_fonts();
    print_info(($opt_remove ? "removing" : "generating") . " font snippets, links, and cidfmap.local for TTF fonts ...\n");
    do_nonotf_fonts();
  }
  print_info(($opt_remove ? "removing" : "generating") . " font aliases ...\n");
  do_aliases();
  print_info("finished\n");
}

sub update_master_cidfmap {
  my $add = shift;
  my $cidfmap_master = "$opt_output/Init/cidfmap";
  print_info(sprintf("%s $add %s cidfmap file ...\n", 
    ($opt_remove ? "removing" : "adding"), ($opt_remove ? "from" : "to")));
  if (-r $cidfmap_master) {
    open(FOO, "<", $cidfmap_master) ||
      die ("Cannot open $cidfmap_master for reading: $!");
    my $found = 0;
    my $newmaster = "";
    # in add mode: just search for the entry and set $found
    # in remove mode: collect all lines that do not match
    while(<FOO>) {
      if (m/^\s*\(\Q$add\E\)\s\s*\.runlibfile\s*$/) {
        $found = 1;
      } else {
        $newmaster .= $_;
      }
    }
    close(FOO);
    if ($found) {
      if ($opt_remove) {
        open(FOO, ">", $cidfmap_master) ||
          die ("Cannot clean up $cidfmap_master: $!");
        print FOO $newmaster;
        close FOO;
      } else {
        print_info("$add already loaded in $cidfmap_master, no changes\n");
      }
    } else {
      return if $dry_run;
      return if $opt_remove;
      open(FOO, ">>", $cidfmap_master) ||
        die ("Cannot open $cidfmap_master for appending: $!");
      print FOO "($add) .runlibfile\n";
      close(FOO);
    }
  } else {
    return if $dry_run;
    return if $opt_remove;
    open(FOO, ">", $cidfmap_master) ||
      die ("Cannot open $cidfmap_master for writing: $!");
    print FOO "($add) .runlibfile\n";
    close(FOO);
  }
}

sub make_dir {
  my ($d, $w) = @_;
  if (-r $d) {
    if (! -d $d) {
      print_error("$d is not a directory, $w\n");
      exit 1;
    }
  } else {
    $dry_run || make_path($d);
  }
}

sub do_otf_fonts {
  my $fontdest = "$opt_output/Font";
  my $ciddest  = "$opt_output/CIDFont";
  make_dir($fontdest, "cannot create CID snippets there!");
  make_dir($ciddest,  "cannot link CID fonts there!");
  make_dir("$opt_texmflink/$otf_pathpart",
           "cannot link fonts to it!")
    if $opt_texmflink;
  for my $k (keys %fontdb) {
    if ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'OTF') {
      generate_font_snippet($fontdest,
        $k, $fontdb{$k}{'class'}, $fontdb{$k}{'target'});
      link_font($fontdb{$k}{'target'}, $ciddest, $k);
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$otf_pathpart", "$fontdb{$k}{'origname'}.otf")
        if $opt_texmflink;
    }
  }
}

sub generate_font_snippet {
  my ($fd, $n, $c, $f) = @_;
  return if $dry_run;
  for my $enc (@{$encode_list{$c}}) {
    if ($opt_remove) {
      unlink "$fd/$n-$enc" if (-f "$fd/$n-$enc");
      next;
    }
    open(FOO, ">$fd/$n-$enc") || 
      die("cannot open $fd/$n-$enc for writing: $!");
    print FOO "%%!PS-Adobe-3.0 Resource-Font
%%%%DocumentNeededResources: $enc (CMap)
%%%%IncludeResource: $enc (CMap)
%%%%BeginResource: Font ($n-$enc)
($n-$enc)
($enc) /CMap findresource
[($n) /CIDFont findresource]
composefont
pop
%%%%EndResource
%%%%EOF
";
    close(FOO);
  }
}

#
# link_font operation
# $opt_force is *not* treated first to warn only 
# at really critical cases
# case 1:
#   exists, is link, link targets agree
#     $opt_force is ignored
#     remove or remove+add according to $opt_remove
# case 2:
#   exists, is link, dangling symlink
#     $opt_force is ignored
#     remove or remove+add according to $opt_remove
# case 3:
#   exists, is link, link target different
#     if $opt_force
#       warn, remove or remove+add according to $opt_remove
#     else
#       error message
# case 4:
#   exists, not a link
#     if $opt_force
#       warn, remove or remove+add according to $opt_remove
#     else
#       error message
# case 5:
#   not exists
#     $opt_force is ignored
#     do nothing or add according to $opt_remove
#     
sub link_font {
  my ($f, $cd, $n) = @_;
  return if $dry_run;
  if (!$n) {
    $n = basename($f);
  }
  my $target = "$cd/$n";
  my $do_unlink = 0;
  if (-l $target) {
    my $linkt = readlink($target);
    if ($linkt) {
      if ($linkt eq $f) {
        # case 1: exists, link, targets agree
        $do_unlink = 1;
      } elsif (-r $linkt) {
        # case 3: exists, link, targets different
        if ($opt_force) {
          print_info("Removing link $target due to --force!\n");
          $do_unlink = 1;
        } else {
          print_error("Link $target already existing, but different target then $target, exiting!\n");
          exit(1);
        }
      } else {
        # case 2: dangling symlink
        print_warning("Removing dangling symlink $target to $linkt\n");
        $do_unlink = 1;
      }
    } else {
      print_error("This should not happen, we have a link but cannot read the target?\n");
      exit(1);
    }
  } elsif (-r $target) {
    # case 4: exists, but not link
    if ($opt_force) {
      print_info("Removing $target due to --force!\n");
      $do_unlink = 1;
    } else {
      print_error("$target already existing, exiting!\n");
      exit(1);
    }
  } # otherwise it is not existing!

  # if we are still here and $do_unlink is set, remove it
  unlink($target) if $do_unlink;
  # recreate link if we are not in the remove case
  if (! $opt_remove) {
    symlink($f, $target) || die("Cannot link font $f to $target: $!");
  }
}

sub do_nonotf_fonts {
  my $fontdest = "$opt_output/Font";
  my $cidfsubst = "$opt_output/CIDFSubst";
  my $outp = '';
  make_dir($fontdest, "cannot create CID snippets there!");
  make_dir($cidfsubst,  "cannot link TTF fonts there!");
  make_dir("$opt_texmflink/$ttf_pathpart",
           "cannot link fonts to it!")
    if $opt_texmflink;
  for my $k (keys %fontdb) {
    if ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'TTF') {
      generate_font_snippet($fontdest,
        $k, $fontdb{$k}{'class'}, $fontdb{$k}{'target'});
      $outp .= generate_cidfmap_entry($k, $fontdb{$k}{'class'}, $fontdb{$k}{'ttfname'}, $fontdb{$k}{'subfont'});
      link_font($fontdb{$k}{'target'}, $cidfsubst, $fontdb{$k}{'ttfname'});
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$ttf_pathpart", $fontdb{$k}{'ttfname'})
        if $opt_texmflink;
    } elsif ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'TTC') {
      generate_font_snippet($fontdest,
        $k, $fontdb{$k}{'class'}, $fontdb{$k}{'target'});
      $outp .= generate_cidfmap_entry($k, $fontdb{$k}{'class'}, $fontdb{$k}{'ttcname'}, $fontdb{$k}{'subfont'});
      link_font($fontdb{$k}{'target'}, $cidfsubst, $fontdb{$k}{'ttcname'});
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$ttf_pathpart", $fontdb{$k}{'ttcname'})
        if $opt_texmflink;
    } elsif ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'OTC') {
    # currently ghostscript does not have OTC support; not creating gs resource
    print_ddebug("gs does not support OTC, not creating gs resource for $k\n");
    # generate_font_snippet($fontdest,
    #  $k, $fontdb{$k}{'class'}, $fontdb{$k}{'target'});
    # $outp .= generate_cidfmap_entry($k, $fontdb{$k}{'class'}, $fontdb{$k}{'otcname'}, $fontdb{$k}{'subfont'});
    # link_font($fontdb{$k}{'target'}, $cidfsubst, $fontdb{$k}{'otcname'});
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$otf_pathpart", $fontdb{$k}{'otcname'})
        if $opt_texmflink;
    }
  }
  return if $dry_run;
  if ($outp) {
    if (! -d "$opt_output/Init") {
      mkdir("$opt_output/Init") ||
        die("Cannot create directory $opt_output/Init: $!");
    }
    open(FOO, ">$opt_output/Init/cidfmap.local") || 
      die "Cannot open $opt_output/cidfmap.local: $!";
    print FOO $outp;
    close(FOO);
  }
  update_master_cidfmap('cidfmap.local');
}

sub do_aliases {
  my $fontdest = "$opt_output/Font";
  my $cidfsubst = "$opt_output/CIDFSubst";
  my $outp = '';
  #
  # alias handling
  # we use two levels of aliases, one is for the default names that
  # are not actual fonts:
  # Ryumin-Light, GothicBBB-Medium, FutoMinA101-Bold, FutoGoB101-Bold, 
  # Jun101-Light which are the original Morisawa names.
  #
  # the second level of aliases is for Morisawa OTF font names:
  # RyuminPro-Light, GothicBBBPro-Medium,
  # FutoMinA101Pro-Bold, FutoGoB101Pro-Bold
  # Jun101Pro-Light
  #
  # the order of fonts selected is
  # defined in the Provides(Priority): Name in the font definiton
  #
  $outp .= "\n\n% Aliases\n";
  #
  my (@jal, @kal, @tal, @sal);
  #
  for my $al (sort keys %aliases) {
    my $target;
    my $class;
    if ($user_aliases{$al}) {
      $target = $user_aliases{$al};
      # determine class
      if ($fontdb{$target}{'available'}) {
        $class = $fontdb{$target}{'class'};
      } else {
        # must be an aliases, we checked this when initializing %user_aliases
        # reset the $al value
        # and since $class is still undefined we will use the next code below
        $al = $target;
      }
    }
    if (!$class) {
      print_warning("Alias candidate for $al is empty!\n") if (!%{$aliases{$al}});
      # search lowest number
      my @ks = keys(%{$aliases{$al}});
      my $first = (sort { $a <=> $b} @ks)[0];
      $target = $aliases{$al}{$first};
      $class  = $fontdb{$target}{'class'};
    }
    # we also need to create font snippets in Font for the aliases!
    generate_font_snippet($fontdest, $al, $class, $target);
    if ($class eq 'Japan') {
      push @jal, "/$al /$target ;";
    } elsif ($class eq 'Korea') {
      push @kal, "/$al /$target ;";
    } elsif ($class eq 'GB') {
      push @sal, "/$al /$target ;";
    } elsif ($class eq 'CNS') {
      push @tal, "/$al /$target ;";
    } else {
      print STDERR "unknown class $class for $al\n";
    }
  }
  $outp .= "\n% Japanese fonts\n" . join("\n", @jal) . "\n" if @jal;
  $outp .= "\n% Korean fonts\n" . join("\n", @kal) . "\n" if @kal;
  $outp .= "\n% Traditional Chinese fonts\n" . join("\n", @tal) . "\n" if @tal;
  $outp .= "\n% Simplified Chinese fonts\n" . join("\n", @sal) . "\n" if @sal;
  #
  return if $dry_run;
  if ($outp && !$opt_remove) {
    if (! -d "$opt_output/Init") {
      mkdir("$opt_output/Init") ||
        die("Cannot create directory $opt_output/Init: $!");
    }
    open(FOO, ">$opt_output/Init/cidfmap.aliases") || 
      die "Cannot open $opt_output/cidfmap.aliases: $!";
    print FOO $outp;
    close(FOO);
  }
  update_master_cidfmap('cidfmap.aliases');
}

sub generate_cidfmap_entry {
  my ($n, $c, $f, $sf) = @_;
  return "" if $opt_remove;
  # $f is already the link target name 'ttfname' (or 'ttcname' or 'otcname')
  # as determined by minimal priority number
  # extract subfont
  my $s = "/$n << /FileType /TrueType 
  /Path pssystemparams /GenericResourceDir get 
  (CIDFSubst/$f) concatstrings
  /SubfontID $sf
  /CSI [($c";
  if ($c eq "Japan") {
    $s .= "1) 6]";
  } elsif ($c eq "GB") {
    $s .= "1) 5]";
  } elsif ($c eq "CNS") {
    $s .= "1) 5]";
  } elsif ($c eq "Korea") {
    $s .= "1) 2]";
  } else {
    print_warning("unknown class $c for $n, skipping.\n");
    return '';
  }
  $s .= " >> ;\n";
  return $s;
}

#
# dump found files
sub info_found_fonts {
  print "List of found fonts:\n\n";
  for my $k (keys %fontdb) {
    my @foundfiles;
    if ($fontdb{$k}{'available'}) {
      print "Font:  $k\n";
      print "Type:  $fontdb{$k}{'type'}\n";
      print "Class: $fontdb{$k}{'class'}\n";
      my $fn = $fontdb{$k}{'target'};
      if ($fontdb{$k}{'type'} eq 'TTC' || $fontdb{$k}{'type'} eq 'OTC') {
        $fn .= "($fontdb{$k}{'subfont'})";
      }
      print "File:  $fn\n";
      if ($fontdb{$k}{'type'} eq 'TTF') {
        print "Link:  $fontdb{$k}{'ttfname'}\n";
      } elsif ($fontdb{$k}{'type'} eq 'TTC') {
        print "Link:  $fontdb{$k}{'ttcname'}\n";
      } elsif ($fontdb{$k}{'type'} eq 'OTC') {
        print "Link:  $fontdb{$k}{'otcname'}\n";
      }
      my @ks = sort { $fontdb{$k}{'files'}{$a}{'priority'}
                      <=>
                      $fontdb{$k}{'files'}{$b}{'priority'} }
                    keys %{$fontdb{$k}{'files'}};
      # remove the top element which is the winner and shown above
      shift @ks;
      if (@ks) {
        print "Other candidates in decreasing order:\n";
        for my $f (@ks) {
          print "       ", $fontdb{$k}{'files'}{$f}{'target'}, "\n";
        }
      }
      print "\n";
    }
  }
}


#
# make all fonts available for listing all aliases
sub make_all_available {
  for my $k (keys %fontdb) {
    $fontdb{$k}{'available'} = 1;
    delete $fontdb{$k}{'files'};
  }
}

#
# checks all file names listed in %fontdb
# and sets
sub check_for_files {
  my @foundfiles;
  if ($opt_filelist) {
    open(FOO, "<", $opt_filelist) || die "Cannot open $opt_filelist: $!";
    @foundfiles = <FOO>;
    close(FOO) || warn "Cannot close $opt_filelist: $!";
  } else {
    # first collect all files:
    my @fn;
    for my $k (keys %fontdb) {
      for my $f (keys %{$fontdb{$k}{'files'}}) {
        # check for subfont extension 
        if ($f =~ m/^(.*)\(\d*\)$/) {
          push @fn, $1;
        } else {
          push @fn, $f;
        }
      }
    }
    #
    # collect extra directories for search
    my @extradirs;
    if (win32()) {
      push @extradirs, "c:/windows/fonts//";
    } else {
      # other dirs to check, for normal unix?
      for my $d (qw!/Library/Fonts /System/Library/Fonts /System/Library/Assets /Network/Library/Fonts /usr/share/fonts!) {
        push @extradirs, "$d//" if (-d $d); # recursive search
      }
      my $home = $ENV{'HOME'};
      push @extradirs, "$home/Library/Fonts//" if (-d "$home/Library/Fonts");
    }
    #
    if (@extradirs) {
      # TODO: we want that files in OSFONTDIR are found first, before
      # links that we have created in TEXMFLOCAL
      # Thus, instead of setting OSFONTDIR which is at the *END* of
      # the kpsewhich variables OPENTYPEFONTS and TTFONTS, we'd like to
      # put all these fonts at the front of them
      #
      # There are problems with case-insensitive file systems like HFS
      # on MacOS, as we might catch different names (batang/Batang)
      # and identify them wrongly.
      # https://github.com/texjporg/cjk-gs-support/issues/9
      # For now until we have dealt with that, do not set the
      # two variables (HY 2016/09/27) and think about a different approach.
      push @extradirs, $ENV{'OSFONTDIR'} if $ENV{'OSFONTDIR'};
      if (@extradirs) {
        # comment out -- HY (2016/09/27)
        # my $newotf = join($sep, @extradirs) . $sep;
        # my $newttf = $newotf;
        # $newotf .= $ENV{'OPENTYPEFONTS'} if ($ENV{'OPENTYPEFONTS'});
        # $newttf .= $ENV{'TTFONTS'} if ($ENV{'TTFONTS'});
        # $ENV{'OPENTYPEFONTS'} = $newotf;
        # $ENV{'TTFONTS'} = $newttf;
        # new code for uppercase/lowercase workaround -- HY (2016/09/27)
        my $extrafontdir = join($sep, @extradirs) . $sep;
        $ENV{'OSFONTDIR'} = $extrafontdir;
      }
    }
    # prepare for kpsewhich call, we need to do quoting
    my $cmdl = 'kpsewhich ';
    for my $f (@fn) {
      $cmdl .= " \"$f\" ";
    }
    # shoot up kpsewhich
    print_ddebug("checking for $cmdl\n");
    @foundfiles = `$cmdl`;
  }
  chomp(@foundfiles);
  print_ddebug("Found files @foundfiles\n");
  # map basenames to filenames
  my %bntofn;
  for my $f (@foundfiles) {
    my $realf = abs_path($f);
    if (!$realf) {
      print_warning("dead link or strange file found: $f - ignored!\n");
      next;
    }
    my $bn = basename($f);
    $bntofn{$bn} = $realf;
  }
  if ($opt_debug > 0) {
    print_debug("dumping font database before file check:\n");
    print_debug(Data::Dumper::Dumper(\%fontdb));
  }
  if ($opt_debug > 1) {
    print_ddebug("dumping basename to filename list:\n");
    print_ddebug(Data::Dumper::Dumper(\%bntofn));
  }

  # update the %fontdb with the found files
  for my $k (keys %fontdb) {
    $fontdb{$k}{'available'} = 0;
    for my $f (keys %{$fontdb{$k}{'files'}}) {
      # check for subfont extension 
      my $realfile = $f;
      $realfile =~ s/^(.*)\(\d*\)$/$1/;
      if ($bntofn{$realfile}) {
        # we found a representative, make it available
        $fontdb{$k}{'files'}{$f}{'target'} = $bntofn{$realfile};
        $fontdb{$k}{'available'} = 1;
      } else {
        # delete the entry for convenience
        delete $fontdb{$k}{'files'}{$f};
      }
    }
  }
  # second round to determine the winner in case of more targets
  for my $k (keys %fontdb) {
    if ($fontdb{$k}{'available'}) {
      my $mp = 1000000; my $mf; my $mt;
      for my $f (keys %{$fontdb{$k}{'files'}}) {
        if ($fontdb{$k}{'files'}{$f}{'priority'} < $mp) {
          $mp = $fontdb{$k}{'files'}{$f}{'priority'};
          $mf = $f;
          $mt = $fontdb{$k}{'files'}{$f}{'type'};
        }
      }
      # extract subfont if necessary
      my $sf = 0;
      if ($mf =~ m/^(.*)\((\d*)\)$/) { $sf = $2; }
      $fontdb{$k}{'target'} = $fontdb{$k}{'files'}{$mf}{'target'};
      $fontdb{$k}{'type'} = $fontdb{$k}{'files'}{$mf}{'type'};
      $fontdb{$k}{'subfont'} = $sf if ($fontdb{$k}{'type'} eq 'TTF' || $fontdb{$k}{'type'} eq 'TTC' || $fontdb{$k}{'type'} eq 'OTC');
    }
    # not needed anymore
    # delete $fontdb{$k}{'files'};
  }
  if ($opt_debug > 0) {
    print_debug("dumping font database:\n");
    print_debug(Data::Dumper::Dumper(\%fontdb));
  }
}

sub compute_aliases {
  # go through fontdb to check for provides
  # accumulate all provided fonts in @provides
  for my $k (keys %fontdb) {
    if ($fontdb{$k}{'available'}) {
      for my $p (keys %{$fontdb{$k}{'provides'}}) {
        # do not check alias if the real font is available in OTF/TTF/TTC format
        if ($fontdb{$p}{'available'}) {
          next if ($fontdb{$p}{'type'} ne 'OTC');
        }
        # use the priority as key
        # if priorities are double, this will pick one at chance
        if ($aliases{$p}{$fontdb{$k}{'provides'}{$p}}) {
          print_warning("duplicate provide levels:\n");
          print_warning("  current $p $fontdb{$k}{'provides'}{$p} $aliases{$p}{$fontdb{$k}{'provides'}{$p}}\n");
          print_warning("  ignored $p $fontdb{$k}{'provides'}{$p} $k\n");
        } else {
          # if OTC font is caught, then skip it as Ghostscript doesn't support it (2016/12/12)
          if ($fontdb{$k}{'type'} eq 'OTC') {
            print_warning("Currently Ghostscript does not support OTC font,\n");
            print_warning("not adding $fontdb{$k}{'otcname'} to alias candidates\n");
          } else {
            $aliases{$p}{$fontdb{$k}{'provides'}{$p}} = $k;
          }
        }
      }
    }
  }
  # check for user supplied aliases
  for my $a (@opt_aliases) {
    if ($a =~ m/^(.*)=(.*)$/) {
      my $ll = $1;
      my $rr = $2;
      # check for consistency of user provided aliases:
      # - ll must not be available
      # - rr needs to be available as font or alias
      # check whether $rr is available, either as real font or as alias
      if ($fontdb{$ll}{'available'}) {
        print_error("left side of alias spec is provided by a real font: $a\n");
        print_error("stopping here\n");
        exit(1);
      }
      if (!($fontdb{$rr}{'available'} || $aliases{$rr})) {
        print_error("right side of alias spec is not available as real font or alias: $a\n");
        print_error("stopping here\n");
        exit(1);
      }
      $user_aliases{$ll} = $rr;
    }
  }
  if ($opt_debug > 0) {
    print_debug("dumping aliases:\n");
    print_debug(Data::Dumper::Dumper(\%aliases));
  }
}

# While the OTF link target is determined by the filename itself
# for TTF we can have ttc with several fonts.
# The following routine determines the link target by selecting
# the file name of the ttf candidates with the lowest priority
# as the link target name for TTF
sub determine_nonotf_link_name {
  for my $k (keys %fontdb) {
    my $ttfname = "";
    my $ttcname = "";
    my $otcname = "";
    my $mpttf = 10000000;
    my $mpttc = 10000000;
    my $mpotc = 10000000;
    for my $f (keys %{$fontdb{$k}{'files'}}) {
      if ($fontdb{$k}{'files'}{$f}{'type'} eq 'TTF') {
        my $p = $fontdb{$k}{'files'}{$f}{'priority'};
        if ($p < $mpttf) {
          $ttfname = $f;
          $ttfname =~ s/^(.*)\(\d*\)$/$1/;
          $mpttf = $p;
        }
      } elsif ($fontdb{$k}{'files'}{$f}{'type'} eq 'TTC') {
        my $p = $fontdb{$k}{'files'}{$f}{'priority'};
        if ($p < $mpttc) {
          $ttcname = $f;
          $ttcname =~ s/^(.*)\(\d*\)$/$1/;
          $mpttc = $p;
        }
      } elsif ($fontdb{$k}{'files'}{$f}{'type'} eq 'OTC') {
        my $p = $fontdb{$k}{'files'}{$f}{'priority'};
        if ($p < $mpotc) {
          $otcname = $f;
          $otcname =~ s/^(.*)\(\d*\)$/$1/;
          $mpotc = $p;
        }
      }
    }
    if ($ttfname) {
      $fontdb{$k}{'ttfname'} = $ttfname;
    }
    if ($ttcname) {
      $fontdb{$k}{'ttcname'} = $ttcname;
    }
    if ($otcname) {
      $fontdb{$k}{'otcname'} = $otcname;
    }
  }
}

sub read_font_database {
  my @dbl;
  if ($opt_fontdef) {
    open (FDB, "<$opt_fontdef") ||
      die "Cannot find $opt_fontdef: $!";
    @dbl = <FDB>;
    close(FDB);
  } else {
    @dbl = <DATA>;
  }
  chomp(@dbl);
  # add a "final empty line" to easy parsing
  push @dbl, "";
  my $fontname = "";
  my $fontclass = "";
  my %fontprovides = ();
  my %fontfiles;
  my $psname = "";
  my $lineno = 0;
  for my $l (@dbl) {
    $lineno++;

    next if ($l =~ m/^\s*#/);
    if ($l =~ m/^\s*$/) {
      if ($fontname || $fontclass || keys(%fontfiles)) {
        if ($fontname && $fontclass && keys(%fontfiles)) {
          my $realfontname = ($psname ? $psname : $fontname);
          $fontdb{$realfontname}{'origname'} = $fontname;
          $fontdb{$realfontname}{'class'} = $fontclass;
          $fontdb{$realfontname}{'files'} = { %fontfiles };
          $fontdb{$realfontname}{'provides'} = { %fontprovides };
          if ($opt_debug > 1) {
            print_ddebug("Dumping fontfiles for $realfontname: " . Data::Dumper::Dumper(\%fontfiles));
          }
          # reset to start
          $fontname = $fontclass = $psname = "";
          %fontfiles = ();
          %fontprovides = ();
        } else {
          print_warning("incomplete entry above line $lineno for $fontname/$fontclass, skipping!\n");
          # reset to start
          $fontname = $fontclass = $psname = "";
          %fontfiles = ();
          %fontprovides = ();
        }
      } else {
        # no term is set, so nothing to warn about
      }
      next;
    }
    if ($l =~ m/^Name:\s*(.*)$/) { $fontname = $1; next; }
    if ($l =~ m/^PSName:\s*(.*)$/) { $psname = $1; next; }
    if ($l =~ m/^Class:\s*(.*)$/) { $fontclass = $1 ; next ; }
    # new code: distinguish 4 types (otf, otc, ttf, ttc)
    if ($l =~ m/^OTFname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_ddebug("filename: $fn\n");
      print_ddebug("type: otf\n");
      $fontfiles{$fn}{'type'} = 'OTF';
      next;
    }
    if ($l =~ m/^OTCname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_ddebug("filename: $fn\n");
      print_ddebug("type: otc\n");
      $fontfiles{$fn}{'type'} = 'OTC';
      next;
    }
    if ($l =~ m/^TTFname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_ddebug("filename: $fn\n");
      print_ddebug("type: ttf\n");
      $fontfiles{$fn}{'type'} = 'TTF';
      next;
    }
    if ($l =~ m/^TTCname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_ddebug("filename: $fn\n");
      print_ddebug("type: ttc\n");
      $fontfiles{$fn}{'type'} = 'TTC';
      next;
    }
    # only for backward compatibility; guess type from the file extension
    if ($l =~ m/^Filename(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_ddebug("filename: $fn\n");
      if ($fn =~ m/\.otf$/i) {
        print_ddebug("type: otf\n");
        $fontfiles{$fn}{'type'} = 'OTF';
      } elsif ($fn =~ m/\.otc(\(\d+\))?$/i) {
        print_ddebug("type: otc\n");
        $fontfiles{$fn}{'type'} = 'OTC';
      } elsif ($fn =~ m/\.ttf$/i) {
        print_ddebug("type: ttf\n");
        $fontfiles{$fn}{'type'} = 'TTF';
      } elsif ($fn =~ m/\.ttc(\(\d+\))?$/i) {
        print_ddebug("type: ttc\n");
        $fontfiles{$fn}{'type'} = 'TTC';
      } else {
        print_warning("cannot determine font type of $fn at line $lineno, skipping!\n");
        delete $fontfiles{$fn};
      }
      next;
    }
    if ($l =~ m/^Provides\((\d+)\):\s*(.*)$/) { $fontprovides{$2} = $1; next; }
    # we are still here??
    print_error("Cannot parse this file at line $lineno, exiting. Strange line: >>>$l<<<\n");
    exit (1);
  }
}

sub find_gs_resource {
  my $foundres = '';
  if (!win32()) {
    # we assume that gs is in the path
    # on Windows we probably have to try something else
    chomp( my $gsver = `gs --version 2>$nul` );
    if ($?) {
      print_error("Cannot get gs version ...\n");
    } else {
      # trial 1: assume the relative path
      # when /path/to/bin/gs is found, then there should be
      # /path/to/share/ghostscript/$(gs --version)/Resource
      print_debug("Finding gs resource by assuming relative path ...\n");
      chomp( $foundres = `which gs` );
      $foundres =~ s!/bin/gs$!/share/ghostscript/$gsver/Resource!;
      if ( ! -d $foundres ) {
        $foundres = '';
      }
      if (!$foundres) {
        print_debug("Found gs but no resource, try another routine ...\n");
      }
    }
    if (!$foundres) {
      chomp( my @ret = `gs --help 2>$nul` );
      if ($?) {
        print_error("Cannot run gs --help ...\n");
      } else {
        # trial 2: parse gs help message
        print_debug("Finding gs resource by parsing help message ...\n");
        $foundres = '';
        # try to find resource line
        for (@ret) {
          if (m!Resource/Font!) {
            $foundres = $_;
            # extract the first substring of non-space chars
            # up to Resource/Font and drop the /Font part
            $foundres =~ s!^.*\s(\S*Resource)/Font.*$!$1!;
            last;
          }
        }
        if (!$foundres) {
          print_error("Found gs but no resource???\n");
        }
      }
    }
  }
  return $foundres;
}

sub version {
  my $ret = sprintf "%s version %s\n", $prg, $version;
  return $ret;
}

sub Usage {
  my $headline = "Configuring GhostScript for CJK CID/TTF fonts";
  my $usage = "[perl] $prg\[.pl\] [OPTIONS]";
  my $options = "
-n, --dry-run         do not actually output anything
--remove              try to remove instead of create
-f, --fontdef FILE    specify alternate set of font definitions, if not
                      given, the built-in set is used
-o, --output DIR      specifies the base output dir, if not provided,
                      the Resource directory of an installed GhostScript
                      is searched and used.
-a, --alias LL=RR     defines an alias, or overrides a given alias;
                      illegal if LL is provided by a real font, or
                      RR is neither available as real font or alias;
                      can be given multiple times
--filelist FILE       read list of available font files from FILE
                      instead of searching with kpathsea
--link-texmf [DIR]    link fonts into
                         DIR/$otf_pathpart
                      and
                         DIR/$ttf_pathpart
                      where DIR defaults to TEXMFLOCAL
--machine-readable    output of --list-aliases is machine readable
--force               do not bail out if linked fonts already exist
-q, --quiet           be less verbose
-d, --debug           output debug information, can be given multiple times
-v, --version         outputs only the version information
-h, --help            this help
";

  my $commandoptions = "
--only-aliases        do only regenerate the cidfmap.alias file instead of all
--list-aliases        lists the available aliases and their options, with the
                      selected option on top
--list-all-aliases    list all possible aliases without searching for actually
                      present files
--list-fonts          lists the fonts found on the system
--info                combines the above two information
";

  my $shortdesc = "
This script searches a list of directories for CJK fonts, and makes
them available to an installed GhostScript. In the simplest case with
sufficient privileges, a run without arguments should effect in a
complete setup of GhostScript.
";

my $operation = "
For each found TrueType (TTF) font it creates a cidfmap entry in

    <Resource>/Init/cidfmap.local

and links the font to

    <Resource>/CIDFSubst/

For each CID font it creates a snippet in

    <Resource>/Font/

and links the font to

    <Resource>/CIDFont/

The `<Resource>` dir is either given by `-o`/`--output`, or otherwise searched
from an installed GhostScript (binary name is assumed to be 'gs').

Aliases are added to 

    <Resource>/Init/cidfmap.aliases

Finally, it tries to add runlib calls to

    <Resource>/Init/cidfmap

to load the cidfmap.local and cidfmap.aliases.
";

  my $dirsearch = '
Search is done using the kpathsea library, in particular using kpsewhich
program. By default the following directories are searched:
  - all TEXMF trees
  - `/Library/Fonts`, `/Library/Fonts/Microsoft`, `/System/Library/Fonts`, 
    `/Network/Library/Fonts`, and `~/Library/Fonts` (all if available)
  - `c:/windows/fonts` (on Windows)
  - the directories in `OSFONTDIR` environment variable

In case you want to add some directories to the search path, adapt the
`OSFONTDIR` environment variable accordingly: Example:

`````
    OSFONTDIR="/usr/local/share/fonts/truetype//:/usr/local/share/fonts/opentype//" $prg
`````

will result in fonts found in the above two given directories to be
searched in addition.
';

  my $outputfile = "
If no output option is given, the program searches for a GhostScript
interpreter 'gs' and determines its Resource directory. This might
fail, in which case one need to pass the output directory manually.

Since the program adds files and link to this directory, sufficient
permissions are necessary.
";

  my $aliases = "
Aliases are managed via the Provides values in the font database.
At the moment entries for the basic font names for CJK fonts
are added:

Japanese:

    Ryumin-Light GothicBBB-Medium FutoMinA101-Bold FutoGoB101-Bold Jun101-Light

Korean:

    HYSMyeongJo-Medium HYGoThic-Medium HYRGoThic-Medium

Simplified Chinese:

    STSong-Light STHeiti-Regular STHeiti-Light STKaiti-Regular
    STFangsong-Light STFangsong-Regular

Traditional Chinese:

    MSung-Light MSung-Medium MHei-Medium MKai-Medium

In addition, we also include provide entries for the OTF Morisawa names:
    RyuminPro-Light GothicBBBPro-Medium FutoMinA101Pro-Bold
    FutoGoB101Pro-Bold Jun101Pro-Light

The order is determined by the Provides setting in the font database,
and for the Japanese fonts it is currently:
    Morisawa Pr6N, Morisawa, Hiragino ProN, Hiragino,
    Kozuka Pr6N, Kozuka ProVI, Kozuka, Yu OSX, Yu Win,
    MS, Moga-Mobo-ex, Moga-Mobo, IPAex, IPA, Ume

That is, the first font found in this order will be used to provide the
alias if necessary.

For the Korean fonts:
    (Hanyang,) Adobe, Solaris-hanyang, MS, Unfonts, Baekmuk

For the Simplified Chinese:
    Adobe, Fandol, Hiragino, MS, CJKUnifonts, Arphic, CJKUnifonts-ttf

For the Traditional Chinese:
    Adobe, MS, CJKUnifonts, Arphic, CJKUnifonts-ttf

#### Overriding aliases ####

Using the command line option `--alias LL=RR` one can add arbitrary aliases,
or override ones selected by the program. For this to work the following
requirements of `LL` and `RR` must be fulfilled:
  * `LL` is not provided by a real font
  * `RR` is available either as real font, or as alias (indirect alias)
";

  my $authors = "
The script and its documentation was written by Norbert Preining, based
on research and work by Yusuke Kuroki, Bruno Voisin, Hironobu Yamashita,
Munehiro Yamamoto and the TeX Q&A wiki page.

The script is licensed under GNU General Public License Version 3 or later.
The contained font data is not copyrightable.
";


  if ($opt_markdown) {
    print "$headline\n";
    print ("=" x length($headline));
    print "\n$shortdesc\nUsage\n-----\n\n`````\n$usage\n`````\n\n";
    print "#### Options ####\n\n`````";
    print_for_out($options, "  ");
    print "`````\n\n#### Command like options ####\n\n`````";
    print_for_out($commandoptions, "  ");
    print "`````\n\nOperation\n---------\n$operation\n";
    print "How and which directories are searched\n";
    print "--------------------------------------\n$dirsearch\n";
    print "Output files\n";
    print "------------\n$outputfile\n";
    print "Aliases\n";
    print "-------\n$aliases\n";
    print "Authors, Contributors, and Copyright\n";
    print "------------------------------------\n$authors\n";
  } else {
    print "\nUsage: $usage\n\n$headline\n$shortdesc";
    print "\nOptions:\n";
    print_for_out($options, "  ");
    print "\nCommand like options:\n";
    print_for_out($commandoptions, "  ");
    print "\nOperation:\n";
    print_for_out($operation, "  ");
    print "\nHow and which directories are searched:\n";
    print_for_out($dirsearch, "  ");
    print "\nOutput files:\n";
    print_for_out($outputfile, "  ");
    print "\nAliases:\n";
    print_for_out($aliases, "  ");
    print "\nAuthors, Contributors, and Copyright:\n";
    print_for_out($authors, "  ");
    print "\n";
  }
  exit 0;
}

sub print_for_out {
  my ($what, $indent) = @_;
  for (split /\n/, $what) { 
    next if m/`````/;
    s/\s*####\s*//g;
    if ($_ eq '') {
      print "\n";
    } else {
      print "$indent$_\n";
    }
  }
}

# info/warning can be suppressed
# verbose/error cannot be suppressed
sub print_info {
  print STDOUT "$prg: ", @_ if (!$opt_quiet);
}
sub print_verbose {
  print STDOUT "$prg: ", @_;
}
sub print_warning {
  print STDERR "$prg [WARNING]: ", @_ if (!$opt_quiet) 
}
sub print_error {
  print STDERR "$prg [ERROR]: ", @_;
}
sub print_debug {
  print STDERR "$prg [DEBUG]: ", @_ if ($opt_debug >= 1);
}
sub print_ddebug {
  print STDERR "$prg [DEBUG]: ", @_ if ($opt_debug >= 2);
}


__DATA__
#
# CJK FONT DEFINITIONS
#

#
# JAPANESE FONTS
#

# Morisawa

Name: A-OTF-RyuminPr6N-Light
PSName: RyuminPr6N-Light
Class: Japan
Provides(10): Ryumin-Light
Provides(10): RyuminPro-Light
Provides(10): HiraMinProN-W3
Provides(10): HiraMinPro-W3
OTFname: A-OTF-RyuminPr6N-Light.otf

Name: A-OTF-RyuminPro-Light
PSName: RyuminPro-Light
Class: Japan
Provides(20): Ryumin-Light
Provides(20): HiraMinPro-W3
OTFname: A-OTF-RyuminPro-Light.otf

Name: A-OTF-FutoMinA101Pr6N-Bold
PSName: FutoMinA101Pr6N-Bold
Class: Japan
Provides(10): FutoMinA101-Bold
Provides(10): FutoMinA101Pro-Bold
Provides(10): HiraMinProN-W6
Provides(10): HiraMinPro-W6
OTFname: A-OTF-FutoMinA101Pr6N-Bold.otf

Name: A-OTF-FutoMinA101Pro-Bold
PSName: FutoMinA101Pro-Bold
Class: Japan
Provides(20): FutoMinA101-Bold
Provides(20): HiraMinPro-W6
OTFname: A-OTF-FutoMinA101Pro-Bold.otf

Name: A-OTF-GothicBBBPr6N-Medium
PSName: GothicBBBPr6N-Medium
Class: Japan
Provides(10): GothicBBB-Medium
Provides(10): GothicBBBPro-Medium
Provides(10): HiraKakuProN-W3
Provides(10): HiraKakuPro-W3
OTFname: A-OTF-GothicBBBPr6N-Medium.otf

Name: A-OTF-GothicBBBPro-Medium
PSName: GothicBBBPro-Medium
Class: Japan
Provides(20): GothicBBB-Medium
Provides(20): HiraKakuPro-W3
OTFname: A-OTF-GothicBBBPro-Medium.otf

Name: A-OTF-FutoGoB101Pr6N-Bold
PSName: FutoGoB101Pr6N-Bold
Class: Japan
Provides(10): FutoGoB101-Bold
Provides(10): FutoGoB101Pro-Bold
Provides(10): HiraKakuProN-W6
Provides(10): HiraKakuPro-W6
OTFname: A-OTF-FutoGoB101Pr6N-Bold.otf

Name: A-OTF-FutoGoB101Pro-Bold
PSName: FutoGoB101Pro-Bold
Class: Japan
Provides(20): FutoGoB101-Bold
Provides(20): HiraKakuPro-W6
OTFname: A-OTF-FutoGoB101Pro-Bold.otf

Name: A-OTF-MidashiGoPr6N-MB31
PSName: MidashiGoPr6N-MB31
Class: Japan
Provides(10): MidashiGo-MB31
Provides(10): MidashiGoPro-MB31
Provides(10): HiraKakuStdN-W8
Provides(10): HiraKakuStd-W8
OTFname: A-OTF-MidashiGoPr6N-MB31.otf

Name: A-OTF-MidashiGoPro-MB31
PSName: MidashiGoPro-MB31
Class: Japan
Provides(20): MidashiGo-MB31
Provides(20): HiraKakuStd-W8
OTFname: A-OTF-MidashiGoPro-MB31.otf

# A-OTF-Jun101Pr6N-Light has been replaced by A-OTF-ShinMGoPr6N-Light
# in otf-(up-)morisawa-pr6n.map since jfontmaps 20140301.0
# now unnecessary, but reserved for backward compatibility
Name: A-OTF-Jun101Pr6N-Light
PSName: Jun101Pr6N-Light
Class: Japan
Provides(11): Jun101-Light
Provides(11): Jun101Pro-Light
Provides(11): HiraMaruProN-W4
Provides(11): HiraMaruPro-W4
OTFname: A-OTF-Jun101Pr6N-Light.otf

Name: A-OTF-Jun101Pro-Light
PSName: Jun101Pro-Light
Class: Japan
Provides(20): Jun101-Light
Provides(20): HiraMaruPro-W4
OTFname: A-OTF-Jun101Pro-Light.otf

Name: A-OTF-ShinMGoPr6N-Light
PSName: ShinMGoPr6N-Light
Class: Japan
Provides(10): Jun101-Light
Provides(10): Jun101Pro-Light
Provides(10): HiraMaruProN-W4
Provides(10): HiraMaruPro-W4
OTFname: A-OTF-ShinMGoPr6N-Light.otf

# Morisawa others (for moriprop);
# A-OTF-Jun101Pro-Light.otf and A-OTF-RyuminPro-Light.otf already added

Name: A-OTF-Jun201Pro-Regular
PSName: Jun201Pro-Regular
Class: Japan
Provides(20): Jun201-Regular
OTFname: A-OTF-Jun201Pro-Regular.otf

Name: A-OTF-Jun34Pro-Medium
PSName: Jun34Pro-Medium
Class: Japan
Provides(20): Jun34-Medium
OTFname: A-OTF-Jun34Pro-Medium.otf

Name: A-OTF-Jun501Pro-Bold
PSName: Jun501Pro-Bold
Class: Japan
Provides(20): Jun501-Bold
OTFname: A-OTF-Jun501Pro-Bold.otf

Name: A-OTF-RyuminPro-Regular
PSName: RyuminPro-Regular
Class: Japan
Provides(20): Ryumin-Regular
OTFname: A-OTF-RyuminPro-Regular.otf

Name: A-OTF-RyuminPro-Medium
PSName: RyuminPro-Medium
Class: Japan
Provides(20): Ryumin-Medium
OTFname: A-OTF-RyuminPro-Medium.otf

Name: A-OTF-RyuminPro-Bold
PSName: RyuminPro-Bold
Class: Japan
Provides(20): Ryumin-Bold
OTFname: A-OTF-RyuminPro-Bold.otf

Name: A-OTF-RyuminPro-Heavy
PSName: RyuminPro-Heavy
Class: Japan
Provides(20): Ryumin-Heavy
OTFname: A-OTF-RyuminPro-Heavy.otf

Name: A-OTF-RyuminPro-Ultra
PSName: RyuminPro-Ultra
Class: Japan
Provides(20): Ryumin-Ultra
OTFname: A-OTF-RyuminPro-Ultra.otf

Name: A-OTF-ShinGoPro-Light
PSName: ShinGoPro-Light
Class: Japan
Provides(20): ShinGo-Light
OTFname: A-OTF-ShinGoPro-Light.otf

Name: A-OTF-ShinGoPro-Regular
PSName: ShinGoPro-Regular
Class: Japan
Provides(20): ShinGo-Regular
OTFname: A-OTF-ShinGoPro-Regular.otf

Name: A-OTF-ShinGoPro-Medium
PSName: ShinGoPro-Medium
Class: Japan
Provides(20): ShinGo-Medium
OTFname: A-OTF-ShinGoPro-Medium.otf

Name: A-OTF-ShinGoPro-Bold
PSName: ShinGoPro-Bold
Class: Japan
Provides(20): ShinGo-Bold
OTFname: A-OTF-ShinGoPro-Bold.otf

Name: A-OTF-ShinGoPro-Heavy
PSName: ShinGoPro-Heavy
Class: Japan
Provides(20): ShinGo-Heavy
OTFname: A-OTF-ShinGoPro-Heavy.otf

Name: A-OTF-ShinGoPro-Ultra
PSName: ShinGoPro-Ultra
Class: Japan
Provides(20): ShinGo-Ultra
OTFname: A-OTF-ShinGoPro-Ultra.otf

# Hiragino (OS X)

# Note about Mac OS X:
#   ヒラギノ角ゴ {Pro,ProN} {W3,W6}.otf
#   ヒラギノ角ゴ {Std,StdN} W8.otf
#   ヒラギノ丸ゴ {Pro,ProN} W4.otf
#   ヒラギノ明朝 {Pro,ProN} {W3,W6}.otf
# are bundled with OS X 10.10.5 Yosemite or earlier versions.
#   ヒラギノ角ゴシック {W0,...W9}.ttc
#   ヒラギノ丸ゴ ProN W4.ttc
#   ヒラギノ明朝 ProN {W3,W6}.ttc
# are bundled with OS X 10.11 El Capitan or later versions.

Name: HiraKakuPro-W3
Class: Japan
Provides(40): GothicBBB-Medium
Provides(40): GothicBBBPro-Medium
# the following two are *not* the same
# one is in decomposed form (for Mac), one is in composed form (for the rest)
OTFname(20): ヒラギノ角ゴ Pro W3.otf
OTFname(19): ヒラギノ角ゴ Pro W3.otf
OTFname(10): HiraKakuPro-W3.otf
OTCname(30): ヒラギノ角ゴシック W3.ttc(3)
OTCname(29): ヒラギノ角ゴシック W3.ttc(3)
OTCname(28): HiraginoSans-W3.ttc(3)

Name: HiraKakuPro-W6
Class: Japan
Provides(40): FutoGoB101-Bold
Provides(40): FutoGoB101Pro-Bold
OTFname(20): ヒラギノ角ゴ Pro W6.otf
OTFname(19): ヒラギノ角ゴ Pro W6.otf
OTFname(10): HiraKakuPro-W6.otf
OTCname(30): ヒラギノ角ゴシック W6.ttc(3)
OTCname(29): ヒラギノ角ゴシック W6.ttc(3)
OTCname(28): HiraginoSans-W6.ttc(3)

Name: HiraKakuProN-W3
Class: Japan
Provides(30): GothicBBB-Medium
Provides(30): GothicBBBPro-Medium
OTFname(20): ヒラギノ角ゴ ProN W3.otf
OTFname(19): ヒラギノ角ゴ ProN W3.otf
OTFname(10): HiraKakuProN-W3.otf
OTCname(30): ヒラギノ角ゴシック W3.ttc(2)
OTCname(29): ヒラギノ角ゴシック W3.ttc(2)
OTCname(28): HiraginoSans-W3.ttc(2)

Name: HiraKakuProN-W6
Class: Japan
Provides(30): FutoGoB101-Bold
Provides(30): FutoGoB101Pro-Bold
OTFname(20): ヒラギノ角ゴ ProN W6.otf
OTFname(19): ヒラギノ角ゴ ProN W6.otf
OTFname(10): HiraKakuProN-W6.otf
OTCname(30): ヒラギノ角ゴシック W6.ttc(2)
OTCname(29): ヒラギノ角ゴシック W6.ttc(2)
OTCname(28): HiraginoSans-W6.ttc(2)

Name: HiraKakuStd-W8
Class: Japan
Provides(40): MidashiGo-MB31
Provides(40): MidashiGoPro-MB31
OTFname(20): ヒラギノ角ゴ Std W8.otf
OTFname(19): ヒラギノ角ゴ Std W8.otf
OTFname(10): HiraKakuStd-W8.otf
OTCname(30): ヒラギノ角ゴシック W8.ttc(2)
OTCname(29): ヒラギノ角ゴシック W8.ttc(2)
OTCname(28): HiraginoSans-W8.ttc(2)

Name: HiraKakuStdN-W8
Class: Japan
Provides(30): MidashiGo-MB31
Provides(30): MidashiGoPro-MB31
OTFname(20): ヒラギノ角ゴ StdN W8.otf
OTFname(19): ヒラギノ角ゴ StdN W8.otf
OTFname(10): HiraKakuStdN-W8.otf
OTCname(30): ヒラギノ角ゴシック W8.ttc(3)
OTCname(29): ヒラギノ角ゴシック W8.ttc(3)
OTCname(28): HiraginoSans-W8.ttc(3)

Name: HiraginoSans-W0
Class: Japan
OTCname(30): ヒラギノ角ゴシック W0.ttc(0)
OTCname(29): ヒラギノ角ゴシック W0.ttc(0)
OTCname(28): HiraginoSans-W0.ttc(0)

Name: HiraginoSans-W1
Class: Japan
OTCname(30): ヒラギノ角ゴシック W1.ttc(0)
OTCname(29): ヒラギノ角ゴシック W1.ttc(0)
OTCname(28): HiraginoSans-W1.ttc(0)

Name: HiraginoSans-W2
Class: Japan
OTCname(30): ヒラギノ角ゴシック W2.ttc(0)
OTCname(29): ヒラギノ角ゴシック W2.ttc(0)
OTCname(28): HiraginoSans-W2.ttc(0)

Name: HiraginoSans-W3
Class: Japan
OTCname(30): ヒラギノ角ゴシック W3.ttc(0)
OTCname(29): ヒラギノ角ゴシック W3.ttc(0)
OTCname(28): HiraginoSans-W3.ttc(0)

Name: HiraginoSans-W4
Class: Japan
OTCname(30): ヒラギノ角ゴシック W4.ttc(0)
OTCname(29): ヒラギノ角ゴシック W4.ttc(0)
OTCname(28): HiraginoSans-W4.ttc(0)

Name: HiraginoSans-W5
Class: Japan
OTCname(30): ヒラギノ角ゴシック W5.ttc(0)
OTCname(29): ヒラギノ角ゴシック W5.ttc(0)
OTCname(28): HiraginoSans-W5.ttc(0)

Name: HiraginoSans-W6
Class: Japan
OTCname(30): ヒラギノ角ゴシック W6.ttc(0)
OTCname(29): ヒラギノ角ゴシック W6.ttc(0)
OTCname(28): HiraginoSans-W6.ttc(0)

Name: HiraginoSans-W7
Class: Japan
OTCname(30): ヒラギノ角ゴシック W7.ttc(0)
OTCname(29): ヒラギノ角ゴシック W7.ttc(0)
OTCname(28): HiraginoSans-W7.ttc(0)

Name: HiraginoSans-W8
Class: Japan
OTCname(30): ヒラギノ角ゴシック W8.ttc(0)
OTCname(29): ヒラギノ角ゴシック W8.ttc(0)
OTCname(28): HiraginoSans-W8.ttc(0)

Name: HiraginoSans-W9
Class: Japan
OTCname(30): ヒラギノ角ゴシック W9.ttc(0)
OTCname(29): ヒラギノ角ゴシック W9.ttc(0)
OTCname(28): HiraginoSans-W9.ttc(0)

Name: HiraMaruPro-W4
Class: Japan
Provides(40): Jun101-Light
Provides(40): Jun101Pro-Light
OTFname(20): ヒラギノ丸ゴ Pro W4.otf
OTFname(19): ヒラギノ丸ゴ Pro W4.otf
OTFname(10): HiraMaruPro-W4.otf
OTCname(30): ヒラギノ丸ゴ ProN W4.ttc(0)
OTCname(29): ヒラギノ丸ゴ ProN W4.ttc(0)
OTCname(28): HiraginoSansR-W4.ttc(0)

Name: HiraMaruProN-W4
Class: Japan
Provides(30): Jun101-Light
Provides(30): Jun101Pro-Light
OTFname(20): ヒラギノ丸ゴ ProN W4.otf
OTFname(19): ヒラギノ丸ゴ ProN W4.otf
OTFname(10): HiraMaruProN-W4.otf
OTCname(30): ヒラギノ丸ゴ ProN W4.ttc(1)
OTCname(29): ヒラギノ丸ゴ ProN W4.ttc(1)
OTCname(28): HiraginoSansR-W4.ttc(1)

Name: HiraMinPro-W3
Class: Japan
Provides(40): Ryumin-Light
Provides(40): RyuminPro-Light
OTFname(20): ヒラギノ明朝 Pro W3.otf
OTFname(19): ヒラギノ明朝 Pro W3.otf
OTFname(10): HiraMinPro-W3.otf
OTCname(30): ヒラギノ明朝 ProN W3.ttc(1)
OTCname(29): ヒラギノ明朝 ProN W3.ttc(1)
OTCname(28): HiraginoSerif-W3.ttc(1)

Name: HiraMinPro-W6
Class: Japan
Provides(40): FutoMinA101-Bold
Provides(40): FutoMinA101Pro-Bold
OTFname(20): ヒラギノ明朝 Pro W6.otf
OTFname(19): ヒラギノ明朝 Pro W6.otf
OTFname(10): HiraMinPro-W6.otf
OTCname(30): ヒラギノ明朝 ProN W6.ttc(1)
OTCname(29): ヒラギノ明朝 ProN W6.ttc(1)
OTCname(28): HiraginoSerif-W6.ttc(1)

Name: HiraMinProN-W3
Class: Japan
Provides(30): Ryumin-Light
Provides(30): RyuminPro-Light
OTFname(20): ヒラギノ明朝 ProN W3.otf
OTFname(19): ヒラギノ明朝 ProN W3.otf
OTFname(10): HiraMinProN-W3.otf
OTCname(30): ヒラギノ明朝 ProN W3.ttc(0)
OTCname(29): ヒラギノ明朝 ProN W3.ttc(0)
OTCname(28): HiraginoSerif-W3.ttc(0)

Name: HiraMinProN-W6
Class: Japan
Provides(30): FutoMinA101-Bold
Provides(30): FutoMinA101Pro-Bold
OTFname(20): ヒラギノ明朝 ProN W6.otf
OTFname(19): ヒラギノ明朝 ProN W6.otf
OTFname(10): HiraMinProN-W6.otf
OTCname(30): ヒラギノ明朝 ProN W6.ttc(0)
OTCname(29): ヒラギノ明朝 ProN W6.ttc(0)
OTCname(28): HiraginoSerif-W6.ttc(0)

# Yu-fonts MacOS version

# Note about Mac OS X:
#   Yu Gothic Medium.otf
#   Yu Gothic Bold.otf
# are bundled with OS X 10.9 Mavericks or later versions.
#   Yu Mincho Medium.otf
#   Yu Mincho Demibold.otf
# are bundled with OS X 10.9 Mavericks -- 10.10.5 Yosemite.
#   YuMincho.ttc
#     index 0: YuMin-Medium
#     index 1: YuMin-Demibold
#     index 2: YuMin_36pKn-Medium
#     index 3: YuMin_36pKn-Demibold
# is bundled with OS X 10.11 -- 10.11.6 El Capitan.
#   YuMincho.ttc
#     index 0: YuMin-Medium
#     index 1: YuMin-Demibold
#     index 2: YuMin-Extrabold
#     index 3: YuMin_36pKn-Medium
#     index 4: YuMin_36pKn-Demibold
#     index 5: YuMin_36pKn-Extrabold
# is bundled with macOS 10.12.0 Sierra or later versions.

Name: YuGo-Medium
Class: Japan
Provides(80): GothicBBB-Medium
Provides(80): GothicBBBPro-Medium
OTFname(20): Yu Gothic Medium.otf
OTFname(10): YuGo-Medium.otf

Name: YuGo-Bold
Class: Japan
Provides(80): FutoGoB101-Bold
Provides(80): FutoGoB101Pro-Bold
Provides(80): Jun101-Light
Provides(80): Jun101Pro-Light
Provides(80): MidashiGo-MB31
Provides(80): MidashiGoPro-MB31
OTFname(20): Yu Gothic Bold.otf
OTFname(10): YuGo-Bold.otf

Name: YuMin-Medium
Class: Japan
Provides(80): Ryumin-Light
Provides(80): RyuminPro-Light
OTFname(20): Yu Mincho Medium.otf
OTFname(10): YuMin-Medium.otf
OTCname(30): YuMincho.ttc(0)

Name: YuMin-Demibold
Class: Japan
Provides(80): FutoMinA101-Bold
Provides(80): FutoMinA101Pro-Bold
OTFname(20): Yu Mincho Demibold.otf
OTFname(10): YuMin-Demibold.otf
OTCname(30): YuMincho.ttc(1)

# Following four entries are for macOS 10.12 Sierra
# The index for OS X 10.11.x El Capitan is different!

Name: YuMin-Extrabold
Class: Japan
OTCname(30): YuMincho.ttc(2)

Name: YuMin_36pKn-Medium
Class: Japan
OTCname(30): YuMincho.ttc(3)

Name: YuMin_36pKn-Demibold
Class: Japan
OTCname(30): YuMincho.ttc(4)

Name: YuMin_36pKn-Extrabold
Class: Japan
OTCname(30): YuMincho.ttc(5)

# Yu-fonts Windows version

# Note about Windows:
#   yugothi{b,c,l}.ttf
#   yumin.ttf, yumin{db,l}.ttf
# are bundled with Windows 8.1.
#   YuGoth{B,L,M,R}.ttc
#   yumin.ttf, yumin{db,l}.ttf
# are bundled with Windows 10.
#   YUGOTH{B,L,M,R}.TTC
#   YUMIN.TTF, YUMIN{DB,L}.TTF
# are bundled with Yu Font Pack for MSOffice 2010/2013.
#   YuGothic.ttf
#   YuGothic-Bold.ttf
# are bundled with VS2013 or later versions.
# Also, symlink names should be consistent with ptex-fontmaps!

Name: YuMincho-Regular
Class: Japan
Provides(90): Ryumin-Light
Provides(90): RyuminPro-Light
Provides(90): HiraMinProN-W3
Provides(90): HiraMinPro-W3
TTFname(20): yumin.ttf
#TTFname(21): YuMincho-Regular.ttf

Name: YuMincho-Light
Class: Japan
TTFname(20): yuminl.ttf
#TTFname(21): YuMincho-Light.ttf

Name: YuMincho-DemiBold
Class: Japan
Provides(90): FutoMinA101-Bold
Provides(90): FutoMinA101Pro-Bold
Provides(90): HiraMinProN-W6
Provides(90): HiraMinPro-W6
TTFname(20): yumindb.ttf
#TTFname(21): YuMincho-DemiBold.ttf

Name: YuGothic-Regular
Class: Japan
Provides(90): GothicBBB-Medium
Provides(90): GothicBBBPro-Medium
Provides(90): HiraKakuProN-W3
Provides(90): HiraKakuPro-W3
TTFname(20): yugothic.ttf
#TTFname(21): YuGothic-Regular.ttf
TTCname(30): YuGothR.ttc(0)

Name: YuGothic-Medium
Class: Japan
TTCname(30): YuGothM.ttc(0)

Name: YuGothic-Light
Class: Japan
TTFname(20): yugothil.ttf
#TTFname(21): YuGothic-Light.ttf
TTCname(30): YuGothL.ttc(0)

Name: YuGothic-Bold
Class: Japan
Provides(90): FutoGoB101-Bold
Provides(90): FutoGoB101Pro-Bold
Provides(90): HiraKakuProN-W6
Provides(90): HiraKakuPro-W6
Provides(90): Jun101-Light
Provides(90): Jun101Pro-Light
Provides(90): HiraMaruProN-W4
Provides(90): HiraMaruPro-W4
Provides(90): MidashiGo-MB31
Provides(90): MidashiGoPro-MB31
Provides(90): HiraKakuStdN-W8
Provides(90): HiraKakuStd-W8
TTFname(20): yugothib.ttf
TTFname(21): YuGothic-Bold.ttf
TTCname(30): YuGothB.ttc(0)

# IPA (free)

Name: IPAMincho
Class: Japan
Provides(130): Ryumin-Light
Provides(130): RyuminPro-Light
Provides(130): HiraMinProN-W3
Provides(130): HiraMinPro-W3
Provides(130): FutoMinA101-Bold
Provides(130): FutoMinA101Pro-Bold
Provides(130): HiraMinProN-W6
Provides(130): HiraMinPro-W6
TTFname(20): ipam.ttf
#TTFname(21): IPAMincho.ttf

Name: IPAGothic
Class: Japan
Provides(130): GothicBBB-Medium
Provides(130): GothicBBBPro-Medium
Provides(130): HiraKakuProN-W3
Provides(130): HiraKakuPro-W3
Provides(130): FutoGoB101-Bold
Provides(130): FutoGoB101Pro-Bold
Provides(130): HiraKakuProN-W6
Provides(130): HiraKakuPro-W6
Provides(130): Jun101-Light
Provides(130): Jun101Pro-Light
Provides(130): HiraMaruProN-W4
Provides(130): HiraMaruPro-W4
Provides(130): MidashiGo-MB31
Provides(130): MidashiGoPro-MB31
Provides(130): HiraKakuStdN-W8
Provides(130): HiraKakuStd-W8
TTFname(20): ipag.ttf
#TTFname(21): IPAGothic.ttf

Name: IPAexMincho
Class: Japan
Provides(120): Ryumin-Light
Provides(120): RyuminPro-Light
Provides(120): HiraMinProN-W3
Provides(120): HiraMinPro-W3
Provides(120): FutoMinA101-Bold
Provides(120): FutoMinA101Pro-Bold
Provides(120): HiraMinProN-W6
Provides(120): HiraMinPro-W6
TTFname(20): ipaexm.ttf
#TTFname(21): IPAexMincho.ttf

Name: IPAexGothic
Class: Japan
Provides(120): GothicBBB-Medium
Provides(120): GothicBBBPro-Medium
Provides(120): HiraKakuProN-W3
Provides(120): HiraKakuPro-W3
Provides(120): FutoGoB101-Bold
Provides(120): FutoGoB101Pro-Bold
Provides(120): HiraKakuProN-W6
Provides(120): HiraKakuPro-W6
Provides(120): Jun101-Light
Provides(120): Jun101Pro-Light
Provides(120): HiraMaruProN-W4
Provides(120): HiraMaruPro-W4
Provides(120): MidashiGo-MB31
Provides(120): MidashiGoPro-MB31
Provides(120): HiraKakuStdN-W8
Provides(120): HiraKakuStd-W8
TTFname(20): ipaexg.ttf
#TTFname(21): IPAexGothic.ttf

# IPA proportional (free)

Name: IPAPMincho
Class: Japan
TTFname(20): ipamp.ttf
#TTFname(21): IPAPMincho.ttf

Name: IPAPGothic
Class: Japan
TTFname(20): ipagp.ttf
#TTFname(21): IPAPGothic.ttf

# Moga-Mobo from Y.Oz Vox (free)

Name: MogaMincho-Regular
Class: Japan
Provides(110): Ryumin-Light
Provides(110): RyuminPro-Light
Provides(110): HiraMinProN-W3
Provides(110): HiraMinPro-W3
TTCname: mogam.ttc(0)

Name: MogaExMincho-Regular
Class: Japan
Provides(100): Ryumin-Light
Provides(100): RyuminPro-Light
Provides(100): HiraMinProN-W3
Provides(100): HiraMinPro-W3
TTCname: mogam.ttc(1)

Name: MogaExMincho-Italic
Class: Japan
TTCname: mogam.ttc(2)

Name: Moga90Mincho-Regular
Class: Japan
TTCname: mogam.ttc(3)

Name: MogaEx90Mincho-Regular
Class: Japan
TTCname: mogam.ttc(4)

Name: MogaEx90Mincho-Italic
Class: Japan
TTCname: mogam.ttc(5)

Name: MogaMincho-Bold
Class: Japan
Provides(110): FutoMinA101-Bold
Provides(110): FutoMinA101Pro-Bold
Provides(110): HiraMinProN-W6
Provides(110): HiraMinPro-W6
TTCname: mogamb.ttc(0)

Name: MogaExMincho-Bold
Class: Japan
Provides(100): FutoMinA101-Bold
Provides(100): FutoMinA101Pro-Bold
Provides(100): HiraMinProN-W6
Provides(100): HiraMinPro-W6
TTCname: mogamb.ttc(1)

Name: Moga90Mincho-Bold
Class: Japan
TTCname: mogamb.ttc(2)

Name: MogaEx90Mincho-Bold
Class: Japan
TTCname: mogamb.ttc(3)

Name: MogaHMincho-Regular
Class: Japan
TTCname: mogahm.ttc(0)

Name: MogaExHMincho-Regular
Class: Japan
TTCname: mogahm.ttc(1)

Name: MogaExHMincho-Italic
Class: Japan
TTCname: mogahm.ttc(2)

Name: Moga90HMincho-Regular
Class: Japan
TTCname: mogahm.ttc(3)

Name: MogaEx90HMincho-Regular
Class: Japan
TTCname: mogahm.ttc(4)

Name: MogaEx90HMincho-Italic
Class: Japan
TTCname: mogahm.ttc(5)

Name: MogaHMincho-Bold
Class: Japan
TTCname: mogahmb.ttc(0)

Name: MogaExHMincho-Bold
Class: Japan
TTCname: mogahmb.ttc(1)

Name: Moga90HMincho-Bold
Class: Japan
TTCname: mogahmb.ttc(2)

Name: MogaEx90HMincho-Bold
Class: Japan
TTCname: mogahmb.ttc(3)

Name: MogaGothic-Regular
Class: Japan
Provides(110): GothicBBB-Medium
Provides(110): GothicBBBPro-Medium
Provides(110): HiraKakuProN-W3
Provides(110): HiraKakuPro-W3
TTCname: mogag.ttc(0)

Name: MogaExGothic-Regular
Class: Japan
Provides(100): GothicBBB-Medium
Provides(100): GothicBBBPro-Medium
Provides(100): HiraKakuProN-W3
Provides(100): HiraKakuPro-W3
TTCname: mogag.ttc(1)

Name: Moga90Gothic-Regular
Class: Japan
TTCname: mogag.ttc(2)

Name: MogaEx90Gothic-Regular
Class: Japan
TTCname: mogag.ttc(3)

Name: MogaGothic-Bold
Class: Japan
Provides(110): FutoGoB101-Bold
Provides(110): FutoGoB101Pro-Bold
Provides(110): HiraKakuProN-W6
Provides(110): HiraKakuPro-W6
Provides(110): MidashiGo-MB31
Provides(110): MidashiGoPro-MB31
Provides(110): HiraKakuStdN-W8
Provides(110): HiraKakuStd-W8
TTCname: mogagb.ttc(0)

Name: MogaExGothic-Bold
Class: Japan
Provides(100): FutoGoB101-Bold
Provides(100): FutoGoB101Pro-Bold
Provides(100): HiraKakuProN-W6
Provides(100): HiraKakuPro-W6
Provides(100): MidashiGo-MB31
Provides(100): MidashiGoPro-MB31
Provides(100): HiraKakuStdN-W8
Provides(100): HiraKakuStd-W8
TTCname: mogagb.ttc(1)

Name: Moga90Gothic-Bold
Class: Japan
TTCname: mogagb.ttc(2)

Name: MogaEx90Gothic-Bold
Class: Japan
TTCname: mogagb.ttc(3)

Name: MoboGothic-Regular
Class: Japan
Provides(110): Jun101-Light
Provides(110): Jun101Pro-Light
Provides(110): HiraMaruProN-W4
Provides(110): HiraMaruPro-W4
TTCname: mobog.ttc(0)

Name: MoboExGothic-Regular
Class: Japan
Provides(100): Jun101-Light
Provides(100): Jun101Pro-Light
Provides(100): HiraMaruProN-W4
Provides(100): HiraMaruPro-W4
TTCname: mobog.ttc(1)

Name: Mobo90Gothic-Regular
Class: Japan
TTCname: mobog.ttc(2)

Name: MoboEx90Gothic-Regular
Class: Japan
TTCname: mobog.ttc(3)

Name: MoboGothic-Bold
Class: Japan
TTCname: mobogb.ttc(0)

Name: MoboExGothic-Bold
Class: Japan
TTCname: mobogb.ttc(1)

Name: Mobo90Gothic-Bold
Class: Japan
TTCname: mobogb.ttc(2)

Name: MoboEx90Gothic-Bold
Class: Japan
TTCname: mobogb.ttc(3)

# Ume-font (free)
# note: in the current release (2016-09-03 umefont_660.7z),
# ume-tms.ttf and ume-pms.ttf ("studybook" family) share the same PSName
# as ume-tmo.ttf and ume-pmo.ttf; we don't add these database
# intentionally -- HY (2017/01/17)

Name: Ume-Mincho
Class: Japan
Provides(140): Ryumin-Light
Provides(140): RyuminPro-Light
Provides(140): HiraMinProN-W3
Provides(140): HiraMinPro-W3
Provides(140): FutoMinA101-Bold
Provides(140): FutoMinA101Pro-Bold
Provides(140): HiraMinProN-W6
Provides(140): HiraMinPro-W6
TTFname(10): ume-tmo3.ttf
#TTFname(11): ume-tms3.ttf

Name: Ume-Gothic
Class: Japan
Provides(140): GothicBBB-Medium
Provides(140): GothicBBBPro-Medium
Provides(140): HiraKakuProN-W3
Provides(140): HiraKakuPro-W3
Provides(140): FutoGoB101-Bold
Provides(140): FutoGoB101Pro-Bold
Provides(140): HiraKakuProN-W6
Provides(140): HiraKakuPro-W6
Provides(140): Jun101-Light
Provides(140): Jun101Pro-Light
Provides(140): HiraMaruProN-W4
Provides(140): HiraMaruPro-W4
Provides(140): MidashiGo-MB31
Provides(140): MidashiGoPro-MB31
Provides(140): HiraKakuStdN-W8
Provides(140): HiraKakuStd-W8
TTFname(10): ume-tgo4.ttf

Name: Ume-Gothic-O5
Class: Japan
TTFname(10): ume-tgo5.ttf

Name: Ume-Gothic-C4
Class: Japan
TTFname(10): ume-tgc4.ttf

Name: Ume-Gothic-C5
Class: Japan
TTFname(10): ume-tgc5.ttf

Name: Ume-Gothic-S4
Class: Japan
TTFname(10): ume-tgs4.ttf

Name: Ume-Gothic-S5
Class: Japan
TTFname(10): ume-tgs5.ttf

Name: Ume-P-Mincho
Class: Japan
TTFname(10): ume-pmo3.ttf
#TTFname(11): ume-pms3.ttf

Name: Ume-P-Gothic
Class: Japan
TTFname(10): ume-pgo4.ttf

Name: Ume-P-Gothic-O5
Class: Japan
TTFname(10): ume-pgo5.ttf

Name: Ume-P-Gothic-C4
Class: Japan
TTFname(10): ume-pgc4.ttf

Name: Ume-P-Gothic-C5
Class: Japan
TTFname(10): ume-pgc5.ttf

Name: Ume-P-Gothic-S4
Class: Japan
TTFname(10): ume-pgs4.ttf

Name: Ume-P-Gothic-S5
Class: Japan
TTFname(10): ume-pgs5.ttf

Name: Ume-UI-Gothic
Class: Japan
TTFname(10): ume-ugo4.ttf

Name: Ume-UI-Gothic-O5
Class: Japan
TTFname(10): ume-ugo5.ttf

Name: Ume-Hy-Gothic
Class: Japan
TTFname(10): ume-hgo4.ttf

# Sazanami (free)

Name: Sazanami-Mincho-Regular
Class: Japan
TTFname: sazanami-mincho.ttf

Name: Sazanami-Gothic-Regular
Class: Japan
TTFname: sazanami-gothic.ttf

# Kozuka (Adobe)

Name: KozGoPr6N-Bold
Class: Japan
Provides(50): FutoGoB101-Bold
Provides(50): FutoGoB101Pro-Bold
Provides(50): HiraKakuProN-W6
Provides(50): HiraKakuPro-W6
OTFname: KozGoPr6N-Bold.otf

Name: KozGoPr6N-Heavy
Class: Japan
Provides(50): Jun101-Light
Provides(50): Jun101Pro-Light
Provides(50): HiraMaruProN-W4
Provides(50): HiraMaruPro-W4
Provides(50): MidashiGo-MB31
Provides(50): MidashiGoPro-MB31
Provides(50): HiraKakuStdN-W8
Provides(50): HiraKakuStd-W8
OTFname: KozGoPr6N-Heavy.otf

Name: KozGoPr6N-Medium
Class: Japan
Provides(50): GothicBBB-Medium
Provides(50): GothicBBBPro-Medium
Provides(50): HiraKakuProN-W3
Provides(50): HiraKakuPro-W3
OTFname: KozGoPr6N-Medium.otf

Name: KozGoPr6N-Regular
Class: Japan
OTFname: KozGoPr6N-Regular.otf

Name: KozGoPr6N-ExtraLight
Class: Japan
OTFname: KozGoPr6N-ExtraLight.otf

Name: KozGoPr6N-Light
Class: Japan
OTFname: KozGoPr6N-Light.otf

Name: KozGoPro-Bold
Class: Japan
Provides(70): FutoGoB101-Bold
Provides(70): FutoGoB101Pro-Bold
Provides(70): HiraKakuProN-W6
Provides(70): HiraKakuPro-W6
OTFname: KozGoPro-Bold.otf

Name: KozGoPro-Heavy
Class: Japan
Provides(70): Jun101-Light
Provides(70): Jun101Pro-Light
Provides(70): HiraMaruProN-W4
Provides(70): HiraMaruPro-W4
Provides(70): MidashiGo-MB31
Provides(70): MidashiGoPro-MB31
Provides(70): HiraKakuStdN-W8
Provides(70): HiraKakuStd-W8
OTFname: KozGoPro-Heavy.otf

Name: KozGoPro-Medium
Class: Japan
Provides(70): GothicBBB-Medium
Provides(70): GothicBBBPro-Medium
Provides(70): HiraKakuProN-W3
Provides(70): HiraKakuPro-W3
OTFname: KozGoPro-Medium.otf

Name: KozGoPro-Regular
Class: Japan
OTFname: KozGoPro-Regular.otf

Name: KozGoPro-ExtraLight
Class: Japan
OTFname: KozGoPro-ExtraLight.otf

Name: KozGoPro-Light
Class: Japan
OTFname: KozGoPro-Light.otf

Name: KozGoProVI-Bold
Class: Japan
Provides(60): FutoGoB101-Bold
Provides(60): FutoGoB101Pro-Bold
Provides(60): HiraKakuProN-W6
Provides(60): HiraKakuPro-W6
OTFname: KozGoProVI-Bold.otf

Name: KozGoProVI-Heavy
Class: Japan
Provides(60): Jun101-Light
Provides(60): Jun101Pro-Light
Provides(60): HiraMaruProN-W4
Provides(60): HiraMaruPro-W4
Provides(60): MidashiGo-MB31
Provides(60): MidashiGoPro-MB31
Provides(60): HiraKakuStdN-W8
Provides(60): HiraKakuStd-W8
OTFname: KozGoProVI-Heavy.otf

Name: KozGoProVI-Medium
Class: Japan
Provides(60): GothicBBB-Medium
Provides(60): GothicBBBPro-Medium
Provides(60): HiraKakuProN-W3
Provides(60): HiraKakuPro-W3
OTFname: KozGoProVI-Medium.otf

Name: KozGoProVI-Regular
Class: Japan
OTFname: KozGoProVI-Regular.otf

Name: KozMinPr6N-Bold
Class: Japan
Provides(50): FutoMinA101-Bold
Provides(50): FutoMinA101Pro-Bold
Provides(50): HiraMinProN-W6
Provides(50): HiraMinPro-W6
OTFname: KozMinPr6N-Bold.otf

Name: KozMinPr6N-Heavy
Class: Japan
OTFname: KozMinPr6N-Heavy.otf

Name: KozMinPr6N-Medium
Class: Japan
OTFname: KozMinPr6N-Medium.otf

Name: KozMinPr6N-Regular
Class: Japan
Provides(50): Ryumin-Light
Provides(50): RyuminPro-Light
Provides(50): HiraMinProN-W3
Provides(50): HiraMinPro-W3
OTFname: KozMinPr6N-Regular.otf

Name: KozMinPr6N-ExtraLight
Class: Japan
OTFname: KozMinPr6N-ExtraLight.otf

Name: KozMinPr6N-Light
Class: Japan
OTFname: KozMinPr6N-Light.otf

Name: KozMinPro-Bold
Class: Japan
Provides(70): FutoMinA101-Bold
Provides(70): FutoMinA101Pro-Bold
Provides(70): HiraMinProN-W6
Provides(70): HiraMinPro-W6
OTFname: KozMinPro-Bold.otf

Name: KozMinPro-Heavy
Class: Japan
OTFname: KozMinPro-Heavy.otf

Name: KozMinPro-Medium
Class: Japan
OTFname: KozMinPro-Medium.otf

Name: KozMinPro-Regular
Class: Japan
Provides(70): Ryumin-Light
Provides(70): RyuminPro-Light
Provides(70): HiraMinProN-W3
Provides(70): HiraMinPro-W3
OTFname: KozMinPro-Regular.otf

Name: KozMinPro-ExtraLight
Class: Japan
OTFname: KozMinPro-ExtraLight.otf

Name: KozMinPro-Light
Class: Japan
OTFname: KozMinPro-Light.otf

Name: KozMinProVI-Bold
Class: Japan
Provides(60): FutoMinA101-Bold
Provides(60): FutoMinA101Pro-Bold
Provides(60): HiraMinProN-W6
Provides(60): HiraMinPro-W6
OTFname: KozMinProVI-Bold.otf

Name: KozMinProVI-Regular
Class: Japan
Provides(60): Ryumin-Light
Provides(60): RyuminPro-Light
Provides(60): HiraMinProN-W3
Provides(60): HiraMinPro-W3
OTFname: KozMinProVI-Regular.otf

Name: KozMinProVI-Light
Class: Japan
OTFname: KozMinProVI-Light.otf

# other Mac OS X fonts

# Note about Mac OS X:
#   Klee.ttc
#   Tsukushi{A,B}MaruGothic.ttc
# are bundled with OS X 10.11 El Capitan or later versions.
#   Kyokasho.ttc
#   ToppanBunkyuMincho-Regular.otf
#   ToppanBunkyuGothic.ttc
#   ToppanBunkyuMidashiMincho-ExtraBold.otf
#   ToppanBunkyuMidashiGothic-ExtraBold.otf
# are bundled with macOS 10.12 Sierra or later versions.

Name: TsukuARdGothic-Regular
Class: Japan
OTCname: TsukushiAMaruGothic.ttc(0)

Name: TsukuARdGothic-Bold
Class: Japan
OTCname: TsukushiAMaruGothic.ttc(1)

Name: TsukuBRdGothic-Regular
Class: Japan
OTCname: TsukushiBMaruGothic.ttc(0)

Name: TsukuBRdGothic-Bold
Class: Japan
OTCname: TsukushiBMaruGothic.ttc(1)

Name: Klee-Medium
Class: Japan
OTCname: Klee.ttc(1)

Name: Klee-Demibold
Class: Japan
OTCname: Klee.ttc(0)

Name: YuKyo_Yoko-Medium
Class: Japan
OTCname: Kyokasho.ttc(0)

Name: YuKyo_Yoko-Bold
Class: Japan
OTCname: Kyokasho.ttc(1)

Name: YuKyo-Medium
Class: Japan
OTCname: Kyokasho.ttc(2)

Name: YuKyo-Bold
Class: Japan
OTCname: Kyokasho.ttc(3)

Name: ToppanBunkyuMincho-Regular
PSName: ToppanBunkyuMinchoPr6N-Regular
Class: Japan
OTFname: ToppanBunkyuMincho-Regular.otf

Name: ToppanBunkyuGothicPr6N-DB
Class: Japan
OTCname: ToppanBunkyuGothic.ttc(0)

Name: ToppanBunkyuGothicPr6N-Regular
Class: Japan
OTCname: ToppanBunkyuGothic.ttc(1)

Name: ToppanBunkyuMidashiMincho-ExtraBold
PSName: ToppanBunkyuMidashiMinchoStdN-ExtraBold
Class: Japan
OTFname: ToppanBunkyuMidashiMincho-ExtraBold.otf

Name: ToppanBunkyuMidashiGothic-ExtraBold
PSName: ToppanBunkyuMidashiGothicStdN-ExtraBold
Class: Japan
OTFname: ToppanBunkyuMidashiGothic-ExtraBold.otf

#
# CHINESE FONTS
#

# Hiragino chinese (OS X)

Name: HiraginoSansGB-W3
Class: GB
Provides(50): STHeiti-Light
OTFname(20): Hiragino Sans GB W3.otf
OTFname(10): HiraginoSansGB-W3.otf
OTCname(30): Hiragino Sans GB W3.ttc(0)
OTCname(28): HiraginoSansGB-W3.ttc(0)

Name: HiraginoSansGB-W6
Class: GB
Provides(50): STHeiti-Regular
OTFname(20): Hiragino Sans GB W6.otf
OTFname(10): HiraginoSansGB-W6.otf
OTCname(30): Hiragino Sans GB W6.ttc(0)
OTCname(28): HiraginoSansGB-W6.ttc(0)

Name: HiraginoSansCNS-W3
Class: CNS
OTCname(30): Hiragino Sans CNS.ttc(0)
OTCname(28): HiraginoSansCNS.ttc(0)

Name: HiraginoSansCNS-W6
Class: CNS
OTCname(30): Hiragino Sans CNS.ttc(1)
OTCname(28): HiraginoSansCNS.ttc(1)

# DynaComware (OS X)

Name: LiHeiPro
Class: CNS
#Provides(??): MHei-Medium # fails
TTFname(20): 儷黑 Pro.ttf
TTFname(10): LiHeiPro.ttf

Name: LiSongPro
Class: CNS
#Provides(??): MSung-Medium # fails
#Provides(??): MSung-Light # fails
TTFname(20): 儷宋 Pro.ttf
TTFname(10): LiSongPro.ttf

# Changzhou SinoType (OS X)

Name: STXihei
Class: GB
#Provides(??): STHeiti-Light # fails
TTFname(20): 华文细黑.ttf
TTFname(10): STXihei.ttf

Name: STHeiti
Class: GB
#Provides(??): STHeiti-Regular # fails
TTFname(20): 华文黑体.ttf
TTFname(10): STHeiti.ttf

Name: STHeitiSC-Light
Class: GB
#Provides(??): STHeiti-Light # fails
TTCname(10): STHeiti-Light.ttc(1)
TTCname(20): STHeiti Light.ttc(1)
#TTFname(30): STHeitiSC-Light.ttf

Name: STHeitiSC-Medium
Class: GB
#Provides(??): STHeiti-Regular # fails
TTCname(10): STHeiti-Medium.ttc(1)
TTCname(20): STHeiti Medium.ttc(1)
#TTFname(30): STHeitiSC-Medium.ttf

Name: STHeitiTC-Light
Class: CNS
TTCname(10): STHeiti-Light.ttc(0)
TTCname(20): STHeiti Light.ttc(0)
#TTFname(30): STHeitiTC-Light.ttf

Name: STHeitiTC-Medium
Class: CNS
#Provides(??): MHei-Medium # fails
TTCname(10): STHeiti-Medium.ttc(0)
TTCname(20): STHeiti Medium.ttc(0)
#TTFname(30): STHeitiTC-Medium.ttf

Name: STFangsong
Class: GB
#Provides(??): STFangsong-Light # fails
#Provides(??): STFangsong-Regular # fails
TTFname(20): 华文仿宋.ttf
TTFname(10): STFangsong.ttf

# TTC entry for Mountain Lion (10.8) or later
# TTF entry for Lion (10.7) or earlier
Name: STSong
Class: GB
#Provides(??): STSong-Light # fails
TTCname(10): Songti.ttc(4)
TTCname(20): 宋体.ttc(3)
TTFname(30): STSong.ttf
TTFname(40): 华文宋体.ttf

Name: STSongti-SC-Light
Class: GB
#Provides(??): STSong-Light # fails
TTCname(10): Songti.ttc(3)
TTCname(20): 宋体.ttc(2)
#TTFname(30): STSongti-SC-Light.ttf

Name: STSongti-SC-Regular
Class: GB
TTCname(10): Songti.ttc(6)
TTCname(20): 宋体.ttc(4)
#TTFname(30): STSongti-SC-Regular.ttf

Name: STSongti-SC-Bold
Class: GB
TTCname(10): Songti.ttc(1)
TTCname(20): 宋体.ttc(1)
#TTFname(30): STSongti-SC-Bold.ttf

Name: STSongti-SC-Black
Class: GB
TTCname(10): Songti.ttc(0)
TTCname(20): 宋体.ttc(0)
#TTFname(30): STSongti-SC-Black.ttf

Name: STSongti-TC-Light
Class: CNS
#Provides(??): MSung-Light # fails
TTCname(10): Songti.ttc(5)
#TTFname(20): STSongti-TC-Light.ttf

Name: STSongti-TC-Regular
Class: CNS
#Provides(??): MSung-Medium # fails
TTCname(10): Songti.ttc(7)
#TTFname(20): STSongti-TC-Regular.ttf

Name: STSongti-TC-Bold
Class: CNS
TTCname(10): Songti.ttc(2)
#TTFname(20): STSongti-TC-Bold.ttf

# Note about Mac OS X:
#   Kaiti.ttc
# contained 6 fonts before OS X 10.11.6 El Capitan.
# After macOS 10.12 Sierra, it contains 7 fonts and
# the order of ttc index has completely changed.

# TTF entry for Lion (10.7) or earlier
# TTC entry for Mountain Lion (10.8) or later
Name: STKaiti
Class: GB
#Provides(??): STKaiti-Regular # fails
## for Sierra (10.12) or later
TTCname(10): Kaiti.ttc(1)
## for El Capitan (10.11.6) or earlier
#TTCname(10): Kaiti.ttc(4)
TTCname(20): 楷体.ttc(3)
TTFname(30): STKaiti.ttf
TTFname(40): 华文楷体.ttf

# TTF entry for Lion (10.7) or earlier
# TTC entry for Mountain Lion (10.8) or later
Name: STKaiti-Adobe-CNS1
Class: CNS
#Provides(??): MKai-Medium # fails
## for Sierra (10.12) or later
TTCname(10): Kaiti.ttc(1)
## for El Capitan (10.11.6) or earlier
#TTCname(10): Kaiti.ttc(4)
TTCname(20): 楷体.ttc(3)
TTFname(30): STKaiti.ttf
TTFname(40): 华文楷体.ttf

# for El Capitan (10.11.6) or earlier
Name: STKaiti-SC-Regular
Class: GB
#Provides(??): STKaiti-Regular # fails
TTCname(10): Kaiti.ttc(3)
TTCname(20): 楷体.ttc(2)
#TTFname(30): STKaiti-SC-Regular.ttf

# for Sierra (10.12) or later
Name: STKaitiSC-Regular
Class: GB
#Provides(??): STKaiti-Regular # fails
TTCname(10): Kaiti.ttc(0)

# for El Capitan (10.11.6) or earlier
Name: STKaiti-SC-Bold
Class: GB
TTCname(10): Kaiti.ttc(1)
TTCname(20): 楷体.ttc(1)
#TTFname(30): STKaiti-SC-Bold.ttf

# for Sierra (10.12) or later
Name: STKaitiSC-Bold
Class: GB
TTCname(10): Kaiti.ttc(3)

# for El Capitan (10.11.6) or earlier
Name: STKaiti-SC-Black
Class: GB
TTCname(10): Kaiti.ttc(0)
TTCname(20): 楷体.ttc(0)
#TTFname(30): STKaiti-SC-Black.ttf

# for Sierra (10.12) or later
Name: STKaitiSC-Black
Class: GB
TTCname(10): Kaiti.ttc(5)

# for El Capitan (10.11.6) or earlier
Name: STKaiTi-TC-Regular
Class: CNS
#Provides(??): MKai-Medium # fails
TTCname(10): Kaiti.ttc(5)
#TTFname(20): STKaiTi-TC-Regular.ttf

# for Sierra (10.12) or later
Name: STKaitiTC-Regular
Class: CNS
#Provides(??): MKai-Medium # fails
TTCname(10): Kaiti.ttc(2)

# for El Capitan (10.11.6) or earlier
Name: STKaiTi-TC-Bold
Class: CNS
TTCname(10): Kaiti.ttc(2)
#TTFname(20): STKaiTi-TC-Bold.ttf

# for Sierra (10.12) or later
Name: STKaitiTC-Bold
Class: CNS
TTCname(10): Kaiti.ttc(4)

# for Sierra (10.12) or later (New!)
Name: STKaitiTC-Black
Class: CNS
TTCname(10): Kaiti.ttc(6)

Name: STBaoliSC-Regular
Class: GB
TTCname: Baoli.ttc(0)

Name: STBaoliTC-Regular
Class: CNS
TTCname: Baoli.ttc(1)

Name: STLibianSC-Regular
Class: GB
TTCname: Libian.ttc(0)

Name: STLibianTC-Regular
Class: CNS
TTCname: Libian.ttc(1)

Name: STXingkaiSC-Bold
Class: GB
TTCname: Xingkai.ttc(0)

Name: STXingkaiTC-Bold
Class: CNS
TTCname: Xingkai.ttc(1)

Name: STXingkaiSC-Light
Class: GB
TTCname: Xingkai.ttc(2)

Name: STXingkaiTC-Light
Class: CNS
TTCname: Xingkai.ttc(3)

Name: STYuanti-SC-Regular
Class: GB
TTCname: Yuanti.ttc(0)

Name: STYuanti-TC-Regular
Class: CNS
TTCname: Yuanti.ttc(1)

Name: STYuanti-SC-Bold
Class: GB
TTCname: Yuanti.ttc(2)

Name: STYuanti-TC-Bold
Class: CNS
TTCname: Yuanti.ttc(3)

Name: STYuanti-SC-Light
Class: GB
TTCname: Yuanti.ttc(4)

Name: STYuanti-TC-Light
Class: CNS
TTCname: Yuanti.ttc(5)

# Beijing Founder Electronics (OS X)

# Lantinghei SC Demibold
Name: FZLTZHK--GBK1-0
Class: GB
TTCname: Lantinghei.ttc(0)

# Lantinghei SC Extralight
Name: FZLTXHK--GBK1-0
Class: GB
TTCname: Lantinghei.ttc(1)

# Lantinghei SC Heavy
Name: FZLTTHK--GBK1-0
Class: GB
TTCname: Lantinghei.ttc(2)

# Lantinghei TC Demibold
Name: FZLTZHB--B51-0
Class: CNS
TTCname: Lantinghei.ttc(3)

# Lantinghei TC Extralight
Name: FZLTXHB--B51-0
Class: CNS
TTCname: Lantinghei.ttc(4)

# Lantinghei TC Heavy
Name: FZLTTHB--B51-0
Class: CNS
TTCname: Lantinghei.ttc(5)

# Arphic Font Design Team (OS X)

Name: WeibeiSC-Bold
PSName: Weibei-SC-Bold
Class: GB
OTFname: WeibeiSC-Bold.otf

Name: WeibeiTC-Bold
PSName: Weibei-TC-Bold
Class: CNS
OTFname: WeibeiTC-Bold.otf

# Monotype Imaging (OS X)

Name: YuppySC-Regular
Class: GB
OTFname: YuppySC-Regular.otf

Name: YuppyTC-Regular
Class: CNS
OTFname: YuppyTC-Regular.otf

# Monotype Hong Kong (OS X)

Name: LingWaiSC-Medium
PSName: MLingWaiMedium-SC
Class: GB
OTFname: LingWaiSC-Medium.otf

Name: LingWaiTC-Medium
PSName: MLingWaiMedium-TC
Class: CNS
OTFname: LingWaiTC-Medium.otf

# DynaComware Taiwan (OS X)

Name: WawaSC-Regular
PSName: DFWaWaSC-W5
Class: GB
OTFname: WawaSC-Regular.otf

Name: HannotateSC-W5
Class: GB
OTCname: Hannotate.ttc(0)

Name: HannotateTC-W5
Class: CNS
OTCname: Hannotate.ttc(1)

Name: HannotateSC-W7
Class: GB
OTCname: Hannotate.ttc(2)

Name: HannotateTC-W7
Class: CNS
OTCname: Hannotate.ttc(3)

Name: HanziPenSC-W3
Class: GB
OTCname: Hanzipen.ttc(0)

Name: HanziPenTC-W3
Class: CNS
OTCname: Hanzipen.ttc(1)

Name: HanziPenSC-W5
Class: GB
OTCname: Hanzipen.ttc(2)

Name: HanziPenTC-W5
Class: CNS
OTCname: Hanzipen.ttc(3)

# Adobe chinese fonts

# simplified chinese

Name: AdobeSongStd-Light
Class: GB
Provides(30): STSong-Light
OTFname(10): AdobeSongStd-Light.otf

Name: AdobeHeitiStd-Regular
Class: GB
Provides(30): STHeiti-Regular
Provides(30): STHeiti-Light
OTFname(20): AdobeHeitiStd-Regular.otf

Name: AdobeKaitiStd-Regular
Class: GB
Provides(30): STKaiti-Regular
OTFname(20): AdobeKaitiStd-Regular.otf

Name: AdobeFangsongStd-Regular
Class: GB
Provides(30): STFangsong-Light
Provides(30): STFangsong-Regular
OTFname(20): AdobeFangsongStd-Regular.otf

# traditional chinese

Name: AdobeMingStd-Light
Class: CNS
Provides(30): MSung-Light
Provides(30): MSung-Medium
OTFname(20): AdobeMingStd-Light.otf

Name: AdobeFanHeitiStd-Bold
Class: CNS
Provides(30): MHei-Medium
Provides(30): MKai-Medium
OTFname(20): AdobeFanHeitiStd-Bold.otf

# Fandol (free)

Name: FandolSong-Regular
Class: GB
Provides(40): STSong-Light
OTFname(10): FandolSong-Regular.otf

Name: FandolSong-Bold
Class: GB
OTFname(10): FandolSong-Bold.otf

Name: FandolKai-Regular
Class: GB
Provides(40): STKaiti-Regular
OTFname(10): FandolKai-Regular.otf

Name: FandolHei-Regular
Class: GB
Provides(40): STHeiti-Regular
Provides(40): STHeiti-Light
OTFname(10): FandolHei-Regular.otf

Name: FandolHei-Bold
Class: GB
OTFname(10): FandolHei-Bold.otf

Name: FandolFang-Regular
Class: GB
Provides(40): STFangsong-Light
Provides(40): STFangsong-Regular
OTFname(10): FandolFang-Regular.otf

# Arphic (free)

Name: BousungEG-Light-GB
Class: GB
Provides(80): STSong-Light
Provides(80): STFangsong-Light
Provides(80): STFangsong-Regular
TTFname: gbsn00lp.ttf

Name: GBZenKai-Medium
Class: GB
Provides(80): STKaiti-Regular
Provides(80): STHeiti-Regular
Provides(80): STHeiti-Light
TTFname: gkai00mp.ttf

Name: ShanHeiSun-Light
Class: CNS
Provides(80): MSung-Light
Provides(80): MSung-Medium
TTFname: bsmi00lp.ttf

Name: ZenKai-Medium
Class: CNS
Provides(80): MKai-Medium
Provides(80): MHei-Medium
TTFname: bkai00mp.ttf

# CJK-Unifonts new ttc edition (free)

Name: UMingCN
Class: GB
Provides(70): STSong-Light
Provides(70): STFangsong-Light
Provides(70): STFangsong-Regular
TTCname: uming.ttc(0)

Name: UMingTW
Class: CNS
Provides(70): MSung-Light
Provides(70): MSung-Medium
TTCname: uming.ttc(2)

Name: UKaiCN
Class: GB
Provides(70): STKaiti-Regular
Provides(70): STHeiti-Regular
Provides(70): STHeiti-Light
TTCname: ukai.ttc(0)

Name: UKaiTW
Class: CNS
Provides(70): MKai-Medium
Provides(70): MHei-Medium
TTCname: ukai.ttc(2)

# CJK-Unifonts old ttf edition (free)

# CNS
Name: ShanHeiSun-Uni
Class: CNS
Provides(90): MSung-Light
Provides(90): MSung-Medium
TTFname: uming.ttf
# GB
Name: ShanHeiSun-Uni-Adobe-GB1
Class: GB
Provides(90): STSong-Light
Provides(90): STFangsong-Light
Provides(90): STFangsong-Regular
TTFname: uming.ttf

# CNS
Name: ZenKai-Uni
Class: CNS
Provides(90): MKai-Medium
Provides(90): MHei-Medium
TTFname: ukai.ttf
# GB
Name: ZenKai-Uni-Adobe-GB1
Class: GB
Provides(90): STKaiti-Regular
Provides(90): STHeiti-Regular
Provides(90): STHeiti-Light
TTFname: ukai.ttf

#
# KOREAN FONTS
#

# Solaris-hanyang (Solaris 10, 11)

Name: Myeongjo
Class: Korea
Provides(40): HYSMyeongJo-Medium
TTFname: h2mjsm.ttf

Name: Gothic
Class: Korea
Provides(40): HYGoThic-Medium
TTFname: h2gtrm.ttf

Name: RoundedGothic
Class: Korea
Provides(40): HYRGoThic-Medium
TTFname: h2drrm.ttf

Name: Haeseo
Class: Korea
TTFname: h2hsrm.ttf

Name: SunDotum
Class: Korea
TTFname: sundotump.ttf

Name: SunDotumChe
Class: Korea
TTFname: sundotumf.ttf

# Baekmuk (free)
# This is a special case, because "batang.ttf" in baekmuk and
# "Batang.ttf" in Microsoft Mac Office font share the same filename;
# symlink name should be "Baekmuk-Batang.ttf"
# similar for "Gulim.ttf" -- HY (2016/09/29)

Name: Baekmuk-Batang
Class: Korea
Provides(70): HYSMyeongJo-Medium
TTFname(20): batang.ttf
TTFname(10): Baekmuk-Batang.ttf

Name: Baekmuk-Dotum
Class: Korea
Provides(70): HYGoThic-Medium
TTFname(20): dotum.ttf
TTFname(10): Baekmuk-Dotum.ttf

Name: Baekmuk-Gulim
Class: Korea
Provides(70): HYRGoThic-Medium
TTFname(20): gulim.ttf
TTFname(10): Baekmuk-Gulim.ttf

Name: Baekmuk-Headline
Class: Korea
TTFname(20): hline.ttf
TTFname(10): Baekmuk-Headline.ttf

# Unfonts-core (free)

Name: UnBatang
Class: Korea
Provides(60): HYSMyeongJo-Medium
TTFname: UnBatang.ttf

Name: UnBatang-Bold
Class: Korea
TTFname: UnBatangBold.ttf

Name: UnDotum
Class: Korea
Provides(60): HYGoThic-Medium
TTFname: UnDotum.ttf

Name: UnDotum-Bold
Class: Korea
TTFname: UnDotumBold.ttf

Name: UnDinaru
Class: Korea
Provides(60): HYRGoThic-Medium
TTFname: UnDinaru.ttf

Name: UnDinaru-Bold
Class: Korea
TTFname: UnDinaruBold.ttf

Name: UnDinaru-Light
Class: Korea
TTFname: UnDinaruLight.ttf

Name: UnGraphic
Class: Korea
TTFname: UnGraphic.ttf

Name: UnGraphic-Bold
Class: Korea
TTFname: UnGraphicBold.ttf

Name: UnGungseo
Class: Korea
TTFname: UnGungseo.ttf

Name: UnPilgi
Class: Korea
TTFname: UnPilgi.ttf

Name: UnPilgi-Bold
Class: Korea
TTFname: UnPilgiBold.ttf

# Unfonts-extra (free)

Name: UnBom
Class: Korea
TTFname: UnBom.ttf

Name: UnPen
Class: Korea
TTFname: UnPen.ttf

Name: UnPenheulim
Class: Korea
TTFname: UnPenheulim.ttf

Name: UnPilgia
Class: Korea
TTFname: UnPilgia.ttf

Name: UnShinmun
Class: Korea
TTFname: UnShinmun.ttf

Name: UnVada
Class: Korea
TTFname: UnVada.ttf

Name: UnYetgul
Class: Korea
TTFname: UnYetgul.ttf

Name: UnTaza
Class: Korea
TTFname: UnTaza.ttf

# UnJamo... family has proportional metrics
Name: UnJamoBatang
Class: Korea
TTFname: UnJamoBatang.ttf

Name: UnJamoDotum
Class: Korea
TTFname: UnJamoDotum.ttf

Name: UnJamoNovel
Class: Korea
TTFname: UnJamoNovel.ttf

Name: UnJamoSora
Class: Korea
TTFname: UnJamoSora.ttf

# Nanum (free - TTF files) and Nanum OS X (free - TTC files)
# note that all fonts have narrow metrics

Name: NanumMyeongjo
Class: Korea
TTFname(10): NanumMyeongjo.ttf
TTCname(20): NanumMyeongjo.ttc(0)

Name: NanumMyeongjoBold
Class: Korea
TTFname(10): NanumMyeongjoBold.ttf
TTCname(20): NanumMyeongjo.ttc(1)

Name: NanumMyeongjoExtraBold
Class: Korea
TTFname(10): NanumMyeongjoExtraBold.ttf
TTCname(20): NanumMyeongjo.ttc(2)

Name: NanumGothic
Class: Korea
TTFname(10): NanumGothic.ttf
TTCname(20): NanumGothic.ttc(0)

Name: NanumGothicBold
Class: Korea
TTFname(10): NanumGothicBold.ttf
TTCname(20): NanumGothic.ttc(1)

Name: NanumGothicExtraBold
Class: Korea
TTFname(10): NanumGothicExtraBold.ttf
TTCname(20): NanumGothic.ttc(2)

Name: NanumGothicLight
Class: Korea
TTFname(10): NanumGothicLight.ttf

Name: NanumBarunGothic
Class: Korea
TTFname(10): NanumBarunGothic.ttf

Name: NanumBarunGothicBold
Class: Korea
TTFname(10): NanumBarunGothicBold.ttf

Name: NanumBarunGothicLight
Class: Korea
TTFname(10): NanumBarunGothicLight.ttf

Name: NanumBarunGothicUltraLight
Class: Korea
TTFname(10): NanumBarunGothicUltraLight.ttf

Name: NanumBarunpen
Class: Korea
TTFname(10): NanumBarunpenR.ttf

Name: NanumBarunpen-Bold
Class: Korea
TTFname(10): NanumBarunpenB.ttf

Name: NanumBrush
Class: Korea
TTFname(10): NanumBrush.ttf
TTCname(20): NanumScript.ttc(0)

Name: NanumPen
Class: Korea
TTFname(10): NanumPen.ttf
TTCname(20): NanumScript.ttc(1)

# Hancom HCR (free)
# note that all fonts have narrow metrics

Name: HCRBatang
Class: Korea
TTFname: HANBatang.ttf

Name: HCRBatang-Bold
Class: Korea
TTFname: HANBatangB.ttf

Name: HCRDotum
Class: Korea
TTFname: HANDotum.ttf

Name: HCRDotum-Bold
Class: Korea
TTFname: HANDotumB.ttf

# Apple

Name: AppleMyungjo
Class: Korea
#Provides(??): HYSMyeongJo-Medium # fails
TTFname: AppleMyungjo.ttf

Name: AppleGothic
Class: Korea
#Provides(??): HYGoThic-Medium # fails
#Provides(??): HYRGoThic-Medium # fails
TTFname: AppleGothic.ttf

Name: AppleSDGothicNeo-Thin
Class: Korea
OTFname: AppleSDGothicNeo-Thin.otf

Name: AppleSDGothicNeo-UltraLight
Class: Korea
OTFname: AppleSDGothicNeo-UltraLight.otf

Name: AppleSDGothicNeo-Light
Class: Korea
OTFname: AppleSDGothicNeo-Light.otf

Name: AppleSDGothicNeo-Regular
Class: Korea
OTFname: AppleSDGothicNeo-Regular.otf

Name: AppleSDGothicNeo-Medium
Class: Korea
OTFname: AppleSDGothicNeo-Medium.otf

Name: AppleSDGothicNeo-SemiBold
Class: Korea
OTFname: AppleSDGothicNeo-SemiBold.otf

Name: AppleSDGothicNeo-Bold
Class: Korea
OTFname: AppleSDGothicNeo-Bold.otf

Name: AppleSDGothicNeo-ExtraBold
Class: Korea
OTFname: AppleSDGothicNeo-ExtraBold.otf

Name: AppleSDGothicNeo-Heavy
Class: Korea
OTFname: AppleSDGothicNeo-Heavy.otf

# Adobe korean fonts

Name: AdobeMyungjoStd-Medium
Class: Korea
Provides(30): HYSMyeongJo-Medium
OTFname: AdobeMyungjoStd-Medium.otf

Name: AdobeGothicStd-Bold
Class: Korea
Provides(30): HYGoThic-Medium
Provides(80): HYRGoThic-Medium
OTFname: AdobeGothicStd-Bold.otf

Name: AdobeGothicStd-Light
Class: Korea
OTFname: AdobeGothicStd-Light.otf

#
# Microsoft Mac Office fonts
#

# korea

Name: Batang
Class: Korea
Provides(50): HYSMyeongJo-Medium
TTFname(50): Batang.ttf

Name: Gulim
Class: Korea
Provides(50): HYRGoThic-Medium
Provides(90): HYGoThic-Medium
TTFname(30): Gulim.ttf
TTCname(50): gulim.ttc

# simplified chinese

Name: SimHei
Class: GB
Provides(60): STHeiti-Regular
Provides(60): STKaiti-Regular
Provides(60): STHeiti-Light
TTFname(50): SimHei.ttf

Name: SimSun
Class: GB
Provides(60): STSong-Light
Provides(60): STFangsong-Light
Provides(60): STFangsong-Regular
TTFname(50): SimSun.ttf

# traditional chinese

Name: MingLiU
Class: CNS
Provides(60): MHei-Medium
Provides(60): MKai-Medium
Provides(60): MSung-Medium
Provides(60): MSung-Light
TTFname(50): MingLiU.ttf

Name: PMingLiU
Class: CNS
TTFname(50): PMingLiU.ttf

# japanese

Name: MS-Gothic
Class: Japan
Provides(95): GothicBBB-Medium
Provides(95): GothicBBBPro-Medium
Provides(95): HiraKakuProN-W3
Provides(95): HiraKakuPro-W3
Provides(95): FutoGoB101-Bold
Provides(95): FutoGoB101Pro-Bold
Provides(95): HiraKakuProN-W6
Provides(95): HiraKakuPro-W6
Provides(95): MidashiGo-MB31
Provides(95): MidashiGoPro-MB31
Provides(95): HiraKakuStdN-W8
Provides(95): HiraKakuStd-W8
Provides(95): Jun101-Light
Provides(95): Jun101Pro-Light
Provides(95): HiraMaruProN-W4
Provides(95): HiraMaruPro-W4
TTFname(50): MS Gothic.ttf
TTFname(30): MS-Gothic.ttf

Name: MS-Mincho
Class: Japan
Provides(95): Ryumin-Light
Provides(95): RyuminPro-Light
Provides(95): HiraMinProN-W3
Provides(95): HiraMinPro-W3
Provides(95): FutoMinA101-Bold
Provides(95): FutoMinA101Pro-Bold
Provides(95): HiraMinProN-W6
Provides(95): HiraMinPro-W6
TTFname(50): MS Mincho.ttf
TTFname(30): MS-Mincho.ttf

Name: MS-PGothic
Class: Japan
TTFname(50): MS PGothic.ttf
TTFname(30): MS-PGothic.ttf

Name: MS-PMincho
Class: Japan
TTFname(50): MS PMincho.ttf
TTFname(30): MS-PMincho.ttf

Name: Meiryo
Class: Japan
TTFname(50): Meiryo.ttf

Name: Meiryo-Bold
Class: Japan
TTFname(50): Meiryo Bold.ttf
TTFname(30): Meiryo-Bold.ttf

Name: Meiryo-BoldItalic
Class: Japan
TTFname(50): Meiryo Bold Italic.ttf
TTFname(30): Meiryo-BoldItalic.ttf

Name: Meiryo-Italic
Class: Japan
TTFname(50): Meiryo Italic.ttf
TTFname(30): Meiryo-Italic.ttf


### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim: set tabstop=2 expandtab autoindent:
