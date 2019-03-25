#! /usr/bin/env perl
###############################################################################
# texdoctk - GUI for TeX documentation access
# Copyright (C) 2000-2004  Thomas Ruedas
# Updated in 2010 by Manuel Pegourie-Gonnard
# Trivial non-code updates in 2012 by Karl Berry
# Patch from Debian for "texdoctk-warn-missing-perltk" installed in 2019
#   by Karl Berry.
# (This program is looking for a maintainer, email tex-live@tug.org.)
# 
# This program is provided under the GNU Public License; see the texdoctk
# README for details about requirements, installation, configuration,
# and the full disclaimer.
###############################################################################
my ($version, $date) = ("0.6.1", "2012-12-23");
use strict;
use Getopt::Long;
Getopt::Long::config('bundling');
use File::Basename;

BEGIN {
    Win32::SetChildShowWindow(0) if defined &Win32::SetChildShowWindow;
}

my $IsWin32 = ($^O =~ /MSWin32/i);

if ($IsWin32) {
  require Win32::API;
#  Win32::API->import( qw(
#    &something
#  ));
}

eval { require Tk; };
if ($@) {
  if (-x "/usr/bin/xmessage") {
    `xmessage -center -buttons Quit "The program texdoctk needs the package perl-tk, please install it!"`;
  } else {
    printf STDERR "The program texdoctk needs the package perl-tk, please install it!\n";
  }
  exit(1);
  # that didn't work out, so warn the user and continue with text mode
} else {
  Tk->import;
}


# initialization of some internal variables
$|=1;
my $quiet=1;
my $autoview;
my $xfmt_viewer=1,
my @tdcolors;
my %butcol;
my $srchentry;
my $srchflag=0;
my $srchtype; # 0 = database, 1 = texdoc
my $tmpfno;
# system variables
my ($texmfmain,$texmfdist,$texmfdoc,$texmflocal,$texmfhome,
    $texdocpath,$distdocpath,$docdocpath,$localdocpath,$homedocpath,
    $datadir,
    $dvi_viewer,$dvips_conv,$ps_viewer,$pdf_viewer,
    $pdfps_conv,$html_viewer,$htmlps_conv,$htmlps_redir,
    $txt_viewer,$txtps_conv,$txtps_redir,$print_cmd);

# defaults: unix values
my $qq="'";
my $HomeEnv=$ENV{HOME};
my $TeXDocRcDefaults="texdocrc.defaults";
my $TempDir; $TempDir=$ENV{TMPDIR} or $TempDir="/tmp";
my $CmdConsole="xterm -e ";

if ($IsWin32) {
  $qq = "\"";
  $HomeEnv = $ENV{'USERPROFILE'};
  chomp $HomeEnv;
  $TeXDocRcDefaults = "texdocrc-win32.defaults";
  $TempDir = &GetTempPath;
  $CmdConsole = "cmd /c ";
}

my $myrc="${HomeEnv}/.texdocrc"; # optional personal settings file

