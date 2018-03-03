#!/usr/bin/env wish

# Copyright 2017, 2018 Siep Kroonenberg

# This file is licensed under the GNU General Public License version 2
# or any later version.

package require Tk

# searchpath:
# windows: most scripts run via [w]runscript, which adjusts the searchpath
# for the current process.
# unix/linux: tlshell.tcl should  be run via a symlink in a directory
# which also contains (a symlink to) kpsewhich.
# This directory will be prepended to the searchpath.
# kpsewhich will disentangle symlinks.

# dis/enable restore dialog
set do_restore 0
# dis/enable debug output (only for private development purposes)
set ddebug 0

##### general housekeeping ############################################

# security: disable send
catch {rename send {}}

# menus: disable tearoff feature
option add *Menu.tearOff 0

# no bold text for messages
option add *Dialog.msg.font TkDefaultFont userDefault

## italicized items; not used
#font create it_font {*}[font configure TkDefaultFont]
#font configure it_font -slant italic

set plain_unix 0
if {$::tcl_platform(platform) eq "unix" && \
        $::tcl_platform(os) ne "Darwin"} {
  set plain_unix 1
}

proc search_nocase {needle haystack} {
  if {$needle eq ""} {return -1}
  if {$haystack eq ""} {return -1}
  return [string first [string tolower $needle] [string tolower $haystack]]
}

# the stderr and stdout of tlmgr are each read into lists of strings
set err_log {}
set out_log {}

#### debug output, ATM only for private development purposes ####

if $ddebug {set dbg_log {}}

proc do_debug {s} {
  if $::ddebug {
    puts stderr $s
    # On windows, stderr output goes nowhere.
    # Therefore also debug output for the log toplevel.
    lappend ::dbg_log $s
    # Track debug output in the log toplevel if it is running:
    if [winfo exists .tllg.dbg.tx] {
      .tllg.dbg.tx configure -state normal
      .tllg.dbg.tx insert end "$s\n"
      if {$::tcl_platform(os) ne "Darwin"} {
        .tllg.dbg.tx configure -state disabled
      }
    }
  }
} ; # do_debug

proc get_stacktrace {} {
  set level [info level]
  set s ""
  for {set i 1} {$i < $level} {incr i} {
    append s [format "Level %u: %s\n" $i [info level $i]]
  }
  return $s
} ; # get_stacktrace

proc maketemp {ext} {
  set fname ""
  foreach i {0 1 2 3 4 5 6 7 8 9} { ; # ten tries
    set fname [file join $::tempsub "[expr {int(10000.0*rand())}]$ext"]
    if {[file exist $fname]} {set fname ""; continue}
    # create empty file. although we just want a name,
    # we must make sure that it can be created.
    set fid [open $fname w]
    close $fid
    if {! [file exists $fname]} {error "Cannot create temporary file"}
    if {$::tcl_platform(platform) eq "unix"} {
      file attributes $fname -permissions 0600
    }
    break
  }
  if {$fname eq ""} {error "Cannot create temporary file"}
  return $fname
} ; # maketemp

set tempsub "" ; # subdir for temp files, set during initialization

### GUI utilities #####################################################

# dummy widgets for vertical spacing within $w
set idummy -1
proc spacing {w} {
  incr ::idummy
  pack [ttk::label $w.$::idummy -text " "]
}

proc pgrid {wdg args} { ; # grid command with padding
  grid $wdg {*}$args -padx 3 -pady 3
}

proc ppack {wdg args} { ; # pack command with padding
  pack $wdg {*}$args -padx 3 -pady 3
}

# mouse clicks: deal with MacOS platform differences
if {[tk windowingsystem] eq "aqua"} {
  event add <<RightClick>> <ButtonRelease-2> <Control-ButtonRelease-1>
} else {
  event add <<RightClick>> <ButtonRelease-3>
}

## default_bg color, only used for menus under ::plain_unix
if [catch {ttk::style lookup TFrame -background} ::default_bg] {
  set ::default_bg white
}

# unicode symbols as fake checkboxes in ttk::treeview widgets
proc mark_sym {mrk} {
  if $mrk {
    return "\u25A3" ; # 'white square containing black small square'
  } else {
    return "\u25A1" ; # 'white square'
  }
} ; # mark_sym

# place a toplevel, or at least its upperleft corner, centered wrt its parent
proc place_dlg {wnd {p ""}} {
  if {$p eq ""} {
    set p [winfo toplevel [winfo parent $wnd]]
    if {$p eq ""} return
  }
  set g [wm geometry $p]
  scan $g "%dx%d+%d+%d" pw ph px py
  set hcenter [expr {$px + $pw / 2}]
  set vcenter [expr {$py + $ph / 2}]
  set g [wm geometry $wnd]
  set wh [winfo reqheight $wnd]
  set ww [winfo reqwidth $wnd]
  set wx [expr {$hcenter - $ww / 2}]
  if {$wx < 0} { set wx 0}
  set wy [expr {$vcenter - $wh / 2}]
  if {$wy < 0} { set wy 0}
  wm geometry $wnd [format "+%d+%d" $wx $wy]
  wm attributes $wnd -topmost 1
  wm attributes $p -topmost 0
  wm state $wnd normal
  raise $wnd $p
  tkwait visibility $wnd
  if {$::tcl_platform(platform) ne "windows"} {
    # dont understand why these give trouble in windows
    focus $wnd
    grab set $wnd
  }
} ; # place_dlg

proc long_message {str type {p "."}} {
  # alternate messagebox implemented as custom dialog
  # not all message types are supported
  if {$type ne "ok" && $type ne "okcancel" && $type ne "yesnocancel"} {
    err_exit "Illegal type $type for long_message"
  }
  set ::lms_parent $p
  unset -nocomplain ::lms_var
  do_debug "type $type"
  catch {destroy .tlmg}
  toplevel .tlmg -class Dialog
  wm withdraw .tlmg
  wm transient .tlmg .
  if $::plain_unix {wm attributes .tlmg -type dialog}

  # wallpaper frame; see make_widgets
  pack [ttk::frame .tlmg.bg] -fill both -expand 1
  ppack [ttk::frame .tlmg.tx] -in .tlmg.bg -side top -fill both -expand 1
  pack [ttk::scrollbar .tlmg.tx.scroll -command ".tlmg.tx.txt yview"] \
      -side right -fill y
  ppack [text .tlmg.tx.txt -height 20 -width 60 -bd 2 -relief groove \
      -wrap word -yscrollcommand ".tlmg.tx.scroll set"] -expand 1 -fill both

  .tlmg.tx.txt insert end $str
  .tlmg.tx.txt configure -state disabled

  # buttons
  pack [ttk::frame .tlmg.bts] -in .tlmg.bg -side bottom -fill x
  if {$type eq "ok" || $type eq "okcancel"} {
    ttk::button .tlmg.ok -text "ok" -command \
        {raise $::lms_parent; destroy .tlmg; set ::lms_var "ok"}
    ppack .tlmg.ok -in .tlmg.bts -side right
  }
  if {$type eq "yesnocancel"} {
    ttk::button .tlmg.yes -text "yes" -command \
      {raise $::lms_parent; destroy .tlmg; set::lms_var "yes"}
    ppack .tlmg.yes -in .tlmg.bts -side right
    ttk::button .tlmg.no -text "no" -command \
      {raise $::lms_parent; destroy .tlmg; set ::lms_var "no"}
    ppack .tlmg.no -in .tlmg.bts -side right
  }
  if {$type eq "yesnocancel" || $type eq "okcancel"} {
    ttk::button .tlmg.cancel -text "cancel" -command \
        {raise $::lms_parent; destroy .tlmg; set ::lms_var "cancel"}
    ppack .tlmg.cancel -in .tlmg.bts -side right
  }

  place_dlg .tlmg $::lms_parent
  tkwait variable ::lms_var
  return $::lms_var
} ; # long_message

proc any_message {str type {p "."}} {
  if {[string length $str] < 400} {
    if {$type ne "ok"} {
      return [tk_messageBox -message $str -type $type -parent $p \
                 -icon question]
    } else {
      return [tk_messageBox -message $str -type $type -parent $p]
    }
  } else {
    # just hope that tcl will do the right thing with multibyte characters
    if {[string length $str] > 500000} {
      set str [string range $str 0 500000]
      append str "\n....."
    }
    return [long_message $str $type $p]
  }
} ; # any_message

### enabling and disabling user interaction

proc enable_menu_controls {yesno} {
  if {! $yesno} {
    . configure -menu .mn_empty
    return
  }
  . configure -menu .mn
  if {! $::n_updates} {
    .mn.pkg entryconfigure $::inx_upd_all -state disabled
    .mn.pkg entryconfigure $::inx_upd_tlmgr -state disabled
  } elseif $::need_update_tlmgr {
    .mn.pkg entryconfigure $::inx_upd_all -state disabled
    .mn.pkg entryconfigure $::inx_upd_tlmgr -state normal
  } else {
    .mn.pkg entryconfigure $::inx_upd_all -state normal
    .mn.pkg entryconfigure $::inx_upd_tlmgr -state disabled
  }
  if {$::tcl_platform(platform) ne "windows"} {
    if $::have_remote {
      .mn.opt entryconfigure $::inx_platforms -state normal
    } else {
      .mn.opt entryconfigure $::inx_platforms -state disabled
    }
  }
}; # enable_menu_controls

