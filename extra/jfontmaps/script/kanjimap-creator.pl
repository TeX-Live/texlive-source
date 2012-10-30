#!/usr/bin/env perl
#
# kanjimap-creator
# (c) 2012 Norbert Preining
# Licenced under the GPLv2 or any higher version
#
# gui to create map files for updmap(-setup-kanji)
#
# ptex/uptex:
#  2 fonts (rml/gbm)
#  2 variants
#  (ev vertical/horizontal)
#
# otf/otf-up:
#    gothic: regular, bold, heavy
#    mincho: regular, bold, light
#    maru gothic
#
# possible improvements:
# - allow editing current files by reading and interpreting them
#   needs better structure of the data
# - more checks, warnings?
# - proper POD documentation for man page
#

$^W = 1;
use strict;

use Tk;
use Tk::NoteBook;
use Tk::Dialog;
use File::Path qw(make_path);

my $prg = "fontmap-creator";

#
# global vars configuring operation
my $group_name = "";
my $do_vertical = 0;
my $do_iso2004  = 0;
my $do_otf      = 0;
my @f_mincho_regular; my @f_gothic_regular;
my @f_mincho_bold; my @f_gothic_bold;
my @f_gothic_heavy;
my @f_gothic_maru;
my @f_mincho_light;
my $b_save;

my $iso_i  = 1;
my $vert_i = 2;
my $isovert_i = 3;
my @order;

my $mw;
my $menu;
my $menu_file;

$order[0] = 'Default';
$order[$iso_i] = 'ISO2004';
$order[$vert_i] = 'Vertical';
$order[$isovert_i] = 'ISO2004/Vertical';

#
# the real data is at the end of the file, since it is a dump
# it contains the default settings for noEmbed
my $data;

&main();


