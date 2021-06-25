#!/usr/bin/env perl
#
# cjk-gs-integrate - setup Ghostscript for CID/TTF CJK fonts
#
# Copyright 2015-2021 by Norbert Preining
# Copyright 2016-2021 by Japanese TeX Development Community
#
# This work is based on research and work by (in alphabetical order)
#   Masamichi Hosoda
#   Yusuke Kuroki
#   Yusuke Terada
#   Bruno Voisin
#   Munehiro Yamamoto
#   Hironobu Yamashita
# and the Japanese TeX Q&A wiki page
#
# This file is licensed under GPL version 3 or any later version.
# For copyright statements see end of file.
#
# For development see
#  https://github.com/texjporg/cjk-gs-support
#
# LIMITATIONS:
# - Running the script (with default mode = actual setup/removing operations)
#   always overwrites "cidfmap.local" and "cidfmap.aliases" without asking,
#   whose file names might be common enough. If you choose to run the script,
#   leave these files untouched. (Do NOT edit these files by yourself!)
#   (This note also applies to MacTeX pre-shipped configuration files.)
#
# TODO:
# - interoperability with kanji-config-updmap
#
# Note that symlink names should be consistent with ptex-fontmaps!

use Getopt::Long qw(:config no_autoabbrev ignore_case_always);
use File::Basename;
use File::Path qw(make_path);
use Cwd 'abs_path';
use strict;
use warnings;
use utf8;
use feature 'state';
use Encode;
use Encode::Alias;

my $debug_msg_before_init;
init_encode_locale ();

binmode (STDIN, ':encoding(console_in)');
binmode (STDOUT, ':encoding(console_out)');
binmode (STDERR, ':encoding(console_out)');

@ARGV = map{ decode('locale', $_) }@ARGV;

(my $prg = basename(decode('locale', $0))) =~ s/\.pl$//;
my $version = '20210625.0';

if (win32()) {
  # some perl functions (symlink, -l test) does not work
  print_warning("Sorry, we have only partial support for Windows!\n");

  require Win32::API;
  Win32::API->import ();
  require File::Compare;
  File::Compare->import ();
}

# The followings are installed by ptex-fontmaps (texjporg):
#   * 2004-H
#   * 2004-V
# The followings are created by Adobe but not considered official
# (see https://forums.adobe.com/thread/537415)
#   * GB-RKSJ-H
#   * GBT-RKSJ-H
#   * KSC-RKSJ-H
# All others are provided in the latest Adobe CMap Resources:
#   https://github.com/adobe-type-tools/cmap-resources
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
    Adobe-Japan1-7
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
    WP-Symbol
    / ],
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
    UniGB-UTF8-V
    / ],
  CNS => [ qw/
    Adobe-CNS1-0
    Adobe-CNS1-1
    Adobe-CNS1-2
    Adobe-CNS1-3
    Adobe-CNS1-4
    Adobe-CNS1-5
    Adobe-CNS1-6
    Adobe-CNS1-7
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
    UniCNS-UTF8-V
    / ],
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
    UniKS-UTF8-V
    / ] );

#
# location where links to fonts in texmf are created, relative to TEXMF
my $otf_pathpart = "fonts/opentype/cjk-gs-integrate";
my $ttf_pathpart = "fonts/truetype/cjk-gs-integrate";

# location where cidfmap, cidfmap.local and cidfmap.aliases are placed
# when found gs is tlgs (win32), then files will be placed in lib/ instead of Resource/Init/
my $cidfmap_pathpart = "Init/cidfmap";
my $cidfmap_local_pathpart = "Init/cidfmap.local";
my $cidfmap_aliases_pathpart = "Init/cidfmap.aliases";

# support for ps2otfps by Akira Kakuto
my $akotfps_pathpart = "dvips/ps2otfps";
my $akotfps_datafilename = "psnames-for-otf";
my $akotfps_datacontent = '';

# dump output for data file (for easy editing for users)
my $dump_datafile = "$prg-data.dat";

my $opt_output;
my $opt_fontdef;
my @opt_fontdef_add;
my @opt_aliases;
my $opt_filelist;
my $opt_texmflink;
my $opt_akotfps;
my $opt_force = 0;
my $opt_remove = 0;
my $opt_cleanup = 0;
my $opt_hardlink = 0;
my $opt_winbatch;
my $opt_dump_data;
my $opt_only_aliases = 0;
my $opt_listaliases = 0;
my $opt_listallaliases = 0;
my $opt_listfonts = 0;
my $opt_info = 0;
my $opt_machine = 0;
my $opt_strictpsname = 0;
my $dry_run = 0;
my $opt_quiet = 0;
my $opt_debug = 0;
my $opt_help = 0;
my $opt_markdown = 0;

if (! GetOptions(
        "o|output=s"       => \$opt_output,
        "f|fontdef=s"      => \$opt_fontdef,
        "fontdef-add=s"    => \@opt_fontdef_add,
        "a|alias=s"        => \@opt_aliases,
        "filelist=s"       => \$opt_filelist,
        "link-texmf:s"     => \$opt_texmflink,
        "otfps:s"          => \$opt_akotfps,
        "force"            => \$opt_force,
        "remove"           => \$opt_remove,
        "cleanup"          => \$opt_cleanup,
        "hardlink"         => \$opt_hardlink,
        "winbatch:s"       => \$opt_winbatch,
        "dump-data:s"      => \$opt_dump_data,
        "only-aliases"     => \$opt_only_aliases,
        "list-aliases"     => \$opt_listaliases,
        "list-all-aliases" => \$opt_listallaliases,
        "list-fonts"       => \$opt_listfonts,
        "info"             => \$opt_info,
        "machine-readable" => \$opt_machine,
        "strict-psname"    => \$opt_strictpsname, # hidden option for debugging
        "n|dry-run"        => \$dry_run,
        "q|quiet"          => \$opt_quiet,
        "d|debug+"         => \$opt_debug,
        "h|help"           => \$opt_help,
        "markdown"         => \$opt_markdown,
        "v|version"        => sub { print &version(); exit(0); }, ) ) {
  die "Try \"$0 --help\" for more information.\n";
}

if ($debug_msg_before_init) {
  print_debug ($debug_msg_before_init);
}

sub win32 { return ($^O=~/^MSWin(32|64)$/i); }
sub macosx { return ($^O=~/^darwin$/i); }
my $nul = (win32() ? 'nul' : '/dev/null') ;
my $sep = (win32() ? ';' : ':');
my %fontdb;
my %aliases;
my %user_aliases;

if ($opt_help || $opt_markdown) {
  Usage();
  exit(0);
}

# check for the existence of kpsewhich, otherwise we cannot do anything
if (system("kpsewhich --version >$nul 2>&1 <$nul" ) != 0) {
  print_error("We need `kpsewhich' being installed! Exiting.\n");
  exit(1);
}

if ($opt_debug >= 2) {
  require Data::Dumper;
  no warnings qw(once);
  $Data::Dumper::Indent = 1;
}

my $zrlistttc = kpse_miscfont("zrlistttc.lua");
my $zrlistttc_available;
my $cmdl = "texlua $zrlistttc 2>$nul";
$cmdl = encode('locale', $cmdl);
chomp(my $zrlistttc_help = `$cmdl`);
if ($?) {
  if ($opt_strictpsname) {
    print_error("The script 'zrlistttc.lua' not found, cannot proceed!\n");
    exit(1);
  }
  # show info only for debugging
  print_debug("The script 'zrlistttc.lua' not found.\n");
  print_debug("Sorry, we can't be safe enough to distinguish\n");
  print_debug("uppercase / lowercase file names.\n");
  $zrlistttc_available = 0;
} else {
  $zrlistttc_available = 1;
}

if (macosx()) {
  # due to frequent incompatible changes in font file names by Apple,
  # our built-in database doesn't support OS X 10.11 El Capitan or
  # later versions
  my $macos_ver = `sw_vers -productVersion`;
  my $macos_ver_major = $macos_ver;
  $macos_ver_major =~ s/^(\d+)\.(\d+).*/$1/;
  my $macos_ver_minor = $macos_ver;
  $macos_ver_minor =~ s/^(\d+)\.(\d+).*/$2/;
  if ($macos_ver_major>=11 || ($macos_ver_major==10 && $macos_ver_minor>=8)) {
    if (!$opt_cleanup && !$opt_fontdef && !@opt_fontdef_add) { # if built-in only
      print_warning("Our built-in database does not support recent\n");
      print_warning("versions of Mac OS (10.8 Mountain Lion or later)!\n");
      print_warning("If you want to use Hiragino fonts bundled with\n");
      print_warning("your OS, obtain external database file and\n");
      print_warning("specify it with --fontdef-add option!\n");
      print_warning("I'll continue with my built-in database ...\n");
    }
  }
}

if (defined($opt_texmflink)) {
  my $foo;
  if ($opt_texmflink eq '') {
    # option was passed but didn't receive a value
    #  -> use TEXMFLOCAL
    $foo = `kpsewhich -var-value=TEXMFLOCAL`;
    # We assume that the output of kpsewhich is
    # the same as perl's locale (or active code page).
    decode('locale', $foo);
    chomp($foo);
  } else {
    # option was passed with an argument
    #  -> use it
    $foo = $opt_texmflink;
  }
  $opt_texmflink = $foo;
}

if (defined($opt_akotfps)) {
  my $foo;
  if ($opt_akotfps eq '') {
    if (defined($opt_texmflink)) {
      $foo = $opt_texmflink;
    } else {
      $foo = `kpsewhich -var-value=TEXMFLOCAL`;
      # We assume that the output of kpsewhich is
      # the same as perl's locale (or active code page).
      decode('locale', $foo);
      chomp($foo);
    }
  } else {
    $foo = $opt_akotfps;
  }
  $opt_akotfps = $foo;
}

if (defined($opt_winbatch)) {
  print_warning("ignoring --winbatch option due to being obsolete\n");
}
if ($opt_hardlink) {
  print_warning("ignoring --hardlink option due to being obsolete\n");
}

if (defined($opt_dump_data)) {
  if ($opt_dump_data ne '') {
    $dump_datafile = $opt_dump_data;
  }
  $opt_dump_data = 1;
  unlink encode('locale_fs', $dump_datafile)
      if (-f encode('locale_fs', $dump_datafile));
} else {
  $opt_dump_data = 0;
}

