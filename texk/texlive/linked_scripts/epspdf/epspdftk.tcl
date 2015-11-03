#!/usr/bin/env wish

# epspdf conversion utility, GUI frontend

#####
# Copyright (C) 2006, 2008, 2009, 2010, 2011, 2013, 2014, 2015 Siep Kroonenberg
# n dot s dot kroonenberg at rug dot nl
#
# This program is free software, licensed under the GNU GPL, >=2.0.
# This software comes with absolutely NO WARRANTY. Use at your own risk!
#####

package require Tk

# tired of writing out this combined test again and again
set classic_unix [expr {$::tcl_platform(platform) eq "unix" && \
 $::tcl_platform(os) ne "Darwin"}]

# combo boxes and -ignorestderr introduced in Tk8.5
set ge_85 [expr {[string index $::tcl_patchLevel 2] > 4}]

# normally, epspdf.tlu should be in the same directory
# and texlua should be on the searchpath.
# However, the Windows installer version wraps this script in a starpack.

# logging: to a log window, not to a file
proc write_log {s} {
  if {[winfo exists .log_t.text]} {
    .log_t.text configure -state normal
    .log_t.text insert end "$s\n"
    .log_t.text yview moveto 1
    .log_t.text configure -state disabled
  # } else {
  #  puts $s
  }
}

### calling epspdf.tlu #########################

# Get full path of epspdf.tlu. It should be in the same directory as
# either this script or of the starpack containing this script,
# For non-windows versions, epspdftk might be called via a symlink.
# Pdftops done elsewhere.

proc set_progs {} {
  set scriptfile [file normalize [info script]]
  set eproot [file dirname $scriptfile]
  # if symlink, get the directory of the file it points to
  if {! [catch {file readlink $scriptfile}]} {
    # evaluate readlink from symlink directory
    set savedir [pwd]
    cd $eproot
    set eproot [file dirname [file normalize [file readlink $scriptfile]]]
    cd $savedir
  }
  # find the lua script
  set ::epspdf_tlu [file join $eproot "epspdf.tlu"]
  if {! [file exists $::epspdf_tlu]} {
    # if starpack, look in binary directory
    set eproot [file dirname [file normalize [info nameofexecutable]]]
    set ::epspdf_tlu [file join $eproot "epspdf.tlu"]
  }
  if {! [file exists $::epspdf_tlu]} {
    tk_messageBox -type ok -icon error -message "Epspdf.tlu not found"
    exit 1
  }
  # texlua should be on the searchpath
  set ::texlua "texlua"

  # icon for starpack: add with sdx or with Windows utilities
}

set_progs

### configured and automatic settings ##################################

# is_prog used for checking configured viewers under non-osx unix
proc is_prog {x} {
  if {[expr {$::tcl_platform(platform) ne "unix"}]} {return 0}
  # avoid current directory except with explicit directory
  if {[expr {[string first "/" $x] >= 0 && \
                 [file executable [file normalize $x]]}]} {
    return 1
  }
  # also check search path
  set p [split $::env(PATH) ":"] ; # no need to accomodate msw
  foreach d $p {
    if {[expr {$d ne "" && [file executable [file join $d $x]]}]} {
      return 1
    }
  }
  return 0
}

# create a global empty settings array
array set ::settings [list]

set ::is_tl 1