sub main {
  #
  #
  $mw = MainWindow->new;
  $mw->title("Fontmap Creator");
  $mw->optionAdd("*Button.Relief", "ridge", 20);
  #
  # Menu
  $menu = $mw->Menu();
  $menu_file = $menu->Menu();
  $menu->add('cascade', -label => 'File', -menu => $menu_file);
  $menu_file->add('command', -label => "Save current dir", -command => \&export_font_maps,
    -state => 'disabled');
  $menu_file->add('command', -label => "Save to TEXMFHOME", -state => 'disabled',
    -command => sub { export_font_maps("TEXMFHOME"); });
  $menu_file->add('command', -label => "Save to TEXMFLOCAL", -state => 'disabled',
    -command => sub { export_font_maps("TEXMFLOCAL"); });
  $menu_file->add('command', -label => "Save to ...", -state => 'disabled',
    -command => sub { my $d = ask_dir(); if ($d) { export_font_maps($d); } });
  $menu_file->add('separator');
  $menu_file->add('command', -label => "Load from ...",
    -command => sub { warning("Loading from file not implemented by now."); });
  $menu_file->add('separator');
  $menu_file->add('command', -label => "Quit", -command => sub { $mw->destroy; exit(0); });
  $mw->configure(-menu => $menu);
  #
  my $tf = $mw->Frame;
  my $nb = $mw->NoteBook;
  my $bf = $mw->Frame;
  #
  # top frame
  #
  my $name_label = $tf->Label(-text => "Group name:");
  my $name_entry = $tf->Entry(-width => 30, -textvariable => \$group_name,
    -validate => "all", -validatecommand => \&validate_group_name);
  my $opt_label  = $tf->Label(-text => "Options:");
  my $opt_otf    = $tf->Checkbutton(-text => "OTF support",
    -variable => \$do_otf);
  my $opt_iso    = $tf->Checkbutton(-text => "separate ISO 2004 support",
    -variable => \$do_iso2004 );
  my $opt_vert   = $tf->Checkbutton(-text => "separate vertical fonts",
    -variable => \$do_vertical);
  #
  # pack the stuff
  $name_label->grid(-row => 0, -column => 0, -sticky => "e");
  $name_entry->grid(-row => 0, -column => 1, -sticky => "w");
  $opt_label->grid(-row => 1, -column => 0, -sticky => "e");
  $opt_otf->grid(-row => 1, -column => 1, -sticky => "w");
  $opt_iso->grid(-column => 1, -sticky => "w");
  $opt_vert->grid(-column => 1, -sticky => "w");
  #
  # notebook part
  my @p;
  for my $i (0..$#order) {
    $p[$i] = $nb->add($order[$i], -label => $order[$i],
      -state => ($i > 0 ? "disabled" : "normal"));
  }
  #
  # pack outer window
  $tf->pack(-expand => 1, -fill => 'x', -padx => '4m', -pady => '4m');
  $nb->pack(-expand => 1, -fill => 'both', -padx => '4m');
  $bf->pack(-expand => 1, -fill => 'x', -padx => '4m', -pady => '4m');
  #
  # 
  my @l_mincho; my @l_gothic;
  my @l_regular;
  my @l_bold;
  my @l_heavy;
  my @l_light;
  my @l_maru;
  my @e_mincho_regular; my @e_gothic_regular;
  my @e_mincho_bold; my @e_gothic_bold;
  my @e_gothic_heavy;
  my @e_gothic_maru;
  my @e_mincho_light;
  my $ew = 20;
  for my $i (0..$#order) {
    $l_gothic[$i] = $p[$i]->Label(-text => "Gothic");
    $l_mincho[$i] = $p[$i]->Label(-text => "Mincho");
    #
    $l_regular[$i] = $p[$i]->Label(-text => "Regular");
    $e_gothic_regular[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_gothic_regular[$i]);
    $e_mincho_regular[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_mincho_regular[$i]);
    #
    $l_bold[$i] = $p[$i]->Label(-text => "Bold", -state => "disabled");
    $e_gothic_bold[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_gothic_bold[$i],
      -state => "disabled", -relief => "flat");
    $e_mincho_bold[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_mincho_bold[$i],
      -state => "disabled", -relief => "flat");
    #
    $l_heavy[$i] = $p[$i]->Label(-text => "Heavy", -state => "disabled");
    $l_light[$i] = $p[$i]->Label(-text => "Light", -state => "disabled");
    $l_maru[$i] =  $p[$i]->Label(-text => "Maru Gothic",  -state => "disabled");
    $e_gothic_heavy[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_gothic_heavy[$i],
      -state => "disabled", -relief => "flat");
    $e_mincho_light[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_mincho_light[$i],
      -state => "disabled", -relief => "flat");
    $e_gothic_maru[$i] = $p[$i]->Entry(-width => $ew,
      -textvariable => \$f_gothic_maru[$i],
      -state => "disabled", -relief => "flat");
    #
    # grid the whole stuff
    $l_gothic[$i]->grid(-row => 0, -column => 1);
    $l_mincho[$i]->grid(-row => 0, -column => 2);
    #
    $l_regular[$i]->grid(-row => 1, -column => 0,  -sticky => "e");
    $e_gothic_regular[$i]->grid(-row => 1, -column => 1);
    $e_mincho_regular[$i]->grid(-row => 1, -column => 2);
    #
    $l_bold[$i]->grid(-row => 3, -column => 0,  -sticky => "e");
    $e_gothic_bold[$i]->grid(-row => 3, -column => 1);
    $e_mincho_bold[$i]->grid(-row => 3, -column => 2);
    #
    $l_heavy[$i]->grid(-row => 4, -column => 0,  -sticky => "e");
    $e_gothic_heavy[$i]->grid(-row => 4, -column => 2);
    #
    $l_light[$i]->grid(-row => 5, -column => 0,  -sticky => "e");
    $e_mincho_light[$i]->grid(-row => 5, -column => 1);
    $l_maru[$i]->grid(-row => 6, -column => 0,  -sticky => "e", -ipady => '3m');
    $e_gothic_maru[$i]->grid(-row => 6, -column => 1);
  }
  #
  # Button frame
  $b_save = $bf->Button(-text => "Save");

  #
  # Actions:
  #
  # activate tabs when options are selected
  $opt_vert->configure(-command => sub { 
      if (!$do_vertical && ($nb->raised() =~ m/Vertical/)) {
        $nb->raise("Default");
      }
      $nb->pageconfigure("Vertical", 
        -state => ($do_vertical ? "normal" : "disabled"));
      $nb->pageconfigure("ISO2004/Vertical",
        -state => (($do_vertical & $do_iso2004) ? "normal" : "disabled"))
    });
  $opt_iso->configure(-command => sub { 
      if (!$do_iso2004 && ($nb->raised() =~ m/ISO2004/)) {
        $nb->raise("Default");
      }
      $nb->pageconfigure("ISO2004",
        -state => ($do_iso2004 ? "normal" : "disabled"));
      $nb->pageconfigure("ISO2004/Vertical",
        -state => (($do_vertical & $do_iso2004) ? "normal" : "disabled")) 
    });

  # activate lower part for when otf is selected
  $opt_otf->configure(-command => sub {
    for my $i (0..$#order) {
      $l_light[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"));
      $e_mincho_light[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"),
        -relief => ($do_otf ? "sunken" : "flat"));
      #
      $l_bold[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"));
      $e_mincho_bold[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"),
        -relief => ($do_otf ? "sunken" : "flat"));
      $e_gothic_bold[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"),
        -relief => ($do_otf ? "sunken" : "flat"));
      #
      $l_heavy[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"));
      $l_maru[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"));
      $e_gothic_maru[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"),
        -relief => ($do_otf ? "sunken" : "flat"));
      $e_gothic_heavy[$i]->configure(
        -state => ($do_otf ? "normal" : "disabled"),
        -relief => ($do_otf ? "sunken" : "flat"));
    }
  });
  #
  #
  $b_save->configure(-command => \&export_font_maps, -state => "disabled");
  $b_save->pack;

  Tk::MainLoop();
}

sub validate_group_name {
  my ($new_val, undef, $old_val) = @_;
  my $ns = ($new_val eq "" ? "disabled" : "normal");
  $b_save->configure(-state => $ns);
  $menu_file->entryconfigure(1, -state => $ns);
  $menu_file->entryconfigure(2, -state => $ns);
  $menu_file->entryconfigure(3, -state => $ns);
  $menu_file->entryconfigure(4, -state => $ns);
  return 1;
}

sub addgroup {
  my ($str, $fref, $i_a, $i_b, @entries) = @_;
  my $do = 1;
  my $nrerr = 0;
  while (@entries) {
    my $tfm = shift @entries;
    my $enc = shift @entries;
    $nrerr += addlines($str, $tfm, $enc, ($do ? $fref->[$i_a] : $fref->[$i_b]));
    $do = !$do;
  } 
  return $nrerr;
}

sub addlines {
  my ($strref, @entries) = @_;
  my $nrerr = 0;
  while (@entries) {
    my $tfm = shift @entries;
    my $enc = shift @entries;
    my $fn  = shift @entries;
    if (defined($fn)) {
      $$strref .= "$tfm $enc $fn\n";
    } else {
      $nrerr++;
    }
  }
  return $nrerr;
}

sub export_font_maps {
  my $dest = shift;

  if ($group_name eq "") {
    warning("That should not happen, terminating!");
    exit 1;
  }

  if (!defined($dest)) {
    $dest = ".";
  } else {
    if ($dest eq "TEXMFHOME" || $dest eq "TEXMFLOCAL") {
      chomp (my $a = `kpsewhich -var-value=$dest`);
      if ($a eq "") {
        warning("Cannot find $dest, please choose a different directory");
        return;
      }
      # generate the real destination
      $dest = "$a/fonts/map/dvipdfmx/local/$group_name";
      make_path($dest);
    }
  }


  # indirections
  my $ii = ($do_iso2004  ?  $iso_i  : 0);
  my $vi = ($do_vertical ?  $vert_i : 0);
  my $ivi = (($do_vertical && $do_vertical) ?  $isovert_i : 0);
  #
  
  my ($ptexlines, $ptex04lines, $uptexlines, $uptex04lines);
  my ($otflines, $otfuplines);
  my $nrerr = 0;
  $nrerr += addlines(\$ptexlines, 
    'rml', 'H', $f_mincho_regular[0],
    'rmlv','V', $f_mincho_regular[$vi],
    'gbm', 'H', $f_gothic_regular[0],
    'gbmv','V', $f_gothic_regular[$vi]);
  $nrerr += addlines(\$uptexlines,
    'urml',     'UniJIS-UTF16-H', $f_mincho_regular[0],
    'urmlv',    'UniJIS-UTF16-V', $f_mincho_regular[$vi],
    'ugbm',     'UniJIS-UTF16-H', $f_gothic_regular[0],
    'ugbmv',    'UniJIS-UTF16-V', $f_gothic_regular[$vi],
    'uprml-h',  'UniJIS-UTF16-H', $f_mincho_regular[0],
    'uprml-v',  'UniJIS-UTF16-V', $f_mincho_regular[$vi],
    'upgbm-h',  'UniJIS-UTF16-H', $f_gothic_regular[0],
    'upgbm-v',  'UniJIS-UTF16-V', $f_gothic_regular[$vi],
    'uprml-hq', 'UniJIS-UCS2-H',  $f_mincho_regular[0],
    'upgbm-hq', 'UniJIS-UCS2-H',  $f_gothic_regular[0]);
  if ($do_iso2004) {
    $nrerr += addlines(\$ptex04lines, 
      'rml', 'H', $f_mincho_regular[$ii],
      'rmlv','V', $f_mincho_regular[$ivi],
      'gbm', 'H', $f_gothic_regular[$ii],
      'gbmv','V', $f_gothic_regular[$ivi]);
    $nrerr += addlines(\$uptex04lines,
      'urml',     'UniJIS-UTF16-H', $f_mincho_regular[$ii],
      'urmlv',    'UniJIS-UTF16-V', $f_mincho_regular[$ivi],
      'ugbm',     'UniJIS-UTF16-H', $f_gothic_regular[$ii],
      'ugbmv',    'UniJIS-UTF16-V', $f_gothic_regular[$ivi],
      'uprml-h',  'UniJIS-UTF16-H', $f_mincho_regular[$ii],
      'uprml-v',  'UniJIS-UTF16-V', $f_mincho_regular[$ivi],
      'upgbm-h',  'UniJIS-UTF16-H', $f_gothic_regular[$ii],
      'upgbm-v',  'UniJIS-UTF16-V', $f_gothic_regular[$ivi],
      'uprml-hq', 'UniJIS-UCS2-H',  $f_mincho_regular[$ii],
      'upgbm-hq', 'UniJIS-UCS2-H',  $f_gothic_regular[$ii]);
  }
 
  
  $nrerr += addlines(\$otflines,
    '%', 'mincho regular', '',
    'otf-ujmr-h', 'UniJIS-UTF16-H', $f_mincho_regular[0],
    'otf-ujmr-v', 'UniJIS-UTF16-V', $f_mincho_regular[$vi],
    'otf-cjmr-h', 'Identity-H',     $f_mincho_regular[0],
    'otf-cjmr-v', 'Identity-V',     $f_mincho_regular[$vi],
    'hminr-h',    'H',              $f_mincho_regular[0],
    'hminr-v',    'V',              $f_mincho_regular[$vi],
    '%', 'gothic regular', '',
    'otf-ujgr-h', 'UniJIS-UTF16-H', $f_gothic_regular[0],
    'otf-ujgr-v', 'UniJIS-UTF16-V', $f_gothic_regular[$vi],
    'otf-cjgr-h', 'Identity-H',     $f_gothic_regular[0],
    'otf-cjgr-v', 'Identity-V',     $f_gothic_regular[$vi],
    'hgothr-h',   'H',              $f_gothic_regular[0],
    'hgothr-v',   'V',              $f_gothic_regular[$vi],
    '%', 'mincho bold', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_mincho_bold : \@f_mincho_regular),
    0, $vi,
    'otf-ujmb-h', 'UniJIS-UTF16-H',
    'otf-ujmb-v', 'UniJIS-UTF16-V',
    'otf-cjmb-h', 'Identity-H',
    'otf-cjmb-v', 'Identity-V',
    'hminb-h',    'H',
    'hminb-v',    'V');
  addlines(\$otflines, '%', 'gothic bold', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_gothic_bold : \@f_gothic_regular),
    0, $vi,
    'otf-ujgb-h', 'UniJIS-UTF16-H',
    'otf-ujgb-v', 'UniJIS-UTF16-V',
    'otf-cjgb-h', 'Identity-H',
    'otf-cjgb-v', 'Identity-V',
    'hgothb-h',   'H',
    'hgothb-v',   'V');
  addlines(\$otflines, '%', 'gothic heavy', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_gothic_heavy : \@f_gothic_regular),
    0, $vi,
    'hgotheb-h', 'H',
    'hgotheb-v', 'V');
  addlines(\$otflines, '%', 'gothic maru', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_gothic_maru : \@f_gothic_regular),
    0, $vi,
    'otf-ujmgr-h', 'UniJIS-UTF16-H',
    'otf-ujmgr-v', 'UniJIS-UTF16-V',
    'otf-cjmgr-h', 'Identity-H',
    'otf-cjmgr-v', 'Identity-V',
    'hmgothr-h',   'H',
    'hmgothr-v',   'V');
  addlines(\$otflines, '%', 'mincho light', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_mincho_light : \@f_mincho_regular),
    0, $vi,
    'otf-ujml-h', 'UniJIS-UTF16-H',
    'otf-ujml-v', 'UniJIS-UTF16-V',
    'otf-cjml-h', 'Identity-H',
    'otf-cjml-v', 'Identity-V',
    'hminl-h',    'H',
    'hminl-v',    'V');
  addlines(\$otflines, '%', 'JIS 2004', '',
    'otf-ujmrn-h', 'UniJIS2004-UTF16-H', $f_mincho_regular[$ii],
    'otf-ujmrn-v', 'UniJIS2004-UTF16-V', $f_mincho_regular[$ivi],
    'hminrn-h',    'H',                  $f_mincho_regular[$ii],
    'hminrn-v',    'V',                  $f_mincho_regular[$ivi],
    '%', '', '',
    'otf-ujgrn-h', 'UniJIS2004-UTF16-H', $f_gothic_regular[$ii],
    'otf-ujgrn-v', 'UniJIS2004-UTF16-V', $f_gothic_regular[$ivi],
    'hgothrn-h',   'H'                 , $f_gothic_regular[$ii],
    'hgothrn-v',   'V'                 , $f_gothic_regular[$ivi],
    '%', '', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_mincho_bold : \@f_mincho_regular),
    $ii, $ivi,
    'otf-ujmbn-h', 'UniJIS2004-UTF16-H',
    'otf-ujmbn-v', 'UniJIS2004-UTF16-V',
    'hminbn-h',    'H',
    'hminbn-v',    'V');
  addlines(\$otflines, '%', '', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_gothic_bold : \@f_gothic_regular),
    $ii, $ivi,
    'otf-ujgbn-h', 'UniJIS2004-UTF16-H',
    'otf-ujgbn-v', 'UniJIS2004-UTF16-V',
    'hgothbn-h',   'H',
    'hgothbn-v',   'V');
  addlines(\$otflines, '%', '', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_gothic_heavy : \@f_gothic_regular),
    $ii, $ivi,
    'otf-ujmgrn-h', 'UniJIS2004-UTF16-H',
    'otf-ujmgrn-v', 'UniJIS2004-UTF16-V',
    'hmgothrn-h', 'H',
    'hmgothrn-v', 'V');
  addlines(\$otflines, '%', '', '');
  $nrerr += addgroup(\$otflines, ($do_otf ? \@f_mincho_light : \@f_mincho_regular),
    $ii, $ivi,
    'otf-ujmln-h', 'UniJIS2004-UTF16-H',
    'otf-ujmln-v', 'UniJIS2004-UTF16-V',
    'hminln-h',    'H',
    'hminln-v',    'V');

  $nrerr += addlines(\$otfuplines,
    'uphminr-h',   'UniJIS-UTF16-H', $f_mincho_regular[0],
    'uphminr-v',   'UniJIS-UTF16-V', $f_mincho_regular[$vi],
    'uphgothr-h',  'UniJIS-UTF16-H', $f_gothic_regular[0],
    'uphgothr-v',  'UniJIS-UTF16-V', $f_gothic_regular[$vi]);
  $nrerr += addgroup(\$otfuplines, ($do_otf ? \@f_mincho_bold : \@f_mincho_regular),
    0, $vi,
    'uphminb-h',   'UniJIS-UTF16-H',
    'uphminb-v',   'UniJIS-UTF16-V');
  $nrerr += addgroup(\$otfuplines, ($do_otf ? \@f_gothic_bold : \@f_gothic_regular),
    0, $vi,
    'uphgothb-h',  'UniJIS-UTF16-H',
    'uphgothb-v',  'UniJIS-UTF16-V');
  $nrerr += addgroup(\$otfuplines, ($do_otf ? \@f_gothic_heavy : \@f_gothic_regular),
    0, $vi,
    'uphgotheb-h', 'UniJIS-UTF16-H',
    'uphgotheb-v', 'UniJIS-UTF16-V');
  $nrerr += addgroup(\$otfuplines, ($do_otf ? \@f_gothic_maru : \@f_gothic_regular),
    0, $vi,
    'uphmgothr-h', 'UniJIS-UTF16-H',
    'uphmgothr-v', 'UniJIS-UTF16-V');
  $nrerr += addgroup(\$otfuplines, ($do_otf ? \@f_mincho_light : \@f_mincho_regular),
    0, $vi,
    'uphminl-h',   'UniJIS-UTF16-H',
    'uphminl-v',   'UniJIS-UTF16-V');

  #
  if ($nrerr > 0) {
    warning("Some of the font definitions are not given ($nrerr)!");
  }

  # check that none of the output files are already existing:
  if (-r "$dest/ptex-$group_name.map" ||
      -r "$dest/ptex-${group_name}-04.map" ||
      -r "$dest/uptex-$group_name.map" ||
      -r "$dest/uptex-${group_name}-04.map" ||
      -r "$dest/otf-$group_name.map" ||
      -r "$dest/otf-up-$group_name.map" ||
      -r "$dest/$group_name.map" ||
      -r "$dest/${group_name}-04.map") {
    warning("Some of the output files already exist in the $dest, aborting!\nPlease choose a different output location.");
    return;
  }

  # generate the output files
  open (OUT, ">$dest/ptex-$group_name.map") 
    or die("Cannot open $dest/ptex-$group_name.map for writing: $!");
  print OUT "% generated by $prg\n$ptexlines\n";
  close(OUT);

  open (OUT, ">$dest/uptex-$group_name.map") 
    or die("Cannot open $dest/uptex-$group_name.map for writing: $!");
  print OUT "% generated by $prg\n$uptexlines\n";
  close(OUT);

  if ($do_iso2004) {
    open (OUT, ">$dest/ptex-${group_name}-04.map")
      or die("Cannot open $dest/ptex-${group_name}-04.map for writing: $!");
    print OUT "% generated by $prg\n$ptex04lines\n";
    close(OUT);

    open (OUT, ">$dest/uptex-${group_name}-04.map")
      or die("Cannot open $dest/uptex-${group_name}-04.map for writing: $!");
    print OUT "% generated by $prg\n$uptex04lines\n";
    close(OUT);
  }

  open (OUT, ">$dest/otf-$group_name.map") 
    or die("Cannot open $dest/otf-$group_name.map for writing: $!");
  print OUT "% generated by $prg\n$otflines\n";
  close(OUT);

  open (OUT, ">$dest/otf-up-$group_name.map") 
    or die("Cannot open $dest/otf-up-$group_name.map for writing: $!");
  print OUT "% generated by $prg\n$otfuplines\n";
  close(OUT);
 
  open (OUT, ">$dest/$group_name.map")
    or die("Cannot open $dest/$group_name.map for writing: $!");
  print OUT "% generated by $prg\n%\n% maps for family $group_name\n\n";
  print OUT "% ptex\n$ptexlines\n";
  print OUT "% uptex\n$uptexlines\n";
  print OUT "% otf\n$otflines\n";
  print OUT "% otf-uptex\n$otfuplines\n";
  close(OUT);
 
  if ($do_iso2004) {
    open (OUT, ">$dest/${group_name}-04.map")
      or die("Cannot open $dest/${group_name}-04.map for writing: $!");
    print OUT "% generated by $prg\n%\n% maps for family $group_name ISO2004\n\n";
    print OUT "% ptex\n$ptex04lines\n";
    print OUT "% uptex\n$uptex04lines\n";
    print OUT "% otf\n$otflines\n";
    print OUT "% otf-uptex\n$otfuplines\n";
    close(OUT);
  }

  $mw->Dialog(-title => "Finished",
    -text => "Fontmaps have been created in $dest\nPlease move them to a place where dvipdfmx can find them.",
    -buttons => [ "Finish" ])->Show();
 
  $mw->destroy;
  exit 0;
}