proc enable_widgets {yesno} {
  # This proc should cover all active interface elements of the main window.
  # But if actions are initiated via a dialog, the main window can be
  # deactivated simply by a grab and focus on the dialog.
  enable_menu_controls $yesno

  if $yesno {
    set st !disabled
    set ::busy "IDLE"
  } else {
    set st disabled
    set ::busy "BUSY"
  }

  # command entry
  .ent.b configure -state $st
  .ent.e configure -state $st

  # filter options
  # status
  .pkfilter.inst configure -state $st
  .pkfilter.alls configure -state $st
  .pkfilter.upd configure -state $st
  # detail
  .pkfilter.alld configure -state $st
  .pkfilter.coll configure -state $st
  .pkfilter.schm configure -state $st

  # mark commands
  .mrk_all configure -state $st
  .mrk_none configure -state $st

  # search
  .pksearch.e configure -state $st
  .pksearch.d configure -state $st

  # packages
  #.pkglist configure -state $st
  .pkglist state $st

  # final buttons
  .q configure -state $st
  .r configure -state $st
  .t configure -state $st
  .showlogs configure -state $st
} ; # enable_widgets

##### tl global data ##################################################

set last_cmd ""

set progname [info script]
regexp {^.*[\\/]([^\\/\.]*)(?:\....)?$} $progname dummy progname
set procid [pid]

# package repository (no suport for a one-off repository switch)
set repo ""
# while selecting another repo:
set new_repo ""

# mirrors: dict of dicts of lists of urls per country per continent
set mirrors {}

# dict of (local and global) package dicts
set pkgs [dict create]

set have_remote 0 ; # remote packages info not loaded
set need_update_tlmgr 0
set n_updates 0
set tlshell_updatable 0

## data to be displayed ##

# sorted display data for packages; package data stored as lists
set filtered [dict create]

# selecting packages for display: status and detail
set stat_opt "inst"
set dtl_opt "all"
# searching packages for display; also search short descriptions?
set search_desc 0

##### handling tlmgr via pipe and stderr tempfile #####################

set prmpt "tlmgr>"
set busy "BUSY"

# about [chan] gets:
# if a second parameter, in this case l, is supplied
# then this variable receives the result, with EOL stripped,
# and the return value is the string length, possibly 0
# EOF is indicated by a return value of -1.

proc read_err_tempfile {} {
  set len 0
  while 1 {
    set len [chan gets $::err l]
    if {$len >= 0} {
      lappend ::err_log $l
    } else {
      break
    }
  }
} ; # read_err_tempfile

proc err_exit {} {
  do_debug "error exit"
  read_err_tempfile
  any_message [join $::err_log "\n"] "ok"
  exit
} ; # err_exit

# Capture stdout of tlmgr into a pipe $::tlshl,
# stderr into a temp file file $::err_file which is set at initialization.

proc start_tlmgr {{args ""}} {
  # Start the TeX Live Manager shell interface.
  # Below, vwait ::done_waiting forces tlshell
  # to process initial tlmgr output before continuing.
  unset -nocomplain ::done_waiting
  do_debug "opening tlmgr"
  if [catch \
          {open "|tlmgr $args --machine-readable shell 2>>$::err_file" w+} \
          ::tlshl] {
    tk_messageBox -message [get_stacktrace]
    exit
  }
  do_debug "done opening tlmgr"
  set ::err [open $::err_file r]
  chan configure $::tlshl -buffering line -blocking 0
  chan event $::tlshl readable read_line
  vwait ::done_waiting
} ; # start_tlmgr

proc close_tlmgr {} {
  catch {chan close $::tlshl}
  catch {chan close $::err}
}; # close_tlmgr

# read a line of tlmgr output
proc read_line {} {
  # a caller of run_cmd needs to explicitly invoke 'vwait ::done_waiting'
  # if it wants to wait for the command to finish
  set l "" ; # will contain the line to be read
  if {([catch {chan gets $::tlshl l} len] || [chan eof $::tlshl])} {
    #do_debug "read_line: failing to read "
    puts stderr "Read failure; tlmgr command was $::last_cmd"
    catch {chan close $::tlshl}
    # note. the right way to terminate is terminating the GUI shell.
    # This closes stdin of tlmgr shell.
    err_exit
  } elseif {$len >= 0} {
    # do_debug "read: $l"
    if $::ddebug {puts $::flid $l}
    if {[string first $::prmpt $l] == 0} {
      # prompt line: we are done with the current command
      enable_widgets 1 ; # this may have to be redone later
      # catch up with stderr
      read_err_tempfile
      if {$::pipe_cb ne ""} {
        do_debug "prompt found, $l"
        $::pipe_cb "finish"
      }
      # for vwait:
      set ::done_waiting 1
    } else {
      # regular output
      lappend ::out_log $l
      if {$::pipe_cb ne ""} {$::pipe_cb "line" "$l"}
    }
  }
} ; # read_line

# copy error strings to error page in logs toplevel .tllg and send it to top.
# This by itself does not map the logs toplevel .tllg

proc show_err_log {} {
  #do_debug "show_err_log"
  .tllg.err.tx configure -state normal
  .tllg.err.tx delete 1.0 end
  if {[llength $::err_log] > 0} {
    foreach l $::err_log {.tllg.err.tx insert end "$l\n"}
    .tllg.err.tx yview moveto 1
    .tllg.logs select .tllg.err
  }
  if {$::tcl_platform(os) ne "Darwin"} {
    # os x: text widget disabled => no selection possible
    .tllg.err.tx configure -state disabled
  }
} ; # show_err_log

##### running tlmgr commands #####

# optional callback for run_cmds/read_line:
set pipe_cb ""

# run a list of commands
proc run_cmds {cmds {cb ""}} {
  set ::pipe_cb $cb
  do_debug "run_cmds \"$cmds\""
  if $::ddebug {puts $::flid "\n$cmds"}
  enable_widgets 0
  set ::out_log {}
  set ::err_log {}
  if {$::pipe_cb ne ""} {$::pipe_cb "init"}
  set l [llength $cmds]
  for {set i 0} {$i<$l} {incr i} {
    set cmd [lindex $cmds $i]
    set ::last_cmd $cmd
    unset -nocomplain ::done_waiting
    chan puts $::tlshl $cmd
    chan flush $::tlshl
    if {$i < [expr {$l-1}]} {vwait ::done_waiting}
  }
} ; # run_cmds

# run a single command
proc run_cmd {cmd {cb ""}} {
  run_cmds [list $cmd] $cb
} ; # run_cmd

proc run_cmd_waiting {cmd} {
  run_cmd $cmd
  vwait ::done_waiting
} ; # run_cmd_waiting

##### callbacks for file events of tlmgr pipe ::tlshl (names *_cb) ####

# callback for reading tlmgr pipe.
# but maybe we just want a boolean whether or not to write
# to the logs notebook.
# consider writing log to file, always or on demand

# In init mode, it is invoked by run_cmds, otherwise by read_line

## template for pipe callback:
#proc template_cb {mode {l ""}} {
#  if {$mode eq "line"} {
#    # do something
#  } elseif {$mode eq "init"} {
#    # do something
#  } elseif {$mode eq "finish"} {
#    # do something BUT DO NOT TRIGGER ANOTHER EVENT LOOP
#  } else {
#    lappend ::err_log "Illegal call of whatever_cb"
#    err_exit
#  }
#}

proc log_widget_cb {mode {l ""}} {
  if {$mode eq "line"} {
    .tllg.log.tx configure -state normal
    .tllg.log.tx insert end "$l\n"
    if {$::tcl_platform(os) ne "Darwin"} {
      .tllg.log.tx configure -state disabled
    }
  } elseif {$mode eq "init"} {
    show_logs
    .tllg.status configure -text "Running"
    .tllg.close configure -state disabled
  } elseif {$mode eq "finish"} {
    .tllg.log.tx yview moveto 1
    .tllg.logs select .tllg.log
    # error log on top if it contains anything
    show_err_log
    if {$::tcl_platform(os) ne "Darwin"} {
      .tllg.log.tx configure -state disabled
    }
    .tllg.status configure -text "Idle"
    .tllg.close configure -state !disabled
    # the caller, read_line, will set ::done_waiting after
    # this callback returns from finish mode
  } else {
    lappend ::err_log "Illegal call of log_widget_cb"
    err_exit
  }
} ; # log_widget_cb

##### Handling package info #####

# what invokes what?
# The main 'globals' are:

# ::have_remote is initialized to false. It is set to true by
# get_packages_info_remote, and remains true except temporarily at
# a change of repository.

# The other globals ones are ::n_updates, ::need_update_tlmgr and
# ::tlshell_updatable. These are initially set to 0 and re-calculated
# by update_globals.

# update_globals is invoked by get_packages_info_remote and
# update_local_revnumbers. It enables and disables buttons as appropriate.

# displayed global status info is updated by update_globals.
# update button/menu states are set at initialization and updated
# by update_globals, both via the enable_menu_controls proc

# get_packages_info_local is invoked only once, at initialization.  After
# installations and removals, the collected information is updated by
# update_local_revnumbers.

# get_packages_info_remote will be invoked by collect_filtered if
# ::have_remote is false. Afterwards, ::have_remote will be true, and
# therefore get_packages_info_remote will not be called again.
# get_packages_info_remote invokes update_globals.

# update_local_revnumbers will be invoked after any updates. It also
# invokes update_globals.