# ask epspdf.tlu for currently configured settings.
# this does not include automatically configured or transient settings.
# the availability of viewers is handled here.
proc getsettings {} {
  if [catch {exec $::texlua $::epspdf_tlu --gui=config_w} set_str] {
    error "Epspdf configuration error: $set_str"
  }
  # write_log "settings from epspdf.tlu:\n$set_str\n"
  set l [split $set_str "\r\n"]
  if {$::tcl_platform(platform) eq "windows"} {
    set ::is_tl 1
    set settings(pdftops_prog) ""
  }
  foreach e $l {
    # puts "settings: $e"
    # $e is either a string "var = value"
    # or the empty string between <cr> and <lf>
    set i [string first "=" $e]
    if {$i>0} {
      # write_log $e
      set para [string trim [string range $e 0 [expr $i-1]]]
      set val [string trim [string range $e [expr $i+1] end]]
      if {$val eq "true"} {set val 1}
      if {$val eq "false"} {set val 0}
      if {$para eq "tl_w"} {
        set ::is_tl 0
        write_log "TL for Windows not detected by epspdf"
      } else {
        set ::settings($para) $val
        # write_log "setting $para is $val"
      }
    }
  }

  # unix: viewer settings
  # configured viewer, if valid, heads the list
  if {$::classic_unix} {

    set ::ps_viewers {}
    if {$::settings(ps_viewer) ne "" && [is_prog $::settings(ps_viewer)]} {
      lappend ::ps_viewers $::settings(ps_viewer)
    }
    foreach v {evince okular gv qpdfview} {
      if {$v ne $::settings(ps_viewer) && [is_prog $v]} {
        lappend ::ps_viewers $v
      }
    }
    # puts [join $::ps_viewers " "]

    set ::pdf_viewers {}
    if {$::settings(pdf_viewer) ne "" && [is_prog $::settings(pdf_viewer)]} {
      lappend ::pdf_viewers $::settings(pdf_viewer)
    }
    foreach v {evince okular xpdf epdfview qpdfview zathura acroread  gv} {
      if {$v ne $::settings(pdf_viewer) && [is_prog $v]} {
        lappend ::pdf_viewers $v
      }
    }
    # puts [join $::pdf_viewers " "]

    if {[llength ::pdf_viewers] == 0 && [llength ::ps_viewers] != 0} {
      lappend ::pdf_viewers [lindex $::ps_viewers 0]
    }
    if {[llength ::pdf_viewers] == 0} {
      tk_messageBox -message "No viewers found"
    } elseif {[llength ::ps_viewers] == 0} {
      tk_messageBox -message "No PostScript viewers found"
    }
    if {[llength ::pdf_viewers] > 0} {
      set ::settings(pdf_viewer) [lindex $::pdf_viewers 0]
    }
    if {[llength ::ps_viewers] > 0} {
      set ::settings(ps_viewer) [lindex $::ps_viewers 0]
    }
  }
}

getsettings

proc write_settings {} {
  set s ""
  foreach el [array names ::settings] {
    set s "$s$el = $::settings($el)\n"
  }
  # write_log "\nsettings for epspdf.tlu\n$s\nend writing settings\n"
  if [catch {exec $::texlua $::epspdf_tlu --gui=config_r << $s} result] {
    error "Epspdf configuration error: $result"
  }
}

# directory and other file data

if {$::argc > 0 && [file isdirectory [lindex $::argv 0]]} {
  set ::gfile(dir) [lindex $::argv 0]
} elseif {[file isdirectory $::settings(default_dir)]} {
  set ::gfile(dir) $::settings(default_dir)
} else {
  set ::gfile(dir) $::env(HOME)
}
set ::gfile(path) ""
set ::gfile(type) ""
set ::gfile(name) ""
set ::gfile(npages) ""

# transient options
array set ::options [list gray "color" format "pdf" bbox 0 clean 1 \
  pages "single" page 1]

proc viewable {} {
  if { $::gfile(name) eq "" || ! [file exists $::gfile(path)] || \
      $::gfile(type) eq "other" || $::gfile(type) eq ""} {
    return 0
  } elseif {! $::classic_unix} {
    return 1
  } elseif {$::settings(ps_viewer) ne "" && [regexp {ps} $::gfile(type)]} {
    return 1
  } elseif {$::settings(pdf_viewer) ne "" && $::gfile(type) eq "pdf"} {
    return 1
  } else {
    return 0
  }
}

### groundwork for GUI #################################

# padding for buttons and frames
proc packb {b args} {
  eval [linsert $args 0 pack $b -padx 3 -pady 3]
  $b configure -border 2
}

proc packf {f args} {
  eval [linsert $args 0 pack $f -padx 3 -pady 3]
}

# dummy widget for vertical spacing
set idummy -1
proc spacing {w} {
  incr ::idummy
  pack [label $w.$::idummy -text " "]
}

# bold font
font create boldfont
# create dummy label widget to get at the default label font properties
label .dummy
font configure boldfont -family [font actual [.dummy cget -font] -family]
font configure boldfont -size [font actual [.dummy cget -font] -size]
font configure boldfont -weight bold
destroy .dummy