sub warning {
  my $str = shift;
  my $sw = $mw->DialogBox(-title => "Warning", -buttons => [ "Ok" ]);
  $sw->add("Label", -text => $str)->pack;
  $sw->Show;
}

sub ask_fam_and_dir {
  
}

sub ask_dir {
  my $val;
  my $done;
  my $sw = $mw->Toplevel(-title => "Select directory");
  $sw->transient($mw);
  $sw->withdraw;
  $sw->Label(-text => "Please select a directory for saving the map files")
    ->pack(-padx => "2m", -pady => "2m");
  my $f1 = $sw->Frame;
  my $entry = $f1->Entry(-textvariable => \$val, -width => 50);
  $entry->pack(-side => "left",-padx => "2m", -pady => "2m");
  
  my $f2 = $sw->Frame;
  $f2->Button(-text => "Choose existing directory",
    -command => sub { $val = $sw->chooseDirectory; })->pack(-side => "left",-padx => "2m", -pady => "2m");

  $f1->pack;
  $f2->pack;

  my $f = $sw->Frame;
  my $okbutton = $f->Button(-text => "Ok",
    -command => sub { $done = 1; }
    )->pack(-side => 'left',-padx => "2m", -pady => "2m");
  my $cancelbutton = $f->Button(-text => "Cancel",
    -command => sub { $val = undef; $done = 1; }
    )->pack(-side => 'right',-padx => "2m", -pady => "2m");
  $f->pack(-expand => 'x');
  $sw->bind('<Return>', [ $okbutton, 'Invoke' ]);
  $sw->bind('<Escape>', [ $cancelbutton, 'Invoke' ]);
  my $old_focus = $sw->focusSave;
  my $old_grab = $sw->grabSave;
  $sw->Popup;
  $sw->grab;
  $sw->waitVariable(\$done);
  $sw->grabRelease if Tk::Exists($sw);
  $sw->destroy if Tk::Exists($sw);
  return $val;
}

