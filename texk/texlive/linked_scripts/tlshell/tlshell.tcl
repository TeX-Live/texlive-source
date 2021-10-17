#!/bin/sh
# next line ignored by wish but not by sh \
TK_SILENCE_DEPRECATION=1 exec wish "$0" "$@"
# The above environment variable is set to suppress
# a warning message under MacOS Catalina and Big Sur

# Copyright 2017-2021 Siep Kroonenberg

# This file is licensed under the GNU General Public License version 2
# or any later version.

package require Tk

# security: disable send
catch {rename send {}}

# unix: make sure TL comes first on process searchpath
# on windows, a wrapper takes care of this.
if {$::tcl_platform(platform) ne "windows"} {
  set texbin [file dirname [file normalize [info script]]]
  # prepend texbin to PATH, unless it is already the _first_
  # path component
  set dirs [split $::env(PATH) ":"]
  if {[lindex $dirs 0] ne $texbin} {
    set ::env(PATH) "${texbin}:$::env(PATH)"
  }
  unset texbin
  unset dirs
}

set ::instroot [exec kpsewhich -var-value=TEXMFROOT]
set ::year 0 ; # to be set when connecting to tlmgr

# try to read a configuration variable (gui-lang, tkfontscale)
# from tlmgr config ($TEXMF[SYS]CONFIG/tlmgr/config
# failure results in an empty string.
# this proc will be invoked by tltcl if tlshell is the invoker of tltcl
proc get_config_var {k} {
  set v ""
  set r [join [list {^\s*} $k {\s*=\s*(\S.*)$}] ""]
  foreach tmf {"TEXMFCONFIG" "TEXMFSYSCONFIG"} {
    if [catch {exec kpsewhich -var-value $tmf} d] {
        break; # apparently there is not yet a TL installation
    }

    if [catch {open [file join $d "tlmgr" "config"] r} fid] continue
    while 1 {
      if [chan eof $fid] {
        break
      }
      if [catch {chan gets $fid} l] break
      # we can assume that $k contains no special characters
      if [regexp $r $l m v] {
        set v [regsub {\s*$} $v ""]
        break
      } else {
        # regexp will have unset v
        set v ""
      }
    }
    chan close $fid
    if {$v ne ""} break
  }
  return $v
}

proc save_config_var {k v} {
  if [catch {exec kpsewhich -var-value "TEXMFCONFIG"} d] {return 0}
  set d [file join $d "tlmgr"]
  if [catch {file mkdir $d}] {return 0} ; # mkdir on existing dir is ok
  set fn [file join $d "config"]
  set oldlines [list]
  set r [join [list {^\s*} $k {\s*=\s*(\S.*)$}] ""]
  # read current config file
  if [file exists $fn] {
    if [catch {open $fn r} fid] {return 0}
    set cnt 0
    while 1 {
      if [catch {chan gets $fid} l] break
      if [chan eof $fid] break
      incr cnt
      if {! [regexp $r $l]} {
          lappend oldlines $l
      }
      if {$cnt>40} break
    }
    catch {chan close $fid}
  }
  lappend oldlines "$k = $v"
  if [catch {open $fn w} fid] {return 0}
  foreach l $oldlines {
    if [catch {puts $fid $l}] {
      catch {chan close $fid}
      return 0
    }
  }
  catch {chan close $fid}
  return 1
}

# tltcl: declarations and utilities shared with install-tl-gui.tcl
# tltcl likes to know who was the invoker
set ::invoker [file tail [info script]]
if [string match -nocase ".tcl" [string range $::invoker end-3 end]] {
  set ::invoker [string range $::invoker 0 end-4]
}
source [file join $::instroot "tlpkg" "tltcl" "tltcl.tcl"]

# now is a good time to ask tlmgr for the _TL_ name of our platform
set ::our_platform [exec -ignorestderr tlmgr print-platform]

# searchpath and locale:
# windows: tlshell runs via a wrapper which adjusts the searchpath
# for the current process.
# others: tlshell.tcl should  be run via a symlink in a directory
# which also contains (a symlink to) kpsewhich.
# This directory will be prepended to the searchpath.
# kpsewhich should disentangle symlinks.

# dis/enable the restore dialog
set do_restore 0

# tlcontrib
set tlcontrib "http://contrib.texlive.info/current"

# dis/enable debug output (only for private development purposes)
set ddebug 0

##### general housekeeping ############################################

# menus: disable tearoff feature
option add *Menu.tearOff 0

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
    if {$::tcl_platform(platform) ne "windows"} {puts stderr $s}
    # On windows, stderr output goes nowhere.
    # Therefore also debug output for the log dialog.
    lappend ::dbg_log $s
    file mkdir ${::instroot}/temp
    set dbg [open "${::instroot}/temp/mydbglog" a]
    puts $dbg "TCL: $s"
    chan close $dbg
    # Track debug output in the log dialog if it is running:
    if [winfo exists .tllg.dbg.tx] {
      .tllg.dbg.tx configure -state normal
      .tllg.dbg.tx insert end "$s\n"
      if {$::tcl_platform(os) ne "Darwin"} {
        .tllg.dbg.tx configure -state disabled
      }
    }
  }
} ; # do_debug

### temporary files and directories #########################