# collect_filtered does not only filter, but also organize the
# information to be displayed.  If necessary, it invokes
# get_packages_info_remote and always invokes display_packes_info.
# It is invoked at initialization, when filtering options change and
# at the end of install-, remove- and update procs.

# display_packages_info is mostly invoked by collect_filtered, but
# also when the search term or the search option changes.

proc check_tlmgr_updatable {} {
  run_cmd_waiting "update --self --list"
  foreach l $::out_log {
    if [regexp {^total-bytes[ \t]+([0-9]+)$} $l m b] {
      do_debug "matches, $b"
      set ::need_update_tlmgr [expr {$b > 0 ? 1 : 0}]
      return
    }
  }
  do_debug "check_tlmgr_uptodate: should not get here"
} ; # check_tlmgr_uptodate

proc is_updatable {nm} {
  set pk [dict get $::pkgs $nm]
  set lr [dict get $pk localrev]
  set rr [dict get $pk remoterev]
  return [expr {$lr > 0 && $rr > 0 && $rr > $lr}]
}

proc update_globals {} {
  if {! $::have_remote} return
  set ::n_updates 0
  foreach nm [dict keys $::pkgs] {
    if [is_updatable $nm] {incr ::n_updates}
  }
  check_tlmgr_updatable
  set ::tlshell_updatable [is_updatable tlshell]

  # also update displayed status info
  if {$::have_remote && $::need_update_tlmgr} {
    .topf.luptodate configure -text "Needs updating"
  } elseif $::have_remote {
    .topf.luptodate configure -text "Up to date"
  } else {
    .topf.luptodate configure -text "Unknown"
  }
  # ... and status of update buttons
  enable_menu_controls 1
}

# The package display treeview widget in the main window has columns
# for both local and remote revision numbers.
# It gets its data from $::filtered rather than directly from $::pkgs.
# ::pkgs should already be up to date.

# I added a field 'marked' to ::pkgs. It is displayed in the first treeview
# column. This looks better than the treeview tag facilities.
# In ::pkgs, 'marked' is represented as a boolean.
# In ::filtered and .pkglist they are represented as unicode symbols.

# display packages obeying both filter and search string.
# even on a relatively slow system, regenerating .pkglist from ::filtered
# at every keystroke is acceptably responsive.
# with future more advanced search options, this scheme may not suffice.

proc display_packages_info {} {
  # do_debug [get_stacktrace]
  set curr [.pksearch.e get]
  .pkglist delete [.pkglist children {}]
  dict for {nm pk} $::filtered {
    set do_show 0
    if {$curr eq ""} {
      set do_show 1
    } elseif {[search_nocase $curr $nm] >= 0} {
      set do_show 1
    } elseif {$::search_desc && \
          [search_nocase $curr [dict get $::pkgs $nm shortdesc]] >= 0} {
      set do_show 1
    }
    if $do_show {
      .pkglist insert {} end -id $nm -values $pk
    }
  }
} ; # display_packages_info

# (re)create ::filtered dictionary; disregard search string.
# The value associated with each key/package is a list, not another dict.
proc collect_filtered {} {
  do_debug \
      "collect_filtered for $::stat_opt and $::dtl_opt"
  if {$::stat_opt ne "inst" && ! $::have_remote} {
    get_packages_info_remote
  }
  foreach nm [dict keys $::filtered] {
    dict unset ::filtered $nm
  }
  foreach nm [lsort [dict keys $::pkgs]] {
    set pk [dict get $::pkgs $nm]
    set do_show 1
    set mrk [mark_sym [dict get $pk marked]]
    set lr [dict get $pk localrev]
    set rr [dict get $pk remoterev]
    set ct [dict get $pk category]
    if {$::stat_opt eq "inst" && $lr == 0} {
      set do_show 0
    } elseif {$::stat_opt eq "upd" && ($lr == 0 || $rr == 0 || $rr <= $lr)} {
      set do_show 0
    }
    if {! $do_show} continue
    if {$::dtl_opt eq "schm" && $ct ne "Scheme"} {
      set do_show 0
    } elseif {$::dtl_opt eq "coll" && \
        $ct ne "Scheme" && $ct ne "Collection"} {
      set do_show 0
    }
    if {! $do_show} continue

    # collect data to be displayed for $nm
    dict lappend ::filtered $nm $mrk
    dict lappend ::filtered $nm $nm
    set v [dict get $pk localrev]
    set lv [dict get $pk lcatv]
    if {$v eq "0" || $v == 0} {
      set v ""
    } elseif {$lv != 0 && $lv ne ""} {
      set v "$v ($lv)"
    }
    dict lappend ::filtered $nm $v
    set v [dict get $pk remoterev]
    set rv [dict get $pk rcatv]
    if {$v eq "0" || $v == 0} {
      set v ""
    } elseif {$rv != 0 && $rv ne ""} {
      set v "$v ($rv)"
    }
    dict lappend ::filtered $nm $v
    dict lappend ::filtered $nm [dict get $pk shortdesc]
  }
  display_packages_info
} ; # collect_filtered

# get fresh package list. invoked at program start
# some local packages may not be available online.
# to test, create local dual-platform installation from dvd, try to update
# from more recent linux-only installation