# read_files(".", "noEmbed");
#
# parse a dvipdfmx kanjimap line as defined in cid-x.map
#
sub parse_map_line {
  my ($a, $b, $c, $d) = split(' ', shift, 4);
  my %r;
  if ($a =~ m/^([^@]+)@([^@]+)@$/) {
    $r{'tfmname'} = $1;
    $r{'sfdname'} = $2;
  } else {
    $r{'tfmname'} = $a;
  }
  $r{'enc'} = $b;
  if ($c =~ m/^(:([0-9]+):)?(!)?([^\/,]+)(\/([^,]+))?(,.*)?$/) {
    $r{'index'} = $2 if (defined($2));
    $r{'embed'} = (defined($3) ? "!" : "");
    $r{'fontname'} = $4;
    $r{'csi'} = $6 if (defined($6));
    $r{'variant'} = $7 if (defined($7));
  } else {
    $r{'fontname'} = $c;
  }

  $r{'options'} = $d if (defined($d));

  return ($a, \%r);
}

sub read_files {
  my $d = shift;
  my $f = shift;
  print "f=$f d=$d\n";
  my $fn;
  if (!defined($f)) {
    if (-f $d) {
      $fn = $d;
    } else {
      print "What should I read!\n";
      exit 1;
    }
  } else {
    $fn = "$d/$f";
  }

  my %r;
  #
  do_one_fam("ptex",    "$d/ptex-$f.map");
  do_one_fam("ptex04",  "$d/ptex-$f-04.map");
  do_one_fam("uptex",   "$d/uptex-$f.map");
  do_one_fam("uptex04", "$d/uptex-$f.map");
  do_one_fam("otf",     "$d/otf-$f.map");
  do_one_fam("otfup",   "$d/otf-up-$f.map");
  print Dumper($data);
}