proc maketemp {ext} {
  set fname ""
  foreach i {0 1 2 3 4 5 6 7 8 9} { ; # ten tries
    set fname [file join $::tempsub "[expr {int(10000.0*rand())}]$ext"]
    if {[file exist $fname]} {set fname ""; continue}
    # create empty file. although we just want a name,
    # we must make sure that it can be created.
    set fid [open $fname w]
    chan close $fid
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

# NOTE
# text widgets aren't ttk widgets:
# state disabled  => configure -state disabled
# state !disabled => configure -state normal

# dialog with textbox
proc long_message {str type {p "."}} {
  # alternate messagebox implemented as custom dialog
  # not all message types are supported
  if {$type ne "ok" && $type ne "okcancel" && $type ne "yesnocancel"} {
    err_exit "Unsupported type $type for long_message"
  }
  create_dlg .tlmg $p
  wm title .tlmg ""

  # wallpaper frame; see populate_main
  pack [ttk::frame .tlmg.bg] -fill both -expand 1

  # buttons
  pack [ttk::frame .tlmg.bts] -in .tlmg.bg -side bottom -fill x
  if {$type eq "ok" || $type eq "okcancel"} {
    ttk::button .tlmg.ok -text [__ "Ok"] -command "end_dlg \"ok\" .tlmg"
    ppack .tlmg.ok -in .tlmg.bts -side right
  }
  if {$type eq "yesnocancel"} {
    ttk::button .tlmg.yes -text [__ "Yes"] -command "end_dlg \"yes\" .tlmg"
    ppack .tlmg.yes -in .tlmg.bts -side right
    ttk::button .tlmg.no -text [__ "No"] -command "end_dlg \"no\" .tlmg"
    ppack .tlmg.no -in .tlmg.bts -side right
  }
  if {$type eq "yesnocancel" || $type eq "okcancel"} {
    ttk::button .tlmg.cancel -text [__ "Cancel"] -command \
        "end_dlg \"cancel\" .tlmg"
    ppack .tlmg.cancel -in .tlmg.bts -side right
  }
  if [winfo exists .tlmg.cancel] {
    bind .tlmg <Escape> {.tlmg.cancel invoke}
    wm protocol .tlmg WM_DELETE_WINDOW {cancel_or_destroy .tlmg.cancel .tlmg}
  } elseif {$type eq "ok"} {
    bind .tlmg <Escape> {.tlmg.ok invoke}
    wm protocol .tlmg WM_DELETE_WINDOW {cancel_or_destroy .tlmg.ok .tlmg}
  }

  ppack [ttk::frame .tlmg.tx] -in .tlmg.bg -side top -fill both -expand 1
  pack [ttk::scrollbar .tlmg.tx.scroll -command ".tlmg.tx.txt yview"] \
      -side right -fill y
  ppack [text .tlmg.tx.txt -height 20 -width 60 -bd 2 -relief groove \
      -wrap word -yscrollcommand ".tlmg.tx.scroll set"] -expand 1 -fill both

  .tlmg.tx.txt insert end $str
  .tlmg.tx.txt configure -state disabled

  # default resizable
  place_dlg .tlmg $p
  tkwait window .tlmg
  return $::dialog_ans
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

##### tl global data ##################################################

set ::last_cmd ""

set ::progname [info script]
regexp {^.*[\\/]([^\\/\.]*)(?:\....)?$} $progname dummy ::progname
set ::procid [pid]

# package repositories
array unset ::repos

# mirrors: dict of dicts of lists of urls per country per continent
# moved to tltcl.tcl
#set ::mirrors [dict create]

# dict of (local and global) package dicts
set ::pkgs [dict create]

# platforms
set ::platforms [dict create]

set ::have_remote 0 ; # remote packages info not yet loaded
set ::need_update_tlmgr 0
set ::n_updates 0
set ::tlshell_updatable 0

## package data to be displayed ##

# sorted display data for packages; package data stored as lists
set ::filtered [dict create]

# selecting packages for display: status and detail
set ::stat_opt "inst"
set ::dtl_opt "all"
# searching packages for display; also search short descriptions?
set ::search_desc 0

### enabling and disabling user interaction based on global data

proc selective_dis_enable {} {
  # disable actions which make no sense at the time

  # buttons in the middle section
  set pkg_buttons [list .mrk_inst .mrk_upd .mrk_rem .upd_tlmgr .upd_all]
  foreach b [list .mrk_inst .mrk_upd .mrk_rem .upd_tlmgr .upd_all] {
    $b state !disabled
  }
  if $::do_restore {.mrk_rest state !disabled}

  if {!$::have_remote} {
    foreach b [list .mrk_inst .mrk_upd .upd_tlmgr .upd_all] {
      $b state disabled
    }
  } elseif {!$::n_updates} {
    foreach b [list .mrk_upd .upd_tlmgr .upd_all] {
      $b state disabled
    }
  } elseif $::need_update_tlmgr {
    foreach b [list .mrk_inst .mrk_upd] {
      $b state disabled
    }
    if {$::tcl_platform(platform) eq "windows"} {
      .upd_all state disabled
    }
  } elseif {!$::need_update_tlmgr} {
    .upd_tlmgr state disabled
  }

  # platforms menu item
  if {$::tcl_platform(platform) ne "windows"} {
    if {!$::have_remote || $::need_update_tlmgr}  {
      .mn.opt entryconfigure $::inx_platforms -state disabled
    } else {
      .mn.opt entryconfigure $::inx_platforms -state normal
    }
  }

  # 64-bit windows
  if {$::tcl_platform(platform) eq "windows" && $::wprocessor eq "AMD64"} {
    dis_enable_w64
  }
}; # selective_dis_enable

proc total_dis_enable {y_n} {
  # to be invoked when tlmgr becomes busy or idle, i.e.
  # if it starts with a tlmgr command through run_cmds
  # or read_line notices the command(s) ha(s|ve) ended.
  # This proc should cover all active interface elements of the main window.
  # But if actions are initiated via a dialog, the main window can instead
  # be deactivated by a grab and focus on the dialog.

  if {!$y_n} { ; # disable
    set ::busy [__ "Running"]
    . configure -menu .mn_empty
    foreach c [winfo children .] {
      if {$c ne ".showlogs" && [winfo class $c] in $::active_cls} {
        # this should cover all relevant widgets in the main window
        $c state disabled
      }
    }
  } else { ; # enable
    . configure -menu .mn
    foreach c [winfo children .] {
      if {[winfo class $c] in $::active_cls} {
        $c state !disabled
      }
    }
    set ::busy [__ "Idle"]
    selective_dis_enable
  }
} ; # total_dis_enable

##### handling tlmgr via pipe and stderr tempfile #####################

set ::prmpt "tlmgr>"
set ::busy [__ "Running"]

# copy logs to log window yes/no
set ::show_output 0

# about [chan] gets:
# if a second parameter, in this case l, is supplied
# then this variable receives the result, with EOL stripped,
# and the return value is the string length, possibly 0
# EOF is indicated by a return value of -1.

proc read_err_tempfile {} {
  if [info exists ::err] {
    set len 0
    while 1 {
      set len [chan gets $::err l]
      if {$len >= 0} {
        lappend ::err_log $l
      } else {
        break
      }
    }
  }
} ; # read_err_tempfile

proc err_exit {{m ""}} {
  do_debug "error exit"
  read_err_tempfile
  if {$m ne ""} {
    set ::err_log [linsert $::err_log 0 $m]
  }
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
  # -gui -. -gui-lang
  for {set i 0} {$i < [llength $args]} {incr i} {
    if {[lindex $args $i] eq "-lang"} {
      set args [lreplace $args $i $i "-gui-lang"]
    }
  }
  set cmd [list "|tlmgr" {*}$args "--machine-readable" "shell" 2>>$::err_file]
  if [catch {open $cmd w+} ::tlshl] {
    tk_messageBox -message [get_stacktrace]
    exit
  }
  set ::perlpid [pid $::tlshl]
  do_debug "done opening tlmgr"
  set ::err [open $::err_file r]
  chan configure $::tlshl -buffering line -blocking 0
  chan event $::tlshl readable read_line
  vwait ::done_waiting
} ; # start_tlmgr

proc close_tlmgr {} {
  run_cmd_waiting "quit"
}

# read a line of tlmgr output
proc read_line {} {
  # a caller of run_cmd needs to explicitly invoke 'vwait ::done_waiting'
  # if it wants to wait for the command to finish
  set l "" ; # will contain the line to be read
  if {([catch {chan gets $::tlshl l} len] || [chan eof $::tlshl])} {
    # copy as much of stderr as possible to ::err_log
    catch {read_err_tempfile ; chan close $::err}
    if [chan eof $::tlshl] {
      catch {chan close $::tlshl}
      unset -nocomplain ::tlshl
      unset -nocomplain ::err
      set ::perlpid 0
      set ::done_waiting 1
    } else {
      #do_debug "read_line: failing to read "
      puts stderr "Read failure; tlmgr command was $::last_cmd"
      # note. the normal way to terminate is terminating the GUI shell.
      # This closes stdin of tlmgr shell.
      err_exit
    }
  } elseif {$len >= 0} {
    # do_debug "read: $l"
    if $::ddebug {puts $::flid $l}
    if {[string first $::prmpt $l] == 0} {
      # prompt line: we are done with the current command
      total_dis_enable 1 ; # this may have to be redone later
      # catch up with stderr
      read_err_tempfile
      if $::show_output {
        do_debug "prompt found, $l"
        log_widget_finish
      }
      # for vwait:
      set ::done_waiting 1
      set ::show_output 0
    } else {
      # regular output
      lappend ::out_log $l
      if $::show_output {
        log_widget_add $l
      }
    }
  }
} ; # read_line

# copy error strings to error page in logs dialog .tllg and send it to top.
# This by itself does not map the logs dialog .tllg

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

proc log_widget_init {} {
  show_logs ; # create the logs dialog
  set ::busy [__ "Running"]
  .tllg.close state disabled
}

proc log_widget_add l {
  .tllg.log.tx configure -state normal
  .tllg.log.tx insert end "$l\n"
  if {$::tcl_platform(os) ne "Darwin"} {
    .tllg.log.tx configure -state disabled
  }
}

proc log_widget_finish {} {
  .tllg.log.tx yview moveto 1
  .tllg.logs select .tllg.log
  # error log on top if it contains anything
  show_err_log
  if {$::tcl_platform(os) ne "Darwin"} {
    .tllg.log.tx configure -state disabled
  }
  set ::busy [__ "Idle"]
  .tllg.close state !disabled
  bind .tllg <Escape> {.tllg.close invoke}
}

##### running tlmgr commands #####

# run a list of commands
proc run_cmds {cmds {show 0}} {
  set ::show_output $show
  do_debug "run_cmds \"$cmds\""
  if $::ddebug {puts $::flid "\n$cmds"}
  set ::out_log {}
  set ::err_log {}
  if $show {
    show_logs
    .tllg.status configure -text [__ "Running"]
    .tllg.close state disabled
  }
  set l [llength $cmds]
  for {set i 0} {$i<$l} {incr i} {
    if {! [info exists ::tlshl]} {
      err_exit "Back end gone. Last command: \n  $::last_cmd"
    }
    set cmd [lindex $cmds $i]
    set ::last_cmd $cmd
    unset -nocomplain ::done_waiting
    # disable widgets for each new command,
    # since read_line will re-enable them
    # when a particular command is finished
    total_dis_enable 0
    chan puts $::tlshl $cmd
    chan flush $::tlshl
    if {$i < [expr {$l-1}]} {vwait ::done_waiting}
  }
} ; # run_cmds

# run a single command
proc run_cmd {cmd {show 0}} {
  run_cmds [list $cmd] $show
} ; # run_cmd

proc run_cmd_waiting {cmd} {
  run_cmd $cmd 0
  vwait ::done_waiting
} ; # run_cmd_waiting

##### Handling package info #####

# what invokes what?
# The main 'globals' are (excepting dicts and arrays):

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
# by update_globals, both via the selective_dis_enable proc

# The repositories play a small part in this front end. Tlmgr mostly works
# with a virtual repository, which is the combined set of repositories,
# with pinning applied if there is more than one repository.
# But get_packages_info_remote must invoke
# show_repos to display updated verification info.
# show_repos is also invoked by initialize.

# get_packages_info_local is invoked only once, at initialization.  After
# installations and removals, the collected information is updated by
# update_local_revnumbers.
# Both procs also invoke get_platforms

# get_packages_info_remote should be invoked before collect_filtered if
# ::have_remote is false. Afterwards, ::have_remote will be true.
# There will be no need to invoke get_packages_info_remote again except
# at a change of repository by repos_commit.
# get_packages_info_remote invokes update_globals, show_repos
# and get_platforms.

# update_local_revnumbers will be invoked after any updates. It also
# invokes update_globals.

# collect_filtered does not only filter, but also organize the
# information to be displayed. It invokes display_packages_info.
# It is invoked at initialization, when filtering options change and
# at the end of install-, remove- and update procs.

# display_packages_info is mostly invoked by collect_filtered, but
# also when the search term or the search option changes.

proc is_updatable {nm} {
  set pk [dict get $::pkgs $nm]
  set lr [dict get $pk localrev]
  set rr [dict get $pk remoterev]
  return [expr {$lr > 0 && $rr > 0 && $rr > $lr}]
}

proc display_updated_globals {} {
  if {$::have_remote && $::need_update_tlmgr} {
    .topfll.luptodate configure -text [__ "Needs updating"]
  } elseif $::have_remote {
    .topfll.luptodate configure -text [__ "Up to date"]
  } else {
    .topfll.luptodate configure -text [__ "Unknown"]
  }
  # ... and status of update buttons
  selective_dis_enable
}

proc update_globals {} {
  if {! $::have_remote} return
  set ::n_updates 0
  foreach nm [dict keys $::pkgs] {
    if [is_updatable $nm] {incr ::n_updates}
  }
  set ::need_update_tlmgr [is_updatable texlive.infra]
  set ::tlshell_updatable [is_updatable tlshell]

  # also update displayed status info
  display_updated_globals
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
  set curr [.pkfilter.search_e get]
  .pkglist delete [.pkglist children {}]
  dict for {nm pk} $::filtered {
    set do_show 0
    if {$curr eq ""} {
      # empty search string
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

proc for_other_platform {nm} {
  set r [string last "." $nm]
  if {$r < 0} {
    return 0
  } else {
    set n [string range $nm 0 [expr {$r-1}]]
    set p [string range $nm [expr {$r+1}] end]
    # p a platform identifier of an installed platform?
    if [dict exists $::platforms $p] {
      return [expr {! [dict get $::platforms $p "cur"]}]
    } else {
      return 0
    }
  }
}

# (re)create ::filtered dictionary; disregard search string in selection
# The value associated with each key/package is a list, not another dict.
proc collect_filtered {} {
  do_debug \
      "collect_filtered for $::stat_opt and $::dtl_opt"
  if {$::stat_opt ne "inst" && ! $::have_remote} {
    # have_remote should have been ensured when changing stat_opt from inst
    err_exit "collect_filtered should not have been invoked at this time"
  }
  foreach nm [dict keys $::filtered] {
    dict unset ::filtered $nm
  }
  foreach nm [lsort [dict keys $::pkgs]] {
    # do not list packages for platforms which are not installed
    if [for_other_platform $nm] continue
    set pk [dict get $::pkgs $nm]
    set do_show 1
    set mrk [mark_sym [dict get $pk marked]]
    set lr [dict get $pk localrev]
    set rr [dict get $pk remoterev]
    set ct [dict get $pk category]
    if {$::stat_opt eq "inst" && $lr == 0} {
      set do_show 0
    } elseif {$::stat_opt eq "notins" && $lr > 0} {
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
      set v "" ; # can this happen?
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
} ; # collect_filtered

proc collect_and_display_filtered {} {
  collect_filtered
  display_packages_info
}

proc get_platforms {} {
  # guarantee fresh start
  foreach k $::platforms {dict unset ::platforms $k}
  set ::platforms [dict create]
  # find binary packages for pdftex; 'platform list' not available on windows
  foreach k [dict keys $::pkgs {pdftex.*}] {
    # {pdftex.*} is a glob pattern, not a regexp: '.' is a literal
    set plname [string range $k 7 end]
    if {[dict get $::pkgs $k localrev] > 0} {
      dict set ::platforms $plname "cur" 1
      #dict set ::platforms $plname "fut" 1
    } else {
      dict set ::platforms $plname "cur" 0
      #dict set ::platforms $plname "fut" 0
    }
  }
}

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
  get_platforms
} ; # get_packages_info_local

proc abort_load {} {
  # try to close back end forcibly
  catch {chan close $::tlshl}
  if {$::tcl_platform(platform) eq "windows"} {
    catch {exec -ignorestderr taskkill /pid $::perlpid /t /f}
  } else {
    catch {exec -ignorestderr kill -9 $::perlpid}
    # should also be ok for darwin
  }
  # start new tlshell process
  exec $::progname &

  # end current tlshell process nicely
  exit
  # kill current tlshell process forcibly if necessary
  if {$::tcl_platform(platform) eq "windows"} {
    catch {exec -ignorestderr taskkill /pid [pid] /t /f}
  } else {
    catch {exec -ignorestderr kill -9 [pid]}
  }
} ; # abort load

# toplevel with abort button in case loading of a repository takes too long.
# it can be closed when loading finishes
proc splash_loading {} {

  create_dlg .loading .

  wm title .loading [__ "Loading"]

  # wallpaper
  pack [ttk::frame .loading.bg -padding 3pt] -fill both -expand 1

  set lbl [__ \
       "If loading takes too long, press Abort and choose another repository."]
  append lbl "\n([__ "Options"] \/ [__ "Repositories"] ...)"
  ppack [ttk::label .loading.l0 -text $lbl \
           -wraplength [expr {60*$::cw}] -justify left] \
    -in .loading.bg -anchor w

  pack [ttk::frame .loading.tfr] -in .loading.bg -expand 1 -fill x
  pack [ttk::scrollbar .loading.scroll -command ".loading.tx yview"] \
      -in .loading.tfr -side right -fill y
  ppack [text .loading.tx -height 5 -wrap word \
            -yscrollcommand ".loading.scroll set"] \
      -in .loading.tfr -expand 1 -fill both
  pack [ttk::frame .loading.buttons] -in .loading.bg -expand 1 -fill x
  ttk::button .loading.close -text [__ "Close"] -command {end_dlg "" .loading}
  ppack .loading.close -in .loading.buttons -side right
  .loading.close configure -state disabled
  ttk::button .loading.abo -text [__ "Abort"] -command abort_load
  ppack .loading.abo -in .loading.buttons -side right
  wm protocol .loading {cancel_or_destroy .loading.abo .loading}
  wm resizable .loading 0 0
  place_dlg .loading .
} ; # splash_loading


proc track_err {} {
  set inx0 [llength $::err_log]
  read_err_tempfile
  .loading.tx configure -state normal
  for {set i $inx0} {$i < [llength $::err_log]} {incr i} {
    .loading.tx insert end "[lindex $::err_log $i]\n"
  }
  .loading.tx configure -state disabled
  update idletasks
  if {![info exists ::loaded]} {
    after 500 track_err
  } else {
    .loading.close state !disabled
    .loading.abo state disabled
    .loading.tx configure -state normal
    .loading.tx insert end [__ "Done loading"]
    .loading.tx configure -state disabled
  }
}

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

  splash_loading

  unset -nocomplain ::loaded
  track_err
  set catv "rcat-version"
  if {[dict get $::pkgs texlive.infra localrev] < 56458} { set catv "cat-version" }
  if [catch {run_cmd \
    "info --data name,remoterev,$catv,category,shortdesc"}] {
    do_debug [get_stacktrace]
    tk_messageBox -message [__ "A configured repository is unavailable."]
    return 0
  }
  vwait ::done_waiting
  set ::loaded 1
  set re {^([^,]+),([0-9]+),([^,]*),([^,]*),(.*)$}
  foreach l $::out_log {
    if [regexp $re $l m nm rrev rcatv catg pdescr] {
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
  get_platforms ; # derive from $::pkgs

  set ::have_remote 1
  update_globals
  show_repos
  return 1
} ; # get_packages_info_remote

## update ::pkgs after installing packages without going online again.
proc update_local_revnumbers {} {
  do_debug "update_local_revnumbers"
  run_cmd_waiting "info --only-installed --data name,localrev,lcat-version"
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
      dict set pk_dict "rcatv" $lv
      dict set ::pkgs $pk $pk_dict
    }
  }
  get_platforms
  update_globals
} ; # update_local_revnumbers

##### Logs notebook ##############################

# if invoked via run_cmds, it tracks progress of (a) tlmgr command(s).
# run_cmds will temporarily disable the close button
# and set .tllg.status to busy via total_dis_enable 0.
# otherwise, it shows the output of the last completed (list of) command(s).

# Note that run_cmds clears ::out_log and ::err_log, but not ::dbg_log.

proc show_logs {} {
  create_dlg .tllg .
  wm title .tllg Logs

  # wallpaper
  pack [ttk::frame .tllg.bg] -fill both -expand 1

  # close button and busy label
  pack [ttk::frame .tllg.bottom] -in .tllg.bg -side bottom -fill x
  ttk::button .tllg.close -text [__ "Close"] -command {end_dlg 0 .tllg}
  ppack .tllg.close -in .tllg.bottom -side right -anchor e
  ppack [ttk::label .tllg.status -textvariable ::busy -anchor w] \
      -in .tllg.bottom -side left
  bind .tllg <Escape> {.tllg.close invoke}
  wm protocol .tllg WM_DELETE_WINDOW \
      {cancel_or_destroy .tllg.close .tllg}

  # notebook pages and scrollbars
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
    if {$::tcl_platform(os) ne "Darwin"} {
      .tllg.dbg.tx configure -state disabled
    }
    .tllg.dbg.tx yview moveto 1
  }

  # collect pages in notebook widget
  pack [ttk::notebook .tllg.logs] -in .tllg.bg -side top -fill both -expand 1
  .tllg.logs add .tllg.log -text [__ "Output"]
  .tllg.logs add .tllg.err -text [__ "Other"]
  if $::ddebug {
    .tllg.logs add .tllg.dbg -text "Debug"
    raise .tllg.dbg .tllg.logs
  }
  raise .tllg.err .tllg.logs
  raise .tllg.log .tllg.logs

  # default resizable
  place_dlg .tllg .
} ; # show_logs

##### repositories ###############################################

### mirrors
#mangle_name {n} {} ; moved to tltcl.tcl
#proc read_mirrors {} {} ; moved to tltcl.tcl

proc pick_local_repo {} {
  set nw_repo [.tlr.cur cget -text]
  if {! [file isdirectory $nw_repo]} {
    set nw_repo $::env(HOME) ; # HOME also o.k. for windows
  }
  while 1 {
    set nw_repo [browse4dir $nw_repo .tlr]
    if {$nw_repo ne "" && ! [possible_repository $nw_repo]} {
      tk_messageBox -message [__ "%s not a repository" $nw_repo] -parent .tlr
      continue
    } else {
      .tlr.save state !disabled
      break
    }
  }
  if {$nw_repo ne ""} {
    .tlr.new delete 0 end
    .tlr.new insert end $nw_repo
  }
} ; # pick_local_repo

proc get_repos_from_tlmgr {} {
  array unset ::repos
  run_cmd_waiting "option repository"
  set rps ""
  foreach l $::out_log {
    if [regexp {repository\t(.*)$} $l dum rps] break
  }
  if {$rps ne ""} {
    set reps [split $rps " "]
    set nr [llength $reps]
    foreach rp $reps {
      # decode spaces and %
      set rp [string map {"%20" " "} $rp]
      set rp [string map {"%25" "%"} $rp]
      if {! [regexp {^(.+)#(.+)$} $rp dum r t]} {
        # no tag; use repository as its own tag
        set r $rp
        set t $rp
      }
      if {$nr == 1} {
        set t "main"
      }
      set ::repos($t) $r
    }
    if {"main" ni [array names ::repos]} {
      array unset ::repos
    }
  }
}; # get_repos_from_tlmgr

proc set_repos_in_tlmgr {} {
  # tlmgr has no command to replace a single repository;
  # we need to compose a string for opt_location ourselves from $::repos.
  # a single repository should not get a tag.
  # apparently, we can safely ignore bogus pinning data.
  set nr [array size ::repos]
  set opt_repos ""
  set rp ""
  foreach nm [array names ::repos] {
    if {$nr==1} {
      if {$nm ne "main"} {
        err_exit "Internal error"
      } else {
        # pinning command only supported for multiple repositories
        set rp $::repos(main)
      }
    } else {
      if {$nm eq $::repos($nm)} {
        set rp $nm
      } else {
        set rp $::repos($nm)
        append rp "#$nm"
      }
    }
    # encode % and spaces
    set rp [string map {"%" "%25"} $rp]
    set rp [string map {" " "%20"} $rp]
    append opt_repos " $rp"
  }
  run_cmd_waiting "repository set [string range $opt_repos 1 end]"
}; # set_repos_in_tlmgr

proc show_repos {} {
  # this proc lists the configured repository/ies
  # in the upper left portion of the main window.
  # it makes one call to the back end, but no need to go online.
  set w .toprepo
  foreach ch [winfo children $w] {destroy $ch}
  set nms [array names ::repos]
  set c [llength $nms]
  grid [ttk::frame $w.repoheadframe] \
      -row 0 -column 0 -columnspan 3 -sticky w
  pack [ttk::label $w.head -font hfont] \
      -in $w.repoheadframe -side left
  pack [ttk::label $w.load -text ""] \
      -in $w.repoheadframe -side left -anchor sw
  if {$c <= 0} {
    $w.head configure -text [__ "No repositories"]
     return
  } elseif {$c == 1} {
    $w.head configure -text [__ "Repository"]
  } else {
    $w.head configure -text [__ "Multiple repositories"]
  }
  if {! $::have_remote} {
    $w.load configure -text " ([__ "Not loaded"])"
  }
  # the $::repos array already contains the configured repositories.
  # 'repository status' adds verification info and actual selected mirror.
  set repodict [dict create]
  if $::have_remote {
    run_cmd_waiting "repository status"
    # a number code for verification status
    set re {^(\S+) (\S+)/tlpkg/texlive.tlpdb (-?\d+) (.*)$}
    foreach l $::out_log {
      if [regexp $re $l dum nm rp n d] {
        # dummy tag repository verification_code description
        # restore spaces and percent characters in nm and rp
        set nm [string map {"%20" " "} $nm]
        set nm [string map {"%25" "%"} $nm]
        set rp [string map {"%20" " "} $rp]
        set rp [string map {"%25" "%"} $rp]
        dict set repodict $nm "url" $rp
        dict set repodict $nm "vericode" $n
        dict set repodict $nm "veridescr" $d
      }
    }
    # the selected mirror is needed in the win64 code, so is a global.
    if {$::repos(main) eq $::any_mirror} {
      set ::actual_main [dict get $repodict "main" "url"]
    } else {
      set ::actual_main $::repos(main)
    }
  }
  set rw 0
  foreach nm [array names ::repos] {
    incr rw
    pgrid [ttk::label $w.u$nm -text $::repos($nm) -justify left] \
        -sticky nw -row $rw -column 0
    if {$::repos($nm) eq $::any_mirror && $::have_remote} {
      $w.u$nm configure -text \
        "$::any_mirror\n[__ "Actual repository"]:\n$::actual_main"
    }
    if {[array size ::repos] > 1 && $nm ne $::repos($nm)} {
      pgrid [ttk::label $w.n$nm -text "($nm)"] \
        -sticky nw -row $rw -column 1
    }
    if $::have_remote {
      pgrid [ttk::label $w.v$nm -text \
                "verification: [dict get $repodict $nm "veridescr"]"] \
            -sticky nw -row $rw -column 2
    }
  }
} ; # show_repos

proc repos_commit {} {
  set changes 0
  # set repositories then add pinning if appropriate
  set new_repo [forward_slashify [.tlr.new get]]
  if {! [regexp {^\s*$} $new_repo]} {
    # repository entry widget non-empty: use it
    if {$::repos(main) ne $new_repo} {
      set ::repos(main) $new_repo
      set changes 1
    }
  }
  set had_contrib 0
  if $::toggle_contrib {
    set changes 1
    foreach nm [array names ::repos] {
      if {$::repos($nm) eq $::tlcontrib} {
        set had_contrib 1
        run_cmds [list "pinning remove $nm --all" \
                      "pinning remove $::repos($nm) --all"] 0
        vwait ::done_waiting
        array unset ::repos $nm
      }
    }
    if {! $had_contrib} {
      set ::repos(tlcontrib) $::tlcontrib
    }
  }
  if $changes {
    set_repos_in_tlmgr
    close_tlmgr
    start_tlmgr
    if {$::toggle_contrib && ! $had_contrib} {
      run_cmd_waiting "pinning add tlcontrib \"*\""
    }
  }
} ; # repos_commit

proc dis_enable_reposave {} {
  if [possible_repository [.tlr.new get]] {
    .tlr.save state !disabled
  } else {
    .tlr.save state disabled
  }
}

proc save_load_repo {} {
  repos_commit
  end_dlg "" .tlr
  # reload remote package information
  set ::have_remote 0
  get_packages_info_remote
  collect_and_display_filtered
}

proc select_mir {m} {
  .tlr.new delete 0 end; .tlr.new insert end $m
  .tlr.save state !disabled
}

# main repository dialog
proc repository_dialog {} {

  # dialog with
  # - popup menu of mirrors (parse tlpkg/installer/ctan-mirrors.pl)
  # - text entry box
  # - directory browser button
  # - ok and cancel buttons

  create_dlg .tlr .
  wm title .tlr [__ "Main Repository"]

  # wallpaper frame; see populate_main
  pack [ttk::frame .tlr.bg] -expand 1 -fill x

  pack [ttk::frame .tlr.info] -in .tlr.bg -expand 1 -fill x
  grid columnconfigure .tlr.info 1 -weight 1
  set row -1

  # current repository
  incr row
  pgrid [ttk::label .tlr.lcur -text [__ "Current:"]] \
      -in .tlr.info -row $row -column 0 -sticky w
  pgrid [ttk::label .tlr.cur -text $::repos(main)] \
      -in .tlr.info -row 0 -column 1 -sticky w
  # proposed new repository
  incr row
  pgrid [ttk::label .tlr.lnew -text [__ "New"]] \
      -in .tlr.info -row $row -column 0 -sticky w
  pgrid [ttk::entry .tlr.new] \
      -in .tlr.info -row $row -column 1 -columnspan 2 -sticky ew
  .tlr.new insert 0 $::repos(main)
  bind .tlr.new <KeyRelease> dis_enable_reposave

  ### three ways to specify a repository ###
  pack [ttk::frame .tlr.mirbuttons] -in .tlr.bg -fill x
  # 1. default remote repository
  ttk::button .tlr.ctan -text [__ "Any CTAN mirror"] -command {
    .tlr.new delete 0 end
    .tlr.new insert end $::any_mirror
    .tlr.save state !disabled
  }
  ppack .tlr.ctan -in .tlr.mirbuttons -side left -fill x
  # 2. specific repository: create a cascading dropdown menu of mirrors
  mirror_menu .tlr.mir select_mir
  ppack .tlr.mir -in .tlr.mirbuttons -side left -fill x
  # 3. local repository
  ttk::button .tlr.browse -text [__ "Local directory..."] -command {
    .tlr.new delete 0 end; .tlr.new insert end [pick_local_repo]}
  ppack .tlr.browse -in .tlr.mirbuttons -side left -fill x

  ### add/remove tlcontrib ###
  ttk::label .tlr.contribt -text [__ "tlcontrib additional repository"] \
      -font bfont
  pack .tlr.contribt -in .tlr.bg -anchor w -padx 3pt -pady [list 10pt 3pt]
  pack [ttk::label .tlr.contribl] -in .tlr.bg -anchor w -padx 3pt -pady 3pt
  ttk::checkbutton .tlr.contribb -variable ::toggle_contrib
  pack .tlr.contribb -in .tlr.bg -anchor w -padx 3pt -pady [list 3pt 10pt]
  set ::toggle_contrib 0
  set has_contrib 0
  foreach nm [array names ::repos] {
    if {$::repos($nm) eq $::tlcontrib} {
      set has_contrib 1
      set contrib_tag $nm
      break
    }
  }
  if $has_contrib {
    .tlr.contribl configure -text [__ "tlcontrib repository is included"]
    .tlr.contribb configure -text [__ "Remove tlcontrib repository"]
  } else {
    .tlr.contribl configure -text [__ "tlcontrib repository is not included"]
    .tlr.contribb configure -text [__ "Add tlcontrib repository"]
  }

  # two ways to close the dialog
  pack [ttk::frame .tlr.closebuttons] -pady [list 10pt 0pt] -in .tlr.bg -fill x
  ttk::button .tlr.save -text [__ "Save and Load"] -command save_load_repo
  ppack .tlr.save -in .tlr.closebuttons -side right
  dis_enable_reposave
  ttk::button .tlr.cancel -text [__ "Cancel"] -command {end_dlg "" .tlr}
  ppack .tlr.cancel -in .tlr.closebuttons -side right
  bind .tlr <Escape> {.tlr.cancel invoke}

  wm protocol .tlr WM_DELETE_WINDOW \
      {cancel_or_destroy .tlr.cancel .tlr}
  wm resizable .tlr 1 0
  place_dlg .tlr .
} ; # repository_dialog

### platforms

# ::platforms is a dictionary of dictionaries.
# each element dictionary has keys cur and fut indicating
# current and future status (i.e. being installed),
# with values 0 meaning 'not installed' and 1 meaning 'installed'.
# elsewhere, we shall only be interested in cur.
# within the treeview list, we shall use the usual unicode mark symbols.

# for windows, there is no platforms menu entry,
# and the platforms dialog returns immediately, i.e. is a no-op.

proc toggle_pl_marked {pl cl} {
  # toggle_pl_marked is triggered by a mouse click only in column #1.
  # 'fut'[ure] should get updated in ::platforms _and_ in .tlpl.pl.

  if {$cl ne "#1"} return
  if {$pl eq $::our_platform} {
    tk_messageBox -message \
        [__ "Cannot remove own platform %s" $::our_platform] -parent .tlpl
    return
  }
  set m1 [expr {[dict get $::platforms $pl "fut"] ? 0 : 1}]
  dict set ::platforms $pl "fut" $m1
  set m0 [dict get $::platforms $pl "cur"]
  if {$m1 eq $m0} {
    .tlpl.pl tag remove "changed" $pl
  } else {
    .tlpl.pl tag add "changed" $pl
  }
  .tlpl.pl set $pl "stat" [mark_sym $m1]
  # any changes to apply?
  .tlpl.do state disabled
  dict for {pname pdict} $::platforms {
    if {[dict get $pdict "fut"] ne [dict get $pdict "cur"]} {
      .tlpl.do state !disabled
      break
    }
  }
} ; # toggle_pl_marked

proc platforms_commit {} {
  set pl_add {}
  set pl_remove {}
  dict for {pname pdict} $::platforms {
    if {[dict get $pdict "cur"] ne [dict get $pdict "fut"]} {
      if {[dict get $pdict "fut"]} {
        lappend pl_add $pname
      } else {
        lappend pl_remove $pname
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
  run_cmds $cmds 1
  vwait ::done_waiting
  update_local_revnumbers
  collect_and_display_filtered

} ; # platforms_commit

# the platforms dialog
proc platforms_select {} {
  if {$::tcl_platform(platform) eq "windows"} return

  create_dlg .tlpl
  wm title .tlpl [__ "Platforms"]
  if $::plain_unix {wm attributes .tlpl -type dialog}

  # wallpaper frame
  pack [ttk::frame .tlpl.bg] -expand 1 -fill both

  # buttons
  pack [ttk::frame .tlpl.but] -in .tlpl.bg -side bottom -fill x
  ttk::button .tlpl.do -text [__ "Apply and close"] -command {
    disable_dlg .tlpl
    platforms_commit
    end_dlg "" .tlpl
  }
  ttk::button .tlpl.dont -text [__ "Close"] -command \
      {end_dlg "" .tlpl}
  ppack .tlpl.do -in .tlpl.but -side right
  ppack .tlpl.dont -in .tlpl.but -side left
  bind .tlpl <Escape> {.tlpl.dont invoke}

  pack [ttk::label .tlpl.bold -text [__ "Changed entries are bold"] \
           -font bfont] -in .tlpl.bg -side bottom

  # platforms treeview; do we need a scrollbar?

  pack [ttk::frame .tlpl.fpl] -in .tlpl.bg -fill both -expand 1
  ttk::treeview .tlpl.pl -columns {stat plat} -show headings \
      -height [dict size $::platforms] -yscrollcommand {.tlpl.plsb set}
  ppack .tlpl.pl -in .tlpl.fpl -side left -fill both -expand 1
  ttk::scrollbar .tlpl.plsb -orient vertical \
      -command {.tlpl.pl yview}
  ppack .tlpl.plsb -in .tlpl.fpl -side right -fill y -expand 1
  .tlpl.pl column stat -width [expr {$::cw * 3}]
  .tlpl.pl heading plat -text [__ "platform"] -anchor w
  .tlpl.pl column plat -width [expr {$::cw * 20}]
  # tag for indicating changed entries
  .tlpl.pl tag configure "changed" -font bfont

  dict for {pname pdict} $::platforms {
    dict set ::platforms $pname "fut" [dict get $pdict "cur"]
  }
  foreach pname [lsort [dict keys $::platforms]] {
    .tlpl.pl insert {} end -id $pname -values \
        [list [mark_sym [dict get $::platforms $pname "cur"]] $pname]
  }
  set_tree_col_width .tlpl.pl "plat"

  bind .tlpl.pl <space> {toggle_pl_marked [.tlpl.pl focus] "#1"}
  bind .tlpl.pl <Return> {toggle_pl_marked [.tlpl.pl focus] "#1"}
  # only toggle when column is #1
  bind .tlpl.pl <ButtonRelease-1> \
      {toggle_pl_marked \
           [.tlpl.pl identify item %x %y] \
           [.tlpl.pl identify column %x %y]}

  # enable commit button only if changes are requested
  .tlpl.do state disabled
  wm resizable .tlpl 0 1
  place_dlg .tlpl .
} ; # platforms_select

### paper sizes

# for the simple options:
proc set_all_papers {p} {
  run_cmd "paper paper $p" 1
}

# the next few procs are for the advanced paper selection dialog

proc paper_diff {} {
  # check for different values and update widgets accordingly
  set n [lindex [array names ::papers] 0]
  set p $::papers($n)
  set ::allpapers $p
  set ::paperchanges 0
  .tlpap.do state disabled
  foreach nm [array names ::papers] {
  .tlpap.radios.$nm configure -font TkDefaultFont
    if {$::papers($nm) ne $p} {set ::allpapers ""}
    if  {$::papers($nm) ne $::prevpapers($nm)} {
     .tlpap.radios.$nm configure -font bfont
      set ::paperchanges 1
      .tlpap.do state !disabled
    }
  }
}

proc allpapers_adv {} {
  # ::allpapers has already been set
  set ::paperchanges 0
  .tlpap.do state disabled
  foreach nm [array names ::papers] {
    set ::papers($nm) $::allpapers
  }
  paper_diff
}

proc commit_papers {} {
  foreach nm [array names ::papers] {
    if {$::papers($nm) ne $::prevpapers($nm)} {
      set cmd "paper $nm paper $::papers($nm)"
      if [catch {run_cmd_waiting $cmd} res] {
        puts "failed paper setting: $res"
      }
    }
  }
  # clean up no longer needed globals
  foreach nm [array names ::papers] {unset -nocomplain ::papers($nm)}
  foreach nm [array names ::prevpapers] {unset -nocomplain ::prevpapers($nm)}
  unset -nocomplain ::allpapers
  unset -nocomplain ::paperchanges
}

proc papersize_advanced {} {
  # dialog for settings paper size for:
  # all | pdftex | dvipdfmx | context | dvips | psutils | xdvi
  # on windows, xdvi is omitted
  # we could have handled papers somewhat like the platforms dialog,
  # but radio buttons seem more natural,
  # and there are not enough items to worry about scrollability.
  # we create a grid with the radio buttons and a column 'changed'.

  ## a non-existent engine 'dummy' to test handling of missing entries
  #set ::papers(dummy) ""
  foreach nm {"pdftex" "dvipdfmx" "context" "dvips" "psutils" "xdvi"} {
    set ::papers($nm) ""
  }
  if {$::tcl_platform(platform) eq "windows"} {
    unset -nocomplain ::papers(xdvi)
  }
  foreach nm [array names ::papers] {
    if {[catch {run_cmd_waiting "paper $nm paper"} ::papers($nm)] ||
        ([llength $::err_log]<1)} {
      unset -nocomplain ::papers($nm)
    } else {
      set l [lindex $::err_log 0]
      set r [string last "): " $l]
      if {$r<0} {
        unset -nocomplain ::papers($nm)
        continue
      } else {
        set r [expr {$r+3}]
        set ::papers($nm) [string tolower [string range $l $r end]]
      }
      if {$::papers($nm) ne "a4" && $::papers($nm) ne "letter"} {
        set $::papers($nm) ""
      }
    }
  }
  set nms [array names ::papers] ; # invalid names have now been weeded out
  if {[llength $nms] < 1} {
    tk_messageBox -title [__ "Error"] -message [__ "No paper sizes available"]
    # we did not yet start building the dialog, so we can just:
    return
  }
  set nm0 [lindex $nms 0]
  set ::allpapers $::papers($nm0)
  foreach nm $nms {
    set ::prevpapers($nm) $::papers($nm)
    if {$::papers($nm) ne $::allpapers} {set ::allpapers ""}
  }
  set ::paperchanges 0

  # build gui
  create_dlg .tlpap
  wm title .tlpap [__ "Paper sizes"]
  if $::plain_unix {wm attributes .tlpap -type dialog}

  # wallpaper frame
  pack [ttk::frame .tlpap.bg] -expand 1 -fill both

  # grid with radio buttons
  pack [ttk::frame .tlpap.radios] -in .tlpap.bg

  foreach {p c} {"a4" 1 "letter" 2} {
    pgrid [ttk::label .tlpap.radios.head$p \
               -text [string totitle $p] -font bfont] -row 0 -column $c
  }
  grid [ttk::separator .tlpap.radios.sep0 -orient horizontal] \
      -row 1 -column 0 -columnspan 3 -sticky ew
  pgrid [ttk::label .tlpap.radios.lall -text [__ "All"]] \
      -row 2 -column 0 -sticky e
  foreach {p c} {"a4" 1 "letter" 2} {
    pgrid [ttk::radiobutton .tlpap.radios.${p}all -value $p \
        -variable ::allpapers -command allpapers_adv] -row 2 -column $c
  }
  grid [ttk::separator .tlpap.radios.sep1 -orient horizontal] \
      -row 3 -column 0 -columnspan 3 -sticky ew
  set rw 3
  foreach nm $nms {
    incr rw
    pgrid [ttk::label .tlpap.radios.$nm -text $nm] \
        -row $rw -column 0 -sticky e
    foreach {p c} {"a4" 1 "letter" 2} {
      pgrid [ttk::radiobutton .tlpap.radios.$p$nm -value $p \
          -variable ::papers($nm) -command paper_diff] -row $rw -column $c
    }
  }
  incr rw
  grid [ttk::separator .tlpap.radios.sep2 -orient horizontal] \
      -row $rw -column 0 -columnspan 3 -sticky ew

  # buttons
  pack [ttk::frame .tlpap.but] -in .tlpap.bg -side bottom -fill x
  ttk::button .tlpap.do -text [__ "Apply and close"] -command {
    disable_dlg .tlpap
    commit_papers
    end_dlg "" .tlpap
  }
  ttk::button .tlpap.dont -text [__ "Close"] -command \
      {end_dlg "" .tlpap}
  ppack .tlpap.do -in .tlpap.but -side right
  ppack .tlpap.dont -in .tlpap.but -side right
  .tlpap.do state disabled
  bind .tlpap <Escape> {.tlpap.dont invoke}

  pack [ttk::label .tlpap.bold -text [__ "Changed entries are bold"] \
           -font bfont] -in .tlpap.bg -side bottom

  wm resizable .tlpap 0 0
  place_dlg .tlpap .
}

##### restore from backup #####

# This is currently rather dangerous.
# ::do_restore is set to 0 or 1 near the top of this source.
# This code, currently disbled, has not been tested in a while.

if $::do_restore {
# dictionary of backups, with mapping to list of available revisions
set bks {}

proc enable_restore {y_n} {
  set st [expr {$y_n ? !disabled : disabled}]
  .tlbk.bklist state $st
  .tlbk.all state $st
  .tlbk.done state $st
} ; # enable_restore

proc finish_restore {} {
  vwait ::done_waiting
  # now log_widget_finish should have run and re-enabled its close button.
  # We won't wait for the log dialog to close, but we will
  # update the packages display in the main window.
  update_local_revnumbers
  collect_and_display_filtered
} ; # finish_restore

proc restore_all {} {
  run_cmd "restore --force --all" 1
  finish_restore
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
  set ans [tk_messageBox -message [__ "Restore %s to revision %s?" $p $r] \
               -type okcancel -parent .tlbk]
  if {$ans ne {ok}} return
  run_cmd "restore --force $p $r" 1
  finish_restore
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
    tk_messageBox -message [__ "No backups configured"]
    return
  }
  run_cmd_waiting "option backupdir"
  set re {backupdir\t(.*)$}
  set bdir ""
  foreach l $::out_log {
    if [regexp $re $l m bdir] break
  }
  if {$bdir eq ""} {
    tk_messageBox -message [__ "No backup directory defined"]
    return
  }
  set bdir [file join [exec kpsewhich -var-value TEXMFROOT] $bdir]
  if {! [file isdirectory $bdir]} {
    tk_messageBox -message [__ "Backup directory %s does not exist" $bdir]
    return
  }
  set pwd0 [pwd]
  cd $bdir
  set backups [lsort [glob -nocomplain *.tar.xz]]
  if {[llength $backups] == 0} {
    tk_messageBox -message [__ "No backups found in $bdir"]
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
    tk_messageBox -message [__ "No packages in backup directory %s" $bdir]
    return
  }
  # invert sort order of revisions for each package
  foreach pk [dict keys $::bks] {
    dict set ::bks $pk [lsort -decreasing [dict get $::bks $pk]]
  }
  toplevel .tlbk -class Dialog
  wm withdraw .tlbk
  wm transient .tlbk .
  wm title .tlbk [__ "Restore from backup"]
  if $::plain_unix {wm attributes .tlbk -type dialog}

  # wallpaper frame; see populate_main
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

  .tlbk.bklist heading "pkg" -text [__ "Package"] -anchor w
  .tlbk.bklist heading "rev" -text [__ "Revision"] -anchor w

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
  ppack [ttk::button .tlbk.all -text [__ "Restore all"] -command restore_all] \
        -in .tlbk.fbut -side right
  ppack [ttk::button .tlbk.done -text [__ "Close"] -command {
    end_dlg "" .tlbk}] -in .tlbk.fbut -side right

  place_dlg .tlbk .
  wm resizable .tlbk 0 0
  tkwait .tlbk
} ; # restore_backups_dialog

} ; # if $::do_restore

##### Supporting procs and callbacks ##################

##### package-related #####

### updating

proc update_tlmgr_w32 {} {
  close_tlmgr
  # don't try pipes or capturing, because of
  # tlmgr's acrobatics with nested command prompts
  wm iconify .
  exec -ignorestderr cmd /k "start cmd /k tlmgr update --self"
  exec $::progname &
  destroy .
}

proc update_tlmgr {} {
  if {! $::need_update_tlmgr} {
    tk_messageBox -message [__ "Nothing to do!"]
    return
  }
  if {$::tcl_platform(platform) eq "windows"} {
    update_tlmgr_w32
    return
  }
  run_cmd "update --self" 1
  vwait ::done_waiting
  # tlmgr restarts itself automatically
  update_local_revnumbers
  .topfr.linfra configure -text \
      "tlmgr: r[dict get $::pkgs texlive.infra localrev]"
  collect_and_display_filtered
} ; # update_tlmgr

proc update_all {} {
  set updated_tlmgr 0
  if $::need_update_tlmgr {
    if {$::tcl_platform(platform) eq "windows"} {
      return ; # just to be sure; 'update all' button should be disabled
    }
    run_cmd "update --self" 1
    vwait ::done_waiting
    # tlmgr restarts itself automatically
    update_local_revnumbers
    set updated_tlmgr 1
  }
  # tlmgr restarts itself automatically
  #  tk_messageBox -message [__ "Update self first!"]
  #  return
  if {! $::n_updates && !$updated_tlmgr} {
    tk_messageBox -message [__ "Nothing to do!"]
    return
  } elseif $::n_updates {
    run_cmd "update --all" 1
    vwait ::done_waiting
    update_local_revnumbers
  }
  collect_and_display_filtered
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
    tk_messageBox -message [__ "Nothing to do!"] -type ok -icon info
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
    set ans \
        [any_message \
             [__ "Also installing dependencies\n\n%s" $deps] \
             "okcancel"]
    if {$ans eq "cancel"} return
  }
  run_cmd "install $todo" 1
  vwait ::done_waiting
  if {[llength $pre_installed] > 0} {
    lappend ::err_log [__ "Already installed: %s" $pre_installed]
    show_err_log
  }
  update_local_revnumbers
  if {$sel_opt eq "marked"} unmark_all
  collect_and_display_filtered
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
    tk_messageBox -message [__ "Nothing to do!"] -type ok -icon info
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
    set ans [any_message [__ "Also updating dependencies\n\n%s?" $deps] \
       "yesnocancel"]
    switch $ans {
      "cancel" return
      "yes" {run_cmd "update $todo" 1}
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
              [__ "Updating some dependencies %s anyway. Continue?" $deps] \
                       "okcancel"]
          if {$ans eq "cancel"} return
        }
        run_cmd "update --no-depends $todo" 1
      }
    }
  } else {
    run_cmd "update $todo" 1
  }
  vwait ::done_waiting
  if {[llength $not_inst] > 0} {
    lappend ::err_log [__ "Skipped because not installed: %s" $not_inst]
  }
  if {[llength $uptodate] > 0} {
    lappend ::err_log [__ "Skipped because already up to date: %s" $uptodate]
  }
  if {[llength $not_inst] > 0 || [llength $uptodate] > 0} {
    show_err_log
  }
  update_local_revnumbers
  if {$sel_opt eq "marked"} unmark_all
  collect_and_display_filtered
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
    tk_messageBox -message [__ "Nothing to do!"] -type ok -icon info
    return
  }
  if {[llength $deps] > 0} {
    set ans [any_message [__ "Also remove dependencies\n\n%s?" $deps] \
                "yesnocancel"]
    switch $ans {
      "cancel" return
      "yes" {run_cmd "remove $todo" 1}
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
              [__ "Removing some dependencies %s anyway. Continue?" $deps] \
                       "okcancel"]
          if {$ans eq "cancel"} return
        }
        run_cmd "remove --no-depends $todo" 1
      }
    }
  } else {
    run_cmd "remove $todo" 1
  }
  vwait ::done_waiting
 if {[llength $not_inst] > 0} {
    lappend ::err_log "Skipped because not installed: $not_inst"
    show_err_log
  }
  update_local_revnumbers
  if {$sel_opt eq "marked"} unmark_all
  collect_and_display_filtered
} ; # remove_pkgs