# bitmaps for mycombo (no combobox in core tk8.4)
image create bitmap dwnarrow -data {
#define dwnarrow_width 15
#define dwnarrow_height 10
static unsigned char dwnarrow_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x03,
  0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 };
}

image create bitmap uparrow -data {
#define uparrow_width 15
#define uparrow_height 10
static unsigned char uparrow_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0xf0, 0x07,
   0xf8, 0x0f, 0xfc, 0x1f, 0x00, 0x00, 0x00, 0x00 };
}

# mycombo
if {$::ge_85} {
  proc update_combo {w vls} {
    upvar $vls vs
    set new [$w get]
    if {$new ni $vs} {
      if {[is_prog $new]} {
        set vs [linsert $vs 0 $new]
        $w configure -values $vs
      } else {
        tk_messageBox -title Error -icon error -message "$vl Not a program"
      }
    }
  }
} else {
  proc mycombo {w} {
    # entry widget and dropdown button
    frame $w
    frame $w.ef
    entry $w.ef.e -width 30 -borderwidth 1
    pack $w.ef.e -side left
    button $w.ef.b -image dwnarrow -command "toggle_list $w" -borderwidth 1
    pack $w.ef.b -side right
    pack $w.ef
    # 'drop-down' listbox; width should match entry widget above
    toplevel $w.lf -bd 0
    listbox $w.lf.l -yscrollcommand "$w.lf.s set" -height 4 -width 30 \
      -bd 1 -relief raised
    grid $w.lf.l -column 0 -row 0 -sticky news
    scrollbar $w.lf.s -command "$w.lf.l yview" -bd 1
    grid $w.lf.s -column 1 -row 0 -sticky ns
    grid columnconfigure $w.lf 0 -weight 1
    wm overrideredirect $w.lf 1
    wm transient $w.lf
    wm withdraw $w.lf
    # next two bindings:
    # final parameter: unmap/toggle listbox
    bind $w.lf.l <KeyRelease-space> {update_e %W [%W get active] 1}
    bind $w.lf.l <KeyRelease-Tab> {update_e %W [%W get active] 1}
    bind $w.lf.l <1> {update_e %W [%W index @%x,%y] 0}
    bind $w.lf.l <Double-1> {update_e %W [%W index @%x,%y] 1}
    bind $w.ef.e <Return> {update_l %W}
    bind $w.ef.e <Tab> {update_l %W}
    return $w
  }

  # toggle state of listbox.
  # this involves calculating the place where it should appear
  # and toggling the arrow image.
  proc toggle_list {w} {
    # $w.ef is the frame with the entry widget
    # $w.lf is the toplevel with the listbox
    # which needs to turn up right below $w.ef
     if {[wm state $w.lf] eq "withdrawn" || [wm state $w.lf] eq "iconified"} {
       set lfx [winfo rootx $w.ef]
       set lfy [expr [winfo rooty $w.ef] + [winfo height $w.ef]]
       wm geometry $w.lf [format "+%d+%d" $lfx $lfy]
       wm deiconify $w.lf
       $w.ef.b configure -image uparrow
     } else {
       wm withdraw $w.lf
       $w.ef.b configure -image dwnarrow
     }
  }

  # note: in tcl/tk 8.5, values of (some) widget variables can be accessed
  # directly and explicit use of upvar is unnecessary.

  # list => entry; optionally toggle list display
  proc update_e {v i toggle} {
    set w [winfo parent [winfo parent $v]]
    set lv [$w.lf.l cget -listvariable]
    upvar $lv l
    set tv [$w.ef.e cget -textvariable]
    upvar $tv t
    set t [lindex $l $i]
    if {$toggle} {toggle_list $w}
  }

  # entry => list
  proc update_l {v} {
    set w [winfo parent [winfo parent $v]]
    set tv [$w.ef.e cget -textvariable]
    upvar $tv t
    set lv [$w.lf.l cget -listvariable]
    upvar $lv l
    set found [lsearch $l $t]
    if { $found < 0} {
      set ok [$v validate]
      if {$ok} {
        lappend l $t
        set l [lsort $l]
      } else {
        tk_messageBox -message "Not a program"
      }
    }
    set the_index [lsearch $l $t]
    $w.lf.l see $the_index
    $w.lf.l activate $the_index
    wm withdraw $w.lf
    $w.ef.b configure -image dwnarrow
  }
}