sub do_one_fam {
  my ($t, $fn) = @_;
  if (!open(FOO, "<$fn")) {
    print STDERR "type $t not supported\n";
    return;
  }
  while (<FOO>) {
    chomp;
    next if (/^\s*(%.*)?$/);
    my ($a, $r) = parse_map_line($_);
    $data->{$t}{$a} = $r;
  }
}


$data = {
  'ptex' => {
    'gbmv' => {
      'embed' => '!',
      'tfmname' => 'gbmv',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'gbm' => {
      'embed' => '!',
      'tfmname' => 'gbm',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'rml' => {
      'embed' => '!',
      'tfmname' => 'rml',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    },
    'rmlv' => {
      'embed' => '!',
      'tfmname' => 'rmlv',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    }
  },
  'uptex04' => {
    'upgbm-h' => {
      'embed' => '!',
      'tfmname' => 'upgbm-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uprml-h' => {
      'embed' => '!',
      'tfmname' => 'uprml-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'urml' => {
      'embed' => '!',
      'tfmname' => 'urml',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'ugbmv' => {
      'embed' => '!',
      'tfmname' => 'ugbmv',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uprml-v' => {
      'embed' => '!',
      'tfmname' => 'uprml-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'urmlv' => {
      'embed' => '!',
      'tfmname' => 'urmlv',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uprml-hq' => {
      'embed' => '!',
      'tfmname' => 'uprml-hq',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UCS2-H'
    },
    'ugbm' => {
      'embed' => '!',
      'tfmname' => 'ugbm',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'upgbm-v' => {
      'embed' => '!',
      'tfmname' => 'upgbm-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'upgbm-hq' => {
      'embed' => '!',
      'tfmname' => 'upgbm-hq',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UCS2-H'
    }
  },
  'uptex' => {
    'upgbm-h' => {
      'embed' => '!',
      'tfmname' => 'upgbm-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uprml-h' => {
      'embed' => '!',
      'tfmname' => 'uprml-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'urml' => {
      'embed' => '!',
      'tfmname' => 'urml',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'ugbmv' => {
      'embed' => '!',
      'tfmname' => 'ugbmv',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uprml-v' => {
      'embed' => '!',
      'tfmname' => 'uprml-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'urmlv' => {
      'embed' => '!',
      'tfmname' => 'urmlv',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uprml-hq' => {
      'embed' => '!',
      'tfmname' => 'uprml-hq',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UCS2-H'
    },
    'ugbm' => {
      'embed' => '!',
      'tfmname' => 'ugbm',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'upgbm-v' => {
      'embed' => '!',
      'tfmname' => 'upgbm-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'upgbm-hq' => {
      'embed' => '!',
      'tfmname' => 'upgbm-hq',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UCS2-H'
    }
  },
  'otfup' => {
    'uphminl-h' => {
      'embed' => '!',
      'tfmname' => 'uphminl-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uphminl-v' => {
      'embed' => '!',
      'tfmname' => 'uphminl-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphmgothr-v' => {
      'embed' => '!',
      'tfmname' => 'uphmgothr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphgotheb-v' => {
      'embed' => '!',
      'tfmname' => 'uphgotheb-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphminr-v' => {
      'embed' => '!',
      'tfmname' => 'uphminr-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphgotheb-h' => {
      'embed' => '!',
      'tfmname' => 'uphgotheb-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uphminr-h' => {
      'embed' => '!',
      'tfmname' => 'uphminr-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uphmgothr-h' => {
      'embed' => '!',
      'tfmname' => 'uphmgothr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uphgothr-v' => {
      'embed' => '!',
      'tfmname' => 'uphgothr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphminb-v' => {
      'embed' => '!',
      'tfmname' => 'uphminb-v',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphgothb-v' => {
      'embed' => '!',
      'tfmname' => 'uphgothb-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'uphgothr-h' => {
      'embed' => '!',
      'tfmname' => 'uphgothr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uphgothb-h' => {
      'embed' => '!',
      'tfmname' => 'uphgothb-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'uphminb-h' => {
      'embed' => '!',
      'tfmname' => 'uphminb-h',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    }
  },
  'otf' => {
    'hgotheb-v' => {
      'embed' => '!',
      'tfmname' => 'hgotheb-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'hminbn-h' => {
      'embed' => '!',
      'tfmname' => 'hminbn-h',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    },
    'hgothr-v' => {
      'embed' => '!',
      'tfmname' => 'hgothr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'otf-ujgb-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgb-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'otf-cjgr-h' => {
      'embed' => '!',
      'tfmname' => 'otf-cjgr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'Identity-H'
    },
    'hgothbn-v' => {
      'embed' => '!',
      'tfmname' => 'hgothbn-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'otf-cjmb-h' => {
      'embed' => '!',
      'tfmname' => 'otf-cjmb-h',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'Identity-H'
    },
    'hminr-v' => {
      'embed' => '!',
      'tfmname' => 'hminr-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    },
    'otf-ujmgr-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmgr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'otf-cjgr-v' => {
      'embed' => '!',
      'tfmname' => 'otf-cjgr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'Identity-V'
    },
    'otf-ujmln-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmln-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS2004-UTF16-H'
    },
    'hminl-v' => {
      'embed' => '!',
      'tfmname' => 'hminl-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    },
    'hminrn-h' => {
      'embed' => '!',
      'tfmname' => 'hminrn-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    },
    'otf-ujgbn-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgbn-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS2004-UTF16-H'
    },
    'otf-ujml-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujml-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'hminln-h' => {
      'embed' => '!',
      'tfmname' => 'hminln-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    },
    'hgothrn-h' => {
      'embed' => '!',
      'tfmname' => 'hgothrn-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'hminln-v' => {
      'embed' => '!',
      'tfmname' => 'hminln-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    },
    'otf-ujmbn-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmbn-h',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS2004-UTF16-H'
    },
    'hgothb-v' => {
      'embed' => '!',
      'tfmname' => 'hgothb-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'otf-ujgbn-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgbn-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS2004-UTF16-V'
    },
    'hgothb-h' => {
      'embed' => '!',
      'tfmname' => 'hgothb-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'hmgothrn-h' => {
      'embed' => '!',
      'tfmname' => 'hmgothrn-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'otf-cjml-v' => {
      'embed' => '!',
      'tfmname' => 'otf-cjml-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'Identity-V'
    },
    'hgothrn-v' => {
      'embed' => '!',
      'tfmname' => 'hgothrn-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'otf-ujgr-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'otf-ujmr-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmr-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'otf-ujmln-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmln-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS2004-UTF16-V'
    },
    'otf-cjmr-h' => {
      'embed' => '!',
      'tfmname' => 'otf-cjmr-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'Identity-H'
    },
    'otf-ujmbn-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmbn-v',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS2004-UTF16-V'
    },
    'otf-cjmb-v' => {
      'embed' => '!',
      'tfmname' => 'otf-cjmb-v',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'Identity-V'
    },
    'otf-cjgb-v' => {
      'embed' => '!',
      'tfmname' => 'otf-cjgb-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'Identity-V'
    },
    'hmgothrn-v' => {
      'embed' => '!',
      'tfmname' => 'hmgothrn-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'hmgothr-h' => {
      'embed' => '!',
      'tfmname' => 'hmgothr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'otf-ujmrn-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmrn-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS2004-UTF16-V'
    },
    'otf-cjgb-h' => {
      'embed' => '!',
      'tfmname' => 'otf-cjgb-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'Identity-H'
    },
    'hgotheb-h' => {
      'embed' => '!',
      'tfmname' => 'hgotheb-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'otf-ujmrn-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmrn-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS2004-UTF16-H'
    },
    'otf-ujgrn-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgrn-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS2004-UTF16-H'
    },
    'hgothr-h' => {
      'embed' => '!',
      'tfmname' => 'hgothr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'hminr-h' => {
      'embed' => '!',
      'tfmname' => 'hminr-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    },
    'otf-ujmgrn-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmgrn-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS2004-UTF16-H'
    },
    'otf-ujgrn-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgrn-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS2004-UTF16-V'
    },
    'otf-ujmb-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmb-v',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'otf-cjmgr-v' => {
      'embed' => '!',
      'tfmname' => 'otf-cjmgr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'Identity-V'
    },
    'otf-ujmgrn-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmgrn-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS2004-UTF16-V'
    },
    'otf-ujgr-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'otf-ujmb-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmb-h',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-H'
    },
    'otf-ujmgr-h' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmgr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-H'
    },
    'hminrn-v' => {
      'embed' => '!',
      'tfmname' => 'hminrn-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    },
    'otf-ujml-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujml-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'hminb-v' => {
      'embed' => '!',
      'tfmname' => 'hminb-v',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    },
    'otf-cjml-h' => {
      'embed' => '!',
      'tfmname' => 'otf-cjml-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'Identity-H'
    },
    'hgothbn-h' => {
      'embed' => '!',
      'tfmname' => 'hgothbn-h',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'H'
    },
    'hminbn-v' => {
      'embed' => '!',
      'tfmname' => 'hminbn-v',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'V'
    },
    'hminl-h' => {
      'embed' => '!',
      'tfmname' => 'hminl-h',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    },
    'otf-ujmr-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujmr-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'UniJIS-UTF16-V'
    },
    'otf-ujgb-v' => {
      'embed' => '!',
      'tfmname' => 'otf-ujgb-v',
      'variant' => ',Bold',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'UniJIS-UTF16-V'
    },
    'otf-cjmgr-h' => {
      'embed' => '!',
      'tfmname' => 'otf-cjmgr-h',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'Identity-H'
    },
    'hmgothr-v' => {
      'embed' => '!',
      'tfmname' => 'hmgothr-v',
      'fontname' => 'GothicBBB-Medium',
      'enc' => 'V'
    },
    'otf-cjmr-v' => {
      'embed' => '!',
      'tfmname' => 'otf-cjmr-v',
      'fontname' => 'Ryumin-Light',
      'enc' => 'Identity-V'
    },
    'hminb-h' => {
      'embed' => '!',
      'tfmname' => 'hminb-h',
      'variant' => ',Bold',
      'fontname' => 'Ryumin-Light',
      'enc' => 'H'
    }
  }
};

# vim:set tabstop=2 expandtab: #