# restoring packages is a rather different story, controlled by the
# contents of the backup directory. see further up.

##### varous callbacks #####

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

proc toggle_marked_pkg {itm cl} {
  # toggle_marked_pkg is triggered by a mouse click only in column #1.
  # 'marked' should get updated in ::pkgs, ::filtered and in .pkglist.

  if {$cl ne "#1"} return
  # clicking in the header:
  if {$itm eq ""} return
  # $mrk: negation of current value of marked for $itm
  set mrk [expr {[dict get $::pkgs $itm "marked"] ? 0 : 1}]
  dict set ::pkgs $itm "marked" $mrk
  set m [mark_sym $mrk]
  dict set ::filtered $itm [lreplace [dict get $::filtered $itm] 0 0 $m]
  .pkglist set $itm mk $m
} ; # toggle_marked_pkg

proc unmark_all {} {
  foreach nm [dict keys $::pkgs] {
    dict set ::pkgs $nm "marked" 0
  }
  set m [mark_sym 0]
  foreach nm [dict keys $::filtered] {
    dict set ::filtered $nm [lreplace [dict get $::filtered $nm] 0 0 $m]
  }
  foreach nm [.pkglist children {}] {
    .pkglist set $nm mk $m
  }
} ; # unmark_all

proc mark_displayed {} {
  set m [mark_sym 1]
  foreach nm [.pkglist children {}] {
    dict set ::pkgs $nm "marked" 1
    dict set ::filtered $nm [lreplace [dict get $::filtered $nm] 0 0 $m]
    .pkglist set $nm mk $m
  }
} ; # mark_displayed