# end mycombo

### and now the actual GUI ###################################

wm title . "PostScript- and pdf conversions"

# help toplevel

proc readhelp {} {
  .help_t.text configure -state normal
  # this also works for the starpack:
  set helpfile [file join [file dirname $::epspdf_tlu] "epspdf.help"]
  if {! [file exists $helpfile]} {
    # helpfile in starpack
    set helpfile [file join [file dirname [info script]] "epspdf.help"]
  }
  if {[catch {set fid [open $helpfile r]}]} {
    .help_t.text insert end "No helpfile $helpfile found\n"
  } else {
    while {[gets $fid line] >= 0} {.help_t.text insert end "$line\n"}
    close $fid
  }
  .help_t.text configure -state disabled
  .help_t.text yview moveto 0
}

toplevel .help_t
wm title .help_t "EpsPdf help"
packb [button .help_t.done -text "Close" -command {wm withdraw .help_t}] \
 -side bottom -anchor e -padx 50
text .help_t.text -wrap word -width 60 -height 20 -setgrid 1 \
  -yscrollcommand ".help_t.scroll set"
scrollbar .help_t.scroll -command ".help_t.text yview"
pack .help_t.scroll -side right -fill y
pack .help_t.text -expand 1 -fill both
readhelp
.help_t.text configure -state disabled
wm withdraw .help_t

# log toplevel

toplevel .log_t
wm title .log_t "Epspdf log"
packb [button .log_t.b -text "Close" -command {wm withdraw .log_t}] \
 -side bottom -anchor e -padx 50
text .log_t.text -wrap word -relief flat -height 15 -width 60 \
     -setgrid 1 -yscrollcommand ".log_t.scroll set"
scrollbar .log_t.scroll -command ".log_t.text yview"
pack .log_t.scroll -side right -fill y
pack .log_t.text -expand 1 -fill both
.log_t.text configure -state disabled
wm withdraw .log_t

### configure toplevel ###################

toplevel .config_t
wm title .config_t "Configure epspdf"

# The settings array is edited directly in the configuration screen:
# for most control widgets, we need a global external variable anyway,
# so we better use the global that is already there.
# If the configuration screen is closed via the cancel button,
# the original values will be re-read from disk.
# If it is closed via the ok button, the new settings are written to disk.

# viewers (not on windows or osx)

if {$::classic_unix} {
  packf [frame .config_t.viewf] -ipadx 4 -fill x
  grid [label .config_t.viewf.title -font boldfont -text "Viewers"] \
    -row 0 -column 0 -sticky w
  grid [label .config_t.viewf.lb_pdf -text "Pdf"] \
    -row 1 -column 0 -sticky w
  grid [label .config_t.viewf.lb_ps -text "PostScript"] \
    -row 2 -column 0 -sticky w
  if {$::ge_85} {
    grid [ttk::combobox .config_t.viewf.pdf] -row 1 -column 1 -sticky e
    .config_t.viewf.pdf configure -values $::pdf_viewers
    .config_t.viewf.pdf configure -textvariable ::settings(pdf_viewer)
    bind .config_t.viewf.pdf <Return> {update_combo %W $::pdf_viewers}
    grid [ttk::combobox .config_t.viewf.ps] -row 2 -column 1 -sticky e
    .config_t.viewf.ps configure -values $::ps_viewers
    .config_t.viewf.ps configure -textvariable ::settings(ps_viewer)
    bind .config_t.viewf.ps <Return> {update_combo %W $::ps_viewers}
  } else {
    grid [mycombo .config_t.viewf.pdf] -row 1 -column 1 -sticky e
    .config_t.viewf.pdf.lf.l configure -listvariable ::pdf_viewers
    .config_t.viewf.pdf.ef.e configure -textvariable ::settings(pdf_viewer)
    .config_t.viewf.pdf.ef.e configure -vcmd {is_prog %P} -validate none
    grid [mycombo .config_t.viewf.ps] -row 2 -column 1 -sticky e -pady 4
    .config_t.viewf.ps.lf.l configure -listvariable ::ps_viewers
    .config_t.viewf.ps.ef.e configure -textvariable ::settings(ps_viewer)
    .config_t.viewf.ps.ef.e configure -vcmd {is_prog %P} -validate none
  }
  grid columnconfigure .config_t.viewf 1 -weight 1 -pad 2

  spacing .config_t
}