my ($line,@dummy);
my @tmpfiles;
# read system-wide defaults from texdocrc.defaults
my $sysrc=`kpsewhich --progname=texdoctk --format=${qq}other text files${qq} ${TeXDocRcDefaults}`;
my $database=`kpsewhich --progname=texdoctk --format=${qq}other text files${qq} texdoctk.dat`;
my $locdatabase=`kpsewhich --progname=texdoctk --format=${qq}other text files${qq} texdoctk-local.dat`;
my $homedatabase=`kpsewhich --progname=texdoctk --format=${qq}other text files${qq} texdoctk-pers.dat`;
chomp $locdatabase;
chomp $homedatabase;
chomp $sysrc;
&readrc($sysrc,1);
@dummy=split("/",$sysrc);
pop @dummy;
$datadir=join('/',@dummy);
# possibly use personal settings instead of defaults
if (-e $myrc) { &readrc($myrc,2); }
# command line options
GetOptions('v'=>\$quiet,'a'=>\$autoview);
my $special=0;
my @button;
$button[17]="Miscellaneous";
my (@packname,@topic,@doc,@keywords,@maxind,%stydoc);
# read database file
my $i=-1;
my $j;
open(DATABASE,"$database") || &fatalmsg("Couldn't open database $database.\n");
while ($line = <DATABASE>) {
  LINETYPE: {
      ($line =~ /^\@/) && do { # category
	  ++$i;
	  if ($i == $#button) {
	      pop @button;
	      push @button,(substr($line,1,-1));
	      $special=1;
	  } else {
	      $button[$i]=substr($line,1,-1);
	  }
	  $j=0;
	  last LINETYPE;
      };
      ($line =~ /^(#|$)/) && do { # comment or empty line
	  last LINETYPE;
      };
#     list item
       @dummy=split(";",$line);
       $packname[$i][$j]=$dummy[0];
       $topic[$i][$j]=$dummy[1];
       $doc[$i][$j]=$dummy[2];
       $keywords[$i][$j]=$dummy[3];
       if ($doc[$i][$j] =~ /sty$/ && $keywords[$i][$j]) {
	   $stydoc{"$doc[$i][$j]"}=substr($keywords[$i][$j],1,1);
	   unless ($stydoc{"$doc[$i][$j]"} =~ /^\d$/) {
	       $stydoc{"$doc[$i][$j]"}=0;
	   }
       }
       $maxind[$i]=$j;
       ++$j;
  };
}
close(DATABASE);
# Read site-local and user's database(s) if it/they exist(s)
foreach my $dbfile ($locdatabase,$homedatabase) {
  if (-e "$dbfile") {
    open(DATABASE,"$dbfile") ||
      print "Couldn't open additional database $dbfile.\n";
    $i=17; # local/user: if no category is specified in database, add to "Misc"
    while ($line = <DATABASE>) {
    LINETYPE: {
	($line =~ /^\@/) && do { # category
	  $i=&arrindex(@button,substr($line,1,-1));
	  if ($i >= $#button) {
	    $i=$#button;
	    pop @button;
	    push @button,(substr($line,1,-1));
	    $special=1;
	  }
	  last LINETYPE;
	};
	($line =~ /^(#|$)/) && do { # comment or empty line
	  last LINETYPE;
	};
#       list item
	@dummy=split(";",$line);
	$j=&arrindex(@{$packname[$i]},$dummy[0]);
	if ($j > $#{$packname[$i]}) {
	  $packname[$i][$j]=$dummy[0];
	  $topic[$i][$j]=$dummy[1];
	  $doc[$i][$j]=$dummy[2];
	  $keywords[$i][$j]=$dummy[3];
	  if ($doc[$i][$j] =~ /sty$/ && $keywords[$i][$j]) {
	    $stydoc{"$doc[$i][$j]"}=substr($keywords[$i][$j],1,1);
	    unless ($stydoc{"$doc[$i][$j]"} =~ /^\d$/) {
	      $stydoc{"$doc[$i][$j]"}=0;
	    }
	  }
	  $maxind[$i]=$j;
	  ++$j;
	}
      };
    }
    close(DATABASE);
    $special=1 if ($i == 17 && $j > 0);
  }
}

# Tk
# hash table for toplevel windows; used to avoid multiple calls of same window
my %tlwins;
my %buttonlist;
# create frames for main window: commands on top, frame for buttons below
my $main=new MainWindow;
$tlwins{'mainwindow'}{'addr'}=$main;
$main->resizable(0,0);
$main->title("TeX Documentation Browser");
$main->bind('all','<Control-q>'=>\&clean_exit);
$main->bind('all','<Control-m>'=>sub { $main->raise(); });
$main->bind('all','<Control-h>'=>\&helptext);
$main->bind('all','<Control-s>'=>sub { $main->raise(); &mksrch; });
$main->bind('all','<Control-t>'=>\&settings);
my $cmdframe=$main->Frame(-background=>"#ffcc99");
my $buttonframe=$main->Frame;
$cmdframe->pack(-side=>'top',-fill=>'x');
$buttonframe->pack(-side=>'bottom');
# normal cursor
my $defcursor=$main->cget(-cursor);
# make buttons for command frame
my $Qbut=$cmdframe->Button(-text=>'Quit',%butcol,
			   -command=>\&clean_exit)->pack(-side=>'left');
$tlwins{'mainwindow'}{'buttons'}[0]=$Qbut;
# define common default font for labels and text explicitly
my $deffont=$Qbut->cget(-font);
# ensure readability on high-res screens (suggested by R.Kotucha)
$deffont='Helvetica -16 bold' if &x_resolution > 1200;
$Qbut->configure(-font=>$deffont);
$tlwins{'mainwindow'}{'buttons'}[1]=$cmdframe->Button(
						      -text=>'Database search',
						      -font=>$deffont,%butcol,
						      -command=>[\&mksrch, 0]
						      )->pack(-side=>'left');
$tlwins{'mainwindow'}{'buttons'}[2]=$cmdframe->Button(
						      -text=>'File search',
						      -font=>$deffont,%butcol,
						      -command=>[\&mksrch, 1]
						      )->pack(-side=>'left');
$tlwins{'mainwindow'}{'buttons'}[3]=$cmdframe->Button(-text=>'Help/About',
						      -font=>$deffont,%butcol,
						      -command=>\&helptext
						      )->pack(-side=>'right');
$tlwins{'mainwindow'}{'buttons'}[4]=$cmdframe->Button(-text=>'Settings',
						      -font=>$deffont,%butcol,
						      -command=>\&settings
						      )->pack(-side=>'right');
# make buttons for category button frame
my $lbut=0;
my $l;
foreach (@button) { $l=length $_; $lbut=$l if ( $l > $lbut) }
my @catg;
my ($i2,$i3);
my $nbutt=scalar @button;
my $ncols=$nbutt/3-1;
foreach (0..$ncols) {
    $i=$_;
    $i2=$i+$nbutt/3;
    $i3=$i+2*$nbutt/3;
    $catg[$i]=$buttonframe->Button(-text=>$button[$i],
				   -font=>$deffont,%butcol,
				   -width=>$lbut,
				   -command=>[\&tpslct,$i,\@dummy])->grid
	($catg[$i2]=$buttonframe->Button(-text=>$button[$i2],
					 -font=>$deffont,%butcol,
					 -width=>$lbut,
					 -command=>[\&tpslct,$i2,\@dummy]),
	 $catg[$i3]=$buttonframe->Button(-text=>$button[$i3],
					 -font=>$deffont,%butcol,
					 -width=>$lbut,
					 -command=>[\&tpslct,$i3,\@dummy]));
}
# disable last button (lower right) if no local specials are found in list
my $ncat;
if ($special == 0) {
    $catg[$#button]->configure(-state=>'disabled');
    $ncat=$nbutt-1;
} else {
    $ncat=$nbutt;
}
for ($i=0,$j=5; $i<@catg; ++$i,++$j) {
    $tlwins{'mainwindow'}{'buttons'}[$j]=$catg[$i];
}
MainLoop();

########## SUBROUTINES ########################################################
# toplevel for selecting a topic of a category for viewing or printing
sub tpslct {
    my($opt,@srchitems)=@_;
    my (@lbitems,@lbdocs,$dspselect,$docselect,$wtitle);
    if ($opt >= 0) {
#       main window buttons
#       see if toplevel window is already there
	if (Exists($tlwins{$opt}{'addr'})) {
	    $tlwins{$opt}{'addr'}->deiconify();
	    $tlwins{$opt}{'addr'}->raise();
	    return;
	}
	for ($j=0; $j <= $maxind[$opt]; ++$j) {
	    push @lbitems,$topic[$opt][$j];
	    push @lbdocs,$doc[$opt][$j];
	}
	$wtitle=$button[$opt];
    } else {
#       search results
        my $spec_wtitle=shift @srchitems;
	for ($j=0; $j < $#srchitems; $j+=2) {
	    my $k=$j+1;
	    push @lbitems,$topic[$srchitems[$j]][$srchitems[$k]];
	    push @lbdocs,$doc[$srchitems[$j]][$srchitems[$k]];
	}
	$wtitle = ($srchtype ? "File" : "Database")
	    . " search results for $spec_wtitle";
    }
# toplevel window of category $opt with two frames
    my $tpwin=$main->Toplevel(-title=>$wtitle);
    $tlwins{$opt}{'addr'}=$tpwin;
    my $tpdsp=$tpwin->Frame(-relief=>'groove')->pack(-side=>'top');
    my $tpslc=$tpwin->Frame()->pack(-side=>'top');
# selection frame with listbox and buttons
#   label for listbox
    my $tplabel=$tpslc->Label(-text=>($srchtype ? "Files" : "Topics"),
			      -font=>$deffont)->pack(-anchor=>'w',
						     -side=>'top');
#   listbox with optional scrollbar
    my $tplist=$tpslc->Scrolled("Listbox",
				-font=>$deffont,
				-scrollbars=>'osoe',
#				-height=>0,
				-width=>0,
				-selectmode=>'single',
				-exportselection=>0,
				-cursor=>'hand2')->pack(-side=>'left',
							-fill=>'y',
							-expand=>1);
    $tplist->insert('end',@lbitems); # fill topics into listbox
#   buttons frame
    my $tpbframe=$tpslc->Frame(-borderwidth=>8);
    $tpbframe->pack(-side=>'right');
#   make buttons for command frame
    my $n_buttons = 0;
    $tlwins{$opt}{'buttons'}[$n_buttons++]=
	$tpbframe->Button(-text=>'View',
			  -font=>$deffont,%butcol,
			  -command=>sub{ &viewslc($docselect,$tpbframe);},
			  -width=>6)->pack(-side=>'top');
    if (! $IsWin32) {
      $tlwins{$opt}{'buttons'}[$n_buttons++]=
	$tpbframe->Button(-text=>'Print',
			  -font=>$deffont,%butcol,
			  -command=>sub{ &prtslc($docselect,$tpbframe);},
			  -width=>6)->pack(-side=>'top');
    }
    $tlwins{$opt}{'buttons'}[$n_buttons++]=
	$tpbframe->Button(-text=>'Cancel',
			  -font=>$deffont,%butcol,
			  -command=>sub{ destroy $tpwin; undef $tlwins{$opt};},
			  -width=>6)->pack(-side=>'bottom');
#   if only 1 item is in the list (most likely in search results), select it
    if (scalar @lbitems == 1) {
        $tplist->selectionSet(0);
        $dspselect=$lbitems[0];
        $docselect=$lbdocs[0];
	&viewslc($docselect,$tpbframe)
	  if ($autoview && $wtitle =~ /^Search results/);
    }
# display frame
    my $dsplabel=$tpdsp->Label(-text=>'Selection:',
			       -font=>$deffont)->pack(-anchor=>'w',,
							 -fill=>'x',
							 -side=>'left');
    my $dspslc=$tpdsp->Label(-textvariable=>\$dspselect,
			     -font=>$deffont,
			     -borderwidth=>2)->pack(-anchor=>'w',
						    -fill=>'x',
						    -side=>'right');
# handle selection; only one selection possible
    $tplist->bind('<Button-1>'=>sub{my ($slctind)=$tplist->curselection();
				    $dspselect=$lbitems[$slctind];
				    $docselect=$lbdocs[$slctind];});
#   key bindings
    $tplist->bind('<Double-Button-1>'=>sub{
	&viewslc($docselect,$tpbframe);});
    $tpwin->bind('<Control-v>'=>sub{
	&viewslc($docselect,$tpbframe);});
    $tpwin->bind('<Control-p>'=>sub{
	&prtslc($docselect,$tpbframe);});
    $tpwin->bind('<Control-c>'=>sub{destroy $tpwin; undef $tlwins{$opt};});
    $tplist->bind('<Button-3>'=>sub{
	if ($docselect) {
	    &showpath($docselect,$tpbframe);
	} else {
	    &popmsg(2,"No selection;\nuse left mouse button.",$tpbframe);
	}});
}

# view document selected in listbox
sub viewslc {
    my($slc,$parframe)=@_;
    my $viewer;
    my $itype=-1;
    my $browser=0;
    my $styflag=0;
    unless (defined $slc) {
	&popmsg(2,"No selection was made.",$parframe);
	return;
    }
    $parframe->configure(-cursor=>'watch');
    chomp $slc;
    my @dummy=split('\.',$slc);
  DOC_FORMAT: { # determine document type
      ($dummy[-1] =~ /dvi/) && do { $viewer=$dvi_viewer; last DOC_FORMAT; };
      ($dummy[-1] =~ /ps/) && do { $viewer=$ps_viewer; last DOC_FORMAT; };
      ($dummy[-1] =~ /pdf/) && do { $viewer=$pdf_viewer; last DOC_FORMAT; };
      ($dummy[-1] =~ /txt/ || $dummy[-1] =~ /faq$/) && do {
	  $viewer=($txt_viewer eq "TDK_OWN" ||
		   $txt_viewer =~ /xterm/) ?
		       $txt_viewer : "${CmdConsole} $txt_viewer";
	  last DOC_FORMAT;
      };
      ($dummy[-1] =~ /README/i || $dummy[-2] =~ /\/README$/i) && do {
	  $viewer=($txt_viewer eq "TDK_OWN" ||
		   $txt_viewer =~ /xterm/) ?
		       $txt_viewer : "${CmdConsole} $txt_viewer";
	  last DOC_FORMAT;
      };
      ($dummy[-1] =~ /htm/) && do {
	  $viewer=$html_viewer;
	  if ($html_viewer =~ /netscape/i) { # check if Netscape is open
	      my $lockfile="${HomeEnv}/.netscape/lock";
	      $browser=1 if (-e $lockfile || -l $lockfile);
	  } elsif ($html_viewer =~ /mozilla/i) { # check if Mozilla is open
	      my $mozcheck=
		`mozilla -remote 'openFile($texmfmain/doc/index.html)'`;
	      $browser=1 unless ($?);
	  }
	  last DOC_FORMAT;
      };
#     some packages have no normal documentation but useful info in the .sty
      ($dummy[-1] =~ /sty$/) && do {
	  $viewer=($txt_viewer eq "TDK_OWN" ||
		   $txt_viewer =~ /xterm/) ?
		       $txt_viewer : "${CmdConsole} $txt_viewer";
	  $slc="../tex/$slc"; # .sty files are in ${tex,local}docpath/../tex/
	  $styflag=1;
	  last DOC_FORMAT;
      };
      $parframe->configure(-cursor=>$defcursor);
      if ($xfmt_viewer != 1) {
	&popmsg(2,"$dummy[-1]: not a known document format",$parframe);
	return;
      }
  };
    if ($viewer eq "") {
      if ($xfmt_viewer == 1) {
	    &popmsg(-1,'Unknown format; assuming plain text.',$parframe);
	    $viewer=($txt_viewer eq "TDK_OWN" ||
		     $txt_viewer =~ /xterm/) ?
		       $txt_viewer : "${CmdConsole} $txt_viewer";
      } else {
	&popmsg(2,"$dummy[-1]: no viewer available/specified for this format",
		$parframe);
	$parframe->configure(-cursor=>$defcursor);
	return;
      }
    }
# build complete path and start viewer if file exists
    my $slcdoc;
    $slcdoc=&finddoc($slcdoc,$slc,$parframe);
    if ($slcdoc ne "") {
#       change to doc directory in case there are pictures
	my $docpath=substr($slcdoc,0,(rindex($slcdoc,'/',(length $slcdoc))));
	chomp $slcdoc;
	chdir ($docpath);
#       try to get doc out of .sty file
	if ($styflag) {
	    &popmsg(-1,'Trying to extract documentation out of .sty file;
might be cluttered with program comments.',
		    $parframe);
	    $slcdoc=&stripsty($slcdoc,substr($slc,7));
	}
#       for text files use built-in text viewer if set
	if ($IsWin32) {
	  if ($viewer eq "TDK_OWN") {
 	    &ShellExecute('open', $slcdoc);
 	    $parframe->configure(-cursor=>$defcursor);
 	    return;
	  }
	  else {
	    system("$viewer $slcdoc");
	  }
	}
	else {
	  if ($viewer eq "TDK_OWN") {
 	    &textview($slcdoc);
 	    $parframe->configure(-cursor=>$defcursor);
 	    return;
	  }
	  if ($quiet == 0) { # show messages
 	    if ($browser == 0) { # normal viewers or new Netscape/Mozilla
	      system("$viewer $slcdoc &");
 	    } else { # open doc in existing Netscape/Mozilla window
	      system("$viewer -remote 'openFile($slcdoc)' &");
	      &popmsg(-1,"Opening document in existing $viewer window.",
		      $parframe);
	    }
	  } else { # viewer messages written to /dev/null instead of terminal
 	    if ($browser == 0) { # normal viewers or new Netscape/Mozilla
	      system("perl -we 'use strict; \
my \$dump=\"\"; \
open(NULL,\">/dev/null\"); \
open (VIEWQ,\"$viewer $slcdoc |\"); \
while (\$dump=<VIEWQ>) { print NULL \$dump; } \
close(VIEWQ); \
close(NULL);' &");
	    } else { # open doc in existing window (Netscape or Mozilla)
	      # not quiet; I didn't get an inline perl call like in
	      # the other branch working
	      system("$viewer -remote 'openFile($slcdoc)' &");
	      &popmsg(-1,"Opening document in existing $viewer window.",
		      $parframe);
	    }
	  }
	}
      }
    $parframe->configure(-cursor=>$defcursor);
}

# Tk widget for text document
sub textview {
    my($txtfile)=@_;
#   see if toplevel window with this file is already there
    if (Exists($tlwins{$txtfile}{'addr'})) {
	$tlwins{$txtfile}{'addr'}->deiconify();
	$tlwins{$txtfile}{'addr'}->raise();
	return;
    }
    my $tfv_tk=$main->Toplevel(-title=>"Text file viewer");
    $tfv_tk->resizable(0,1);
    $tlwins{$txtfile}{'addr'}=$tfv_tk;
    $tfv_tk->Label(-text=>"File: $txtfile",
		   -font=>$deffont,
		   -relief=>'ridge',
		   -borderwidth=>3)->pack(-side=>'top',
					  -fill=>'x',
					  -ipady=>10,
					  -anchor=>'s');
    my $txtbody=$tfv_tk->Scrolled("Text",
				  -relief=>'flat',
				  -font=>$deffont,
				  -height=>20,
				  -width=>80,
				  -scrollbars=>"e")->pack(-side=>'top',
							  -fill=>'y',
							  -expand=>1);
    open(TXTFILE,"$txtfile");
    while (<TXTFILE>) {	$txtbody->insert('end',$_); }
    close(TXTFILE);
    $txtbody->configure(-state=>'disabled');
    $tlwins{$txtfile}{'buttons'}[0]=
	$tfv_tk->Button(-text=>'Close',
			-font=>$deffont,%butcol,
			-command=>sub{ destroy $tfv_tk;
				       undef $tlwins{$txtfile};
				   })->pack(-side=>'bottom',
					    -fill=>'x');
    $tfv_tk->bind('<Control-c>'=>sub{destroy $tfv_tk;
				     undef $tlwins{$txtfile};
				 });
}

# print document selected in listbox
sub prtslc {
    my($slc,$parframe)=@_;
    $tmpfno=&randname;
    my $pstmpfile="${TempDir}/texdoc_$tmpfno.ps";
    push @tmpfiles,$pstmpfile;
    my $status;
    chomp $slc;
    unless (defined $slc) {
	&popmsg(2,"No selection was made.",$parframe);
	return;
    }
    if ($print_cmd eq "") {
	&popmsg(2,"No printer specified.",$parframe);
	return;
    }
# build complete path and check existence of file
    my @dummy=split('\.',$slc);
    if ($dummy[-1] =~ /sty$/) {
	$slc="../tex/$slc"; # .sty files are in ${tex,local,home}docpath/../tex
    }
    my $slcdoc;
    $slcdoc=&finddoc($slcdoc,$slc,$parframe);
    return if ($slcdoc eq "");
#   change to doc directory in case there are pictures
    my $docpath=substr($slcdoc,0,(rindex($slcdoc,'/',(length $slcdoc))));
    chdir ($docpath);
  DOC_FORMAT: { # determine document type
#     convert dvi files to ps before printing
      ($dummy[-1] =~ /dvi/) && do {
	  if ($dvips_conv eq "") {
	      &popmsg(2,"No converter available for dvi->ps conversion.",
		      $parframe);
	      return;
	  }
	  &popmsg(-1,"Converting dvi to ps for printing and sending file to print...",$parframe);
	  $status=system("$dvips_conv $slcdoc -o $pstmpfile");
	  if ($status != 0) {
	      &popmsg(2,"Error: Conversion dvi->ps failed!",$parframe);
	  }
	  last DOC_FORMAT;
      };
#     PostScript is printed directly
      ($dummy[-1] =~ /ps/) && do {
	  &popmsg(-1,"Sending file to $print_cmd...",$parframe);
	  $pstmpfile=$slcdoc;
	  last DOC_FORMAT;
      };
#     convert pdf files to ps before printing
      ($dummy[-1] =~ /pdf/) && do {
	  if ($pdfps_conv eq "") {
	      &popmsg(2,"No converter available for pdf->ps conversion.",
		      $parframe);
	      return;
	  }
	  &popmsg(-1,"Converting pdf to ps for printing and sending file to print...",$parframe);
	  system("$pdfps_conv $slcdoc $pstmpfile");
	  last DOC_FORMAT;
      };
#     convert html files to ps before printing
      ($dummy[-1] =~ /htm/) && do {
	  if ($htmlps_conv eq "") {
	      &popmsg(2,"No converter available for html->ps conversion.",
		      $parframe);
	      return;
	  }
	  &popmsg(-1,"Converting html to ps for printing and sending file to print...",$parframe);
	  my $htmlps_redir_sign=($htmlps_redir == 1) ? ">" :"";
	  system("$htmlps_conv $slcdoc $htmlps_redir_sign $pstmpfile");
	  last DOC_FORMAT;
      };
#     convert txt files to ps before printing
      ($dummy[-1] =~ /txt/ || $dummy[-1] =~ /faq$/
       || $dummy[-1] =~ /README/i) && do {
	  if ($txtps_conv eq "") {
	      &popmsg(2,"No converter available for txt->ps conversion.",
		      $parframe);
	      return;
	  }
	  &popmsg(-1,"Converting txt to ps for printing and sending file to print...",$parframe);
	  my $txtps_redir_sign=($txtps_redir == 1) ? ">" :"";
	  system("$txtps_conv $slcdoc $txtps_redir_sign $pstmpfile");
	  last DOC_FORMAT;
      };
#     convert extracted comments from sty files to ps before printing
      ($dummy[-1] =~ /sty$/) && do {
	  if ($txtps_conv eq "") {
	      &popmsg(2,"No converter available for sty(txt)->ps conversion.",
		      $parframe);
	      return;
	  }
	  &popmsg(-1,"Converting sty(txt) to ps for printing and sending file to print...",$parframe);
#         try to get doc out of .sty file
	  &popmsg(-1,'Trying to extract documentation out of .sty file;
might be cluttered with program comments.',
		    $parframe);
	  $slcdoc=&stripsty($slcdoc,substr($slc,7));
	  my $txtps_redir_sign=($txtps_redir == 1) ? ">" :"";
	  system("$txtps_conv $slcdoc $txtps_redir_sign $pstmpfile");
	  last DOC_FORMAT;
      };
      &popmsg(2,"$dummy[-1]: cannot print document format",$parframe);
      return;
  };
    system("$print_cmd $pstmpfile");
}

# test existence and search a documentation which is not in specified place
sub finddoc {
    my($slcdoc,$slc,$parframe)=@_;
    my ($dummy,$status);
  SLCDOCFIND: foreach ($texdocpath,$distdocpath,$docdocpath) {
      $slcdoc="$_/$slc";
      if (-e $slcdoc) {
#       found where it should be
	return $slcdoc;
      } else {
#       see if the documentation file is in the local doc tree...
	my @srchslcdoc=($slcdoc);
	if ($localdocpath) {
	    push @srchslcdoc,"$localdocpath/$slc";
	    return $srchslcdoc[-1] if (-e $srchslcdoc[-1]);
	}
#       ... or the user's doc tree...
	if ($homedocpath) {
	    push @srchslcdoc,"$homedocpath/$slc";
	    return $srchslcdoc[-1] if (-e $srchslcdoc[-1]);
	}
#       ... otherwise proceed with searching
	$status=-1;
#       see if the documentation file is compressed
      CMPREND: foreach my $cmprtype ("gz","bz2","zip") {
	    foreach (@srchslcdoc) {
		$dummy="$_.$cmprtype";
		if (-e $dummy) { # yes, it is compressed
        	    $tmpfno=&randname;
		    my $rawname=basename($slc,"");
		    $slcdoc="${TempDir}/texdoc_$tmpfno$rawname";
		    push @tmpfiles,$slcdoc;
		    my ($dcmp,$dcmp_opt);
		  COMPRESS: { # determine compression type
		      ($cmprtype eq "gz") && do {
			  $dcmp="gzip";
			  $dcmp_opt="-cd";
			  last COMPRESS;
		      };
		      ($cmprtype eq "bz2") && do {
			  $dcmp="bzip2";
			  $dcmp_opt="-cd";
			  last COMPRESS;
		      };
		      ($cmprtype eq "zip") && do {
			  $dcmp="zip";
			  $dcmp_opt="-p";
			  last COMPRESS;
		      };
		  };
#                   try to decompress
		    $status=system("$dcmp $dcmp_opt $dummy > $slcdoc");
		    if ($status != 0) { # failure
			&popmsg(2,
				"$dcmp: Couldn't decompress file, cancelling.",
				$parframe);
			$slcdoc="";
			return $slcdoc;
		    }
#		    last CMPREND; does not work, what we need is:
		    return $slcdoc;
		}
	    }
	}
      }
    }
    if ($status != 0) {
      $parframe->configure(-cursor=>'watch');
#     try to find it elsewhere with kpsewhich
      my $rawname=basename($slc,"");
      $slcdoc=($slcdoc =~ /\.sty$/) ?
	`kpsewhich $rawname` :
	  `kpsewhich --format=${qq}TeX system documentation${qq} $rawname`;
      if ($slcdoc eq "") { # nothing found; cancel
	&popmsg(2,"$rawname not found, cancelling.",$parframe);
	$slcdoc="";
      }
      $parframe->configure(-cursor=>$defcursor);
    }
    return $slcdoc;
}

# determine existence and show paths of files (Button-3)
sub showpath {
    my ($docselect,$tpbframe)=@_;
    my $found=0;
    if ($docselect =~ /\.sty$/) {
    STYPATH: foreach my $fullpath ("$texmfmain/tex/$docselect",
				   "$texmfdist/tex/$docselect",
				   "$texmflocal/tex/$docselect",
				   "$texmfhome/tex/$docselect") {
	if (-e $fullpath) {
	  &popmsg(0,"Selected file:\n$fullpath",$tpbframe);
	  $found=1;
	  last STYPATH;
	}
      }
      if (!$found) {
	&popmsg(2,"Selected file:\n$docselect\ndoes not exist.",$tpbframe) }
    } else {
    DOCPATH: foreach my $fullpath ("$texdocpath/$docselect",
				   "$distdocpath/$docselect",
				   "$docdocpath/$docselect",
				   "$localdocpath/$docselect",
				   "$homedocpath/$docselect") {
	foreach ('','.gz','.bz2','.zip') {
	  my $fullpathext="$fullpath$_";
	  if (-e $fullpathext) {
	    &popmsg(0,"Selected file:\n$fullpathext",$tpbframe);
	    $found=1;
	    last DOCPATH;
	  }
	}
      }
      if (!$found) {
	&popmsg(2,"Selected file:\n$docselect\ndoes not exist (neither in normal form nor compressed).",$tpbframe) }
    }
}

# make or destroy search entry widget
sub mksrch {
    my ($type) = @_;
    if ($srchflag == 1) {
	if ($type == $srchtype) {
	    destroy $srchentry;
	    $srchflag=0;
	} else {
	    $srchtype = $type;
	}
    } else {
	$srchtype = $type;
#       get the search string
	$srchentry=$cmdframe->Entry(-cursor=>'xterm',
				    -font=>$deffont,
				    -width=>20,
				    -takefocus=>1)->pack(-side=>'left');
	$srchentry->focus();
	$srchentry->bind('<Return>'=>[\&srchstr, $srchentry]);
	$srchflag=1;
#       key binding
	$srchentry->bind('<Control-c>'=>sub{destroy $srchentry; $srchflag=0;});
	$srchentry->bind('<Escape>'=>sub{destroy $srchentry; $srchflag=0;});
    }
}

# call search routine and display results
sub srchstr {
    my $string;
    my @reslist;
    $string=$srchentry->get();
# search
    $main->Busy(-recurse => 1);
    if ($srchtype == 0) {
	@reslist = &srchdb($string);
    } else {
	@reslist = &srchtd($string);
    }
    $main->Unbusy();
# destroy entry widget and show results
    destroy $srchentry;
    $srchflag=0;
    if (scalar @reslist == 0) {
	&popmsg(0, ($srchtype == 0 ? 'Database' : 'File').
	    " search for $string: no matches found.\n".
	    "You may want to try a ".
	    ($srchtype == 0 ? 'File' : 'Databse') . " search.\n\n".
	    "If nothing else works, CTAN offers an online search form:\n".
	    "http://ctan.org/search.html\n",
	    $cmdframe);
    } else {
	&tpslct(-1, $string, @reslist);
    }
}

# search using texdoc
sub srchtd {
    my ($string) = @_;
    my @res;
    my $tdout = `texdoc -M -l $string`;
    if ($?) {
	&popmsg(0,"texdoc failed, sorry: $!\n".
	    "Please report this problem on the texlive mailing list.\n");
	return;
    } else {
	my $i = 0;
	for my $line (split /\n/, $tdout) {
	    my @fields = split /\t/, $line;
	    $doc[$ncat][$i] = $fields[2];
	    $topic[$ncat][$i] = basename($fields[2]);
	    $packname[$ncat][$i] = "no package name";
	    push @res, ($ncat, $i++);
	}
	return @res;
    }
}

# search a string in @packname, @topic and @keywords
sub srchdb {
    my ($string) = @_;
    my (@results, @reslist);
    if ($string) {
	for ($i=0; $i<$ncat; ++$i) {
	    for ($j=0; $j<$maxind[$i]+1; ++$j) {
	      SRCH: {
		  ($topic[$i][$j] =~ /$string/i) && do {
		      push @results,($i,$j);
		      last SRCH;
		  };
		  (defined $keywords[$i][$j]) && do {
		      if ($keywords[$i][$j] =~ /$string/i) {
			  push @results,($i,$j);
			  last SRCH;
		      }
		  };
		  ($packname[$i][$j] =~ /$string/i) && do {
		      push @results,($i,$j);
		      last SRCH;
		  };
	      };
	    }
	}
    } else { # return full database
	$string='all database entries';
	for ($i=0; $i<$ncat; ++$i) {
	    for ($j=0; $j<$maxind[$i]+1; ++$j) { push @results,($i,$j); }
	}
    }
    unless (scalar @results == 0) {
#       cancel multiple entries
	my ($omit,$pack1,$pack2);
	@reslist=($results[0],$results[1]);
	for ($i=0; $i<$#results; $i+=2) {
	    $omit=0;
	    $pack1=$packname[$results[$i]][$results[$i+1]];
	    for ($j=0; $j<$#reslist; $j+=2) {
		$pack2=$packname[$reslist[$j]][$reslist[$j+1]];
		if ($pack1 eq $pack2) { $omit=1; last; }
	    }
	    if ($omit == 0) { push @reslist,($results[$i],$results[$i+1]); }
	}
    }
    return @reslist;
}

# extract documentation of .sty files; a flag in the @keywords array shows
# where the doc is located:
# -0-: no specific place; -1-: at end, behind \endinput; -2-: at beginning,
# terminated by %%%%%%; -3-: at beginning, terminated with blank line
sub stripsty {
    my ($slcdoc,$slc)=@_;
    my @stydoc;
    $tmpfno=&randname;
    my $tmpslcdoc="${TempDir}/texdoc_$tmpfno.txt";
    push @tmpfiles,$tmpslcdoc;
    open (STY,"$slcdoc");
  LOC: {
      ($stydoc{$slc} == 0) && do { # no specific place, suck in everything :-(
	  while ($line = <STY>) { push @stydoc,$line if ($line =~ /^%/); }
	  last LOC;
      };
      ($stydoc{$slc} == 1) && do { # at end, behind \endinput
	  while ($line = <STY>) {
	      last if ($line =~ /^\s*\\endinput/ && !($line =~ /^%/));
	  }
	  while ($line = <STY>) { push @stydoc,$line; }
	  last LOC;
      };
      ($stydoc{$slc} == 2) && do { # up to a certain %%%%%%%, hopefully the 1st
	  for (my $i=0; $i<9; ++$i) { $stydoc[$i]=<STY>; } # after the 8th line
	  while ($line = <STY>) {
	      last if ($line =~ /^%{4,}/);
	      push @stydoc,$line;
	  }
	  last LOC;
      };
      ($stydoc{$slc} == 3) && do { # up to 1st blank/apparently empty line
	  while ($line = <STY>) {
	      last if ($line =~ /^\s*$/);
	      push @stydoc,$line;
	  }
	  last LOC;
      };
  };
    close(STY);
    open(TMPSTY,">$tmpslcdoc");
    foreach (@stydoc) { print TMPSTY $_; }
    close(TMPSTY);
    return $tmpslcdoc;
}

# pop up a note or an error or warning message toplevel window
sub popmsg {
    my($level,$msg,$parframe)=@_;
    my $degree;
  LEVEL: {
      ($level == -1) && do { $degree="Info"; last LEVEL; };
      ($level == 0) && do { $degree="Note"; last LEVEL; };
      ($level == 1) && do {
	return if ($quiet == 1);
	$degree="Warning";
	last LEVEL
      };
      ($level == 2) && do { $degree="Error"; last LEVEL; };
  };
    my $msgwin=$parframe->Toplevel(-title=>$degree);
    my $lbl=$msgwin->Label(-text=>uc($degree),
			   -font=>$deffont)->pack(-side=>'top', -fill=>'x');
#   get size of message text
    my @dummy=split("\n",$msg);
    my $nline=scalar @dummy;
    my $msgwidth=0;
    my $lline;
    foreach (@dummy) {
	$lline=length $_;
	if ($lline > $msgwidth) { $msgwidth=$lline; }
    }
    my $message=$msgwin->Text(-relief=>'flat',
			      -font=>$deffont,
			      -height=>$nline,
			      -width=>$msgwidth)->pack(-side=>'top');
    $message->insert('end',$msg);
    $message->configure(-state=>'disabled');
    if ($level < 0) { # transient message window
	$msgwin->after(3000,sub{destroy $msgwin});
    } else { # persistent message window
	$msgwin->Button(-text=>'Close',
			-font=>$deffont,%butcol,
			-command=>sub{destroy $msgwin})->pack(-side=>'bottom',
							      -fill=>'x');
	$msgwin->bind('<Control-c>'=>sub{destroy $msgwin});
    }
}

# show/change settings (don't show this to an expert :-/ )
sub settings {
    my ($homedocpath_tmp,$quiet_tmp,$autoview_tmp,$xfmt_viewer_tmp,
	$dvi_viewer_tmp,$dvips_conv_tmp,
	$ps_viewer_tmp,$pdf_viewer_tmp,$pdfps_conv_tmp,
	$html_viewer_tmp,$htmlps_conv_tmp,$htmlps_redir_tmp,
	$txt_viewer_tmp,$txtps_conv_tmp,$txtps_redir_tmp,$print_cmd_tmp)=
	    ($homedocpath,$quiet,$autoview,$xfmt_viewer,
	     $dvi_viewer,$dvips_conv,$ps_viewer,
	     $pdf_viewer,$pdfps_conv,$html_viewer,
	     $htmlps_conv,$htmlps_redir,$txt_viewer,
	     $txtps_conv,$txtps_redir,$print_cmd);
    my @tdcolors_tmp=@tdcolors;
#   see if toplevel window is already there
    if (Exists($tlwins{"setmenu"}{'addr'})) {
	$tlwins{"setmenu"}{'addr'}->deiconify();
	$tlwins{"setmenu"}{'addr'}->raise();
	return;
    }
    my $setmenu=$main->Toplevel(-title=>'Settings');
    $setmenu->resizable(0,0);
    $tlwins{"setmenu"}{'addr'}=$setmenu;
# documentation path
    my $datlabel;
    $datlabel="Database file(s) used: teTeX";
    $datlabel.=", local database" if (-e $locdatabase);
    $datlabel.=", user database" if (-e $homedatabase);
    my $docframe=$setmenu->Frame(-relief=>'groove',
				 -borderwidth=>2)->pack(-side=>'top',
							-fill=>'x',
							-expand=>1);
    my $docframestring="\nDistribution documentation root path(s):\n$texdocpath";
    $docframestring.=", $distdocpath" if (-e $distdocpath);
    $docframestring.=", $docdocpath" if (length $docdocpath);
    if ($localdocpath && $localdocpath ne $texdocpath) {
	$docframestring.="\nLocal documentation root path: $localdocpath\n";
    }
    $docframe->Label(-text=>"$docframestring",
		     -font=>$deffont)->pack(-side=>'top',
					    -anchor=>'w',
					    -fill=>'x',
					    -expand=>1);
    if (defined $texmfhome && -d $texmfhome) {
      $homedocpath_tmp=basename($homedocpath_tmp,"");
      my $usrdocframe=$docframe->Frame(-relief=>'flat')->pack(-side=>'top',
							      -fill=>'x');
      my $usrdoclabel=$usrdocframe->Label(-text=>"User's documentation root path: $texmfhome/",
					  -font=>$deffont)->pack(-side=>'left');
      my $usrdocentry=$usrdocframe->Entry(-textvariable=>\$homedocpath_tmp,
					  -font=>$deffont)->pack(-side=>'left');
    }
    $docframe->Label(-text=>"$datlabel.",
		     -font=>$deffont)->pack(-side=>'top',
					    -anchor=>'w',
					    -ipady=>2,
					    -fill=>'x',
					    -expand=>1);
# General viewer behaviour
    my $genvframe=$setmenu->Frame(-label=>'General viewer behaviour',
				  -relief=>'groove',
				  -borderwidth=>2)->pack(-side=>'top',
							 -fill=>'x',
							 -ipady=>6,
							 -expand=>1);
    my $genvframe1=$genvframe->Frame()->pack(-side=>'left');
    my $genvsub1=$genvframe1->Frame()->pack(-side=>'top',
					    -anchor=>'w',
					    -ipady=>3);
    my $genvecbut=$genvsub1->
	Checkbutton(-variable=>\$quiet_tmp)->pack(-side=>'left',
						  -anchor=>'sw');
    my $genvelabel=$genvsub1->Label(-text=>'Suppress error messages',
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'sw');
    my $genvsub2=$genvframe1->Frame()->pack(-side=>'top',
					    -anchor=>'w');
    my $genvscbut=$genvsub2->
	Checkbutton(-variable=>\$autoview_tmp)->pack(-side=>'left',
						     -anchor=>'sw');
    my $genvslabel=$genvsub2->Label(-text=>'Autostart viewer for one-item listboxes',
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'sw');
    my $genvsub3=$genvframe1->Frame()->pack(-side=>'top',
					    -anchor=>'w');
    my $genvxcbut=$genvsub3->
	Checkbutton(-variable=>\$xfmt_viewer_tmp)->pack(-side=>'left',
							-anchor=>'sw');
    my $genvxlabel=$genvsub3->Label(-text=>'Use text viewer for unknown file format',
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'sw');
#   colours of GUI
    my @tdcolors_d=@tdcolors_tmp;
    $tlwins{'setmenu'}{'buttons'}[0]=
	$genvframe->Button(-text=>'Change viewer colours',
			   -command=>sub {
			       @tdcolors_tmp=&popcolor($setmenu,@tdcolors_tmp);
			       @tdcolors_d=@tdcolors_tmp;
			   },
			   -font=>$deffont,%butcol)->pack(-side=>'right');
# DVI
    my $dviframe=$setmenu->Frame(-label=>'DVI',
				 -relief=>'groove',
				 -borderwidth=>2)->pack(-side=>'top',
							-fill=>'x',
							-ipady=>6,
							-expand=>1);
    my $dvisub1=$dviframe->Frame()->pack(-side=>'top',
					 -anchor=>'w');
    my $dvisub2=$dviframe->Frame()->pack(-side=>'top',
					 -anchor=>'w',
					 -ipady=>6);
    my $dvivlabel=$dvisub1->Label(-text=>'Viewer command',
				  -anchor=>'w',
				  -font=>$deffont)->pack(-side=>'left');
    my $dviventry=$dvisub1->Entry(-textvariable=>\$dvi_viewer_tmp,
				  -font=>$deffont)->pack(-side=>'left');
    my $dviclabel=$dvisub2->Label(-text=>'DVI->PS converter command',
				  -anchor=>'w',
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
    my $dvilbwidth=length 'DVI->PS converter command';
    $dvivlabel->configure(-width=>1.1*$dvilbwidth);
    $dviclabel->configure(-width=>1.1*$dvilbwidth);
    my $dvicentry=$dvisub2->Entry(-textvariable=>\$dvips_conv_tmp,
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
# PostScript
    my $psframe=$setmenu->Frame(-label=>'PostScript',
				-relief=>'groove',
				-borderwidth=>2)->pack(-side=>'top',
						       -fill=>'x',
						       -ipady=>10,
						       -expand=>1);
    my $psvlabel=$psframe->Label(-text=>'Viewer command',
				 -font=>$deffont)->pack(-side=>'left',
							-anchor=>'w');
    my $psventry=$psframe->Entry(-textvariable=>\$ps_viewer_tmp,
				 -font=>$deffont)->pack(-side=>'left');
# PDF
    my $pdfframe=$setmenu->Frame(-label=>'PDF',
				 -relief=>'groove',
				 -borderwidth=>2)->pack(-side=>'top',
							-fill=>'x',
							-ipady=>6,
							-expand=>1);
    my $pdfsub1=$pdfframe->Frame()->pack(-side=>'top',
					 -anchor=>'w');
    my $pdfsub2=$pdfframe->Frame()->pack(-side=>'top',
					 -anchor=>'w',
					 -ipady=>6);
    my $pdfvlabel=$pdfsub1->Label(-text=>'Viewer command',
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'w');
    my $pdfventry=$pdfsub1->Entry(-textvariable=>\$pdf_viewer_tmp,
				  -font=>$deffont)->pack(-side=>'left');
    my $pdfclabel=$pdfsub2->Label(-text=>'PDF->PS converter command',
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
    my $pdfcentry=$pdfsub2->Entry(-textvariable=>\$pdfps_conv_tmp,
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
# HTML
    my $htmlframe=$setmenu->Frame(-label=>'HTML',
				  -relief=>'groove',
				  -borderwidth=>2)->pack(-side=>'top',
							 -fill=>'x',
							 -ipady=>6,
							 -expand=>1);
    my $htmlsub1=$htmlframe->Frame()->pack(-side=>'top',
					   -anchor=>'w');
    my $htmlsub2=$htmlframe->Frame()->pack(-side=>'top',
					   -anchor=>'w',
					   -ipady=>6);
    my $htmlvlabel=$htmlsub1->Label(-text=>'Viewer command',
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'w');
    my $htmlventry=$htmlsub1->Entry(-textvariable=>\$html_viewer_tmp,
				    -font=>$deffont)->pack(-side=>'left');
    my $htmlclabel=$htmlsub2->Label(-text=>'HTML->PS converter command',
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'sw');
    my $htmlcentry=$htmlsub2->Entry(-textvariable=>\$htmlps_conv_tmp,
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'sw');
    my $htmlrlabel=$htmlsub2->Label(-text=>'Output redirect needed',
				    -font=>$deffont)->pack(-side=>'left',
							   -anchor=>'sw');
    my $htmlrcbut=$htmlsub2->
	Checkbutton(-variable=>\$htmlps_redir_tmp)->pack(-side=>'left',
							 -anchor=>'sw');
# plain text
    my $txtframe=$setmenu->Frame(-label=>'Plain text',
				 -relief=>'groove',
				 -borderwidth=>2)->pack(-side=>'top',
							-fill=>'x',
							-ipady=>6,
							-expand=>1);
    my $txtsub1=$txtframe->Frame()->pack(-side=>'top',
					 -anchor=>'w');
    my $txtsub2=$txtframe->Frame()->pack(-side=>'top',
					 -anchor=>'w',
					 -ipady=>6);
    my $txtvlabel=$txtsub1->Label(-text=>'Viewer command',
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'w');
    my $txtventry=$txtsub1->Entry(-textvariable=>\$txt_viewer_tmp,
				  -font=>$deffont)->pack(-side=>'left');
    my $txt_view_flag;
    if ($txt_viewer_tmp eq "TDK_OWN") {
	$txt_view_flag=1;
	$txtventry->configure(-state=>'disabled');
    } else {
	$txt_view_flag=0;
	$txtventry->configure(-state=>'normal');
    }
    my $txtvilabel=$txtsub1->Label(-text=>"\tUse texdoctk\'s own viewer",
				   -font=>$deffont)->pack(-side=>'left',
							  -anchor=>'sw');
    my $txtvcbut=$txtsub1->
	Checkbutton(-variable=>\$txt_view_flag,
		    -command=>sub {
			if ($txt_view_flag == 1) {
			    $txt_viewer_tmp="TDK_OWN";
			    $txtventry->configure(-state=>'disabled');
			} else {
			    $txt_viewer_tmp=$txt_viewer;
			    $txtventry->configure(-state=>'normal');
			    $txtventry->focus();
			}
		    })->pack(-side=>'left',
			     -anchor=>'sw');
    my $txtclabel=$txtsub2->Label(-text=>'Text->PS converter command',
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
    my $txtcentry=$txtsub2->Entry(-textvariable=>\$txtps_conv_tmp,
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
    my $txtrlabel=$txtsub2->Label(-text=>'Output redirect needed',
				  -font=>$deffont)->pack(-side=>'left',
							 -anchor=>'sw');
    my $txtrcbut=$txtsub2->
	Checkbutton(-variable=>\$txtps_redir_tmp)->pack(-side=>'left',
							-anchor=>'sw');
# printer
    my $prtframe=$setmenu->Frame(-label=>'Printer',
				 -relief=>'groove',
				 -borderwidth=>2)->pack(-side=>'top',
							-fill=>'x',
							-ipady=>10,
							-expand=>1);
    my $prtclabel=$prtframe->Label(-text=>'Command',
				   -font=>$deffont)->pack(-side=>'left',
							  -anchor=>'w');
    my $prtcentry=$prtframe->Entry(-textvariable=>\$print_cmd_tmp,
				   -font=>$deffont)->pack(-side=>'left');
# buttons and key bindings
    my $setbfr=$setmenu->Frame()->pack(-side=>'bottom');
    $tlwins{'setmenu'}{'buttons'}[1]=
	$setbfr->Button(-text=>'OK',
			-font=>$deffont,%butcol,
			-command=>sub{
			    $homedocpath="$texmfhome/$homedocpath_tmp";
			    ($quiet,$autoview,$xfmt_viewer,
			     $dvi_viewer,$dvips_conv,
			     $ps_viewer,$pdf_viewer,
			     $pdfps_conv,$html_viewer,
			     $htmlps_conv,$htmlps_redir,
			     $txt_viewer,$txtps_conv,$txtps_redir,$print_cmd)=
				 ($quiet_tmp,$autoview_tmp,$xfmt_viewer_tmp,
				  $dvi_viewer_tmp,$dvips_conv_tmp,
				  $ps_viewer_tmp,
				  $pdf_viewer_tmp,$pdfps_conv_tmp,
				  $html_viewer_tmp,
				  $htmlps_conv_tmp,$htmlps_redir_tmp,
				  $txt_viewer_tmp,$txtps_conv_tmp,
				  $txtps_redir_tmp,$print_cmd_tmp);
			    @tdcolors=@tdcolors_tmp;
			    %butcol=('-background'=>$tdcolors[0],
				     '-foreground'=>$tdcolors[1],
				     '-activebackground'=>$tdcolors[2],
				     '-activeforeground'=>$tdcolors[3]);
			    foreach my $tlkey (keys %tlwins) {
				foreach (@{$tlwins{$tlkey}{'buttons'}}) {
				    $_->configure(%butcol);
				}
			    }
			    if ($txt_viewer eq "" &&
				defined $ENV{PAGER}) {
				$txt_viewer=$ENV{PAGER};
			    }
			    destroy $setmenu},
			-width=>6)->pack(-side=>'left',
					 -padx=>10,
					 -pady=>10);
    $tlwins{'setmenu'}{'buttons'}[2]=
	$setbfr->Button(-text=>'Save',
			-font=>$deffont,%butcol,
			-command=>sub{
			    $setmenu->configure(-cursor=>'watch');
			    &popmsg(-1,"Writing settings to $myrc",$setmenu);
			    my $tdcolors_tmp=join(' ',@tdcolors_tmp);
			    &writerc($homedocpath_tmp,
				     $quiet_tmp,$autoview_tmp,$xfmt_viewer_tmp,
				     $tdcolors_tmp,
				     $dvi_viewer_tmp,$dvips_conv_tmp,
				     $ps_viewer_tmp,
				     $pdf_viewer_tmp,$pdfps_conv_tmp,
				     $html_viewer_tmp,
				     $htmlps_conv_tmp,$htmlps_redir_tmp,
				     $txt_viewer_tmp,$txtps_conv_tmp,
				     $txtps_redir_tmp,$print_cmd_tmp,$setmenu);
			    $setmenu->configure(-cursor=>$defcursor);
			})->pack(-side=>'left',
				 -padx=>10);
    $tlwins{'setmenu'}{'buttons'}[3]=
	$setbfr->Button(-text=>'Cancel',
			-font=>$deffont,%butcol,
			-command=>sub{destroy $setmenu;
				      undef $tlwins{"setmenu"};
				  })->pack(-side=>'left',
					   -padx=>10);
    $setmenu->bind('<Control-c>'=>sub{destroy $setmenu;
				      undef $tlwins{"setmenu"};});
}

# dialog toplevel for color settings
sub popcolor {
    my ($parframe,@entryvar)=@_;
#   see if toplevel window is already there
    if (Exists($tlwins{'chngcol'}{'addr'})) {
	$tlwins{'chngcol'}{'addr'}->deiconify();
	$tlwins{'chngcol'}{'addr'}->raise();
	return;
    }
    my $dlgwin=$parframe->Toplevel(-title=>'Change colours');
    $dlgwin->resizable(0,0);
    $tlwins{'chngcol'}{'addr'}=$dlgwin;
#   entry widgets for default/active back/foreground
    $dlgwin->Label(-text=>'Enter colours as names or RGB #xxxxxx values',
		   -font=>$deffont)->pack(-side=>'top');
    my $dlgdef=$dlgwin->Frame()->pack(-side=>'top');
    my $dlgact=$dlgwin->Frame()->pack(-side=>'top');
    my $dlgdef1=$dlgdef->Frame()->pack(-side=>'left');
    my $dlgtfr0=$dlgdef1->Frame()->pack(-side=>'top');
    my $dlgtfr1=$dlgdef1->Frame()->pack(-side=>'top');
    my $dlgact1=$dlgact->Frame()->pack(-side=>'left');
    my $dlgtfr2=$dlgact1->Frame()->pack(-side=>'top');
    my $dlgtfr3=$dlgact1->Frame()->pack(-side=>'top');
    $dlgtfr0->Label(-text=>"Default background",
		    -anchor=>'w',
		    -width=>18,
		    -font=>$deffont)->pack(-side=>'left');
    $dlgtfr0->Entry(-textvariable=>\$entryvar[0],
		    -font=>$deffont)->pack(-side=>'left');
    $dlgtfr1->Label(-text=>"Default foreground",
		    -anchor=>'w',
		    -width=>18,
		    -font=>$deffont)->pack(-side=>'left');
    $dlgtfr1->Entry(-textvariable=>\$entryvar[1],
		    -font=>$deffont)->pack(-side=>'left');
    my $deflabel=$dlgdef->Label(-text=>'Default',
				-relief=>'ridge',
				-width=>10,
				-height=>2)->pack(-side=>'right',
						  -padx=>7);
    $dlgtfr2->Label(-text=>"Active background",
		    -anchor=>'w',
		    -width=>18,
		    -font=>$deffont)->pack(-side=>'left');
    $dlgtfr2->Entry(-textvariable=>\$entryvar[2],
		    -font=>$deffont)->pack(-side=>'left');
    $dlgtfr3->Label(-text=>"Active foreground",
		    -anchor=>'w',
		    -width=>18,
		    -font=>$deffont)->pack(-side=>'left');
    $dlgtfr3->Entry(-textvariable=>\$entryvar[3],
		    -font=>$deffont)->pack(-side=>'left');
    my $actlabel=$dlgact->Label(-text=>'Active',
				-background=>$entryvar[2],
				-foreground=>$entryvar[3],
				-relief=>'ridge',
				-width=>10,
				-height=>2)->pack(-side=>'right',
						  -padx=>7);
#   buttons
    my $setbfr=$dlgwin->Frame()->pack(-side=>'bottom');
    my $chngflag=0;
    $tlwins{'chngcol'}{'buttons'}[0]=
	$setbfr->Button(-text=>'OK',
			-font=>$deffont,%butcol,
			-command=>sub {
			    foreach (@entryvar) { # RGB form with leading #
				$_="#$_" if (/^[A-Fa-f0-9]{6}$/);
			    }
			    $chngflag=1 if (&colcheck($dlgwin,@entryvar));
			},
			-width=>6)->pack(-side=>'left',
					 -padx=>5,
					 -pady=>10);
    $tlwins{'chngcol'}{'buttons'}[1]=
	$setbfr->Button(-text=>'Preview',
			-font=>$deffont,%butcol,
			-command=>sub {
			    foreach (@entryvar) { # RGB form with leading #
				$_="#$_" if (/^[A-Fa-f0-9]{6}$/);
			    }
			    if (&colcheck($dlgwin,@entryvar)) {
				$deflabel->
				    configure(-background=>$entryvar[0],
					      -foreground=>$entryvar[1]);
				$actlabel->
				    configure(-background=>$entryvar[2],
					      -foreground=>$entryvar[3]);
			    }
			},
			-width=>6)->pack(-side=>'left',
					 -padx=>5,
					 -pady=>10);
    $tlwins{'chngcol'}{'buttons'}[2]=
	$setbfr->Button(-text=>'Cancel',
			-font=>$deffont,%butcol,
			-command=>sub{destroy $dlgwin;
				      undef $tlwins{'chngcol'};
				  })->pack(-side=>'right',
					   -padx=>5,
					   -pady=>10);
    $dlgwin->bind('<Control-c>'=>sub{destroy $dlgwin;
				     undef $tlwins{'chngcol'}; });
    $dlgwin->waitVariable(\$chngflag);
    destroy $dlgwin;
    undef $tlwins{'chngcol'};
    return @entryvar;
}

# check colours set in popcolor; relies on the existence of X11's showrgb
sub colcheck {
    my ($dlgwin,@entryvar)=@_;
    my $ecnt=0;
    foreach (@entryvar) { # all colours defined?
	last unless ($_);
	++$ecnt;
    }
    &popmsg(2,'Some colours undefined.',$dlgwin) if ($ecnt != 4);
    my $defcol=0;
    if (`which showrgb`) { # test for proper name
	my @rgb=`showrgb`;
	my @norgb;
	foreach my $colour (@entryvar) {
	    if ($colour =~ /^#[A-Fa-f0-9]{6}$/ || grep /\b$colour\b/i,@rgb) {
		++$defcol;
	    } else {
		push @norgb,$colour;
	    }
	}
	if ($defcol != 4) {
	    my $norgb=join("\n",@norgb);
	    &popmsg(2,"Some colours invalid:\n$norgb",$dlgwin);
	}
    } else { $defcol=4; }
    return ($ecnt == 4 && $defcol == 4) ? 1 : 0;
}

# write user's ~/.texdocrc
sub writerc {
    use File::Copy qw/cp mv/;
    my ($homedocpath_tmp,$quiet_tmp,$autoview_tmp,$xfmt_viewer_tmp,
	$tdcolors_tmp,
	$dvi_viewer_tmp,$dvips_conv_tmp,
	$ps_viewer_tmp,
	$pdf_viewer_tmp,$pdfps_conv_tmp,
	$html_viewer_tmp,
	$htmlps_conv_tmp,$htmlps_redir_tmp,
	$txt_viewer_tmp,$txtps_conv_tmp,
	$txtps_redir_tmp,$print_cmd_tmp,$parframe)=@_;
    my $err=0;
    mv $myrc,"$myrc.save" if (-e $myrc);
    open(MYRC,">$myrc");
    print MYRC "# user's .texdocrc, generated by program\n
# root of user's doc directory (from \$HOMETEXMF)
# can be left empty if \$TEXMFHOME is undefined
HOMEDOCPATH=$homedocpath_tmp
# general viewer behaviour (y or yes to make active)
QUIET=$quiet_tmp
AUTOVIEW=$autoview_tmp
XFMT_VIEWER=$xfmt_viewer_tmp
# button colours: default and active back-/foreground
# names and RGB values are both allowed; use the form #xxxxxx for RGB
TDCOLORS=$tdcolors_tmp
# the following variables contain whole commands, i.e. options can be given
# DVI settings
DVI_VIEWER=$dvi_viewer_tmp
DVIPS_CONV=$dvips_conv_tmp
# PostScript handling
PS_VIEWER=$ps_viewer_tmp
# PDF handling
PDF_VIEWER=$pdf_viewer_tmp
PDFPS_CONV=$pdfps_conv_tmp
# HTML handling
HTML_VIEWER=$html_viewer_tmp
HTMLPS_CONV=$htmlps_conv_tmp
# HTMLPS_REDIR must be on, if the html->ps converter normally writes its output
# to stdout instead of a file (e.g. as html2ps) (y or yes to make active)
HTMLPS_REDIR=$htmlps_redir_tmp
# Plain text handling
# TDK_OWN is texdoctk's own text file viewer
TXT_VIEWER=$txt_viewer_tmp
TXTPS_CONV=$txtps_conv_tmp
# TXTPS_REDIR must be on, if the txt->ps converter normally writes its output
# to stdout instead of a file (e.g. as a2ps) (y or yes to make active)
TXTPS_REDIR=$txtps_redir_tmp
# printer
PRINT_CMD=$print_cmd_tmp
\n# end of config\n";
    close(MYRC);
}

# help window (toplevel)
sub helptext {
#   see if toplevel window is already there
    if (Exists($tlwins{"helptext"}{'addr'})) {
	$tlwins{"helptext"}{'addr'}->deiconify();
	$tlwins{"helptext"}{'addr'}->raise();
	return;
    }
    my $help=$main->Toplevel(-title=>'Help/About');
    $help->resizable(0,1);
    $tlwins{'helptext'}{'addr'}=$help;
    $help->Label(-text=>"texdoctk
TeX documentation browser
v$version ($date)",
		 -font=>$deffont)->pack(-side=>'top',
					-ipady=>10,
					-anchor=>'s');
    my $helpbody=$help->Scrolled("Text",
				 -relief=>'flat',
				 -font=>$deffont,
				 -height=>20,
				 -width=>60,
				 -scrollbars=>"e")->pack(-side=>'top',
							 -fill=>'y',
							 -expand=>1);
    my $pos=tell DATA;
    while (<DATA>) {
	$_ =~ s/\$sysrc/$sysrc/;
	last if (/^#/);
	$helpbody->insert('end',$_);
    }
    seek(DATA,$pos,0);
    $helpbody->configure(-state=>'disabled');
    $tlwins{'helptext'}{'buttons'}[0]=
	$help->Button(-text=>'Close',
		      -font=>$deffont,%butcol,
		      -command=>sub{destroy $help;
				    undef $tlwins{"helptext"};
				})->pack(-side=>'bottom',
					 -fill=>'x');
    $help->bind('<Control-c>'=>sub{destroy $help; undef $tlwins{"helptext"};});
}

# read config file; personal settings override the default settings
sub readrc {
    my($rcfile,$icall)=@_;
    my ($var,$val);
    my $pathflag=0;
    my $pathflagl=0;
    my $pathflagh=0;
    my $tdcolors='';
#   for backward compatibility of converter/printer options
    my ($dvips_opts,$pdfps_opts,$print_opts);
#   read defaults resp. local/personal settings
  CALL: {
      ($icall == 1) && do {
	open (RC,"$rcfile") ||
	  &fatalmsg("Couldn't open system-wide default config file ${TeXDocRcDefaults}.\n");
	last CALL;
      };
      ($icall == 2) && do {
	open (RC,"$rcfile") ||
	  &fatalmsg("Couldn't open personal config file ~/.texdocrc.\n");
	last CALL;
      };
    };
    while ($line = <RC>) {
	next if ($line =~ /^($|#)/);
	chomp $line;
	($var,$val)=split("=",$line,2);
      RCVARS: {
	  ($var =~ /texdocpath/i) && do { # TEXDOCPATH
	    if (length $val > 0) {
	      $pathflag=1;
	      eval(join("",("\$",lc($var),"=\$val")));
	    }
	    last RCVARS;
	  };
	  ($var =~ /localdocpath/i) && do { # LOCALDOCPATH
	    if (length $val > 0) {
	      $pathflagl=1;
	      eval(join("",("\$",lc($var),"=\$val")));
	    }
	    last RCVARS;
	  };
	  ($var =~ /homedocpath/i) && do { # HOMEDOCPATH
	    if ($icall == 2 && length $val > 0) { # set only in ~/.texdocrc
	      $pathflagh=1;
	      eval(join("",("\$",lc($var),"=\$val")));
	    }
	    last RCVARS;
	  };
	  # other variables
	  eval(join("",("\$",lc($var),"=\$val")));
	};
    }
    close(RC);
#   find texmf/doc, and possibly other distribution texmf trees
    if ($rcfile eq $sysrc || $pathflag == 1) {
	$texmfmain=`kpsewhich --expand-path=${qq}\$TEXMFMAIN${qq}`;
	chomp $texmfmain;
	$texdocpath="$texmfmain/$texdocpath";
	$texmfdist=`kpsewhich --expand-path=${qq}\$TEXMFDIST${qq}`;
	chomp $texmfdist;
	$distdocpath=join('/',"$texmfdist",basename($texdocpath,""))
			  if (length $texmfdist);
	# TeXLive has this texmf-doc
	$texmfdoc=join('/',"$texmfmain-doc",basename($texdocpath,""));
	if (-e $texmfdoc) {
	  chomp $texmfdoc;
	  $docdocpath="$texmfdoc/$docdocpath";
	}
    }
#   find texmf-local/doc, if there is one
    if ($rcfile eq $sysrc || $pathflagl == 1) {
	$texmflocal=`kpsewhich --expand-path=${qq}\$TEXMFLOCAL${qq}`;
	chomp $texmflocal;
	if (length $texmflocal) {
	  # if LOCALDOCPATH is not explicitly defined, it equals TEXDOCPATH
	  unless (defined $localdocpath) {
	    $localdocpath=basename($texdocpath,"");
	  }
	  $localdocpath="$texmflocal/$localdocpath";
	}
    }
#   find user's $HOMETEXMF/doc, if there is one
    if ($icall == 2 || $pathflagh == 1) {
        $texmfhome=`kpsewhich --expand-path=${qq}\$HOMETEXMF${qq}`;
	chomp $texmfhome;
	if (length $texmfhome) {
	  # if HOMEDOCPATH is not explicitly defined, it equals TEXDOCPATH
	  unless (defined $homedocpath) {
	    $homedocpath=basename($texdocpath,"");
	  }
	  $homedocpath="$texmfhome/$homedocpath";
	}
    }
#   colours: create array
    @tdcolors=split(' ',$tdcolors);
    if (scalar @tdcolors != 4) { # if old texdocrc file is used
	if (!(-e $myrc) || (-e $myrc && $icall == 2)) {
	    print "NOTE: Button colours not set in $rcfile; using defaults.\n";
	    @tdcolors=('#d9d9d9','Black','#ececec','red');
	}
    }
    %butcol=('-background'=>$tdcolors[0],
	     '-foreground'=>$tdcolors[1],
	     '-activebackground'=>$tdcolors[2],
	     '-activeforeground'=>$tdcolors[3]);
#   viewer: suppress viewer warnings/use $PAGER if no $txt_viewer is defined
    $quiet=($quiet =~ /y/i || $quiet eq "1") ? 1 : 0;
    if (!(defined $txt_viewer) && defined $ENV{PAGER}) {
	$txt_viewer=$ENV{PAGER};
    }
#   viewer: autostart viewer if listbox of search results contains only 1 item
    $autoview=($autoview =~ /y/i || $autoview eq "1") ? 1 : 0;
#   viewer: use text viewer to open files with unrecognized format
    $xfmt_viewer=($xfmt_viewer =~ /y/i || $xfmt_viewer eq "1") ? 1 : 0;
#   converter: output redirection flags for HTML/Text->PS converters
    $htmlps_redir=($htmlps_redir =~ /(y|1)/i) ? 1 : 0;
    $txtps_redir=($txtps_redir =~ /(y|1)/i) ? 1 : 0;
#   for backward compatibility of converter/printer options
    $dvips_conv.=" $dvips_opts" if ($dvips_opts);
    $pdfps_conv.=" $pdfps_opts" if ($pdfps_opts);
    $print_cmd.=" $print_opts" if ($print_opts);
}

# determine resolution if xwininfo is available
# slightly modified from a patch contributed by Reinhard Kotucha
sub x_resolution {
    my $x_res;
    if ($IsWin32) {
      $x_res = &GetScreenWidth;
    }
    else {
      open(XINF,"xwininfo -root|") or return 1200;
      while (<XINF>) {
        chop;
        if (/Width:/) { ($x_res=$_)=/Width:\s+(\d+)/; last; }
      }
      close(XINF);
    }
    return $x_res;
}

# generate a random name for temporary files
sub randname {
    my $namestr;
    my @chars=("A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z");
    my $time=time;
    my $maxcnt=3+substr($time,-1);
    for (my $i=0; $i<=$maxcnt; $i++) { $namestr.=$chars[rand(51)] }
    my @tmp=split(/./,$time/$$);
    $namestr.=$tmp[1];
    until (length($namestr) > 16) { $namestr.=int(rand(1000000)) }
    return $namestr;
}

# find index of an array entry
sub arrindex {
  my @array=@_;
  my $item=pop @array;
  my $i=0;
  foreach (@array) {
    last if ($_ eq $item);
    ++$i;
  }
  return $i;
}

# clean up and exit
sub clean_exit {
    foreach (@tmpfiles) { if (-e $_) { unlink $_; } }
    exit;
}

# alternative main window, launched for fatal error messages on startup
sub fatalmsg {
    my($msg)=@_;
    print STDERR $msg; # also print to stderr
# create frame for main window
    my $main=new MainWindow;
    $main->resizable(0,0);
    $main->title("TeX Documentation Browser");
    $main->bind('<Control-q>'=>sub { exit });
    $main->bind('<Control-k>'=>sub { exit });
    my $msgframe=$main->Frame(-background=>"#ffcc99");
    my $cmdframe=$main->Frame;
    $msgframe->pack(-side=>'top');
    $cmdframe->pack(-side=>'bottom',-fill=>'x');
# make buttons for command frame
    my $Qbut=$cmdframe->Button(-text=>'Kill',%butcol,
			       -command=>sub { exit })->pack(-fill=>'x');
# define common default font for labels and text explicitly
    my @deffont=$Qbut->configure(-font);
# ensure readability on high-res screens (suggested by R.Kotucha)
    $deffont='Helvetica -16 bold' if &x_resolution > 1200;
    $Qbut->configure(-font=>$deffont);
    $msgframe->Label(-text=>'FATAL ERROR',
		     -font=>$deffont)->pack(-side=>'top', -fill=>'x');
#   get size of message text
    my @dummy=split("\n",$msg);
    my $nline=scalar @dummy;
    my $msgwidth=0;
    my $lline;
    foreach (@dummy) {
	$lline=length $_;
	if ($lline > $msgwidth) { $msgwidth=$lline; }
    }
    my $message=$msgframe->Text(-relief=>'flat',
				-font=>$deffont,
				-height=>$nline,
				-width=>$msgwidth)->pack(-side=>'top');
    $message->insert('end',$msg);
    $message->configure(-state=>'disabled');
    MainLoop();
}

if ($IsWin32) {
  sub GetScreenWidth {
    my $GetDeviceCaps = new Win32::API('gdi32', 'GetDeviceCaps', 'NN', 'N');
    my $HORZRES = 8; # from WinGDI.h
    if(not defined $GetDeviceCaps) {
      die "Can't import API GetDeviceCaps: $!\n";
    }
    return $GetDeviceCaps->Call(0, $HORZRES);
  }

  sub GetTempPath {
    my $GetTempPath = new Win32::API('kernel32', 'GetTempPath', 'NP', 'N');
    if(not defined $GetTempPath) {
      die "Can't import API GetTempPath: $!\n";
    }
    my $lpBuffer = " " x 260;

    $GetTempPath->Call(80, $lpBuffer);
  }

  sub ShellExecute {
    my ($command, $file) = @_;
    my $ShellExecute = new Win32::API('shell32', 'ShellExecute', 'NPPPPN', 'N');
    if(not defined $ShellExecute) {
      die "Can't import API ShellExecute: $!\n";
    }
    $ShellExecute->Call(0, $command, $file, '', '', 0);
  }

  sub GetHomeDir {
    my $home = $ENV{'HOME'};
    # Should check for CSIDL_...
    return $home;
  }
}
__END__
Many TeX programmers provide more or less detailed manuals for
their programs or packages. They are usually available as .dvi,
.ps, .pdf, .html or plain text files (sometimes included in the
.sty files instead of a separate documentation file) and can be
accessed with this browser, which is simply an interface to find a
documentation more easily. It starts the respective viewer for
reading the selected documentation making use of a database file
which contains the path entries according to the current teTeX
texmf/doc structure; additional usage of a system-wide local and
of individual user texmf trees with corresponding databases is also
possible.
The documentations are grouped in several categories shown in the
main window; pressing one of its buttons lists all documentations
belonging to this topic.
The topic window lets you select one documentation file, view or
send it to the default printer. By right-clicking on the selected
item you get the complete path of the file.
The search button of the main window allows you to search the
database for a string; it does not search file names. Enter the
string and hit <Return> to start the search or <Control-c> to
cancel. Just hitting <Return> without typing something in will
show the full list of files in the database.
Defaults for the documentation root directory, the viewers, the
converters, certain options and the printer are set in the global
configuration file $sysrc.
However, each user can put a copy of it as .texdocrc into his home
directory to modify them according to his needs; modification or
generation of ~/.texdocrc can also be done with the Settings menu.
Additionally, the settings can be changed temporarily with this
menu.

The following key shortcuts are defined for use with the browser:
  <Ctrl-q>	Quit browser
  <Ctrl-m>	Raise the Main window to the foreground
  <Ctrl-s>	Search a keyword in database (case insensitive)
  <Ctrl-c>	Cancel/close subwindow or search entry widget
  <Ctrl-v>	View selected document (topic windows)
  <Ctrl-p>	Print selected document (topic windows)
  <Ctrl-t>	Open settings menu
  <Ctrl-k>	Kill fatal error message window (same as
		<Ctrl-q>)
  <Ctrl-h>	Open this help
  <Return>	Equivalent to button press;
		OK in selection menu and search
  <Tab>		Next/previous widget

There are the following command line options:
  -a		autostart viewer if a listbox of search results
		contains only one item; can also be set in a
		configuration file
  -v		disable suppression of viewer messages sent to
		stderr and of certain permanent message popup
		windows, in case this was not set in a
		configuration file; default: off

Some more information is available in the manpage of texdoctk.

Comments and suggestions to:
Thomas Ruedas
tr@dlc.ku.dk or tex-k@tug.org
# Local Variables:
# mode: perl
# auto-fill-hook: do-auto-fill
# End:
# vim: ts=8 sw=4 noexpandtab