##### package popup #####

proc do_package_popup_menu {x y X Y} {
  # as focused item, the identity of the clicked item will be
  # globally available:
  .pkglist focus [.pkglist identify item $x $y]
  # recreate menu with only applicable items
  set lr [dict get $::pkgs [.pkglist focus] "localrev"]
  set rr [dict get $::pkgs [.pkglist focus] "remoterev"]
  .pkg_popup delete 0 end

  .pkg_popup add command -label [__ "Info"] -command {
    run_cmd "info [.pkglist focus]" 1; vwait ::done_waiting
  }
  if {$::have_remote && ! $::need_update_tlmgr && $rr > 0 && $lr == 0} {
    .pkg_popup add command -label [__ "Install"] -command {
      install_pkgs "focus"
    }
  }
  if {$::have_remote && ! $::need_update_tlmgr && $lr > 0 && $rr > $lr} {
    .pkg_popup add command -label [__ "Update"] -command {
      update_pkgs "focus"
    }
  }
  if {$lr > 0} {
    .pkg_popup add command -label [__ "Remove"] -command {
      remove_pkgs "focus"
    }
  }
  .pkg_popup post [expr {$X - 2}] [expr {$Y - 2}]
  focus .pkg_popup
} ; # do_package_popup_menu

#### gui options ####