# settings for conversion to pdf

packf [frame .config_t.pdff] -ipadx 4 -fill x
pack [label .config_t.pdff.title -font boldfont -text "Conversion to pdf"] \
  -anchor w

pack [label .config_t.pdff.l_target -text "Target use"] -anchor w
pack [frame .config_t.pdff.f_targets] -fill x
foreach t {default printer prepress screen ebook} {
  pack [radiobutton .config_t.pdff.f_targets.$t \
      -variable ::settings(pdf_target) \
      -text $t -value $t] -side left -padx 2 -pady 4 -anchor w
}

pack [label .config_t.pdff.l_version -text "Pdf version"] -anchor w
pack [frame .config_t.pdff.f_version] -fill x
foreach t {1.2 1.3 1.4 default} {
  regsub {\.} $t _ tp ; # replace dot in name: dots are path separators!
  pack [radiobutton .config_t.pdff.f_version.$tp \
      -variable ::settings(pdf_version) \
      -text $t -value $t] -side left -padx 2 -pady 4 -anchor w
}

#pack [label .config_t.pdff.l_gs \
#  -text "Custom Ghostscript/ps2pdf parameters"] -anchor w
#pack [entry .config_t.pdff.e_gs -border 1] -fill x -padx 2 -pady 2
#.config_t.pdff.e_gs configure -textvariable settings(pdf_custom)

spacing .config_t

# settings for conversion to EPS and PostScript

packf [frame .config_t.psf] -ipadx 4 -fill x
pack [label .config_t.psf.l_ps -text "Conversion to EPS and PostScript" \
          -font boldfont] -anchor w
if {! $::is_tl} {
  if {[string tolower [string range $::settings(pdftops_prog) end-3 end]] ne \
          ".exe"} {set ::settings(pdftops_prog) ""}
  pack [label .config_t.psf.l_pdftops -text "Find pdftops"] -anchor w
  pack [frame .config_t.psf.findf] -anchor w
  pack [entry .config_t.psf.findf.e -width 40] -side left -padx 4
  .config_t.psf.findf.e configure -textvariable ::settings(pdftops_prog)
  packb [button .config_t.psf.findf.b -text "Browse..." \
     -command find_pdftops] -side left
}

proc find_pdftops {} {
  set try [tk_getOpenFile -title "Find pdftops.exe" \
       -filetypes {{"Programs" {.exe}}} -initialdir "c:/"]
  if {$try ne ""} {
    .config_t.psf.findf.e delete 0 end
    .config_t.psf.findf.e insert 0 $try
  }
}

pack [checkbutton .config_t.psf.c \
          -text "Use pdftops if available (recommended)"] -anchor w
.config_t.psf.c configure -variable ::settings(use_pdftops) \
  -onvalue 1 -offvalue 0

spacing .config_t

## hires boundingbox setting
#
#packf [frame .config_t.hiresf] -ipadx 4 -fill x
#pack [label .config_t.hiresf.title -font boldfont -text "Hires BoundingBox"] \
#  -anchor w
#pack [label .config_t.hiresf.l -text "Uncheck to prevent clipping"] \
#  -anchor w
#
#pack [checkbutton .config_t.hiresf.c \
#          -text "Use hires boundingbox if possible"] -anchor w
#.config_t.hiresf.c configure \
#  -variable ::settings(ignore_hires_bb) -onvalue 0 -offvalue 1
#
#spacing .config_t

# buttons for closing the configuration screen

pack [frame .config_t.buttonsf] -fill x
packb [button .config_t.buttonsf.done -text "Done" -command putsettings] \
 -side right
packb [button .config_t.buttonsf.cancel -text "Cancel" \
 -command cancelsettings] -side right

wm transient .config_t
wm overrideredirect .config_t
wm withdraw .config_t

proc edit_settings {} {
  wm deiconify .config_t
  raise .config_t
  grab set .config_t
}