if ($opt_cleanup) {
  $opt_remove = 1;
}

if ($opt_info) {
  $opt_listfonts = 1;
  $opt_listaliases = 1;
}

# check exclusive options; unsafe due to make_all_available()
if ($opt_listallaliases && $opt_listaliases) {
  print_error("Both --list-all-aliases and --list-aliases!? I'm confused!\n");
  exit(1);
}
if ($opt_listallaliases && $opt_listfonts) {
  print_error("Options --list-all-aliases and --list-fonts cannot be used at the same time!\n");
  exit(1);
}
if ($opt_cleanup && $opt_listfonts) {
  print_error("Options --cleanup and --list-fonts cannot be used at the same time!\n");
  exit(1);
}
if ($opt_cleanup && $opt_listaliases) {
  print_error("Options --cleanup and --list-aliases cannot be used at the same time!\n");
  exit(1);
}

main(@ARGV);

#
# only sub definitions from here on
#
sub main {
  # first, read font database to obtain %fontdb
  print_info("reading font database ...\n");
  read_font_database();
  if ($opt_dump_data) {
    # with --dump-data, dump only effective database and exit
    dump_font_database();
    if (-f encode('locale_fs', $dump_datafile)) {
      print_info("*** Data dumped to $dump_datafile ***\n");
      exit(0);
    } else {
      print_error("Failed to dump data to $dump_datafile!\n");
      exit(1);
    }
  }
  # second, determine non-otf link name
  # this is actually required only by info_found_fonts() and do_nonotf_fonts()
  # operations, but it does no harm for other cases too
  determine_nonotf_link_name(); # see comments there

  # set 'available' flags and 'type' by kpsewhich search
  # if $opt_cleanup or $opt_listallaliases is given, treat all files
  # in the database as if they were actually available as OTF
  if (!$opt_cleanup && !$opt_listallaliases) {
    print_info("checking for files ...\n");
    check_for_files();
  } else {
    make_all_available();
  }
  # obtain %aliases and %user_aliases
  compute_aliases();

  # informative operations
  if ($opt_listfonts) {
    info_found_fonts();
  }
  if ($opt_listaliases || $opt_listallaliases) {
    info_list_aliases();
  }
  exit(0) if ($opt_listfonts || $opt_listaliases || $opt_listallaliases);
  # if $opt_machine is still alive after the above exit(0), it's useless
  if ($opt_machine) {
    print_error("Option --machine-readable should be used with at least one of the followings:\n");
    print_error("  --list-aliases, --list-all-aliases, --list-fonts, --info\n");
    print_error("terminating.\n");
    exit(1);
  }

  # do actual setup/removing operations
  if (!$opt_output) {
    print_info("searching for Ghostscript resource\n");
    my $gsres = find_gs_resource();
    if (!$gsres) {
      print_error("Cannot find Ghostscript, terminating!\n");
      exit(1);
    } else {
      $opt_output = $gsres;
    }
  }
  if (! -d encode('locale_fs', $opt_output)) {
    $dry_run || mkdir(encode('locale_fs', $opt_output)) ||
      die("Cannot create directory $opt_output: $!");
  }
  if ($opt_cleanup) {
    print_info("going to clean up $opt_output\n");
  } else {
    print_info("output is going to $opt_output\n");
  }
  if (!$opt_only_aliases) {
    if ($opt_cleanup) {
      # all font types are handled at the same time
      print_info("cleaning up all links, snippets and cidfmap.local ...\n");
      do_all_fonts();
    } else {
      # OTF and TTF/TTC/OTC must be handled separately, depending on the found files
      print_info(($opt_remove ? "removing" : "generating") . " links and snippets for CID fonts ...\n");
      do_otf_fonts();
      print_info(($opt_remove ? "removing" : "generating") . " links, snippets and cidfmap.local for non-CID fonts ...\n");
      do_nonotf_fonts();
    }
  }
  print_info(($opt_remove ? "removing" : "generating") . " snippets and cidfmap.aliases for font aliases ...\n");
  do_aliases();
  do_cmaps();
  write_akotfps_datafile() if $opt_akotfps;
  if ($opt_texmflink && !$dry_run) {
    print_info("running mktexlsr ...\n");
    system("mktexlsr");
  }
  print_info("finished\n");
}

sub do_all_fonts {
  # try to clean up all possible links/snippets/cidfmap which could have been
  # generated in the previous runs
  # previous versions of cjk-gs-integrate included following bugs:
  #   * the database sometimes identified GB/CNS classes wrongly
  #   * symlink names were sometimes invalid (some of which contained
  #     white-spaces, due to the absence of proper database entry) or
  #     inconsistent with ptex-fontmaps (Name <-> PSName or redundant Filename)
  #   * confused symlinks between TTF/TTC/OTC (including ttf <-> ttc links)
  # also, current version generates OTC links into $otf_pathpart instead of
  # $ttf_pathpart, which was not true in the older versions
  # we'd like to clean up all such files
  my $fontdest = "$opt_output/Font";
  my $ciddest  = "$opt_output/CIDFont";
  my $cidfsubst = "$opt_output/CIDFSubst";
  for my $k (sort keys %fontdb) {
    #
    # remove snippets: note that $class = $fontdb{$k}{'class'} is not enough
    # due to previous bugs
    for my $class (%encode_list) {
      for my $enc (@{$encode_list{$class}}) {
        unlink encode('locale_fs', "$fontdest/$k-$enc")
            if (-f encode('locale_fs', "$fontdest/$k-$enc"));
      }
    }
    #
    # remove links; borrow link_font operation here for convenience
    # since we don't need target in cleanup mode, initialize with stub "none"
    #
    # for OTF/TTF fonts, first remove both $k[.otf,ttf] and $fontdb{$k}{'origname'}[.otf]
    # the links $k.otf and $fontdb{$k}{'origname'} are coming from previous bugs,
    # and the links $k.ttf are (sometimes) coming from inconsistency
    if ($fontdb{$k}{'origname'}) { # this test skips alias-only fonts (e.g. Ryumin-Light)
      link_font("none", $ciddest, $k);
      link_font("none", $cidfsubst, "$k.ttf");
      link_font("none", "$opt_texmflink/$otf_pathpart", "$k.otf")
        if $opt_texmflink;
      link_font("none", "$opt_texmflink/$ttf_pathpart", "$k.ttf")
        if $opt_texmflink;
      link_font("none", $ciddest, $fontdb{$k}{'origname'});
      link_font("none", "$opt_texmflink/$otf_pathpart", "$fontdb{$k}{'origname'}.otf")
        if $opt_texmflink;
    }
    # for OTF/TTF/TTC/OTC fonts, loop through all file candidates
    for my $f (keys %{$fontdb{$k}{'files'}}) {
      # check for subfont extension
      my $foo = $f;
      $foo =~ s/^(.*)\(\d*\)$/$1/;
      link_font("none", $cidfsubst, "$foo");
      link_font("none", "$opt_texmflink/$otf_pathpart", "$foo") if $opt_texmflink;
      link_font("none", "$opt_texmflink/$ttf_pathpart", "$foo") if $opt_texmflink;
    }
  }
  update_master_cidfmap('cidfmap.local');
  # we are in cleanup mode, also remove cidfmap.local itself
  if (-f encode('locale_fs', "$opt_output/$cidfmap_local_pathpart")) {
    unlink encode('locale_fs', "$opt_output/$cidfmap_local_pathpart");
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
  for my $k (sort keys %fontdb) {
    if ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'OTF') {
      generate_font_snippet($fontdest,
        $k, $fontdb{$k}{'class'}, $fontdb{$k}{'target'});
      link_font($fontdb{$k}{'target'}, $ciddest, $k);
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$otf_pathpart", "$fontdb{$k}{'origname'}.otf")
        if $opt_texmflink;
    }
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
  for my $k (sort keys %fontdb) {
    if ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'TTF') {
      $outp .= generate_cidfmap_entry($k, $fontdb{$k}{'class'}, $fontdb{$k}{'ttfname'}, -1);
      link_font($fontdb{$k}{'target'}, $cidfsubst, $fontdb{$k}{'ttfname'});
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$ttf_pathpart", $fontdb{$k}{'ttfname'})
        if $opt_texmflink;
    } elsif ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'TTC') {
      $outp .= generate_cidfmap_entry($k, $fontdb{$k}{'class'}, $fontdb{$k}{'ttcname'}, $fontdb{$k}{'subfont'});
      link_font($fontdb{$k}{'target'}, $cidfsubst, $fontdb{$k}{'ttcname'});
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$ttf_pathpart", $fontdb{$k}{'ttcname'})
        if $opt_texmflink;
    } elsif ($fontdb{$k}{'available'} && $fontdb{$k}{'type'} eq 'OTC') {
      # currently Ghostscript does not have OTC support; we don't know what to do
      print_debug("gs does not support OTC, not creating gs resource for $k\n");
      link_font($fontdb{$k}{'target'}, "$opt_texmflink/$otf_pathpart", $fontdb{$k}{'otcname'})
        if $opt_texmflink;
    }
  }
  return if $dry_run;
  if ($outp) {
    if (! -d encode('locale_fs', "$opt_output/Init")) {
      mkdir(encode('locale_fs', "$opt_output/Init")) ||
        die("Cannot create directory $opt_output/Init: $!");
    }
    # It seems that tlgs can handle UTF-8 TTF filename in cidfmap.local.
    open(FOO, ">:encoding(UTF-8)",
         encode('locale_fs', "$opt_output/$cidfmap_local_pathpart")) ||
      die("Cannot open $opt_output/$cidfmap_local_pathpart: $!");
    print FOO $outp;
    close(FOO);
  }
  update_master_cidfmap('cidfmap.local');
}