proc set_language {l} {
  set ::lang $l
  load_translations
  rebuild_interface
  if {! [save_config_var "gui-lang" $::lang]} {
    tk_messageBox -message [__"Cannot save language setting"]
  }
}

proc set_fontscale {s} {
  set ::tkfontscale $s
  redo_fonts
  rebuild_interface
  if {[dict get $::pkgs texlive.infra localrev] >= 54766} {
    if {! [save_config_var "tkfontscale" $::tkfontscale]} {
      tk_messageBox -message [__"Cannot save font scale setting"]
    }
  }
}

proc zoom {n} {
  if {$n <= 0} {set n 1}
  set_fontscale [expr {$n*$::tkfontscale}]
}

##### running external commands #####
# for when a simple 'set var [exec command]' won't do

# For capturing an external command, we need a separate output channel,
# but we reuse ::out_log.
# stderr is bundled with stdout so ::err_log should stay empty.
proc read_capture {} {
  set l "" ; # will contain the line to be read
  if {([catch {chan gets $::capt l} len] || [chan eof $::capt])} {
    catch {chan close $::capt}
    log_widget_finish
    set ::done_waiting 1
  } elseif {$len >= 0} {
    lappend ::out_log $l
    log_widget_add $l
  }
}; # read_capture

proc run_external {cmd {mess ""}} {
  set ::out_log {}
  set ::err_log {}
  if {$mess ne ""} {lappend ::out_log $mess}
  unset -nocomplain ::done_waiting
  # treat cmd as a list, possibly of one element
  # using a list enables a direct invocation, bypassing a shell
  set cmd0 [lindex $cmd 0]
  set cmd [lreplace $cmd 0 0 "|$cmd0"]
  set cmd [list {*}$cmd 2>@1]
  if [catch {open $cmd r} ::capt] {
    tk_messageBox -message "Failure to launch $cmd"
  }
  chan configure $::capt -buffering line -blocking 0
  chan event $::capt readable read_capture
  log_widget_init
}