# store new settings
proc putsettings {} {
  if {$::classic_unix && ! $::ge_85} {
    wm withdraw .config_t.viewf.pdf.lf
    wm withdraw .config_t.viewf.ps.lf
  }
  wm withdraw .config_t
  grab release .config_t
  write_settings
}

proc cancelsettings {} {
  if {$::classic_unix && ! $::ge_85} {
    wm withdraw .config_t.viewf.pdf.lf
    wm withdraw .config_t.viewf.ps.lf
  }
  wm withdraw .config_t
  grab release .config_t
  # re-read config file / reg entries
  getsettings
}

### main screen #############################

proc show_w {w} {
  wm deiconify $w
  raise $w
}

# buttons to call up configure-, log- and help screens

pack [frame .topf] -fill x
packb [button .topf.config_t -text "Configure" \
          -command edit_settings] -side left
packb [button .topf.help_t -text "Help" \
 -command {show_w .help_t}] -side right
packb [button .topf.logb -text "Show log" -command {show_w .log_t}] \
  -side right -anchor w

# file info in grid layout

packf [frame .infof -relief sunken -border 1] -fill x
grid [label .infof.dir_label -text "Directory" -anchor w] \
 -row 1 -column 1 -sticky w
grid [label .infof.dir_value -textvariable ::gfile(dir) -anchor w] \
 -row 1 -column 2 -sticky w

grid [label .infof.name_label -text "File" -anchor w] \
 -row 2 -column 1 -sticky w
grid [label .infof.name_value -textvariable ::gfile(name) -anchor w] \
 -row 2 -column 2 -sticky w

grid [label .infof.type_label -text "Type" -anchor w] \
 -row 3 -column 1 -sticky w
grid [label .infof.type_value -textvariable ::gfile(type) -anchor w] \
 -row 3 -column 2 -sticky w

grid [label .infof.npages_label -text "Pages" -anchor w] \
 -row 4 -column 1 -sticky w
grid [label .infof.npages_value -textvariable ::gfile(npages) -anchor w] \
 -row 4 -column 2 -sticky w

grid columnconfigure .infof 1 -weight 1 -pad 2
grid columnconfigure .infof 2 -weight 3 -pad 2

spacing .

# conversion options

pack [frame .optsf] -fill x

# grayscaling
pack [frame .optsf.gray] -side left -anchor nw
pack [label .optsf.gray.l -text "Grayscaling"] -anchor w
pack [radiobutton .optsf.gray.off -text "No color conversion" \
          -variable ::options(gray) -value "color"] -anchor w
pack [radiobutton .optsf.gray.gray -text "Grayscale" \
          -variable ::options(gray) -value "gray"] -anchor w
#pack [radiobutton .optsf.gray.gRAY -text "Try harder to grayscale" \
#          -variable ::options(gray) -value "gRAY"] -anchor w

# output format
pack [label .optsf.format] -side right -anchor ne
pack [label .optsf.format.l -text "Output format"] -anchor w
pack [radiobutton .optsf.format.pdf -text "pdf" -command set_widget_states \
          -variable ::options(format) -value "pdf"] -anchor w
pack [radiobutton .optsf.format.eps -text "eps" -command set_widget_states \
          -variable ::options(format) -value "eps"] -anchor w
pack [radiobutton .optsf.format.ps -text "ps" -command set_widget_states \
          -variable ::options(format) -value "ps"] -anchor w

spacing .

# boundingbox
pack [checkbutton .bbox -text "Compute tight boundingbox" \
 -variable ::options(bbox) -command set_widget_states] -anchor w

# page selection
pack [frame .pagesf] -fill x
pack [radiobutton .pagesf.all -text "Convert all pages" \
   -variable ::options(pages) -value "all" -command set_widget_states] \
   -side left
pack [radiobutton .pagesf.single -text "Page:" \
   -variable ::options(pages) -value "single" -command set_widget_states] \
   -side left
pack [entry .pagesf.e -width 6 -textvariable ::options(page)] -side left
#.pagesf.e configure -vcmd {page_valid %W} -validate focusout \
#  -invcmd "focusAndFlash %W [.pagesf.e cget -fg] [.pagesf.e cget -bg]"
.pagesf.e configure -vcmd {page_valid %W} -validate focusout \
  -invcmd "see_red %W"