sub do_aliases {
  my $fontdest = "$opt_output/Font";
  my $ciddest  = "$opt_output/CIDFont"; # required for Heisei* check only
  my $cidfsubst = "$opt_output/CIDFSubst";
  my $outp = '';
  #
  # alias handling
  # we use two levels of aliases
  #  * one is for the default generic names (these are not actual fonts)
  #      Ryumin-Light, GothicBBB-Medium, ... etc.
  #  * the second level of aliases is for Morisawa OTF font names
  #      RyuminPro-Light, GothicBBBPro-Medium, ... etc.
  # the order of fonts selected is
  # defined in the Provides(Priority): Name in the font definiton
  #
  $outp .= "\n\n% Aliases\n";
  #
  my (@jal, @kal, @tal, @sal, @ai0al);
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
      if (!%{$aliases{$al}}) {
        print_warning("Alias candidate for $al is empty, skipping!\n");
        next;
      }
      # search lowest number
      my @ks = keys(%{$aliases{$al}});
      my $first = (sort { $a <=> $b} @ks)[0];
      $target = $aliases{$al}{$first};
      $class  = $fontdb{$target}{'class'};
    }
    # we also need to create font snippets in Font (or add configuration)
    # for the aliases!
    generate_font_snippet($fontdest, $al, $class, $target);
    if ($class eq 'Japan') {
      push @jal, "/$al /$target ;";
    } elsif ($class eq 'Korea') {
      push @kal, "/$al /$target ;";
    } elsif ($class eq 'GB') {
      push @sal, "/$al /$target ;";
    } elsif ($class eq 'CNS') {
      push @tal, "/$al /$target ;";
    } elsif ($class eq 'AI0') {
      push @ai0al, "/$al /$target ;";
    } else {
      print STDERR "unknown class $class for $al\n";
    }
  }
  # special case for native CID fonts in ancient days
  # if not readable, add aliases for substitution
  push @jal, "/HeiseiMin-W3 /Ryumin-Light ;"
      if (! -r encode('locale_fs', "$ciddest/HeiseiMin-W3"));
  push @jal, "/HeiseiKakuGo-W5 /GothicBBB-Medium ;"
      if (! -r encode('locale_fs', "$ciddest/HeiseiKakuGo-W5"));
  #
  $outp .= "\n% Japanese fonts\n" . join("\n", @jal) . "\n" if @jal;
  $outp .= "\n% Korean fonts\n" . join("\n", @kal) . "\n" if @kal;
  $outp .= "\n% Traditional Chinese fonts\n" . join("\n", @tal) . "\n" if @tal;
  $outp .= "\n% Simplified Chinese fonts\n" . join("\n", @sal) . "\n" if @sal;
  $outp .= "\n% Adobe-Identity-0 fonts\n" . join("\n", @ai0al) . "\n" if @ai0al;
  #
  return if $dry_run;
  if ($outp && !$opt_remove) {
    if (! -d encode('locale_fs', "$opt_output/Init")) {
      mkdir(encode('locale_fs', "$opt_output/Init")) ||
        die("Cannot create directory $opt_output/Init: $!");
    }
    open(FOO, ">:encoding(UTF-8)",
         encode('locale_fs', "$opt_output/$cidfmap_aliases_pathpart")) ||
      die("Cannot open $opt_output/$cidfmap_aliases_pathpart: $!");
    print FOO $outp;
    close(FOO);
  }
  update_master_cidfmap('cidfmap.aliases');
  # if we are in cleanup mode, also remove cidfmap.aliases itself
  if (-f encode('locale_fs', "$opt_output/$cidfmap_aliases_pathpart")) {
    unlink encode('locale_fs', "$opt_output/$cidfmap_aliases_pathpart")
        if $opt_cleanup;
  }
}

sub do_cmaps {
  # add symlinking CMaps
  # for which we generate snippets but gs does not provide
  my $cmapdest = "$opt_output/CMap";
  return if $dry_run;
  if ($opt_remove) {
    # we remove only if both of the following conditions are met:
    #   (1) it is a link
    #   (2) the link target is the same as kpsewhich result
    # otherwise it's unsafe to remove, as it may have been added
    # by others or distributed by gs itself
    for my $class (%encode_list) {
      for my $enc (@{$encode_list{$class}}) {
        if (-l encode('locale_fs', "$cmapdest/$enc")) {
          my $linkt = decode('locale_fs',
                             readlink(encode('locale_fs',
                                             "$cmapdest/$enc")));
          if ($linkt) {
            if ($linkt eq search_cmap($enc)) {
              unlink(encode('locale_fs', "$cmapdest/$enc"));
            }
          }
        }
      }
    }
    return;
  }
  # add mode
  if (! -d encode('locale_fs', "$cmapdest")) {
    print_debug("Creating directory $cmapdest ...\n");
    make_dir("$cmapdest", "cannot create CMap directory");
  }
  for my $class (%encode_list) {
    if ($class =~ m/^AI0-(.*)$/) {
      # skip AI0 font-specific CMap when the real font is unavailable
      next if (!$fontdb{$1}{'available'});
    }
    for my $enc (@{$encode_list{$class}}) {
      if (! -f encode('locale_fs', "$cmapdest/$enc")) {
        print_debug("CMap $enc is not found in gs resource directory\n");
        my $dest = search_cmap($enc);
        if ($dest) {
          print_debug("Symlinking CMap $dest ...\n");
          link_font($dest, "$cmapdest", $enc);
        } else {
          print_debug("CMap $enc is not found by kpsewhich\n");
        }
      }
    }
  }
}

my %cmap_cache;

sub search_cmap {
  my ($cmap) = @_;
  # search CMap with kpsewhich and cache
  if (! exists $cmap_cache{$cmap}) {
    my $cmdl = "kpsewhich -format=cmap $cmap";
    $cmdl = encode('locale', $cmdl);
    $cmap_cache{$cmap} = `$cmdl`;
    # We assume that the output of kpsewhich is
    # the same as perl's locale (or active code page).
    $cmap_cache{$cmap} = decode('locale', $cmap_cache{$cmap});
    chomp($cmap_cache{$cmap});
    $cmap_cache{$cmap} =~ s/[\r\n]+\z//; # perl's chomp() on git-bash cannot strip CR of CRLF ??
  }
  return $cmap_cache{$cmap};
}

sub update_master_cidfmap {
  # what we have to do is:
  #   in add mode:
  #     * add an entry for the given argument
  #     * for tlgs.win32 pre-shipped cidfmap, prepend '%' to override
  #       the default of "(cidfmap.TeXLive) .runlibfile",
  #   in remove mode:
  #     * remove an entry for the given argument
  #     * for tlgs.win32 pre-shipped cidfmap, remove '%' to restore the default
  my $add = shift;
  my $cidfmap_master = "$opt_output/$cidfmap_pathpart";
  print_info(sprintf("%s $add %s cidfmap file ...\n",
    ($opt_remove ? "removing" : "adding"), ($opt_remove ? "from" : "to")));
  if (-r encode('locale_fs', $cidfmap_master)) {
    open(FOO, "<:encoding(UTF-8)", encode('locale_fs', $cidfmap_master)) ||
      die("Cannot open $cidfmap_master for reading: $!");
    my $found = 0;
    my $found_tl = 0;
    my $newmaster = "";
    # in add mode: just search for the entry and set $found
    # in remove mode: collect all lines that do not match
    # also, we handle "cidfmap.TeXLive" now
    while(<FOO>) {
      if (m/^\s*\(\Q$add\E\)\s\s*\.runlibfile\s*$/) {
        $found = 1;
      } elsif (m/^\s*\(cidfmap\.TeXLive\)\s\s*\.runlibfile\s*$/) {
        # if found, it has to be disabled in add mode in a way in which it can
        # be detected in the (future) remove mode
        next if $found_tl; # skip it as duplicate (though unlikely to happen)
        $found_tl = 1;
        $newmaster .= "\%" if (!$opt_remove); # in add mode, disable it
        $newmaster .= $_; # pass it as-is
      } elsif (m/^\s*\%\%*\s*\(cidfmap\.TeXLive\)\s\s*\.runlibfile\s*$/) {
        # if found, it should be the one disabled by myself in the previous run;
        # restore it in remove mode
        next if $found_tl; # skip it as duplicate (though unlikely to happen)
        $found_tl = 1;
        $_ =~ s/\%//g if $opt_remove; # in remove mode, enable it
        $newmaster .= $_; # pass it
      } else {
        $newmaster .= $_;
      }
    }
    close(FOO);
    # if the original master cidfmap has a new line at end of file,
    # then $newmaster should end with "\n".
    # otherwise we add a new line, since there is a possibility of %EOF comment
    # without trailing new line (e.g. TL before r44039)
    $newmaster =~ s/\n$//;
    $newmaster =~ s/$/\n/;
    if ($opt_remove) {
      if ($found || $found_tl) {
        return if $dry_run;
        open(FOO, ">:encoding(UTF-8)", encode('locale_fs', $cidfmap_master)) ||
          die("Cannot clean up $cidfmap_master: $!");
        print FOO $newmaster;
        close FOO;
      }
    } else {
      if ($found && !$found_tl) {
        print_info("$add already loaded in $cidfmap_master, no changes\n");
      } else {
        return if $dry_run;
        open(FOO, ">:encoding(UTF-8)", encode('locale_fs', $cidfmap_master)) ||
          die("Cannot open $cidfmap_master for appending: $!");
        print FOO $newmaster;
        print FOO "($add) .runlibfile\n";
        close(FOO);
      }
    }
  } else {
    return if $dry_run;
    return if $opt_remove;
    open(FOO, ">:encoding(UTF-8)", encode('locale_fs', $cidfmap_master)) ||
      die("Cannot open $cidfmap_master for writing: $!");
    print FOO "($add) .runlibfile\n";
    close(FOO);
  }
}

sub generate_cidfmap_entry {
  my ($n, $c, $f, $sf) = @_;
  return "" if $opt_remove;
  # $f is already the link target name 'ttfname' (or 'ttcname' or 'otcname')
  # as determined by minimal priority number
  # extract subfont
  my $s = "/$n << /FileType /TrueType 
  /Path pssystemparams /GenericResourceDir get 
  (CIDFSubst/$f) concatstrings\n";
  if ($sf >= 0) { # in this script, $sf < 0 represents TTF
    $s .= "  /SubfontID $sf\n";
  }
  $s .= "  /CSI [($c";
  if ($c eq "Japan") {
    $s .= "1) 6]";
  } elsif ($c eq "GB") {
    $s .= "1) 5]";
  } elsif ($c eq "CNS") {
    $s .= "1) 5]";
  } elsif ($c eq "Korea") {
    $s .= "1) 2]";
  } elsif ($c eq "AI0") {
    print_warning("cannot use class AI0 for non-OTF $n, skipping.\n");
    return '';
  } else {
    print_warning("unknown class $c for $n, skipping.\n");
    return '';
  }
  $s .= " >> ;\n";
  return $s;
}