proc about_cmd {} {
  set msg "\u00a9 2017-2021 Siep Kroonenberg\n\n"
  append msg [__ "GUI interface for TeX Live Manager\nImplemented in Tcl/Tk"]
  tk_messageBox -message $msg
}

proc tlshell_help {} {
  set msg \
      [__ "This TeX Live Manager front-end should be pretty self-explanatory.
Note that it does not cover the full functionality of tlmgr. Notably, it does not cover user mode or backup and restore.
Consult the 'Tlmgr Help' menu for full documentation on tlmgr itself."]
  tk_messageBox -message $msg
}

proc tlmgr_help {} {
  set ::env(NOPERLDOC) 1
  long_message [exec tlmgr --help] ok
}

## no arbitrary commands: no way to know what data have to be updated
#proc custom_command {} {
#  create_dlg .tlcust .
#  wm title .tlcust [__ "Custom command"]
#  pack [ttk::frame .tlcust.bg] -expand 1 -fill x
#
#  ppack [ttk::entry .tlcust.e] \
#      -in .tlcust.bg -side left -fill x -expand 1
#  ppack [ttk::button .tlcust.b -text [__ "Go"] -command run_entry] \
#      -in .tlcust.bg -side left
#  bind .tlcust.e <Return> run_entry
#  bind .tlcust <Escape> {end_dlg "" .tlcust}
#  wm .tlcust resizable 1 0
#  place_dlg .tlcust .
#}

##### main window #####

proc try_loading_remote {} {
  if {[possible_repository $::repos(main)]} {
    get_packages_info_remote
    collect_and_display_filtered
  } else {
    set mes [__ "%s is not a local or remote repository.
Please configure a valid repository" $::repos(main)]
    append mes "\n([__ "Options"] \/ [__ "Repositories"] ...)"
    tk_messageBox -message $mes -title [__ "Error"] -type ok -icon error
  }
}

##### w64 binaries ####################################################

# Although w64 binaries can be added if windows is only an
# additional platform, the extra w64 command-prompt can only be
# added from within windows. The situation is a bit difficult to
# explain to begin with, so it seems better to offer the option only
# for a native windows install.

# I would have liked to avoid pointless downloads,
# but comparing dates is fraught with problems:
# - remote file info is reported differently by different protocols
# - the date of a local file may be installation time
# - spurious differences due to local time and daylight saving time
# we CAN timestamp zipfiles to their latest member
# with 'unzip -T', but this can only be done AFTER downloading.
# See also tcl commands 'file mtime', and 'clock scan'

# $::wprocessor will later decide whether a w64 menu will be created.
if {$::tcl_platform(platform) eq "windows"} {
  set ::wprocessor $::env(PROCESSOR_ARCHITECTURE)
  # in case of 32-bit programs on 64-bit system:
  if [info exists ::env(PROCESSOR_ARCHITEW6432)] {
    set ::wprocessor $::env(PROCESSOR_ARCHITEW6432)
  }
}

proc w64_about {} {
  # read and display file with warning message
  set fn [exec kpsewhich -format texmfscripts help-w64.txt]
  if {$fn eq "" || [catch {open $fn} wf]} {
    tk_messageBox -message "Failed to open help-w64.txt"
    return 0
  }
  set s ""
  while {! [catch {chan gets $wf} line] && ! [chan eof $wf]} {
    set s [string cat $s "\n" $line]
  }
  chan close $wf
  long_message [string range $s 1 end] "okcancel"
}

set ::w64_zipdir "${::instroot}/bin/win64/zip"
set ::w64_zipname "tl-win64"

proc create_w64_shortcut {} {
  # delegate the creation of a start menu shortcut to powershell.
  # consider failure non-fatal
  set shortcutfile \
     [file join \
          [expr {$::multiuser ? $::env(programdata) : $::env(appdata)}] \
          "microsoft\\windows\\start menu\\programs" \
          "TeX Live $::year" \
          "TeX Live $::year 64-bit.lnk"]
  set shortcut_bsl [string map {\/ \\} $shortcutfile]

  set inst_bsl [string map {\/ \\} $::instroot]
  set path_add "${inst_bsl}\\bin\\win64;${inst_bsl}\\bin\\win32;"
  # below, escape '%' for the sake of tcl, probably unnecessary here
  # i believe '%' has no special significance  for powershell.
  set shargs "/k path ${path_add}\%PATH\% && title TeX Live 64-bit"
  set cmd [string cat \
      "\$ws = new-object -comobject wscript.shell;" \
      "\$s = \$ws.createshortcut('$shortcut_bsl');" \
      "\$s.targetpath = 'cmd';" \
      "\$s.arguments = '$shargs';" \
      "\$s.workingdirectory = '\%userprofile\%';" \
               "\$s.save()"]
  # powershell will silently overwrite an existing shortcut
  set res [catch {exec cmd /c powershell.exe -NoLogo -NonInteractive \
                      -NoProfile -command $cmd}]
  if {$res || ![file exists $shortcutfile]} {
    return 0
  }
  return 1
}

proc remove_w64 {} {
  total_dis_enable 0
  update idletasks
  # remove shortcut
  set shortcutfile \
     [file join \
          [expr {$::multiuser ? $::env(programdata) : $::env(appdata)}] \
          "microsoft\\windows\\start menu\\programs" \
          "TeX Live $::year" \
          "TeX Live $::year 64-bit.lnk"]
  file delete -force $shortcutfile
  cd $::instroot
  set w64dir [file join $::instroot "bin" "win64"]
  total_dis_enable 1
  update idletasks
  catch {file delete -force $w64dir}
  if {[file exists $shortcutfile] || [file exists $w64dir]} {
    tk_messageBox -message "$w64dir not completely removed"
    dis_enable_w64
    return 0
  } else {
    tk_messageBox -message "$w64dir completely removed"
    dis_enable_w64
    return 1
  }
}

proc make_64_dirs {} {
  if [catch {file mkdir "$::instroot/bin/win64/zip"}] {
    tk_messageBox -message "Cannot create required directory"
    return 0
  }
  return 1
}

# local or remote source: these are separate submenu entries

proc get_remote_w64 {} {

  set curl "$::instroot/tlpkg/installer/curl/curl.exe"
  set url ""
  if {[string first {://} $::repos(main)]<0} {
    # local repository, we assume w64 not included
    set url ""
  } elseif $::have_remote {
    set url $::actual_main
  } else {
    set url $::repos(main)
    if {$url eq $::any_mirror} {
      set url ""
    }
  }
  # we are currently in $::w64_zipdir
  if [file exists ${::w64_zipname}.zip] {
    # here and elsewhere, we do not consider a rename failure fatal
    catch {file rename -force ${::w64_zipname}.zip ${::w64_zipname}_old.zip}
  }
  set notfound 1
  set ntries 1
  while {$notfound && $ntries<6} {
    if {$url eq ""} {
      # ask curl for a specific mirror
      if [catch {exec $curl -Ls -o nul -w %{url_effective} $::any_mirror} url] {
        tk_messageBox -message "Cannot get any mirror"
        return 0
      }
    }
    set c [string last "/texlive" $url]
    if {$c<0} {
      lappend ::err_log "Mirror $url no good at try $ntries"
      continue
    }
    set url [string range $url 0 $c] ; # the new value includes a final '/'
    set url "${url}win32/TLW64/${::w64_zipname}.zip"
    # download options:
    # -s: silent; -f: silent fail; -o: target name; -R: preserve time
    incr ntries
    set notfound [catch {
      exec -keepnewline -ignorestderr $curl -s -f -o ${::w64_zipname}.zip -R $url
    }]
    if {!$notfound} {
      lappend ::err_log "Success after $ntries tries"
      return 1
    } else {
      set url ""
    }
  }
  tk_messageBox -message \
   "No success after $ntries tries.\nFailed to download ${::w64_zipname}.zip"
  return 0
}

proc get_local_w64 {} {
  # we are currently in $::w64_zipdir
  # invoke file browser
  set zipfile [tk_getOpenFile \
                   -filetypes {{"Zip files" {.zip .ZIP}}} \
                   -initialfile "${::w64_zipname}.zip" \
                   -title "Zipfile with 64-bit binaries"]
  if {$zipfile eq ""} {
    return 0
  } else {
    if {[file normalize $zipfile] ne "${::w64_zipdir}/${::w64_zipname}.zip"} {
      # nothing to be done if zipfile is at its intended location
      if [file exists "${::w64_zipname}.zip"] {
        file rename -force "${::w64_zipname}.zip" "${::w64_zipname}_old.zip"
        # ignore failure; below, we shall just try to overwrite
      }
      if [catch {file copy -force $zipfile ${::w64_zipname}.zip}] {
        tk_messageBox -message \
            "Cannot copy $zipfile to ${::w64_zipdir}/${::w64_zipname}.zip"
        return 0
      }
    }
  }
  return 1
}

proc dis_enable_w64 {} {
  if [file exists [file join $::instroot "bin" "win64"]] {
    .mn.w64 entryconfigure $::inx_remove64 -state normal
  } else {
    .mn.w64 entryconfigure $::inx_remove64 -state disabled
  }
  if [file exists [file join $::w64_zipdir "${::w64_zipname}.zip"]] {
    .mn.w64 entryconfigure $::inx_sync64 -state normal
  } else {
    .mn.w64 entryconfigure $::inx_sync64 -state disabled
  }
}

proc sync_w64_w32 {} {
  # disable interface, but only if invoked directly
  set caller [lindex [info level 1] 0]
  if {$caller ne "add_or_update_w64"} {
    total_dis_enable 0
    update idletasks
  }
  # try to clear out bin/win64
  foreach f [glob -nocomplain -directory $::instroot/bin/win64 *] {
    if {![file isdirectory $f]} {
      catch {file delete $f}
    }
  }
  cd $::instroot; # actually already done by invoker
  if [catch {
    exec unzip -d $::instroot -qo ${::w64_zipdir}/${::w64_zipname}.zip} r] {
    tk_messageBox -message "$r:\nDid not succeed in extracting all win64 files"
    return 0
  }
  cd $::instroot/bin/win64
  # all 64-bit executable files should have corresponding 32-bit files
  set sync_mess {}
  foreach f [glob -nocomplain *] {
    # exempt .dll files
    if {[string tolower [string range $f end-3 end]] ne ".dll"} {
      if {! [file exists ../win32/$f] && ![file isdirectory $f]} {
        if [catch {file delete $f}] {
          lappend sync_mess $f
        }
      }
    }
  }
  cd $::instroot
  if {$caller ne "add_or_update_w64"} {
    total_dis_enable 1
    update idletasks
  }
  return $sync_mess
}

proc add_or_update_w64 {lr} {
  if {! [file exists ${::w64_zipname}.zip]} {
    # 'about' message if no prior w64
    w64_about
  }
  set ok 1
  # below, fatal errors in invoked procs produce error message boxes
  # so nothing remains but abandoning the effort.
  if {! [make_64_dirs]} {set ok 0}

  if {$ok && [catch {cd $::w64_zipdir}]} {
    tk_messageBox -message "Cannot access $::w64_zipdir"
    set ok 0
  }
  if {! $ok} {return 0}

  # operations below may take time: disable interface
  total_dis_enable 0
  update idletasks
  if {$lr eq "remote"} {
    if {! [get_remote_w64]} {
      set ok 0
    }
  } else {
    if {! [get_local_w64]} {
      set ok 0
    }
  }
  if $ok {
    # unpack new zip selectively
    set mess [sync_w64_w32] ; # a list of filenames
    if {[llength $mess]>0} {
      set mess [linsert $mess 0 \
        "The following files should have been deleted from bin/w64 but were not:"]
    }
    # now w64 shortcut for cmd.exe using powershell invocation
    if [create_w64_shortcut] {
      lappend mess "64-bit TeXLive shortcut created"
    } else {
      lappend mess "Failed to create 64-bit TeXLive shortcut"
    }
    lappend mess "Downloaded and installed 64-bit binaries."
      lappend mess "Done"
    any_message [join $mess "\n"] ok
  }

  total_dis_enable 1
  update idletasks
  return 1
}

##### main window #####################################################

proc populate_main {} {

  wm withdraw .

  wm title . "TeX Live Shell"

  ## menu ##

  # dummy empty menu to replace the real menu .mn in disabled states.
  # the "File" cascade should ensure that the dummy menu
  # occupies the same vertical space as the real menu.
  menu .mn_empty
  .mn_empty add cascade -label [__ "File"] -menu .mn_empty.file -underline 0
  if $::plain_unix {
    .mn_empty configure -borderwidth 0
    .mn_empty configure -background $::default_bg
  menu .mn_empty.file
  }
  # real menu
  menu .mn
  . configure -menu .mn
  if $::plain_unix {
    .mn configure -borderwidth 0
    .mn configure -background $::default_bg

    # plain_unix: avoid a possible RenderBadPicture error on quitting
    # when there is a menu.
    # 'send' bypasses the bug by changing the shutdown sequence.
    # 'tk appname <something>' restores 'send'.
    bind . <Destroy> {
      catch {tk appname appname}
    }
  }

  .mn add cascade -label [__ "File"] -menu .mn.file -underline 0
  menu .mn.file
  .mn.file add command -label [__ "Load repository"] \
      -command try_loading_remote
  .mn.file add command -command {destroy .} -label [__ "Exit"] -underline 1

  .mn add cascade -label [__ "Actions"] -menu .mn.act -underline 0
  menu .mn.act
  .mn.act add command -label [__ "Regenerate filename database"] -command \
      {run_external "mktexlsr" [__ "Regenerating filename database..."]}
  .mn.act add command -label [__ "Regenerate formats"] -command \
      {run_external "fmtutil-sys --all" [__ "Rebuilding formats..."]}
  .mn.act add command -label [__ "Regenerate fontmaps"] -command \
      {run_external "updmap-sys" [__ "Rebuilding fontmap files..."]}
  #.mn.act add command -label [__ "Custom command"] -command custom_command

  .mn add cascade -label [__ "Options"] -menu .mn.opt -underline 0

  # need to keep track of indices of this submenu
  menu .mn.opt
  set inx -1
  incr inx
  .mn.opt add command -label "[__ "Repositories"] ..." \
      -command repository_dialog
  incr inx
  .mn.opt add cascade -label [__ "Paper ..."] -menu .mn.opt.paper
  incr inx

  menu .mn.opt.paper
  foreach p {A4 Letter} {
    .mn.opt.paper add command -label $p -command \
        "set_all_papers [string tolower $p]"
  }
  .mn.opt.paper add command -label "[__ "Advanced"] ..." \
      -command papersize_advanced


  if {$::tcl_platform(platform) ne "windows"} {
    incr inx
    set ::inx_platforms $inx
    .mn.opt add command -label "[__ "Platforms"] ..." -command platforms_select
  }

  if {$::tcl_platform(platform) eq "windows" && $::wprocessor eq "AMD64"} {
    .mn add cascade -label "64-bit Windows" -menu .mn.w64
    menu .mn.w64
    set inx -1
    incr inx
    .mn.w64 add command -label [__ "About"] -command w64_about
    incr inx
    .mn.w64 add command -label [__ "Add/replace 64-bit binaries"] \
        -command "add_or_update_w64 remote"
    incr inx
    .mn.w64 add command -label [__ "Add/replace from local file"] \
        -command "add_or_update_w64 local"
    incr inx
    set ::inx_sync64 $inx
    .mn.w64 add command -label [__ "Resynchronize with 32-bit"] \
        -command sync_w64_w32
    incr inx
    set ::inx_remove64 $inx
    .mn.w64 add command -label [__ "Remove 64-bit binaries"] \
        -command remove_w64
  }

  if {[llength $::langs] > 1} {
    .mn add cascade -label [__ "GUI language"] \
        -menu .mn.lang
    menu .mn.lang
    foreach l [lsort $::langs] {
      if {$l eq $::lang} {
        set mlabel "$l *"
      } else {
        set mlabel $l
      }
      .mn.lang add command -label $mlabel \
          -command "set_language $l"
    }
  }

  .mn add cascade -label [__ "GUI font scaling"] \
      -menu .mn.fscale
  menu .mn.fscale
  .mn.fscale add command -label \
      "[__ "Current:"]  [format {%.2f} $::tkfontscale]"
  foreach s {0.6 0.8 1 1.2 1.6 2 2.5 3 3.8 5 6 7.5 9} {
    .mn.fscale add command -label $s -command "set_fontscale $s"
  }

  # browser-style keyboard shortcuts for scaling
  bind . <Control-KeyRelease-minus> {zoom 0.8}
  bind . <Control-KeyRelease-equal> {zoom 1.25}
  bind . <Control-Shift-KeyRelease-equal> {zoom 1.25}
  bind . <Control-KeyRelease-plus> {zoom 1.25}
  bind . <Control-KeyRelease-0> {set_fontscale 1}
  if {$::tcl_platform(os) eq "Darwin"} {
    bind . <Command-KeyRelease-minus> {zoom 0.8}
    bind . <Command-KeyRelease-equal> {zoom 1.25}
    bind . <Command-Shift-KeyRelease-equal> {zoom 1.25}
    bind . <Command-KeyRelease-plus> {zoom 1.25}
    bind . <Command-KeyRelease-0> {set_fontscale 1}
  }

  .mn add cascade -label [__ "Help"] -menu .mn.help -underline 0
  menu .mn.help
  .mn.help add command -label [__ "About"] -command about_cmd
  .mn.help add command -label [__ "TLShell Help"] -command tlshell_help
  .mn.help add command -label [__ "Tlmgr Help"] -command tlmgr_help

  ## menu end

  # wallpaper frame
  # it is possible to set a background color for a toplevel, but on
  # MacOS I did not find a way to determine the right $::default_bg
  # color. Instead, all toplevels are given a wallpaper ttk::frame
  # with the default ttk::frame color, which seems to work
  # everywhere.
  pack [ttk::frame .bg] -expand 1 -fill both
  .bg configure -padding 5pt

  # bottom of main window
  pack [ttk::frame .endbuttons] -in .bg -side bottom -fill x
  ttk::label .busy -textvariable ::busy -font hfont -anchor w
  ppack .busy -in .endbuttons -side left
  ppack [ttk::button .q -text [__ Quit] -command {destroy .}] \
      -in .endbuttons -side right
  ppack [ttk::button .r -text [__ "Restart self"] -command restart_self] \
      -in .endbuttons -side right
  ppack [ttk::button .t -text [__ "Restart tlmgr"] \
             -command {close_tlmgr; start_tlmgr}] \
      -in .endbuttons -side right
  ttk::button .showlogs -text [__ "Show logs"] -command show_logs
  ppack .showlogs -in .endbuttons -side right

  # top of main window
  ppack [ttk::frame .topf] -in .bg -side top -anchor w -fill x

  if $::ddebug {
    ppack [ttk::label .topf.test -text [info nameofexecutable]]
    ppack [ttk::label .topf.test2 -text $::progname]
    ppack [ttk::label .topf.test3 -text \
           [string range $::env(PATH) 0 59]]
  }

  # left info frame
  pack [ttk::frame .topfl] -in .topf -side left -anchor nw

  # subframe for repositories, to be filled by show_repos
  pack [ttk::frame .toprepo] -in .topfl -side top -anchor w

  # various info (left frame)
  pack [ttk::frame .topfll] -in .topfl -side top -anchor nw -pady {6pt 0pt}
  ttk::label .topfll.lluptodate -text [__ "TL Manager up to date?"] -anchor w
  pgrid .topfll.lluptodate -row 2 -column 0 -sticky w
  ttk::label .topfll.luptodate -text [__ "Unknown"] -anchor w
  pgrid .topfll.luptodate -row 2 -column 1 -sticky w

  ttk::label .topfll.llcmd -text [__ "Last tlmgr command:"] -anchor w

  pgrid .topfll.llcmd -row 3 -column 0 -sticky w
  ttk::label .topfll.lcmd -textvariable ::last_cmd \
      -wraplength [expr {60*$::cw}] -justify left -anchor w
  pgrid .topfll.lcmd -row 3 -column 1 -sticky w

  # right info frame
  ppack [ttk::frame .topfr] -in .topf -side right -anchor ne
  if {$::tcl_platform(platform) eq "windows"} {
    pack [ttk::label .topfr.ladmin] -side top -anchor e
  }
  pack [ttk::label .topfr.lroot] -side top -anchor e
  .topfr.lroot configure -text [__ "Root at %s" $::instroot]
  pack [ttk::label .topfr.linfra] -side top -anchor e
  pack [ttk::label .topfr.lshell] -side top -anchor e

  pack [ttk::separator .sp -orient horizontal] \
      -in .bg -side top -fill x -pady 3pt

  # controls frame, between info frame and package list
  pack [ttk::frame .middle] -in .bg -side top -fill x
  pack [ttk::frame .pkcontrol] \
      -in .middle -side left -anchor nw -fill x

  # package list display options
  ttk::label .lpack -text [string toupper [__ "Package list"]] \
      -font hfont
  pack .lpack -in .pkcontrol -side top -padx 3pt -pady {6pt 6pt} -anchor w

  pack [ttk::frame .pkfilter -relief groove -borderwidth 2 -padding 3pt] \
    -in .pkcontrol -side top -anchor nw
  # on my current linux, groove works only with a dimensionless borderwidth

  # separator columns
  grid columnconfigure .pkfilter 1 -minsize 20pt
  grid [ttk::separator .pkfilter.sep1 -orient vertical] \
    -column 1 -row 0 -rowspan 5 -sticky ns
  grid columnconfigure .pkfilter 3 -minsize 20pt
  grid [ttk::separator .pkfilter.sep3 -orient vertical] \
    -column 3 -row 0 -rowspan 5 -sticky ns

  # filter on status: inst, all, upd
  ttk::label .pkfilter.lstat -font bfont -text [__ "Status"]
  ttk::radiobutton .pkfilter.inst -text [__ "Installed"] -value inst \
      -variable ::stat_opt -command collect_and_display_filtered
  ttk::radiobutton .pkfilter.notins -text [__ "Not installed"] -value notins \
      -variable ::stat_opt -command {
        if {! $::have_remote} get_packages_info_remote
        collect_and_display_filtered
      }
  ttk::radiobutton .pkfilter.alls -text [__ "All"] -value all \
      -variable ::stat_opt -command {
        if {! $::have_remote} get_packages_info_remote
        collect_and_display_filtered
      }
  ttk::radiobutton .pkfilter.upd -text [__ "Updatable"] -value upd \
      -variable ::stat_opt -command {
        if {! $::have_remote} get_packages_info_remote
        collect_and_display_filtered
      }
  grid  .pkfilter.lstat  -column 0 -row 0 -sticky w -padx {3pt 50pt}
  pgrid .pkfilter.inst   -column 0 -row 1 -sticky w
  pgrid .pkfilter.notins -column 0 -row 2 -sticky w
  pgrid .pkfilter.alls   -column 0 -row 3 -sticky w
  pgrid .pkfilter.upd    -column 0 -row 4 -sticky w

  # filter on detail level: all, coll, schm
  ttk::label .pkfilter.ldtl -font bfont -text [__ "Detail >> Global"]
  ttk::radiobutton .pkfilter.alld -text [__ All] -value all \
      -variable ::dtl_opt -command collect_and_display_filtered
  ttk::radiobutton .pkfilter.coll -text [__ "Collections and schemes"] \
      -value coll -variable ::dtl_opt -command collect_and_display_filtered
  ttk::radiobutton .pkfilter.schm -text [__ "Only schemes"] -value schm \
      -variable ::dtl_opt -command collect_and_display_filtered
  pgrid .pkfilter.ldtl -column 2 -row 0 -sticky w
  pgrid .pkfilter.alld -column 2 -row 1 -sticky w
  pgrid .pkfilter.coll -column 2 -row 2 -sticky w
  pgrid .pkfilter.schm -column 2 -row 3 -sticky w

  # search interface
  ttk::label .pkfilter.search_l -font bfont -text [__ "Search"]
  ttk::entry .pkfilter.search_e -width 30
  ttk::radiobutton .pkfilter.search_n -variable ::search_desc \
             -value 0 -text [__ "By name"]
  ttk::radiobutton .pkfilter.search_d -variable ::search_desc \
             -value 1 -text [__ "By name and description"]
  pgrid .pkfilter.search_l -column 4 -row 0 -sticky w
  pgrid .pkfilter.search_e -column 4 -row 1 -sticky w
  pgrid .pkfilter.search_n -column 4 -row 2 -sticky w
  pgrid .pkfilter.search_d -column 4 -row 3 -sticky w

  bind .pkfilter.search_e <KeyRelease> display_packages_info
  bind .pkfilter.search_n <ButtonRelease> \
      {set ::search_desc 0; display_packages_info}
  bind .pkfilter.search_d <ButtonRelease> \
      {set ::search_desc 1; display_packages_info}

  # actions
  pack [ttk::frame .pkact] -in .middle -side right -anchor ne
  ttk::button .mrk_inst -text [__ "Install marked"] -command {
      install_pkgs "marked"}
  ppack .mrk_inst -in .pkact -side top -fill x
  ttk::button .mrk_upd -text [__ "Update marked"] -command {
    update_pkgs "marked"}
  ppack .mrk_upd -in .pkact -side top -fill x
  ttk::button .mrk_rem -text [__ "Remove marked"] -command {
    remove_pkgs "marked"}
  ppack .mrk_rem -in .pkact -side top -fill x
  if $::do_restore {
    ttk::button .mrk_rest -text "[__ "Restore from backup"] ..." -command \
        restore_backups_dialog
    ppack .mrk_rest -in .pkact -side top -fill x
  }
  ttk::button .upd_tlmgr -text [__ "Update tlmgr"] -command update_tlmgr
  ppack .upd_tlmgr -in .pkact -side top -fill x
  ttk::button .upd_all -text [__ "Update all"] -command update_all
  ppack .upd_all -in .pkact -side top -fill x

  # marking all/none
  pack [ttk::frame .pksel] \
      -in .bg -pady 6pt -side top -fill x
  pack [ttk::button .mrk_all -text [__ "Mark all displayed"] \
       -command mark_displayed] -in .pksel -side left
  pack [ttk::button .mrk_none -text [__ "Mark none"] -command unmark_all] \
      -in .pksel -padx 6pt -side left
  ttk::label .binwarn \
    -text [__ "Only packages for installed platforms are displayed"]
  pack .binwarn -in .pksel -padx 3pt -side right -anchor s

  # packages list itself
  pack [ttk::frame .fpkg] -in .bg -side top -fill both -expand 1
  ttk::treeview .pkglist -columns \
      {mk name localrev remoterev shortdesc} \
      -show headings -height 8 -selectmode extended \
      -yscrollcommand {.pkvsb set}
  .pkglist heading mk -text "" -anchor w
  .pkglist heading name -text [__ "Name"] -anchor w
  .pkglist heading localrev -text [__ "Local rev. (ver.)"] -anchor w
  .pkglist heading remoterev -text [__ "Remote rev. (ver.)"] -anchor w
  .pkglist heading shortdesc -text [__ "Description"] -anchor w
  .pkglist column mk -width [expr {$::cw * 3}] -stretch 0
  # column widths are resizable: no need to accommodate the longest names
  .pkglist column name -width [expr {$::cw * 25}] -stretch 1
  .pkglist column localrev -width [expr {$::cw * 18}] -stretch 0
  .pkglist column remoterev -width [expr {$::cw * 18}] -stretch 0
  .pkglist column shortdesc -width [expr {$::cw * 50}] -stretch 1

  ttk::scrollbar .pkvsb -orient vertical -command {.pkglist yview}
  ppack .pkglist -in .fpkg -side left -expand 1 -fill both
  ppack .pkvsb -in .fpkg -side left -fill y

  # "#1" refers to the first column (with mark symbols)
  bind .pkglist <space> {toggle_marked_pkg [.pkglist focus] "#1"}
  bind .pkglist <Return> {toggle_marked_pkg [.pkglist focus] "#1"}
  # only toggle when column is "mk" i.e. #1
  bind .pkglist <ButtonRelease-1> {toggle_marked_pkg [
      .pkglist identify item %x %y] [.pkglist identify column %x %y]}

  menu .pkg_popup ; # entries added on-the-fly
  bind .pkglist <<RightClick>> {do_package_popup_menu %x %y %X %Y}
  if $::plain_unix {
    bind .pkg_popup <Leave> {.pkg_popup unpost}
  }

  wm protocol . WM_DELETE_WINDOW {cancel_or_destroy .q .}
  wm resizable . 1 1
  wm state . normal
  raise .
}

proc display_all_data {} {
  # (re)display all data
  if {$::tcl_platform(platform) eq "windows"} {
    .topfr.ladmin configure -text \
        [expr {$::multiuser ? [__ "Multi-user"] : [__ "Single-user"]}]
  }
  # revision numbers for tlmgr and tlshell
  .topfr.linfra configure -text \
      "tlmgr: r[dict get $::pkgs texlive.infra localrev]"
  .topfr.lshell configure -text \
      "tlshell: r[dict get $::pkgs tlshell localrev]"
  show_repos
  display_packages_info
  display_updated_globals
}

# to be invoked at initialization and after a font scaling- or language change
proc rebuild_interface {} {
  foreach c [winfo children .] {catch {destroy $c}}

  # for busy/idle indicators
  set ::busy [__ "Idle"]
  populate_main
  display_all_data
  if {$::tcl_platform(platform) eq "windows"} {wm deiconify .}
}

##### initialize ######################################################

proc initialize {} {
  # seed random numbers
  expr {srand([clock seconds])}

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

  # store language in tlmgr configuration
  save_config_var "gui-lang" $::lang

  # in case we are going to do something with json:
  # add json subdirectory to auto_path, but at low priority
  # since the tcl/tk installation may already have a better implementation.
  # Trust kpsewhich to find out own directory and bypass symlinks.
  #set tlsdir [file dirname [exec kpsewhich -format texmfscripts tlshell.tcl]]
  #lappend ::auto_path [file join $tlsdir "json"]

  populate_main

  # testing writablilty earlier led to sizing problems
  if {! [dir_writable $::instroot]} {
    set ans [tk_messageBox -type yesno -icon warning -message \
         [__ "%s is not writable. You can probably not do much.
  Are you sure you want to continue?" $::instroot]]
    if {$ans ne "yes"} {exit}
  }

  start_tlmgr {*}$::argv
  if {$::tcl_platform(platform) eq "windows"} {
    run_cmd_waiting "option multiuser"
    set ::multiuser 0
    foreach l $::out_log {
      if [regexp {^\s*multiuser\s+([01])\s*$} $l d ::multiuser] break
    }
  }
  run_cmd_waiting "version"
  foreach l $::out_log {
    if {[string range $l 0 8] eq "tlversion"} {
      set ::year [string range $l end-3 end]
      break
    }
  }
  get_packages_info_local
  collect_filtered
  get_repos_from_tlmgr
  display_all_data
}; # initialize

initialize