spacing .

# temp files
pack [checkbutton .clean -text "Remove temp files" \
 -variable ::options(clean)] -anchor w

proc focusAndFlash {w fg bg {count 9}} {
  focus $w
  if {$count<1} {
    $w configure -foreground $fg -background $bg
  } else {
    if {$count%2} {
      $w configure -foreground $bg -background $fg
    } else {
      $w configure -foreground $fg -background $bg
    }
    after 200 [list focusAndFlash $w $fg $bg [expr {$count-1}]]
  }
}

proc see_red {w} {
  focus $w
  set orig [$w cget -bg]
  $w configure -bg red
  update idletasks
  after 1000
  $w configure -bg $orig
  update idletasks
}

proc page_valid {w} {
  # w entry widget
  set p [$w get]
  set isvalid 1
  #puts "check pageno $p"
  if {! [string is integer $p] || $p<=0} {
    set isvalid 0
  } elseif {$::gfile(npages) ne "" && [scan $p "%d"] > $::gfile(npages)} {
    set isvalid 0
  }
  return $isvalid
}

# end conversion options

pack [label .status -justify left] -side bottom -anchor w -fill x -expand 1

# main buttons

pack [frame .bottomf] -side bottom -fill x
packb [button .bottomf.view -text "View" -command view] -side left
packb [button .bottomf.open -text "Open" -command openDialog] \
 -side left -padx 2
packb [button .bottomf.convert -text "Convert and save..." \
 -command saveDialog] -side left -padx 2
packb [button .bottomf.done -text "Done" -command exit] -side right

proc view {} {
  if {! [viewable]} {
    tk_messageBox -icon warning -type ok \
      -message "No viewer for $::gfile(path)"
    return
  }
  if {$::tcl_platform(os) eq "Darwin"} {
    catch {exec open $::gfile(path) &}
  } elseif {$::tcl_platform(platform) ne "unix"} {
    # use shortname to sidestep quoting problems
    catch {exec cmd /x /c start [file attributes $::gfile(path) -shortname]}
  } else {
    if {$::gfile(type) eq "pdf"} {
      catch {exec $::settings(pdf_viewer) $::gfile(path) &}
    } elseif {$::gfile(type) ne "other" && $::gfile(type) ne ""} {
       catch {exec $::settings(ps_viewer) $::gfile(path) &}
    }
  }
}

proc openDialog {} {
  set types {
    {"PostScript and pdf" {.eps .epi .epsi .ps .prn .pdf}}
    {"Encapsulated PostScript" {.eps .epi .epsi}}
    {"General PostScript" {.ps .prn}}
    {"Pdf" {.pdf}}
    {"All files" *}
  }
  if {[file isdirectory $::gfile(dir)]} {
      set try [tk_getOpenFile -filetypes $types -initialdir $::gfile(dir)]
  } else {
      set try [tk_getOpenFile -filetypes $types]
  }
  if {$try != ""} {
    set ::gfile(path) [file normalize $try]
    set ::gfile(dir) [file dirname $::gfile(path)]
    set ::gfile(name) [file tail $::gfile(path)]
    set ::gfile(type) ""
    if {! [catch {exec $::texlua $::epspdf_tlu --gui=gui -i $::gfile(path)} \
          result]} {
      # parse output
      regexp {has type (\w+)(?:.+ (\d+) pages)?\.} $result \
        mtc ::gfile(type) ::gfile(npages)
      if {[regexp {^eps} $::gfile(type)]} {set ::gfile(npages) 1}
    }
    if {$::gfile(type) eq ""} {
      # unsupported type
      tk_messageBox -message "$try: unreadable or unsupported type" \
          -title "Error" -icon error
    }
    set ::settings(default_dir) $::gfile(dir)
    putsettings
  }
  set_widget_states
}

proc saveDialog {} {
  if {$::options(format) eq "pdf"} {
    set types {{"Pdf" {.pdf}}}
  } elseif {$::options(format) eq "ps"} {
    set types {{"PostScript" {.ps}}}
  } elseif {$::options(format) eq "eps"} {
    set types {{"Encapsulated PostScript" {.eps}}}
  }
  set try [tk_getSaveFile -filetypes $types -initialdir $::gfile(dir)]
  if {$try != ""} {
    # fix extension
    set try [file normalize $try]
    set idot [string last "." [file tail $try]]
    if {$idot == -1} {
      append try ".$::options(format)"
    } else {
      # don't bother checking whether the extension is already correct
      set try [string range $try 0 [string last "." $try]]
      append try $::options(format)
    }
    set try [file normalize $try]
    # epspdf can read persistent options from configuration.
    # only options from the options array need to be converted to parameters.
    set args [list]
    if {$::options(gray) eq "gray"} {
      lappend args "-g"
    }
    if {$::options(bbox)} {lappend args "-b"}
    if {! $::options(clean)} {lappend args "-d"}
    if {$::options(pages) eq "single"} {
      if {$::options(page) eq ""} {set ::options(page) 1}
      lappend args "-p" $::options(page)
    }
    lappend args $::gfile(path) $try
    .status configure -text "Working..." -justify "left"
    foreach b {view open convert done} {
      .bottomf.$b configure -state disabled
    }
    update idletasks; # force immediate redisplay main window

    if {$::ge_85} {
      set failed [catch [linsert $args 0 \
          exec -ignorestderr $::texlua $::epspdf_tlu --gui=gui] result]
    } else {
      set failed [catch [linsert $args 0 \
          exec $::texlua $::epspdf_tlu --gui=gui] result]
    }
    write_log $result
    if {$failed} {
      tk_messageBox -icon error -type ok -message "Error; see log window"
    } else {
      set ::gfile(path) [file normalize $try]
      set ::gfile(dir) [file dirname $::gfile(path)]
      set ::gfile(type) $::options(format)
      set ::gfile(name) [file tail $::gfile(path)]
      # parse result output
      regexp { is (\w+)(?: with (\d+) pages)?[\r\n]*$} \
        [string range $result [string last "File type of" $result] end] \
        mtc ::gfile(type) ::gfile(npages)
      if {$::gfile(type) eq "eps"} {set ::gfile(npages) 1}
      set ::settings(default_dir) $::gfile(dir)
      putsettings
      set ::options(page) 1
    }
    .status configure -text ""
    foreach b {view open convert done} {
      .bottomf.$b configure -state normal
    }
    focus .bottomf.view
    set_widget_states
  }
}

proc set_widget_states {} {
  # states (normal/disabled) for various widgets.
  # their values should already be mutually consistent.
  # widgets concerned:
  # view / save
  # output format / bbox / single page / page no.

  # view
  if {[viewable]} {
    .bottomf.view configure -state normal
  } else {
    .bottomf.view configure -state disabled
  }

  # convert
  .bottomf.convert configure -state normal
  if {$::gfile(path) eq "" || ! [file exists $::gfile(path)]} { \
    .bottomf.convert configure -state disabled
  }
  if {$::gfile(type) eq "other"} {
    .bottomf.convert configure -state disabled
  }
  if {$::gfile(npages) ne "" && $::options(pages) eq "single" && \
          $::options(page) > $::gfile(npages)} {
    .bottomf.convert configure -state disabled
  }

  # type
  if {[regexp {ps|pdf} $::gfile(type)]} {
    foreach f {pdf eps ps} {.optsf.format.$f configure -state normal}
  } else {
    foreach f {pdf eps ps} {.optsf.format.$f configure -state disabled}
  }

  # pages and page
  foreach f {all single} {.pagesf.$f configure -state normal}
  if {$::options(format) eq "eps"} {
    .pagesf.all configure -state disabled
    set ::options(pages) "single"
  }
  if {$::options(format) eq "ps"} {
    .pagesf.single configure -state disabled
    set ::options(pages) "all"
  }
  if {$::options(pages) eq "all"} {
    .pagesf.e configure -state disabled
  } else {
    .pagesf.e configure -state normal
  }

  # boundingbox
  if {$::options(format) eq "eps"} {
    .bbox configure -state normal
  } elseif {$::options(pages) eq "all"} {
    .bbox configure -state disabled
    set ::options(bbox) 0
  } else {
    .bbox configure -state normal
  }
  update idletasks
}

set_widget_states