sub generate_font_snippet {
  my ($fd, $n, $c, $f) = @_;
  return if $dry_run;
  if ($opt_akotfps) {
    add_akotfps_data($n);
    return;
  }
  if ($c eq "AI0") {
    $c = "AI0-$n";
  }
  for my $enc (@{$encode_list{$c}}) {
    if ($opt_remove) {
      unlink encode('locale_fs', "$fd/$n-$enc")
          if (-f encode('locale_fs', "$fd/$n-$enc"));
      next;
    }
    open(FOO, ">:encoding(UTF-8)", encode('locale_fs', "$fd/$n-$enc")) ||
      die("cannot open $fd/$n-$enc for writing: $!");
    print FOO "%!PS-Adobe-3.0 Resource-Font
%%DocumentNeededResources: $enc (CMap)
%%IncludeResource: $enc (CMap)
%%BeginResource: Font ($n-$enc)
($n-$enc)
($enc) /CMap findresource
[($n) /CIDFont findresource]
composefont
pop
%%EndResource
%%EOF
";
    close(FOO);
  }
}

sub add_akotfps_data {
  my ($fn) = @_;
  return if $dry_run;
  if (!$opt_remove) {
    $akotfps_datacontent .= "$fn\n";
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
  if (-l encode('locale_fs', $target)) {
    if ($opt_cleanup) {
      $do_unlink = 1;
    } else {
      my $linkt = decode('locale_fs', readlink(encode('locale_fs', $target)));
      if ($linkt) {
        if ($linkt eq $f) {
          # case 1: exists, link, targets agree
          $do_unlink = 1;
        } elsif (-r encode('locale_fs', $linkt)) {
          # case 3: exists, link, targets different
          if ($opt_force) {
            print_info("Removing link $target due to --force!\n");
            $do_unlink = 1;
          } else {
            print_error("Link $target already existing, but target different from $f, exiting!\n");
            exit(1);
          }
        } else {
          # case 2: dangling symlink or link-to-link
          print_warning("Removing dangling symlink $target to $linkt\n");
          $do_unlink = 1;
        }
      } else {
        print_error("This should not happen, we have a link but cannot read the target?\n");
        exit(1);
      }
    }
  } elsif (-r encode('locale_fs', $target)) {
    # case 4: exists, but not link (NTFS hardlink on win32 falls into this)
    if (-s encode('locale_fs', $target)) {
      if ($opt_force) {
        print_info("Removing $target due to --force!\n");
        $do_unlink = 1;
      } elsif (win32()) {
        my $result = compare (encode ('locale_fs', $f),
                              encode ('locale_fs', $target));
        if ($result == -1) {
          print_error("file compare failed: ${f}, ${target}\n");
          exit(1);
        } elsif ($result) {
          print_error("$target already existing and different content, exiting!\n");
          exit(1);
        }
        print_debug("$target already existing but same content, skipping!\n");
        return;
      } else {
        print_error("$target already existing, exiting!\n");
        exit(1);
      }
    } else {
      # NTFS symlink on win32 has file size 0, we're safe to remove it
      $do_unlink = 1;
    }
  } # case 5: otherwise it is not existing!

  # if we are still here and $do_unlink is set, remove it
  maybe_unlink($target) if $do_unlink;
  # recreate link if we are not in the remove case
  if (!$opt_remove) {
    maybe_symlink($f, $target) || die("Cannot link font $f to $target: $!");
  }
}

sub make_dir {
  my ($d, $w) = @_;
  if (-r encode('locale_fs', $d)) {
    if (! -d encode('locale_fs', $d)) {
      print_error("$d is not a directory, $w\n");
      exit(1);
    }
  } else {
    $dry_run || make_path(encode('locale', $d));
  }
}

# perl symlink function does not work on windows, so we use Win32 API.
sub maybe_symlink {
  my ($realname, $targetname) = @_;
  if (win32()) {
    # Try symbolic link, hard link, copy by Win32 API

    # On Windows, the creation of a symbolic link requires a privilege.
    # Therefore, if the creation of the symbolic link fails,
    # create a hard link.
    # However, the creation of the hard link requires the write permission
    # to the original existing file.
    # Thus, if the hard link also fails, copy the file.

    if (win32_symlink ($realname, $targetname)) {
      print_debug ("Symbolic link succeeded: ${targetname}\n");
      return 1;
    }

    if (win32_hardlink ($realname, $targetname)) {
      print_debug ("Hard link succeeded: ${targetname}\n");
      return 1;
    }

    if (win32_copy ($realname, $targetname)) {
      print_debug ("Copy succeeded: ${targetname}\n");
      return 1;
    }

    print_warning ("Link/copy failed: ${targetname}\n");
  } else {
    symlink ($realname, $targetname);
  }
}

sub maybe_unlink {
  my ($targetname) = @_;
  unlink (encode('locale_fs', $targetname));
}

# For Windows: Create symbolic link
sub win32_symlink {
    my ($existingfilename, $newfilename) = @_;

    # We use CreateSymbolicLinkW API directly for creating symbolic link
    # because perl's symlink function may not work in Windows.
    state $createsymboliclinkw = load_createsymboliclinkw ();

    $existingfilename =~ s|/|\\|g;
    $newfilename =~ s|/|\\|g;

    my $r = $createsymboliclinkw->Call (
        encode ('UTF-16LE', $newfilename),
        encode ('UTF-16LE', $existingfilename),
        0
        );
    if (ord($r) != 0) {
        return 1;
    }
    my $msg = decode ('locale',
                      Win32::FormatMessage (Win32::GetLastError ()));
    $msg =~ s/[\r\n]+\z//;
    print_debug ("CreateSymbolicLinkW failed: ${msg}\n");

    return 0;
}

# For Windows: Create hard link
sub win32_hardlink {
    my ($existingfilename, $newfilename) = @_;

    # We use CreateHardLinkW API directly for creating hard link
    # because -W API is not affected by the perl's active code page.
    state $createhardlinkw = load_createhardlinkw ();

    $existingfilename =~ s|/|\\|g;
    $newfilename =~ s|/|\\|g;

    my $r = $createhardlinkw->Call (
        encode ('UTF-16LE', $newfilename),
        encode ('UTF-16LE', $existingfilename),
        0
        );
    if ($r) {
        return 1;
    }
    my $msg = decode ('locale',
                      Win32::FormatMessage (Win32::GetLastError ()));
    $msg =~ s/[\r\n]+\z//;
    print_debug ("CreateHardLinkW failed: ${msg}\n");

    return 0;
}

# For Windows: Copy file
sub win32_copy {
    my ($existingfilename, $newfilename) = @_;

    # We use CopyFileW API directly for copying file
    # because -W API is not affected by the perl's active code page.
    state $copyfilew = load_copyfilew ();

    $existingfilename =~ s|/|\\|g;
    $newfilename =~ s|/|\\|g;

    my $r = $copyfilew->Call (
        encode ('UTF-16LE', $existingfilename),
        encode ('UTF-16LE', $newfilename),
        1
        );
    if ($r) {
        return 1;
    }
    my $msg = decode ('locale',
                      Win32::FormatMessage (Win32::GetLastError ()));
    $msg =~ s/[\r\n]+\z//;
    print_debug ("CopyFileW failed: ${msg}\n");

    return 0;
}

# For Windows: Load CreateSymbolicLinkW API
sub load_createsymboliclinkw {
    my $createsymboliclinkw = Win32::API::More->new (
        'kernel32.dll',
        'BOOLEAN CreateSymbolicLinkW(
           LPCWSTR lpSymlinkFileName,
           LPCWSTR lpTargetFileName,
           DWORD   dwFlags
         )'
        ) or die ('Failed: Win32::API::More->new CreateSymbolicLinkW');

    return $createsymboliclinkw;
}
# For Windows: Load CreateHardLinkW API
sub load_createhardlinkw {
    my $createhardlinkw = Win32::API::More->new (
        'kernel32.dll',
        'BOOL CreateHardLinkW(
           LPCWSTR  lpFileName,
           LPCWSTR  lpExistingFileName,
           UINT_PTR lpSecurityAttributes
          )'
        ) or die ('Failed: Win32::API::More->new CreateHardLinkW');

    return $createhardlinkw;
}

# For Windows: Load CopyFileW API
sub load_copyfilew {
    my $copyfilew = Win32::API::More->new (
        'kernel32.dll',
        'BOOL CopyFileW(
           LPCWSTR lpExistingFileName,
           LPCWSTR lpNewFileName,
           BOOL    bFailIfExists
         )'
        ) or die ('Failed: Win32::API::More->new CopyFileW');

    return $copyfilew;
}

# write to psnames-for-otfps
sub write_akotfps_datafile {
  return if $dry_run;
  make_dir("$opt_akotfps/$akotfps_pathpart",
         "cannot create $akotfps_datafilename in it!");
  open(FOO, ">:encoding(UTF-8)",
       encode('locale_fs',
              "$opt_akotfps/$akotfps_pathpart/$akotfps_datafilename")) ||
    die("cannot open $opt_akotfps/$akotfps_pathpart/$akotfps_datafilename for writing: $!");
  print FOO "% psnames-for-otf
%
% PostSctipt names for OpenType fonts
%
% This file is used by a program ps2otfps
% in order to add needed information to a ps file
% created by the dvips
%
$akotfps_datacontent";
  close(FOO);
}

#
# dump found files
sub info_found_fonts {
  print "List of found fonts:\n\n";
  for my $k (sort keys %fontdb) {
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
# dump aliases
sub info_list_aliases {
  print "List of ", ($opt_listallaliases ? "all" : "available"), " aliases and their options (in decreasing priority):\n" unless $opt_machine;
  my (@jal, @kal, @tal, @sal, @ai0al);
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
    } elsif ($cl eq 'AI0') {
      push @ai0al, $foo;
    } else {
      print STDERR "unknown class $cl for $al\n";
    }
  }
  if ($opt_machine) {
    print @jal if @jal;
    print @kal if @kal;
    print @sal if @sal;
    print @tal if @tal;
    print @ai0al if @ai0al;
  } else {
    print "Aliases for Japanese fonts:\n", @jal, "\n" if @jal;
    print "Aliases for Korean fonts:\n", @kal, "\n" if @kal;
    print "Aliases for Simplified Chinese fonts:\n", @sal, "\n" if @sal;
    print "Aliases for Traditional Chinese fonts:\n", @tal, "\n" if @tal;
    print "Aliases for Adobe-Identity-0 fonts:\n", @ai0al, "\n" if @ai0al;
  }
}