proc get_packages_info_local {} {
  # start from scratch; see also update_local_revnumbers
  foreach nm [dict keys $::pkgs] {
    dict unset ::pkgs $nm
  }
  set ::have_remote 0
  set ::need_update_tlmgr 0
  set ::updatable 0
  set ::tlshell_updatable 0

  run_cmd_waiting \
    "info --only-installed --data name,localrev,cat-version,category,shortdesc"
  set re {^([^,]+),([0-9]+),([^,]*),([^,]*),(.*)$}
  foreach l $::out_log {
    if [regexp $re $l m nm lrev lcatv catg pdescr] {
      # double-quotes in short description: remove outer, unescape inner
      if {[string index $pdescr 0] eq "\""} {
        set pdescr [string range $pdescr 1 end-1]
      }
      set pdescr [string map {\\\" \"} $pdescr]
      dict set ::pkgs $nm \
          [list "marked" 0 "localrev" $lrev "lcatv" $lcatv "remoterev" 0 \
               "rcatv" 0 "category" $catg shortdesc $pdescr]
    }
  }
} ; # get_packages_info_local

# remote: preserve information on installed packages
proc get_packages_info_remote {} {
  # remove non-local database entries
  foreach k [dict keys $::pkgs] {
    if {! [dict get $::pkgs $k localrev]} {
      dict unset ::pkgs $k
    }
  }
  set ::need_update_tlmgr 0
  set ::updatable 0
  set ::tlshell_updatable 0

  if [catch {run_cmd_waiting \
    "info --data name,localrev,remoterev,cat-version,category,shortdesc"}] {
    do_debug [get_stacktrace]
    tk_messageBox -message \
        "Repository $::repo unavailable. Please choose another one."
    return 0
  }
  set re {^([^,]+),([0-9]+),([0-9]+),([^,]*),([^,]*),(.*)$}
  foreach l $::out_log {
    if [regexp $re $l m nm lrev rrev rcatv catg pdescr] {
      # double-quotes in short description: remove outer, unescape inner
      if {[string index $pdescr 0] eq "\""} {
        set pdescr [string range $pdescr 1 end-1]
      }
      set pdescr [string map {\\\" \"} $pdescr]
      if [catch {dict get $::pkgs $nm} pk] {
        # package entry does not yet exist, create it
        dict set ::pkgs $nm [list "marked" 0 "localrev" 0 "lcatv" 0]
      }
      dict set ::pkgs $nm "remoterev" $rrev
      dict set ::pkgs $nm "rcatv" $rcatv
      dict set ::pkgs $nm "category" $catg
      dict set ::pkgs $nm "shortdesc" $pdescr
    }
  }
  set ::have_remote 1
  .topf.loaded configure -text "Loaded"
  update_globals
  return 1
} ; # get_packages_info_remote

## update ::pkgs after installing packages without going online again.
proc update_local_revnumbers {} {
  do_debug "update_local_revnumbers"
  run_cmd_waiting "info --only-installed --data name,localrev,cat-version"
  set re {^([^,]+),([0-9]+),(.*)$}
  dict for {pk pk_dict} $::pkgs {
    do_debug "zeroing local data for $pk"
    dict set pk_dict "localrev" 0
    dict set pk_dict "lcatv" 0
    dict set ::pkgs $pk $pk_dict
  }
  foreach l $::out_log {
    if [regexp $re $l m pk lr lv] {
      set pk_dict [dict get $::pkgs $pk]
      dict set pk_dict "localrev" $lr
      dict set pk_dict "lcatv" $lv
      dict set ::pkgs $pk $pk_dict
    }
  }
  update_globals
} ; # update_local_revnumbers

##### Dialogs and their supporting procs ##############################

# look at dialog.tcl, part of Tk itself, how to implement dialog-type behavior

# So far:
# - logs notebook,
# - maybe a toplevel for restoring packages from backup, and
# - a toplevel for picking a different local or remote repository.

##### logs notebook #####

# if invoked via log_widget_cb init, it tracks progress of a tlmgr command.
# log_widget_cb will temporarily disable the close button
# and set .tllg.status to busy.
# otherwise, it shows the output of the last completed (list of) command(s).

# Note that run_cmds clears ::out_log and ::err_log, but not ::dbg_log.

proc show_logs {} {
  toplevel .tllg -class Dialog
  wm withdraw .tllg
  set p [winfo toplevel [winfo parent .tllg]]
  wm transient .tllg $p
  wm title .tllg Logs
  if $::plain_unix {wm attributes .tllg -type dialog}

  # wallpaper
  pack [ttk::frame .tllg.bg] -fill both -expand 1

  ttk::frame .tllg.log
  pack [ttk::scrollbar .tllg.log.scroll -command ".tllg.log.tx yview"] \
      -side right -fill y
  ppack [text .tllg.log.tx -height 10 -wrap word \
      -yscrollcommand ".tllg.log.scroll set"] \
      -expand 1 -fill both
  .tllg.log.tx configure -state normal
  foreach l $::out_log {
    .tllg.log.tx insert end "$l\n"
  }
  if {$::tcl_platform(os) ne "Darwin"} {.tllg.log.tx configure -state disabled}
  .tllg.log.tx yview moveto 1

  ttk::frame .tllg.err
  pack [ttk::scrollbar .tllg.err.scroll -command ".tllg.err.tx yview"] \
      -side right -fill y
  ppack [text .tllg.err.tx -height 10 -wrap word \
      -yscrollcommand ".tllg.err.scroll set"] \
      -expand 1 -fill both
  .tllg.err.tx configure -state normal
  foreach l $::err_log {
    .tllg.err.tx configure -state normal
    .tllg.err.tx insert end "$l\n"
  }
  if {$::tcl_platform(os) ne "Darwin"} {.tllg.err.tx configure -state disabled}
  .tllg.err.tx yview moveto 1

  if $::ddebug {
    ttk::frame .tllg.dbg
    pack [ttk::scrollbar .tllg.dbg.scroll -command ".tllg.dbg.tx yview"] \
        -side right -fill y
    ppack [text .tllg.dbg.tx -height 10 -bd 2 -relief groove -wrap word \
        -yscrollcommand ".tllg.dbg.scroll set"] \
        -expand 1 -fill both
    .tllg.dbg.tx configure -state normal
    foreach l $::dbg_log {
      .tllg.dbg.tx insert end "$l\n"
    }
    if {$::tcl_platform(os) ne "Darwin"} {.tllg.dbg.tx configure -state disabled}
    .tllg.dbg.tx yview moveto 1
  }

  pack [ttk::notebook .tllg.logs] -in .tllg.bg -side top -fill both -expand 1
  .tllg.logs add .tllg.log -text "Output"
  .tllg.logs add .tllg.err -text "Errors"
  if $::ddebug {
    .tllg.logs add .tllg.dbg -text "Debug"
    raise .tllg.dbg .tllg.logs
  }
  raise .tllg.err .tllg.logs
  raise .tllg.log .tllg.logs

  pack [ttk::frame .tllg.bottom] -in .tllg.bg -side bottom -fill x
  ttk::button .tllg.close -text close -command {
    set p [winfo toplevel [winfo parent .tllg]]
    if {$p eq ""} {set p "."}
    raise $p; destroy .tllg}
  ppack .tllg.close -in .tllg.bottom -side right -anchor e
  ppack [ttk::label .tllg.status -anchor w] -in .tllg.bottom -side left

  place_dlg .tllg $p
  wm resizable .tllg 1 1
} ; # show_logs

##### repositories #####

proc get_repo {} {
  run_cmd_waiting "option repository"
  # this returns the configured repository.
  # for now, do not support a temporary change.
  set re {repository\t(.*)$}
  foreach l $::out_log {
    if [regexp $re $l m ::repo] break
  }
} ; # get_repo

proc is_repo_local {r} {
  set db [file join $r "tlpkg/texlive.tlpdb"]
  return [file exists $db]
}

### mirrors

proc edit_name {n} {
  set n [string tolower $n]
  set n [string map {" "  "_"} $n]
  return $n
} ; # edit_name

set mirrors [dict create]
proc read_mirrors {} {
  if [catch {open [file join [exec kpsewhich -var-value SELFAUTOPARENT] \
                       "tlpkg/installer/ctan-mirrors.pl"] r} fm] {return 0}
    set re_geo {^\s*'([^']+)' => \{\s*$}
  set re_url {^\s*'(.*)' => ([0-9]+)}
  set re_clo {^\s*\},?\s*$}
  set starting 1
  set lnum 0 ; # line number for error messages
  set ok 1 ; # no errors encountered yet
  set countries {} ; # aggregate list of countries
  set urls {} ; # aggregate list of urls
  set continent ""
  set country ""
  set u ""
  set in_cont 0
  set in_coun 0
  while {! [catch {chan gets $fm} line] && ! [chan eof $fm]} {
    incr lnum
    if $starting {
      if {[string first "\$mirrors =" $line] == 0} {
        set starting 0
        continue
      } else {
        set ok 0
        set msg "Unexpected line '$line' at start"
        break
      }
    }
    # starting is now dealt with.
    if [regexp $re_geo $line dummy c] {
      if {! $in_cont} {
        set in_cont 1
        set continent $c
        set cont_dict [dict create]
        if {$continent in [dict keys $::mirrors]} {
          set ok 0
          set msg "Duplicate continent $c at line $lnum"
          break
        }
      } elseif {! $in_coun} {
        set in_coun 1
        set country $c
        if {$country in $countries} {
          set ok 0
          set msg "Duplicate country $c at line $lnum"
          break
        }
        lappend countries $country
        dict set cont_dict $country {}
      } else {
        set ok 0
        set msg "Unexpected continent- or country line $line at line $lnum"
        break
      }
    } elseif [regexp $re_url $line dummy u n] {
      if {! $in_coun} {
        set ok 0
        set msg "Unexpected url line $line at line $lnum"
        break
      } elseif {$n ne "1"} {
        continue
      }
      append u "systems/texlive/tlnet"
      if {$u in $urls} {
          set ok 0
          set msg "Duplicate url $u at line $lnum"
          break
      }
      dict lappend cont_dict $country $u
      lappend urls $u
      set u ""
    } elseif [regexp $re_clo $line] {
      if $in_coun {
        set in_coun 0
        set country ""
      } elseif $in_cont {
        set in_cont 0
        dict set ::mirrors $continent $cont_dict
        set continent ""
      } else {
        break ; # should close mirror list
      }
    } ; # ignore other lines
  }
  close $fm
  if {! $ok} {do_debug $msg}
} ; # read_mirrors

proc find_local_repo {} {
  if [is_repo_local $::new_repo] {
    set inidir $::new_repo
  } elseif [is_repo_local $::repo] {
    set inidir $::repo
  } else {
    set inidir $::env(HOME) ; # HOME also ok for windows
  }
  set ::new_repo ""
  while 1 {
    set ::new_repo [tk_chooseDirectory -initialdir $inidir -mustexist 1 \
                        -parent .tlr -title "Local repository..."]
    if {$::new_repo ne "" && ! [is_repo_local $::new_repo]} {
      tk_messageBox -message "$::new_repo not a repository" -parent .tlr
      set inidir $::new_repo
      set ::new_repo ""
      continue
    } else {
      break
    }
  }
} ; # find_local_repo

proc close_repos {} {
  raise .
  destroy .tlr
  set ::repo $::new_repo
  if {$::tcl_platform(platform) eq "windows"} {
    set ::repo [string map {\\ /} $::repo]
  }
  set ::new_repo ""
  run_cmd_waiting "option repository $::repo"
  close_tlmgr
  start_tlmgr
  # reload remote package information
  set ::have_remote 0
  get_packages_info_remote
  collect_filtered
} ; # close_repos

proc repositories {} {

  set ::new_repo $::repo

  # dialog toplevel with
  # - popup menu of mirrors (parse tlpkg/installer/ctan-mirrors.pl)
  # - text entry box
  # - directory browser button
  # - ok and cancel buttons

  toplevel .tlr -class Dialog
  wm withdraw .tlr
  wm transient .tlr .
  wm title .tlr "Repositories"
  if $::plain_unix {wm attributes .tlr -type dialog}

  # wallpaper frame; see make_widgets
  pack [ttk::frame .tlr.bg] -expand 1 -fill x

  pack [ttk::frame .tlr.info] -in .tlr.bg -fill x
  grid columnconfigure .tlr.info 1 -weight 1
  set row -1

  # current repository
  incr row
  pgrid [ttk::label .tlr.lcur -text "Current:"] \
      -in .tlr.info -row $row -column 0 -sticky w
  pgrid [ttk::label .tlr.cur -textvariable ::repo] \
      -in .tlr.info -row 0 -column 1 -sticky w
  # new repository
  incr row
  pgrid [ttk::label .tlr.lnew -text "New"] \
      -in .tlr.info -row $row -column 0 -sticky w
  pgrid [ttk::entry .tlr.new -textvariable ::new_repo -width 40] \
      -in .tlr.info -row $row -column 1 -columnspan 2 -sticky w

  ### three ways to specify a repository ###
  pack [ttk::frame .tlr.mirbuttons] -in .tlr.bg -fill x
  # default remote repository
  ttk::button .tlr.ctan -text "Any CTAN mirror" \
      -command {set ::new_repo "http://mirror.ctan.org/systems/texlive/tlnet"}
  ppack .tlr.ctan -in .tlr.mirbuttons -side left -fill x
  # freshly create a cascading mirror dropdown menu
  destroy .tlr.mir.m
  if {[dict size $::mirrors] == 0} read_mirrors
  if {[dict size $::mirrors] > 0} {
    ttk::menubutton .tlr.mir -text "Specific mirror..." -direction below \
      -menu .tlr.mir.m
    ppack .tlr.mir -in .tlr.mirbuttons -side left -fill x
    menu .tlr.mir.m
    dict for {cont d_cont} $::mirrors {
      set c_ed [edit_name $cont]
      menu .tlr.mir.m.$c_ed
      .tlr.mir.m add cascade -label $cont -menu .tlr.mir.m.$c_ed
      dict for {cntr urls} $d_cont {
        set n_ed [edit_name $cntr]
        menu .tlr.mir.m.$c_ed.$n_ed
        .tlr.mir.m.$c_ed add cascade -label $cntr -menu .tlr.mir.m.$c_ed.$n_ed
        foreach u $urls {
          .tlr.mir.m.$c_ed.$n_ed add command -label $u \
              -command "set ::new_repo $u"
        }
      }
    }
  }
  # local repository
  ttk::button .tlr.browse -text "Local directory..." \
      -command find_local_repo
  ppack .tlr.browse -in .tlr.mirbuttons -side left -fill x

  spacing .tlr.bg

  # two ways to close the dialog
  pack [ttk::frame .tlr.closebuttons] -in .tlr.bg -fill x
  ttk::button .tlr.save -text "Save and Load" -command close_repos
  ppack .tlr.save -in .tlr.closebuttons -side right
  ttk::button .tlr.abort -text "Abort"\
      -command {raise .; destroy .tlr}
  ppack .tlr.abort -in .tlr.closebuttons -side right

  place_dlg .tlr .
  wm resizable .tlr 0 0
} ; # repositories

### platforms

if {$::tcl_platform(platform) ne "windows"} {

  set ::platforms {}

  proc toggle_pl_marked {pl cl} {
    # toggle_pl_marked is triggered by a mouse click only in column #1.
    # 'fut' should get updated in ::platforms and in .tlpl.pl.

    if {$cl ne "#1"} return
    if {$pl eq $::our_platform} {
      tk_messageBox -message "Cannot remove own platform $::our_platform" \
          -parent .tlpl
      return
    }
    # $mrk: negation of current value of marked for $pl
    set m1 [expr {[dict get $::platforms $pl "fut"] ? 0 : 1}]
    dict set ::platforms $pl "fut" $m1
    set m0 [dict get $::platforms $pl "cur"]
    if {$m0 == $m1} {
      .tlpl.pl set $pl "sup" [mark_sym $m0]
    } else {
      .tlpl.pl set $pl "sup" "[mark_sym $m0] \u21d2 [mark_sym $m1]"
    }
    .tlpl.do configure -state disabled
    dict for {p mrks} $::platforms {
      if {[dict get $mrks "fut"] ne [dict get $mrks "cur"]} {
        .tlpl.do configure -state !disabled
        break
      }
    }
  } ; # toggle_pl_marked

  proc platform_do {} {
    raise .
    destroy .tlpl
    set pl_add {}
    set pl_remove {}
    dict for {p pd} $::platforms {
      if {[dict get $pd "cur"] ne [dict get $pd "fut"]} {
        if {[dict get $pd "fut"]} {
          lappend pl_add $p
        } else {
          lappend pl_remove $p
        }
      }
    }
    if {[llength $pl_add] == 0 && [llength $pl_remove] == 0} return
    set cmds {}
    if {[llength $pl_add] > 0} {
      set cmd "platform add "
      append cmd [join $pl_add " "]
      lappend cmds $cmd
    }
    if {[llength $pl_remove] > 0} {
      set cmd "platform remove "
      append cmd [join $pl_remove " "]
      lappend cmds $cmd
    }
    run_cmds $cmds log_widget_cb
    vwait ::done_waiting
    update_local_revnumbers
    collect_filtered

  } ; # platform_do

  proc platform_select {} {
    run_cmd_waiting "platform list"
    set ::platforms {}
    foreach l $::out_log {
      if [regexp {^\s+(\S+)$} $l m p] {
        dict set ::platforms $p {}
        dict set ::platforms $p "cur" 0
        dict set ::platforms $p "fut" 0
      } elseif [regexp {^\(i\)\s+(\S+)$} $l m p] {
        dict set ::platforms $p {}
        dict set ::platforms $p "cur" 1
        dict set ::platforms $p "fut" 1
      }
    }
    destroy .tlpl
    toplevel .tlpl -class Dialog
    wm withdraw .tlpl
    wm transient .tlpl .
    wm title .tlpl "Platforms"
    if $::plain_unix {wm attributes .tlpl -type dialog}

    # wallpaper frame
    pack [ttk::frame .tlpl.bg] -expand 1 -fill both

    # platforms treeview
    pack [ttk::frame .tlpl.fpl] -in .tlpl.bg -fill both -expand 1
    ttk::treeview .tlpl.pl -columns {sup plat} -show headings \
        -height [dict size $::platforms] ; # -yscrollcommand {.tlpl.plsb set}
    ppack .tlpl.pl -in .tlpl.fpl -side left -fill both -expand 1
    #ttk::scrollbar .tlpl.plsb -orient vertical \
    #    -command {.tlpl.pl yview}
    #ppack .tlpl.plsb -in .tlpl.fpl -side right -fill y -expand 1
    foreach col {sup plat} nm {"" "Platform"} {
      .tlpl.pl heading $col -text $nm -anchor w
    }
    .tlpl.pl column sup -width [expr {$::cw * 6}]
    .tlpl.pl column plat -width [expr {$::cw * 20}]
    dict for {p mks} $::platforms {
      .tlpl.pl insert {} end -id $p -values \
          [list [mark_sym [dict get $mks "cur"]] $p]
    }

    # "#2" refers to the second column, with editable mark symbols
    bind .tlpl.pl <space> {toggle_pl_marked [.tlpl.pl focus] "#1"}
    bind .tlpl.pl <Return> {toggle_pl_marked [.tlpl.pl focus] "#1"}
    # only toggle when column is "sup" i.e. #1
    bind .tlpl.pl <ButtonRelease-1> \
        {toggle_pl_marked \
             [.tlpl.pl identify item %x %y] \
             [.tlpl.pl identify column %x %y]}

    # buttons
    pack [ttk::frame .tlpl.but] -in .tlpl.bg -fill x
    ttk::button .tlpl.do -text "Apply and close" -command platform_do
    ttk::button .tlpl.dont -text "Close" -command \
        {raise .; destroy .tlpl}
    ppack .tlpl.do -in .tlpl.but -side right
    .tlpl.do configure -state disabled
    ppack .tlpl.dont -in .tlpl.but -side right

    place_dlg .tlpl .
    wm resizable .tlpl 0 0
  } ; # platform_select

} ; # $::tcl_platform(platform) ne "windows"

##### restore from backup #####

# This is currently rather dangerous.
# ::do_restore is set to 0 or 1 near the top of this source.

if $::do_restore {
# dictionary of backups, with mapping to list of available revisions
set bks {}

proc enable_restore {yesno} {
  set st [expr {$yesno ? !disabled : disabled}]
  .tlbk.bklist state $st
  .tlbk.all configure -state $st
  .tlbk.done configure -state $st
} ; # enable_restore

proc finish_restore {} {
  vwait ::done_waiting
  # now log_widget_cb should have done finish mode
  # and re-enabled its close button.
  # We won't wait for the log toplevel to close, but we will
  # update the packages display in the main window.
  update_local_revnumbers
  collect_filtered
} ; # finish_restore

proc restore_all {} {
  run_cmd "restore --force --all" log_widget_cb
  finish_restore
  raise .
  destroy .tlbk
} ; # restore_all

proc restore_this {} {
  set id [.tlbk.bklist focus]
  if {$id eq {}} return
  set r [.tlbk.bklist set $id rev]
  if {$r eq {}} return
  set p [.tlbk.bklist set $id pkg]
  if {$p eq {}} {
    set id [.tlbk.bklist parent $id]
    if {$id ne {}} {set p [.tlbk.bklist set $id pkg]}
  }
  if {$p eq {}} return
  set ans [tk_messageBox -message "Restore $p to revision $r?" \
               -type okcancel -parent .tlbk]
  if {$ans ne {ok}} return
  run_cmd "restore --force $p $r" log_widget_cb
  finish_restore
  # tkwait window .tllg
} ; # restore_this

proc bklist_callback_click {x y} {
  set cl [.tlbk.bklist identify column $x $y]
  if {$cl eq "#0"} return
  set id [.tlbk.bklist identify item $x $y]
  .tlbk.bklist focus $id
  restore_this
} ; # bklist_callback_click

proc restore_backups_dialog {} {
  run_cmd_waiting "option autobackup"
  set re {autobackup\t(.*)$}
  set abk 0
  foreach l $::out_log {
    if [regexp $re $l m abk] break
  }
  if {$abk == 0} {
    tk_messageBox -message "No backups configured"
    return
  }
  run_cmd_waiting "option backupdir"
  set re {backupdir\t(.*)$}
  set bdir ""
  foreach l $::out_log {
    if [regexp $re $l m bdir] break
  }
  if {$bdir eq ""} {
    tk_messageBox -message "No backup directory defined"
    return
  }
  set bdir [file join [exec kpsewhich -var-value SELFAUTOPARENT] $bdir]
  if {! [file isdirectory $bdir]} {
    tk_messageBox -message "Backup directory $bdir does not exist"
    return
  }
  set pwd0 [pwd]
  cd $bdir
  set backups [lsort [glob *.tar.xz]]
  if {[llength $backups] == 0} {
    tk_messageBox -message "No backups found in $bdir"
    return
  }
  # dictionary of backups; package => list of available revisions
  set ::bks [dict create]
  set re {^(.*)\.r(\d+)\.tar\.xz$}
  foreach b $backups {
    if [regexp $re $b m pk r] {
      if {$pk in [dict keys $::bks]} {
        dict lappend ::bks $pk $r
      } else {
        dict set ::bks $pk [list $r]
      }
    }
  }
  if {[llength [dict keys $::bks]] == 0} {
    tk_messageBox -message "No packages in backup directory $bdir"
    return
  }
  # invert sort order of revisions for each package
  foreach pk [dict keys $::bks] {
    dict set ::bks $pk [lsort -decreasing [dict get $::bks $pk]]
  }
  toplevel .tlbk -class Dialog
  wm withdraw .tlbk
  wm transient .tlbk .
  wm title .tlbk "Restore from backup"
  if $::plain_unix {wm attributes .tlbk -type dialog}

  # wallpaper frame; see make_widgets
  pack [ttk::frame .tlbk.bg] -expand 1 -fill x

  # the displayed list of backed-up packages
  pack [ttk::frame .tlbk.fbk] -in .tlbk.bg -side top
  # package names not in #0, because we want to trigger actions
  # without automatically opening a parent.
  pack [ttk::treeview .tlbk.bklist -columns {"pkg" "rev"} \
            -show {tree headings} -height 8 -selectmode browse \
            -yscrollcommand {.tlbk.bkvsb set}] -in .tlbk.fbk -side left
  pack [ttk::scrollbar .tlbk.bkvsb -orient vertical -command \
            {.tlbk.bklist yview}] -in .tlbk.fbk -side right -fill y

  foreach col {"pkg" "rev"} nm {"Package" "Revision"} {
    .tlbk.bklist heading $col -text $nm -anchor w
  }
  .tlbk.bklist column "#0" -width [expr {$::cw * 2}]
  .tlbk.bklist column "pkg" -width [expr {$::cw * 25}]
  .tlbk.bklist column "rev" -width [expr {$::cw * 12}]

  # fill .tlbk.bklist. Use the last available revision.
  # Remember that $::bks is sorted and revisions inversely sorted
  # id must be unique in the entire list: rev does not qualify
  # must as well use $id for package items too
  set id 0
  dict for {pk rlist} $::bks {
    # package
    .tlbk.bklist insert {} end -id [incr id] \
        -values [list $pk [lindex $rlist 0]] -open 0
    set l [llength $rlist]
    # additional revisions
    if {$l > 1} {
      set idparent $id
      for {set i 1} {$i<$l} {incr i} {
        .tlbk.bklist insert $idparent end -id [incr id] \
            -values [list "" [lindex $rlist $i]]
      }
    }
  }

  # since we can only restore one non-last revision at a time, it is better
  # to only show one non-last revision at a time too.
  bind .tlbk.bklist <<TreeviewOpen>> {
    foreach p [.tlbk.bklist children {}] {
      if {$p ne [.tlbk.bklist focus]} {
        .tlbk.bklist item $p -open 0
      }
    }
  }
  # restoring a single package
  bind .tlbk.bklist <<RightClick>> {bklist_callback_click %x %y}
  bind .tlbk.bklist <space> restore_this

  # frame with buttons
  pack [ttk::frame .tlbk.fbut] -in .tlbk.bg -side bottom -fill x
  ppack [ttk::button .tlbk.all -text "Restore all" -command restore_all] \
        -in .tlbk.fbut -side right
  ppack [ttk::button .tlbk.done -text "Close" \
             -command {raise .; destroy .tlbk}] -in .tlbk.fbut -side right

  place_dlg .tlbk .
  wm resizable .tlbk 0 0
} ; # restore_backups_dialog

} ; # if $::do_restore

##### Main window and supporting procs and callbacks ##################

##### package-related #####

proc update_tlmgr {} {
  if {! $::need_update_tlmgr} {
    tk_messageBox -message "Nothing to do!"
    return
  }
  run_cmd "update --self" log_widget_cb
  vwait ::done_waiting
  # tlmgr restarts itself automatically
  update_local_revnumbers
  collect_filtered
} ; # update_tlmgr

proc update_all {} {
  if $::need_update_tlmgr {
    tk_messageBox -message "Update self first!"
    return
  } elseif {! $::n_updates} {
    tk_messageBox -message "Nothing to do!"
    return
  }
  run_cmd "update --all" log_widget_cb
  vwait ::done_waiting
  update_local_revnumbers
  collect_filtered
} ; # update_all

### doing something with some packages

proc pkglist_from_option {opt {pk ""}} {
  if {$opt eq "marked"} {
    set pks {}
    dict for {p props} $::pkgs {
      if [dict get $props "marked"] {lappend pks $p}
    }
  } elseif {$opt eq "focus"} {
    set p [.pkglist focus]
    if {$p ne {}} {lappend pks $p}
  } elseif {$opt eq "name"} {
    lappend pks $pk
  }
  return $pks
} ; # pkglist_from_option

proc install_pkgs {sel_opt {pk ""}} {
  set pks [pkglist_from_option $sel_opt $pk]
  # check whether packages are installed
  set pre_installed {}
  set todo {}
  foreach p $pks {
    if {[dict get $::pkgs $p localrev] > 0} {
      lappend pre_installed $p
    } else {
      lappend todo $p
    }
  }
  if {[llength $todo] == 0} {
    tk_messageBox -message "Nothing to do!" -type ok -icon info
    return
  }
  run_cmd_waiting "install --dry-run $todo"
  # check whether dependencies are going to be installed
  set r {^(\S+)\s+i\s}
  set deps {}
  foreach l $::out_log {
    if {[regexp $r $l d p] && $p ni $pks} {
      lappend deps $p
    }
  }
  if {[llength $deps] > 0} {
    set ans [any_message \
       "Also installing dependencies\n\n$deps.\n\nContinue?" "okcancel"]
    if {$ans eq "cancel"} return
  }
  run_cmd "install $todo" log_widget_cb
  vwait ::done_waiting
  if {[llength $pre_installed] > 0} {
    lappend ::err_log "Already installed: $pre_installed"
    show_err_log
  }
  update_local_revnumbers
  collect_filtered
} ; # install_pkgs

proc update_pkgs {sel_opt {pk ""}} {
  set pks [pkglist_from_option $sel_opt $pk]
  # check whether packages are installed
  set not_inst {}
  set uptodate {}
  set todo {}
  foreach p $pks {
    set lv [dict get $::pkgs $p localrev]
    if {[dict get $::pkgs $p localrev] == 0} {
      lappend not_inst $p
    } else {
      set rv [dict get $::pkgs $p remoterev]
      if {$lv >= $rv} {
        lappend uptodate $p
      } else {
        lappend todo $p
      }
    }
  }
  if {[llength $todo] == 0} {
    tk_messageBox -message "Nothing to do!" -type ok -icon info
    return
  }
  run_cmd_waiting "update --dry-run $todo"
  # check whether dependencies are going to be updated
  set r {^(\S+)\s+u\s}
  set deps {}
  foreach l $::out_log {
    if {[regexp $r $l d p] && $p ni $pks} {
      lappend deps $p
    }
  }
  if {[llength $deps] > 0} {
    set ans [any_message "Also updating dependencies\n\n$deps?" \
       "yesnocancel"]
    switch $ans {
      "cancel" return
      "yes" {run_cmd "update $todo" log_widget_cb}
      "no" {
        set deps {}
        run_cmd_waiting "update --dry-run --no-depends $todo"
        foreach l $::out_log {
          if {[regexp $r $l u p] && $p ni $pks} {
            lappend deps $p
          }
        }
        if {[llength $deps] > 0} {
          set ans [any_message \
                       "Updating hard dependencies $deps anyway. Continue?" \
                       "okcancel"]
          if {$ans eq "cancel"} return
        }
        run_cmd "update --no-depends $todo" log_widget_cb
      }
    }
  } else {
    run_cmd "update $todo" log_widget_cb
  }
  vwait ::done_waiting
  if {[llength $not_inst] > 0} {
    lappend ::err_log "Skipped because not installed: $not_inst"
  }
  if {[llength $uptodate] > 0} {
    lappend ::err_log "Skipped because already up to date: $uptodate"
  }
  if {[llength $not_inst] > 0 || [llength $uptodate] > 0} {
    show_err_log
  }
  update_local_revnumbers
  collect_filtered
} ; # update_pkgs

proc remove_pkgs {sel_opt {pk ""}} {
  set pks [pkglist_from_option $sel_opt $pk]
  # check whether packages are installed
  set not_inst {}
  set todo {}
  foreach p $pks {
    if {[dict get $::pkgs $p localrev] > 0} {
      lappend todo $p
    } else {
      lappend not_inst $p
    }
  }
  run_cmd_waiting "remove --dry-run $todo"
  # check whether dependencies are going to be removed
  set r {^(\S+)\s+d\s}
  set deps {}
  foreach l $::out_log {
    if {[regexp $r $l d p] && $p ni $pks} {
      lappend deps $p
    }
  }
  if {[llength $todo] == 0} {
    tk_messageBox -message "Nothing to do!" -type ok -icon info
    return
  }
  if {[llength $deps] > 0} {
    set ans [any_message "Also remove dependencies\n\n$deps?" \
                "yesnocancel"]
    switch $ans {
      "cancel" return
      "yes" {run_cmd "remove $todo" log_widget_cb}
      "no" {
        set deps {}
        run_cmd_waiting "remove --dry-run --no-depends $todo"
        foreach l $::out_log {
          if {[regexp $r $l d p] && $p ni $pks} {
            lappend deps $p
          }
        }
        if {[llength $deps] > 0} {
          set ans [any_message \
                       "Removing hard dependencies $deps anyway. Continue?" \
                       "okcancel"]
          if {$ans eq "cancel"} return
        }
        run_cmd "remove --no-depends $todo" log_widget_cb
      }
    }
  } else {
    run_cmd "remove $todo" log_widget_cb
  }
  vwait ::done_waiting
 if {[llength $not_inst] > 0} {
    lappend ::err_log "Skipped because not installed: $not_inst"
    show_err_log
  }
  update_local_revnumbers
  collect_filtered
} ; # remove_pkgs

# restoring packages is a rather different story, controlled by the
# contents of the backup directory. see further up.

##### varous callbacks #####

proc run_entry {} {
  # TODO: some validation of $cmd
  do_debug "run_entry"
  set cmd [.ent.e get]
  if {$cmd eq ""} return
  do_debug $cmd
  .ent.e delete 0 end
  run_cmd $cmd log_widget_cb
}

proc restart_self {} {
  do_debug "trying to restart"
  if {$::progname eq ""} {
    tk_messageBox -message "progname not found; not restarting"
    return
  }
  close_tlmgr
  exec $::progname &
  # on windows, it may take several seconds before
  # the old tlshell disappears.
  # oh well, windows is still windows....
  destroy .
} ; # restart_self

proc toggle_marked {itm cl} {
  # toggle_marked is triggered by a mouse click only in column #1.
  # 'marked' should get updated in ::pkgs, ::filtered and in .pkglist.

  if {$cl ne "#1"} return
  # $mrk: negation of current value of marked for $itm
  set mrk [expr {[dict get $::pkgs $itm "marked"] ? 0 : 1}]
  dict set ::pkgs $itm "marked" $mrk
  set m [mark_sym $mrk]
  dict set ::filtered $itm [lreplace [dict get $::filtered $itm] 0 0 $m]
  .pkglist set $itm mk $m
} ; # toggle_marked

proc mark_all {mrk} {
  foreach nm [dict keys $::pkgs] {
    dict set ::pkgs $nm "marked" $mrk
  }
  set m [mark_sym $mrk]
  foreach nm [dict keys $::filtered] {
    dict set ::filtered $nm [lreplace [dict get $::filtered $nm] 0 0 $m]
  }
  foreach nm [.pkglist children {}] {
    .pkglist set $nm mk $m
  }
  # alternatively: regenerate ::filtered and .pkglist from ::pkgs
} ; # mark_all

proc toggle_search_desc {} {
  # when this proc is called, ::search_desc is not yet toggled
  # so we temporarily pre-toggle and post-untoggle it
  set ::search_desc [expr {$::search_desc ? 0 : 1}]
  display_packages_info
  set ::search_desc [expr {$::search_desc ? 0 : 1}]
}

##### package popup #####

proc do_package_popup_menu {x y X Y} {
  # as focused item, the identity of the clicked item will be
  # globally available:
  .pkglist focus [.pkglist identify item $x $y]
  # recreate menu with only applicable items
  set lr [dict get $::pkgs [.pkglist focus] "localrev"]
  set rr [dict get $::pkgs [.pkglist focus] "remoterev"]
  .pkg_popup delete 0 end
  .pkg_popup add command -label "Info" -command \
      {run_cmd "info [.pkglist focus]" log_widget_cb; \
           vwait ::done_waiting}
  if {$::have_remote && ! $::need_update_tlmgr && $rr > 0 && $lr == 0} {
    .pkg_popup add command -label "Install" -command \
        {install_pkgs "focus"}
  }
  if {$::have_remote && ! $::need_update_tlmgr && $lr > 0 && $rr > $lr} {
    .pkg_popup add command -label "Update" -command \
        {update_pkgs "focus"}
  }
  if {$lr > 0} {
    .pkg_popup add command -label "Remove" -command \
        {remove_pkgs "focus"}
  }
  .pkg_popup post [expr {$X - 2}] [expr {$Y - 2}]
  focus .pkg_popup
} ; # do_package_popup_menu

proc set_paper {p} {
  run_cmd "paper paper $p" log_widget_cb
}

##### running external commands #####

# For capturing an external command, we need a separate output channel,
# but we reuse ::out_log.
# stderr is bundled with stdout so ::err_log should stay empty.
proc read_capt {} {
  set l "" ; # will contain the line to be read
  if {([catch {chan gets $::capt l} len] || [chan eof $::capt])} {
    catch {chan close $::capt}
    log_widget_cb "finish"
    set ::done_waiting 1
  } elseif {$len >= 0} {
    lappend ::out_log $l
    log_widget_cb "line" $l
  }
}; # read_capt

proc run_external {cmd mess} {
  set ::out_log {}
  set ::err_log {}
  lappend ::out_log $mess
  unset -nocomplain ::done_waiting
  # dont understand why, on windows, start_tlmgr does not trigger
  # a console window but this proc does
  if [catch {open "|$cmd 2>&1" "r"} ::capt] {
    tk_messageBox -message "Failure to launch $cmd"
  }
  chan configure $::capt -buffering line -blocking 0
  chan event $::capt readable read_capt
  log_widget_cb "init"
}

##### main window #####

proc make_widgets {} {

  wm title . "$::progname $::procid"

  # width of '0', as a rough estimate of average character width
  set ::cw [font measure TkTextFont "0"]

  # dummy empty menu to replace the real menu .mn in disabled states.
  # the "File" cascade should ensure that the dummy menu
  # occupies the same vertical space as the real menu.
  menu .mn_empty
  .mn_empty add cascade -label "File" -menu .mn_empty.file -underline 0
  if $::plain_unix {
    .mn_empty configure -borderwidth 1
    .mn_empty configure -background $::default_bg
  menu .mn_empty.file
  }
  # real menu
  menu .mn
  . configure -menu .mn
  if $::plain_unix {
    .mn configure -borderwidth 1
    .mn configure -background $::default_bg

    # plain_unix: avoid a RenderBadPicture error on quitting.
    # 'send' changes the shutdown sequence,
    # which avoids triggering the bug.
    # 'tk appname <something>' restores 'send' and avoids the bug
    bind . <Destroy> {
      catch {tk appname appname}
    }
  }

  .mn add cascade -label "File" -menu .mn.file -underline 0
  menu .mn.file
  .mn.file add command -label "Load default repository" \
      -command {get_packages_info_remote; collect_filtered}
  .mn.file add command -command {destroy .} -label "Exit" -underline 1

  # inx: keeping count where needed, i.e. when an entry needs to be referenced
  .mn add cascade -label "Packages" -menu .mn.pkg
  menu .mn.pkg
  set inx 0
  set ::inx_upd_tlmgr $inx
  .mn.pkg add command -label "Update tlmgr" -command update_tlmgr
  incr inx
  set ::inx_upd_all $inx
  .mn.pkg add command -label "Update all" -command update_all
  incr inx
  .mn.pkg add command -label "Install marked" \
      -command {install_pkgs "marked"}
  incr inx
  .mn.pkg add command -label "Update marked" \
      -command {update_pkgs "marked"}
  incr inx
  .mn.pkg add command -label "Remove marked" \
      -command {remove_pkgs "marked"}
  if $::do_restore {
  incr inx
  .mn.pkg add command -label "Restore from backup..." \
      -command restore_backups_dialog
  }

  .mn add cascade -label "Actions" -menu .mn.act -underline 0
  menu .mn.act
  .mn.act add command -label "Regenerate filename database" -command \
      {run_external "mktexlsr" "Regenerating filename database..."}
  .mn.act add command -label "Regenerate formats" -command \
      {run_external "fmtutil-sys --all" "Rebuilding formats..."}
  .mn.act add command -label "Regenerate fontmaps" -command \
      {run_external "updmap-sys" "Rebuilding fontmap files..."}

  .mn add cascade -label "Options" -menu .mn.opt -underline 0
  menu .mn.opt
  set inx 0
  .mn.opt add command -label "Change repository..." \
      -command repositories
  incr inx
  .mn.opt add cascade -label "Paper" -menu .mn.opt.paper
  menu .mn.opt.paper
  foreach p [list a4 letter] {
    .mn.opt.paper add command -label $p -command "set_paper $p"
  }
  if {$::tcl_platform(platform) ne "windows"} {
    incr inx
    set ::inx_platforms $inx
    .mn.opt add command -label "Platforms..." -command platform_select
  }

  .mn add cascade -label "Help" -menu .mn.help -underline 0
  menu .mn.help
  .mn.help add command -command {tk_messageBox -message "Helpless"} \
      -label "About"

  # wallpaper frame
  # it is possible to set a background color for a toplevel, but on
  # MacOS I did not find a way to determine the right $::default_bg
  # color. Instead, all toplevels are given a wallpaper ttk::frame
  # with the default ttk::frame color, which seems to work
  # everywhere.
  pack [ttk::frame .bg] -expand 1 -fill both

  # various info
  ttk::frame .topf
  pack .topf -in .bg -side top -anchor w

  pgrid [ttk::label .topf.llrepo -text "Default repository" -anchor w] \
      -row 0 -column 0 -sticky w
  pgrid [ttk::label .topf.lrepo -textvariable ::repo] \
      -row 0 -column 1 -sticky w
  pgrid [ttk::label .topf.loaded -text "Not loaded"] \
      -row 1 -column 1 -sticky w

  ttk::label .topf.lluptodate -text "TL Manager up to date?" -anchor w
  pgrid .topf.lluptodate -row 2 -column 0 -sticky w
  ttk::label .topf.luptodate -text "Unknown" -anchor w
  pgrid .topf.luptodate -row 2 -column 1 -sticky w

  pgrid [ttk::label .topf.llcmd -anchor w -text "Last tlmgr command: "] \
      -row 3 -column 0 -sticky w
  pgrid [ttk::label .topf.lcmd -anchor w -textvariable ::last_cmd] \
      -row 3 -column 1 -sticky w

  # command entry widget
  spacing .bg
  ttk::frame .ent
  ppack [ttk::label .ent.l -text "Type command:"] -side left
  ppack [ttk::entry .ent.e -width 40] -side left -padx 3
  ppack [ttk::button .ent.b -text Go -command run_entry] -side left
  bind .ent.e <Return> run_entry
  pack .ent -in .bg -fill x -side top

  spacing .bg

  # package list
  ttk::label .lpack -text "Package list" -font TkHeadingFont -anchor w
  ppack .lpack -in .bg -side top -fill x

  # controlling package list
  ttk::frame .pkfilter
  # filter on status: inst, all, upd
  ttk::label .pkfilter.lstat -font TkHeadingFont -text "Status"
  ttk::radiobutton .pkfilter.inst -text Installed -value inst \
      -variable ::stat_opt -command collect_filtered
  ttk::radiobutton .pkfilter.alls -text All -value all \
      -variable ::stat_opt -command collect_filtered
  ttk::radiobutton .pkfilter.upd -text Updatable -value upd \
      -variable ::stat_opt -command collect_filtered
  grid .pkfilter.lstat -column 0 -row 0 -sticky w -padx {3 50}
  pgrid .pkfilter.inst -column 0 -row 1 -sticky w
  pgrid .pkfilter.alls -column 0 -row 2 -sticky w
  pgrid .pkfilter.upd -column 0 -row 3 -sticky w

  # filter on detail level: all, coll, schm
  ttk::label .pkfilter.ldtl -font TkHeadingFont -text "Detail >> Global"
  ttk::radiobutton .pkfilter.alld -text All -value all \
      -variable ::dtl_opt -command collect_filtered
  ttk::radiobutton .pkfilter.coll -text "Collections and schemes" -value coll \
      -variable ::dtl_opt -command collect_filtered
  ttk::radiobutton .pkfilter.schm -text "Only schemes" -value schm \
      -variable ::dtl_opt -command collect_filtered
  pgrid .pkfilter.ldtl -column 1 -row 0 -sticky w
  pgrid .pkfilter.alld -column 1 -row 1 -sticky w
  pgrid .pkfilter.coll -column 1 -row 2 -sticky w
  pgrid .pkfilter.schm -column 1 -row 3 -sticky w

  # marks
  grid [ttk::button .mrk_all -text "Mark all" -command {mark_all 1}] \
      -in .pkfilter -column 2 -row 1 -sticky w -padx {50 3} -pady 3
  grid [ttk::button .mrk_none -text "Mark none" -command {mark_all 0}] \
      -in .pkfilter -column 2 -row 2 -sticky w -padx {50 3} -pady 3

  pack .pkfilter -in .bg -side top -fill x

  # search interface
  pack [ttk::frame .pksearch] -in .bg -side top -fill x
  ppack [ttk::label .pksearch.l \
      -text "Search package names"] \
      -side left
  pack [ttk::entry .pksearch.e -width 30] -side left -padx {3 0} -pady 3
  ppack [ttk::checkbutton .pksearch.d -variable ::search_desc \
             -text "Also search short descriptions"] -side left
  bind .pksearch.e <KeyRelease> display_packages_info
  bind .pksearch.d <ButtonRelease> toggle_search_desc

  # packages list
  pack [ttk::frame .fpkg] -in .bg -side top -fill both -expand 1
  ttk::treeview .pkglist -columns \
      {mk name localrev remoterev shortdesc} \
      -show headings -height 8 -selectmode extended \
      -xscrollcommand {.pkhsb set} -yscrollcommand {.pkvsb set}
  foreach \
      col {mk name localrev remoterev shortdesc} \
      nm {"" Name "Local Rev. (ver.)" "Remote Rev. (ver.)" Description} {
    .pkglist heading $col -text $nm -anchor w
  }
  .pkglist column mk -width [expr {$::cw * 3}]
  .pkglist column name -width [expr {$::cw * 25}]
  .pkglist column localrev -width [expr {$::cw * 18}]
  .pkglist column remoterev -width [expr {$::cw * 18}]
  .pkglist column shortdesc -width [expr {$::cw * 50}]

  ttk::scrollbar .pkhsb -orient horizontal -command {.pkglist xview}
  ttk::scrollbar .pkvsb -orient vertical -command {.pkglist yview}
  pgrid .pkglist -in .fpkg -row 0 -column 0 -sticky news
  grid .pkvsb -in .fpkg -row 0 -column 1 -sticky ns
  grid .pkhsb -in .fpkg -row 1 -column 0 -sticky ew
  grid columnconfigure .fpkg 0 -weight 1
  grid rowconfigure .fpkg 0 -weight 1

  # "#1" refers to the first column (with mark symbols)
  bind .pkglist <space> {toggle_marked [.pkglist focus] "#1"}
  bind .pkglist <Return> {toggle_marked [.pkglist focus] "#1"}
  # only toggle when column is "mk" i.e. #1
  bind .pkglist <ButtonRelease-1> {toggle_marked \
      [.pkglist identify item %x %y] [.pkglist identify column %x %y]}

  menu .pkg_popup ; # entries added on-the-fly
  bind .pkglist <<RightClick>> {do_package_popup_menu %x %y %X %Y}
  if $::plain_unix {
    bind .pkg_popup <Leave> {.pkg_popup unpost}
  }

  # bottom of main window
  pack [ttk::frame .endbuttons] -in .bg -side bottom -fill x
  ttk::label .busy -textvariable ::busy -font TkHeadingFont -anchor w
  ppack .busy -in .endbuttons -side left
  ppack [ttk::button .q -text Quit -command {destroy .}] \
      -in .endbuttons -side right
  ppack [ttk::button .r -text "Restart self" -command restart_self] \
      -in .endbuttons -side right
  ppack [ttk::button .t -text "Restart tlmgr" \
             -command {close_tlmgr; start_tlmgr}] \
      -in .endbuttons -side right
  ttk::button .showlogs -text "Show logs" -command show_logs
  ppack .showlogs -in .endbuttons -side right
} ; # make_widgets

##### initialize ######################################################

proc initialize {} {
  # seed random numbers
  expr {srand([clock seconds])}
  # prepend TL to process searchpath (not needed on windows)
  if {$::tcl_platform(platform) ne "windows"} {
    set texbin [file dirname [info script]]
    set savedir [pwd]
    cd $texbin
    set texbin [pwd]
    cd $savedir
    # prepend texbin to PATH, unless it is already the _first_
    # path component
    if {$::tcl_platform(platform) eq "unix"} {
      set pathsep ":"
    } else {
      set pathsep ";"
    }
    set dirs [split $::env(PATH) $pathsep]
    if {[lindex $dirs 0] ne $texbin} {
      set ::env(PATH) "$texbin$pathsep$::env(PATH)"
    }
    # now is a good time to ask tlmgr for the tl name of our platform
    set ::our_platform [exec tlmgr print-platform]
  }
  # directory for temp files
  set attemptdirs {}
  foreach tmp {TMPDIR TEMP TMP} {
    if {$tmp in [array names ::env]} {
      lappend attemptdirs $::env($tmp)
    }
  }
  if {$::tcl_platform(platform) eq "unix"} {
    lappend attemptdirs "/tmp"
  }
  lappend attemptdirs [pwd]
  set ::tempsub ""
  foreach tmp $attemptdirs {
    if {$::tcl_platform(platform) eq "windows"} {
      regsub -all {\\} $tmp {/} tmp
    }
    if {[file isdirectory $tmp]} {
      # no real point in randomizing directory name itself
      if {$::tcl_platform(platform) eq "unix"} {
        set ::tempsub [file join $tmp $::env(USER)]
      } else {
        set ::tempsub [file join $tmp $::env(USERNAME)]
      }
      append ::tempsub "-tlshell"
      if {! [catch {file mkdir $::tempsub}]} {break} ;# success
    }
  }

  if {$::tempsub eq "" || [file isdirectory $::tempsub] == 0} {
    error "Cannot create directory for temporary files"
  }
  # temp file for stderr
  set ::err_file [maketemp ".err_tlshl"]

  # logfile
  if $::ddebug {
    set fname [file join $::tempsub \
      [clock format [clock seconds] -format {%H:%M}]]
    set ::flid [open $fname w]
  }

  # add json subdirectory to auto_path, but at low priority
  # since the tcl/tk installation may already have a better implementation.
  # Use kpsewhich to find out own directory and bypass symlinks.
  #set tlsdir [file dirname [exec kpsewhich -format texmfscripts tlshell.tcl]]
  #lappend ::auto_path [file join $tlsdir "json"]

  make_widgets

  start_tlmgr
  get_repo
  get_packages_info_local
  collect_filtered ; # invokes display_packages_info
  enable_menu_controls 1
}; # initialize

initialize