#
# make all fonts available for listing all aliases
sub make_all_available {
  for my $k (keys %fontdb) {
    $fontdb{$k}{'available'} = 1;
    $fontdb{$k}{'type'} = 'OTF';
    delete $fontdb{$k}{'files'} if (!$opt_cleanup);
  }
}

#
# checks all file names listed in %fontdb
# and sets
sub check_for_files {
  my @foundfiles;
  if ($opt_filelist) {
    open(FOO, "<", encode('locale_fs', $opt_filelist)) ||
         die("Cannot open $opt_filelist: $!");
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
      for my $d (qw!/Library/Fonts /System/Library/Fonts
                    /System/Library/Assets /System/Library/AssetsV2
                    /Network/Library/Fonts /usr/share/fonts!) {
        push @extradirs, "$d//"
            if (-d encode('locale_fs', $d)); # recursive search
      }
      # the path contains white space, so hack required
      for my $d (qw!/Library/Application__Support/Apple/Fonts!) {
        my $sd = $d;
        $sd =~ s/__/ /;
        push @extradirs, "$sd//"
            if (-d encode('locale_fs', "$sd")); # recursive search
      }
      # office for mac 2016
      for my $d (qw!/Applications/Microsoft__Word.app
                    /Applications/Microsoft__Excel.app
                    /Applications/Microsoft__PowerPoint.app!) {
        my $sd = $d;
        $sd =~ s/__/ /;
        push @extradirs, "$sd/Contents/Resources/Fonts/"
            if (-d encode('locale_fs', "$sd/Contents/Resources/Fonts"));
        push @extradirs, "$sd/Contents/Resources/DFonts/"
            if (-d encode('locale_fs', "$sd/Contents/Resources/DFonts"));
      }
      my $home = $ENV{'HOME'};
      push @extradirs, "$home/Library/Fonts//"
          if (-d encode('locale_fs', "$home/Library/Fonts"));
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
        # $newotf .= $ENV{'OPENTYPEFONTS'} if $ENV{'OPENTYPEFONTS'};
        # $newttf .= $ENV{'TTFONTS'} if $ENV{'TTFONTS'};
        # $ENV{'OPENTYPEFONTS'} = $newotf;
        # $ENV{'TTFONTS'} = $newttf;
        # new code for uppercase/lowercase workaround -- HY (2016/09/27)
        my $extrafontdir = join($sep, @extradirs) . $sep;
        $ENV{'OSFONTDIR'} = $extrafontdir;
      }
    }
    # prepare for kpsewhich call, we need to do quoting
    # we need as much candidate files as possible, so -all is needed
    my $cmdl = 'kpsewhich -all ';
    for my $f (@fn) {
      $cmdl .= " \"$f\" ";
    }
    # shoot up kpsewhich
    print_ddebug("checking for $cmdl\n");
    $cmdl = encode('locale', $cmdl);
    @foundfiles = `$cmdl`;
    # We assume that the output of kpsewhich is
    # the same as perl's locale (or active code page).
    @foundfiles = map { decode('locale', $_) } @foundfiles;
  }
  chomp(@foundfiles);
  print_ddebug("Found files @foundfiles\n");
  # map basenames to filenames
  my %bntofn;
  for my $f (@foundfiles) {
    $f =~ s/[\r\n]+\z//; # perl's chomp() on git-bash cannot strip CR of CRLF ??
    my $realf = decode('locale_fs', abs_path(encode('locale_fs', $f)));
    if (!$realf) {
      print_warning("dead link or strange file found: $f - ignored!\n");
      next;
    }
    if (win32()) {
      # abs_path cannot read NTFS symlink/hardlink, and it serves as
      # identity transformation.
      # this might lead to dangling links for multiple runs of cjk-gs-integrate,
      # when $opt_texmflink (in the previous run) is contained in the (current)
      # kpsewhich search path.
      # to avoid this, ignore targets which contain $otf_pathpart or $ttf_pathpart
      my $temp_realfdir = "$realf";
      $temp_realfdir =~ s!^(.*)/(.*)$!$1!;
      next if ($temp_realfdir =~ $otf_pathpart || $temp_realfdir =~ $ttf_pathpart);
    }
    my $bn = basename($f);
    # kpsewhich -all might return multiple files with the same basename;
    # collect all of them
    $bntofn{$bn}{$realf} = 1;
  }

  # show the %fontdb before file check
  if ($opt_debug >= 2) {
    print_ddebug("dumping font database before file check:\n");
    print_ddebug(Data::Dumper::Dumper(\%fontdb));
  }
  if ($opt_debug >= 3) {
    print_dddebug("dumping basename to filename list:\n");
    print_dddebug(Data::Dumper::Dumper(\%bntofn));
  }

  # update the %fontdb with the found files
  for my $k (keys %fontdb) {
    $fontdb{$k}{'available'} = 0;
    for my $f (keys %{$fontdb{$k}{'files'}}) {
      # check for subfont extension
      my $realfile = $f;
      $realfile =~ s/^(.*)\(\d*\)$/$1/;
      my $index = 0;
      if ($fontdb{$k}{'files'}{$f}{'type'} eq 'TTC' || $fontdb{$k}{'files'}{$f}{'type'} eq 'OTC') {
        if ($f =~ m/^(.*)\((\d*)\)$/) {
          $index = $2;
        }
      }
      # double check for casefolding or incompatible OTC/TTC index
      #   [1] casefolding issue
      #     we might catch different names (batang/Batang) and identify them wrongly on
      #        * case-insensitive file systems (like HFS on MacOS)
      #        * kpathsea 6.3.0 or later, with casefolding fallback search (TL2018)
      #     check the actual psname using zrlistttc.lua, only when we "know"
      #     both uppercase/lowercase font files are possible and they are different
      #   [2] incompatible index
      #     the index in msgothic.ttc changed at some time between Win7 and Win10.
      my $actualpsname;
      my $bname;
      for my $b (sort keys %{$bntofn{$realfile}}) {
        if ($opt_strictpsname && !$fontdb{$k}{'doublecheck'}) {
          $fontdb{$k}{'doublecheck'} = "debug"; # stub
        }
        if ($fontdb{$k}{'doublecheck'} && $fontdb{$k}{'doublecheck'} ne "false" && $zrlistttc_available) {
          print_debug("We need to test whether\n");
          print_debug("  $b:$index\n");
          print_debug("is the correct one ($k). Invoking zrlistttc ...\n");
          my $cmdl = "texlua $zrlistttc -i $index \"$b\"";
          $cmdl = encode('locale', $cmdl);
          chomp($actualpsname = `$cmdl`);
          if ($?) {
            # something is wrong with the font file, or zrlistttc does not support it;
            # still there is a chance that Ghostscript supports, so don't discard it
            print_debug("... command exited with $?!\n");
            print_debug("OK, I'll take this, but it may not work properly.\n");
            print_warning("zrlistttc check failed for $b\n") if $opt_strictpsname;
            $bname = $b;
            last;
          }
          # We assume that the output of texlua is
          # the same as perl's locale (or active code page).
          $actualpsname = decode('locale', $actualpsname);
          $actualpsname =~ s/[\r\n]+\z//; # perl's chomp() on git-bash cannot strip CR of CRLF ??
          if ($actualpsname ne $k) {
            print_debug("... PSName returned by zrlistttc ($actualpsname) is\n");
            print_debug("different from our database ($k), discarding!\n");
            if ($opt_strictpsname && $fontdb{$k}{'doublecheck'} eq "debug") {
              # in our database, we've set $fontdb{$k}{'doublecheck'} to "true" intentionally
              # when we *know* doublecheck is actually required;
              # if the stub "debug" detects a difference, our database should contain a bug!
              print_warning("zrlistttc check failed for $b: please report to the author!\n");
            }
          } else {
            print_debug("... test passed.\n");
            $bname = $b;
            last;
          }
        } else {
          $bname = $b;
          last;
        }
      }
      if ($bname) {
        # we found a representative, make it available
        $fontdb{$k}{'files'}{$f}{'target'} = $bname;
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
      my $mp = 1000000; my $mf;
      for my $f (keys %{$fontdb{$k}{'files'}}) {
        if ($fontdb{$k}{'files'}{$f}{'priority'} < $mp) {
          $mp = $fontdb{$k}{'files'}{$f}{'priority'};
          $mf = $f;
        }
      }
      # extract subfont if necessary
      my $sf = 0;
      if ($mf =~ m/^(.*)\((\d*)\)$/) { $sf = $2; }
      $fontdb{$k}{'target'} = $fontdb{$k}{'files'}{$mf}{'target'};
      $fontdb{$k}{'type'} = $fontdb{$k}{'files'}{$mf}{'type'};
      $fontdb{$k}{'subfont'} = $sf if ($fontdb{$k}{'type'} eq 'TTC' || $fontdb{$k}{'type'} eq 'OTC');
    }
    # not needed anymore
    # delete $fontdb{$k}{'files'};
  }
  if ($opt_debug >= 2) {
    print_ddebug("dumping font database:\n");
    print_ddebug(Data::Dumper::Dumper(\%fontdb));
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
            print_debug("Currently Ghostscript does not support OTC font,\n");
            print_debug("not adding $fontdb{$k}{'otcname'} to alias candidates\n");
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
  if ($opt_debug >= 2) {
    print_ddebug("dumping aliases:\n");
    print_ddebug(Data::Dumper::Dumper(\%aliases));
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
  # if --fontdef=foo is given, disregard built-in database and
  # use "foo" as a substitute; otherwise, use built-in database
  if ($opt_fontdef) {
    my $foo = kpse_miscfont($opt_fontdef);
    open(FDB, "<:encoding(UTF-8)", encode('locale_fs', $foo)) ||
      die("Cannot find $opt_fontdef: $!");
    @dbl = <FDB>;
    close(FDB);
    print_debug("New database file: $opt_fontdef...\n");
  } else {
    @dbl = <DATA>;
  }
  read_each_font_database(@dbl);
  # if --fontdef-add=bar is given, use "bar" as an addition
  # to the current database; if the same Name entry appears,
  # overwrite existing one (that is, the addition wins)
  for (@opt_fontdef_add) {
    my $foo = kpse_miscfont($_);
    open(FDB, "<:encoding(UTF-8)", encode('locale_fs', $foo)) ||
      die("Cannot find $_: $!");
    @dbl = <FDB>;
    close(FDB);
    print_debug("Additional database file: $_...\n");
    read_each_font_database(@dbl);
  }
}

sub read_each_font_database {
  my (@curdbl) = @_;
  my $fontname = "";
  my $fontclass = "";
  my @fontcmaps = ();
  my %fontprovides = ();
  my $fontdoublecheck = "";
  my %fontfiles;
  my $psname = "";
  my $lineno = 0;
  chomp(@curdbl);
  push @curdbl, ""; # add a "final empty line" to easy parsing
  for my $l (@curdbl) {
    $lineno++;
    next if ($l =~ m/^\s*#/); # skip comment line
    $l =~ s/\s*#.*$//; # skip comment after '#'
    if ($l =~ m/^\s*$/) { # empty line is a separator between entries
      if ($fontname || $fontclass || keys(%fontfiles)) {
        if ($fontname && $fontclass && keys(%fontfiles)) {
          my $realfontname = ($psname ? $psname : $fontname);
          if ($fontdb{$realfontname}{'origname'}) {
            # needed for --fontdef-add, which allows overwriting with external database given by user
            print_debug("$fontdb{$realfontname}{'origname'} is already registered in database,\n");
            print_debug("overwriting with the new one ...\n");
          }
          $fontdb{$realfontname}{'origname'} = $fontname;
          $fontdb{$realfontname}{'class'} = $fontclass;
          $fontdb{$realfontname}{'doublecheck'} = $fontdoublecheck;
          $fontdb{$realfontname}{'files'} = { %fontfiles };
          $fontdb{$realfontname}{'provides'} = { %fontprovides };
          if ($fontclass eq "AI0") {
            $encode_list{"AI0-$realfontname"} = [ @fontcmaps ];
          } elsif (@fontcmaps) {
            print_warning("CMap entry for $realfontname (Class: $fontclass) ignored!\n");
          }
          if ($opt_debug >= 3) {
            print_dddebug("Dumping fontfiles for $realfontname: " . Data::Dumper::Dumper(\%fontfiles));
          }
          # reset to start
          $fontname = $fontclass = $psname = "";
          $fontdoublecheck = "";
          @fontcmaps = ();
          %fontfiles = ();
          %fontprovides = ();
        } else {
          print_warning("incomplete entry above line $lineno for $fontname/$fontclass, skipping!\n");
          # reset to start
          $fontname = $fontclass = $psname = "";
          $fontdoublecheck = "";
          @fontcmaps = ();
          %fontfiles = ();
          %fontprovides = ();
        }
      } else {
        # no term is set, so nothing to warn about
      }
      next;
    }
    if ($l =~ m/^!INCLUDE\s*(.*)$/) { # for remove-only database
      next if (!$opt_cleanup);
      my @dbl;
      my $foo = kpse_miscfont($1);
      if (!open(FDB, "<:encoding(UTF-8)", encode('locale_fs', $foo))) {
        print_warning("Cannot find $1, skipping!\n");
        next;
      }
      @dbl = <FDB>;
      close(FDB);
      print_debug("Reading database file $1...\n");
      read_each_font_database(@dbl);
      next;
    }
    if ($l =~ m/^INCLUDE\s*(.*)$/) {
      my @dbl;
      my $foo = kpse_miscfont($1);
      if (!open(FDB, "<:encoding(UTF-8)", encode('locale_fs', $foo))) {
        print_warning("Cannot find $1, skipping!\n");
        next;
      }
      @dbl = <FDB>;
      close(FDB);
      print_debug("Reading database file $1...\n");
      read_each_font_database(@dbl);
      next;
    }
    if ($l =~ m/^Name:\s*(.*)$/) { $fontname = $1; next; }
    if ($l =~ m/^PSName:\s*(.*)$/) { $psname = $1; next; }
    if ($l =~ m/^Class:\s*(.*)$/) { $fontclass = $1 ; next ; }
    if ($l =~ m/^CMap:\s*(.*)$/) { push(@fontcmaps, $1); next ; }
    if ($l =~ m/^Provides\((\d+)\):\s*(.*)$/) { $fontprovides{$2} = $1; next; }
    if ($l =~ m/^Doublecheck:\s*(.*)$/) { $fontdoublecheck = $1 ; next ; }
    if ($l =~ m/^Casefold:\s*(.*)$/) { $fontdoublecheck = $1 ; next ; } # no longer used
    # new code: distinguish 4 types (otf, otc, ttf, ttc)
    if ($l =~ m/^OTFname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_dddebug("filename: ${fn}\n");
      print_dddebug("type: otf\n");
      $fontfiles{$fn}{'type'} = 'OTF';
      next;
    }
    if ($l =~ m/^OTCname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_dddebug("filename: ${fn}\n");
      print_dddebug("type: otc\n");
      $fontfiles{$fn}{'type'} = 'OTC';
      next;
    }
    if ($l =~ m/^TTFname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_dddebug("filename: ${fn}\n");
      print_dddebug("type: ttf\n");
      $fontfiles{$fn}{'type'} = 'TTF';
      next;
    }
    if ($l =~ m/^TTCname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_dddebug("filename: ${fn}\n");
      print_dddebug("type: ttc\n");
      $fontfiles{$fn}{'type'} = 'TTC';
      next;
    }
    # only for backward compatibility; guess type from the file extension
    if ($l =~ m/^Filename(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_dddebug("filename: ${fn}\n");
      if ($fn =~ m/\.otf$/i) {
        print_dddebug("type: otf\n");
        $fontfiles{$fn}{'type'} = 'OTF';
      } elsif ($fn =~ m/\.otc(\(\d+\))?$/i) {
        print_dddebug("type: otc\n");
        $fontfiles{$fn}{'type'} = 'OTC';
      } elsif ($fn =~ m/\.ttf$/i) {
        print_dddebug("type: ttf\n");
        $fontfiles{$fn}{'type'} = 'TTF';
      } elsif ($fn =~ m/\.ttc(\(\d+\))?$/i) {
        print_dddebug("type: ttc\n");
        $fontfiles{$fn}{'type'} = 'TTC';
      } else {
        print_warning("cannot determine font type of $fn at line $lineno, skipping!\n");
        delete $fontfiles{$fn};
      }
      next;
    }
    # only for removing
    if ($l =~ m/^RMVname(\((\d+)\))?:\s*(.*)$/) {
      my $fn = $3;
      $fontfiles{$fn}{'priority'} = ($2 ? $2 : 10);
      print_dddebug("filename: ${fn}\n");
      print_dddebug("type: remove\n");
      $fontfiles{$fn}{'type'} = 'RMV';
      next;
    }
    # we are still here??
    print_error("Cannot parse this file at line $lineno, exiting.
                 Strange line: >>>$l<<<\n");
    exit(1);
  }
}

sub dump_font_database {
  open(FOO, ">:encoding(UTF-8)", encode('locale_fs', $dump_datafile)) ||
    die("cannot open $dump_datafile for writing: $!");
  for my $k (sort keys %fontdb) {
    print FOO "Name: $fontdb{$k}{'origname'}\n";
    print FOO "PSName: $k\n" if ($fontdb{$k}{'origname'} ne $k);
    my $class = $fontdb{$k}{'class'};
    print FOO "Class: $class\n";
    if ($class eq "AI0") {
      for my $cmap (@{$encode_list{"AI0-$k"}}) {
        print FOO "CMap: $cmap\n";
      }
    }
    for my $p (sort keys %{$fontdb{$k}{'provides'}}) {
      print FOO "Provides($fontdb{$k}{'provides'}{$p}): $p\n";
    }
    print FOO "Doublecheck: $fontdb{$k}{'doublecheck'}\n" if ($fontdb{$k}{'doublecheck'});
    for my $f (sort { $fontdb{$k}{'files'}{$a}{'priority'}
                      <=>
                      $fontdb{$k}{'files'}{$b}{'priority'} }
                    keys %{$fontdb{$k}{'files'}}) {
      print FOO "$fontdb{$k}{'files'}{$f}{'type'}name($fontdb{$k}{'files'}{$f}{'priority'}): $f\n";
    }
    print FOO "\n"; # empty line is a separator between entries
  }
  close(FOO);
}

sub find_gs_resource {
  my $foundres = '';
  if (win32()) {
    # determine tlgs or native gs
    my $foo = `kpsewhich -var-value=SELFAUTOPARENT`;
    # We assume that the output of kpsewhich is
    # the same as perl's locale (or active code page).
    decode('locale', $foo);
    chomp($foo);
    if ( -d encode('locale_fs', "$foo/tlpkg/tlgs") ) {
      # should be texlive with tlgs
      print_debug("Assuming tlgs win32 ...\n");
      $foundres = "$foo/tlpkg/tlgs/Resource";
      # for TL2016, tlgs binary has built-in Resource,
      # so we cannot set up CJK fonts correctly.
      # the following test forces to exit in such case
      if ( ! -d encode('locale_fs', $foundres) ) {
        print_error("No Resource directory available for tlgs,\n");
        print_error("we cannot support such gs, sorry.\n");
        $foundres = '';
      }
      # change output location
      $cidfmap_pathpart = "../lib/cidfmap";
      $cidfmap_local_pathpart = "../lib/cidfmap.local";
      $cidfmap_aliases_pathpart = "../lib/cidfmap.aliases";
    } else {
      # we assume gswin32c is in the path
      # TODO: what should we do for gswin64c?
      $foundres = `where gswin32c 2>$nul`; # assume 'where' is available
      if ($?) {
        print_error("Cannot run where gswin32c ...\n");
      } else {
        # We assume that the output of 'where' is
        # the same as perl's locale (or active code page).
        chomp($foundres = decode('locale', $foundres));
        # trial 1: assume the relative path
        # when C:\path\to\bin\gswin32c.exe is found, then there should be
        # C:\path\to\Resource (note that 'where' returns backslash-ed path)
        print_debug("Finding gs resource by assuming relative path ...\n");
        $foundres =~ s!\\!/!g;
        $foundres =~ s!/bin/gswin32c\.exe$!/Resource!;
        if ( ! -d encode('locale_fs', $foundres) ) {
          $foundres = '';
        }
        if (!$foundres) {
          print_debug("Found gs but no resource, try another routine ...\n");
        }
      }
      if (!$foundres) {
        chomp(my $gsver = `gswin32c --version 2>$nul`);
        if ($?) {
          print_error("Cannot run gswin32c --version ...\n");
        } else {
          # trial 2: assume the fixed path, c:/gs/gs$gsver/Resource
          print_debug("Finding gs resource by assuming fixed path ...\n");
          $foundres = "c:/gs/gs$gsver/Resource";
          if ( ! -d encode('locale_fs', $foundres) ) {
            $foundres = '';
          }
          if (!$foundres) {
            print_error("Found gs but no resource???\n");
          }
        }
      }
    }
  } else {
    # we assume that gs is in the path
    chomp(my $gsver = `gs --version 2>$nul`);
    if ($?) {
      print_error("Cannot run gs --version ...\n");
    } else {
      # trial 1: assume the relative path
      # when /path/to/bin/gs is found, then there should be
      # /path/to/share/ghostscript/$(gs --version)/Resource
      print_debug("Finding gs resource by assuming relative path ...\n");
      $foundres = `which gs`;
      # We assume that the output of 'which' is
      # the same as perl's locale (or active code page).
      $foundres = decode('locale', $foundres);
      chomp($foundres);
      $foundres =~ s!/bin/gs$!/share/ghostscript/$gsver/Resource!;
      if ( ! -d encode('locale_fs', $foundres) ) {
        $foundres = '';
      }
      if (!$foundres) {
        print_debug("Found gs but no resource, try another routine ...\n");
      }
    }
    if (!$foundres) {
      my @ret = `gs --help 2>$nul`;
      if ($?) {
        print_error("Cannot run gs --help ...\n");
      } else {
        # We assume that the output of gs is
        # the same as perl's locale (or active code page).
        @ret = map { decode('locale', $_) } @ret;
        chomp(@ret);
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

sub kpse_miscfont {
  my ($file) = @_;
  my $foo = '';
  # first, prioritize GitHub repository diretory structure
  $foo = "database/$file" if (-f encode('locale_fs', "database/$file"));
  if ($foo eq "") {
    my $cmdl = "kpsewhich -format=miscfont $file";
    $cmdl = encode('locale', $cmdl);
    $foo = `$cmdl`;
    # We assume that the output of kpsewhich is
    # the same as perl's locale (or active code page).
    $foo = decode('locale', $foo);
    chomp($foo);
  }
  return $foo;
}

sub init_encode_locale {
  eval {
    require Encode::Locale;
    Encode::Locale->import();

    $debug_msg_before_init .= "Encode::Locale is loaded.\n";
  };
  if ($@) {
    if (win32) {
      die ("For Windows, Encode::Locale is required.\n");
    }

    $debug_msg_before_init .=
        "Encode::Locale is not found. Assuming all encodings are UTF-8.\n";
    Encode::Alias::define_alias('locale' => 'UTF-8');
    Encode::Alias::define_alias('locale_fs' => 'UTF-8');
    Encode::Alias::define_alias('console_in' => 'UTF-8');
    Encode::Alias::define_alias('console_out' => 'UTF-8');
  }
}

sub version {
  my $ret = sprintf "%s version %s\n", $prg, $version;
  return $ret;
}

sub Usage {
  my $headline = "Configuring Ghostscript for CJK CID/TTF fonts";
  my $usage = "[perl] $prg\[.pl\] [OPTIONS]";
  my $options = "
-o, --output DIR      specifies the base output dir, if not provided,
                      the Resource directory of an installed Ghostscript
                      is searched and used.
-f, --fontdef FILE    specify alternate set of font definitions, if not
                      given, the built-in set is used
--fontdef-add FILE    specify additional set of font definitions, to
                      overwrite subset of built-in definitions;
                      can be given multiple times
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
--otfps [DIR]         generate configuration file (psnames-for-otf) into
                         DIR/$akotfps_pathpart
                      which is used by ps2otfps (developed by Akira Kakuto),
                      instead of generating snippets
--force               do not bail out if linked fonts already exist
--remove              try to remove instead of create
--cleanup             try to clean up all possible links/snippets and
                      cidfmap.local/cidfmap.aliases, which could have been
                      generated in the previous runs
-n, --dry-run         do not actually output anything
-q, --quiet           be less verbose
-d, --debug           output debug information, can be given multiple times
-v, --version         outputs only the version information
-h, --help            this help
";

  my $commandoptions = "
--dump-data [FILE]    dump the set of font definitions which is currently
                      effective, where FILE (the dump output) defaults to
                      $dump_datafile; you can easily modify it,
                      and tell me with -f (or --fontdef) option
--only-aliases        regenerate only cidfmap.aliases file, instead of all
--list-aliases        lists the available aliases and their options, with the
                      selected option on top
--list-all-aliases    list all possible aliases without searching for
                      actually present files
--list-fonts          lists the fonts found on the system
--info                combines the information of --list-aliases and
                      --list-fonts
--machine-readable    output of --list-aliases is machine readable
";

  my $shortdesc = "
This script searches a list of directories for CJK fonts, and makes
them available to an installed Ghostscript. In the simplest case with
sufficient privileges, a run without arguments should effect in a
complete setup of Ghostscript.
Search is done using the kpathsea library, in particular `kpsewhich`
program. To run this script, you will need some TeX distribution in
your system.
";

my $operation = "
For each found TrueType (TTF) font it creates a cidfmap entry in

    <Resource>/Init/cidfmap.local
      -- if you are using tlgs win32, tlpkg/tlgs/lib/cidfmap.local instead

and links the font to

    <Resource>/CIDFSubst/

For each CID font it creates a snippet in

    <Resource>/Font/

and links the font to

    <Resource>/CIDFont/

The `<Resource>` dir is either given by `-o`/`--output`, or otherwise searched
from an installed Ghostscript (binary name is assumed to be 'gs' on unix,
'gswin32c' on win32).

Aliases are added to

    <Resource>/Init/cidfmap.aliases
      -- if you are using tlgs win32, tlpkg/tlgs/lib/cidfmap.aliases instead

Finally, it tries to add runlib calls to

    <Resource>/Init/cidfmap
      -- if you are using tlgs win32, tlpkg/tlgs/lib/cidfmap

to load the cidfmap.local and cidfmap.aliases.
";

  my $dirsearch = '
Search is done using the kpathsea library, in particular using kpsewhich
program. By default the following directories are searched:
  - all TEXMF trees
  - `/Library/Fonts`, `/Library/Fonts/Microsoft`, `/System/Library/Fonts`,
    `/System/Library/Assets`, `/Network/Library/Fonts`,
    `~/Library/Fonts` and `/usr/share/fonts` (all if available)
  - `/Applications/Microsoft Word.app/Contents/Resources/{Fonts,DFonts}`,
    `/Applications/Microsoft Excel.app/Contents/Resources/{Fonts,DFonts}`,
    `/Applications/Microsoft PowerPoint.app/Contents/Resources/{Fonts,DFonts}`
     (all if available, meant for Office for Mac 2016)
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
If no output option is given, the program searches for a Ghostscript
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

    Ryumin-Light GothicBBB-Medium FutoMinA101-Bold FutoGoB101-Bold
    MidashiMin-MA31 MidashiGo-MB31 Jun101-Light

Korean:

    HYSMyeongJo-Medium HYGoThic-Medium HYRGoThic-Medium

Simplified Chinese:

    STSong-Light STSong-Regular STHeiti-Regular STHeiti-Light
    STKaiti-Regular STFangsong-Light STFangsong-Regular

Traditional Chinese:

    MSung-Light MSung-Medium MHei-Medium MKai-Medium

In addition, we also include provide entries for the OTF Morisawa names:

    RyuminPro-Light GothicBBBPro-Medium
    FutoMinA101Pro-Bold FutoGoB101Pro-Bold
    MidashiMinPro-MA31 MidashiGoPro-MB31 Jun101Pro-Light

The order is determined by the `Provides` setting in the font database.
That is, the first font found in this order will be used to provide the
alias if necessary.

For the Japanese fonts:
    Morisawa Pr6N, Morisawa, Hiragino ProN, Hiragino,
    Kozuka Pr6N, Kozuka ProVI, Kozuka Pro, Kozuka Std,
    HaranoAji, Yu OS X, Yu Win, MS,
    Moga-Mobo-ex, Moga-Mobo, IPAex, IPA, Ume

For the Korean fonts:
    (Hanyang,) Adobe, Solaris, MS, Unfonts, Baekmuk

For the Simplified Chinese:
    Adobe, Fandol, HaranoAji, Hiragino, Founder, MS,
    CJKUnifonts, Arphic, CJKUnifonts-ttf

For the Traditional Chinese:
    Adobe, HaranoAji, MS,
    CJKUnifonts, Arphic, CJKUnifonts-ttf

#### Overriding aliases ####

Using the command line option `--alias LL=RR` one can add arbitrary aliases,
or override ones selected by the program. For this to work the following
requirements of `LL` and `RR` must be fulfilled:
  * `LL` is not provided by a real font
  * `RR` is available either as real font, or as alias (indirect alias)
";

  my $authors = "
The script and its documentation was written by Norbert Preining, based
on research and work by Masamichi Hosoda, Yusuke Kuroki, Yusuke Terada,
Bruno Voisin, Hironobu Yamashita, Munehiro Yamamoto and the TeX Q&A wiki
page.

Maintained by Japanese TeX Development Community. For development, see
  https://github.com/texjporg/cjk-gs-support

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
  exit(0);
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
sub print_dddebug {
  print STDERR "$prg [DEBUG]: ", @_ if ($opt_debug >= 3);
}


__DATA__
#
# CJK FONT DEFINITIONS
#

# Noto
INCLUDE cjkgs-notoserif.dat
INCLUDE cjkgs-notosans.dat

# SourceHan
INCLUDE cjkgs-sourcehanserif.dat
INCLUDE cjkgs-sourcehansans.dat

#
# JAPANESE FONTS
#

# Morisawa -- Provides J10(Pr6N), J15(Pr6), J18(Pr5), J20(Pro)
INCLUDE cjkgs-morisawa.dat
#INCLUDE cjkgs-morisawa-extra.dat

# Hiragino -- Provides J30(ProN), J40(Pro)
INCLUDE cjkgs-hiragino.dat

# Kozuka -- Provides J50(Pr6N), J55(ProVI), J60(Pro), J65(Std)
INCLUDE cjkgs-kozuka.dat
INCLUDE cjkgs-ryokana.dat

# Yu-fonts MacOS version -- Provides J80
INCLUDE cjkgs-yu-osx.dat

# Yu-fonts Windows/MSOffice version -- Provides J90
INCLUDE cjkgs-yu-win.dat

# MS -- Provides J95
INCLUDE cjkgs-microsoft.dat

# BIZ UD
INCLUDE cjkgs-bizud.dat

# TypeBank
INCLUDE cjkgs-typebank.dat

# Fontworks
INCLUDE cjkgs-fontworks.dat

# Toppan
INCLUDE cjkgs-toppan.dat

# Heisei
INCLUDE cjkgs-heisei.dat

# Moga-Mobo from Y.Oz Vox (free) -- Provides J100(Ex), J110(none)
INCLUDE cjkgs-mogamobo.dat

# IPA (free) -- Provides J120(Ex), J130(none)
INCLUDE cjkgs-ipa.dat

# Ume-font (free) -- Provides J140
INCLUDE cjkgs-ume.dat

# Sazanami (free)
INCLUDE cjkgs-sazanami.dat

# Harano Aji Fonts (free) -- Provides J70, J71
INCLUDE cjkgs-haranoaji.dat

# Osaka (Apple)

Name: Osaka
Class: Japan
TTFname: Osaka.ttf

Name: Osaka-Mono
Class: Japan
TTFname: OsakaMono.ttf

#
# CHINESE FONTS
#

# Adobe -- Provides S30, T30
INCLUDE cjkgs-adobe.dat

# HaranoAji -- Provides S45, T45
# (already included in JAPANESE section)

# Hiragino -- Provides S50
# (already included in JAPANESE section)

# Beijing Founder Electronics -- Provides S55
INCLUDE cjkgs-founder.dat

# Changzhou SinoType -- Provides S??
INCLUDE cjkgs-sinotype.dat

# DynaComware -- Provides T??
INCLUDE cjkgs-dynacomware.dat

# Monotype
INCLUDE cjkgs-monotype.dat

# Shanghai Ikarus Ltd./URW Software & Type GmbH

Name: SIL-Hei-Med-Jian
Class: GB
TTFname: Hei.ttf

Name: SIL-Kai-Reg-Jian
Class: GB
TTFname: Kai.ttf

# Fandol (free) -- Provides S40
INCLUDE cjkgs-fandol.dat

# Arphic (free) -- Provides S80, T80
INCLUDE cjkgs-arphic.dat

# CJK-Unifonts new ttc edition (free) -- Provides T70, S70
# CJK-Unifonts old ttf edition (free) -- Provides T90, S90
INCLUDE cjkgs-cjkuni.dat

# WenQuanYi (free)
INCLUDE cjkgs-wenquanyi.dat

# cwTeX (free)

Name: cwTeXMing
Class: CNS
TTFname: cwming.ttf

Name: cwTeXHeiBold
Class: CNS
TTFname: cwheib.ttf

Name: cwTeXKai
Class: CNS
TTFname: cwkai.ttf

Name: cwTeXYen
Class: CNS
TTFname: cwyen.ttf

Name: cwTeXFangSong
Class: CNS
TTFname: cwfs.ttf

#
# KOREAN FONTS
#

# Adobe -- Provides K30/80
# (already included in CHINESE section)

# Solaris -- Provides K40
INCLUDE cjkgs-solaris.dat

# HaranoAji -- Provides K45
# (already included in JAPANESE section)

# Baekmuk (free)
# This is a special case, because "batang.ttf" in baekmuk and
# "Batang.ttf" in Microsoft Mac Office font share the same filename;
# symlink name should be "Baekmuk-Batang.ttf"
# similar for "Gulim.ttf" -- HY (2016/09/29)

Name: Baekmuk-Batang
Class: Korea
Provides(70): HYSMyeongJo-Medium
Doublecheck: true
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
Doublecheck: true
TTFname(20): gulim.ttf
TTFname(10): Baekmuk-Gulim.ttf

Name: Baekmuk-Headline
Class: Korea
TTFname(20): hline.ttf
TTFname(10): Baekmuk-Headline.ttf

# Unfonts (free) -- Provides K60
INCLUDE cjkgs-unfonts.dat

# Nanum (free)
INCLUDE cjkgs-nanum.dat

# Apple
INCLUDE cjkgs-apple.dat

# Design font by Ho-Seok Ee, aka. "ALee's font" (free)

Name: Bandal
Class: Korea
TTFname: Bandal.ttf

Name: Bangwool
Class: Korea
TTFname: Bangwool.ttf

Name: Eunjin
Class: Korea
TTFname: Eunjin.ttf

Name: EunjinNakseo
Class: Korea
TTFname: EunjinNakseo.ttf

Name: Guseul
Class: Korea
TTFname: Guseul.ttf

# Woowa Brothers (free)

Name: BMHANNA
Class: Korea
TTFname: BM-HANNA.ttf

# Hancom HCR (free)
INCLUDE cjkgs-hancom.dat


#
# Microsoft Windows, Windows/Mac Office fonts
#

# korea

Name: Batang
Class: Korea
Doublecheck: true
TTFname(50): Batang.ttf
TTCname(20): batang.ttc(0)

Name: BatangChe
Class: Korea
Provides(50): HYSMyeongJo-Medium
TTCname(20): batang.ttc(1)

Name: Dotum
Class: Korea
TTCname(20): gulim.ttc(2)

Name: DotumChe
Class: Korea
Provides(50): HYGoThic-Medium
TTCname(20): gulim.ttc(3)

Name: Gulim
Class: Korea
Doublecheck: true
TTFname(50): Gulim.ttf
TTCname(20): gulim.ttc(0)

Name: GulimChe
Class: Korea
Provides(50): HYRGoThic-Medium
Provides(90): HYGoThic-Medium
TTCname(20): gulim.ttc(1)

Name: Gungsuh
Class: Korea
TTCname(20): batang.ttc(2)

Name: GungsuhChe
Class: Korea
TTCname(20): batang.ttc(3)

# for Windows 10
# and Office for Mac 2016 (at least Ver.16.11.0, 2018-03)
Name: MalgunGothic
Class: Korea
Doublecheck: true
TTFname: malgun.ttf

# for Windows 7
# and Office for Mac 2016 (Ver.15.32.0, 2017-03)
Name: MalgunGothicRegular
Class: Korea
Doublecheck: true
TTFname: malgun.ttf

Name: MalgunGothicBold
Class: Korea
TTFname: malgunbd.ttf

Name: MalgunGothic-Semilight
Class: Korea
TTFname: malgunsl.ttf

# simplified chinese

Name: SimHei
Class: GB
Provides(60): STHeiti-Regular
Provides(60): STHeiti-Light
TTFname(50): SimHei.ttf
TTFname(20): simhei.ttf

Name: SimSun
Class: GB
Provides(60): STSong-Light
Provides(60): STSong-Regular
TTFname(50): SimSun.ttf
TTFname(21): simsun.ttf
TTCname(20): simsun.ttc(0)

Name: NSimSun
Class: GB
TTCname(20): simsun.ttc(1)

Name: KaiTi
Class: GB
Provides(60): STKaiti-Regular
TTFname(40): Kaiti.ttf
TTFname(20): simkai.ttf

Name: FangSong
Class: GB
Provides(60): STFangsong-Light
Provides(60): STFangsong-Regular
TTFname(40): Fangsong.ttf
TTFname(20): simfang.ttf

Name: LiSu
Class: GB
TTCname(20): SIMLI.TTF
TTCname(19): simli.ttf

Name: YouYuan
Class: GB
TTCname(20): SIMYOU.TTF
TTCname(19): simyou.ttf

Name: MicrosoftYaHei
Class: GB
TTFname(20): msyh.ttf
TTCname(30): msyh.ttc(0)

Name: MicrosoftYaHei-Bold
Class: GB
TTFname(20): msyhbd.ttf
TTCname(30): msyhbd.ttc(0)

Name: MicrosoftYaHeiLight
Class: GB
TTFname(20): msyhl.ttf
TTCname(30): msyhl.ttc(0)

Name: DengXian-Regular
Class: GB
TTFname: Deng.ttf

Name: DengXian-Bold
Class: GB
TTFname: Dengb.ttf

Name: DengXian-Light
Class: GB
TTFname: Dengl.ttf

# traditional chinese

Name: MingLiU
Class: CNS
Provides(60): MSung-Medium
Provides(60): MSung-Light
TTFname(50): MingLiU.ttf
TTCname(20): mingliu.ttc(0)

Name: PMingLiU
Class: CNS
TTFname(50): PMingLiU.ttf
TTCname(20): mingliu.ttc(1)

Name: DFKaiShu-SB-Estd-BF
Class: CNS
Provides(60): MKai-Medium
TTFname(50): BiauKai.ttf
TTFname(20): kaiu.ttf

Name: MicrosoftJhengHeiRegular
Class: CNS
Provides(60): MHei-Medium
TTFname(40): MSJH.ttf
TTFname(20): msjh.ttf
TTCname(30): msjh.ttc(0)

Name: MicrosoftJhengHeiBold
Class: CNS
TTFname(40): MSJHBD.ttf
TTFname(20): msjhbd.ttf
TTCname(30): msjhbd.ttc(0)

Name: MicrosoftJhengHeiLight
Class: CNS
TTCname(30): msjhl.ttc(0)

Name: MicrosoftMHei
Class: CNS
Provides(65): MHei-Medium
TTFname(10): MSMHei.ttf

Name: MicrosoftMHei-Bold
Class: CNS
TTFname(10): MSMHei-Bold.ttf

# Remove-only database (should begin with !INCLUDE)
# that is, entries which contain at least one 'RMVname' line
# note that this line should come at the _end_ of all INCLUDE files
!INCLUDE cjkgs-removeonly.dat
!INCLUDE cjkgs-macos-removeonly.dat


### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim: set tabstop=2 expandtab autoindent:
