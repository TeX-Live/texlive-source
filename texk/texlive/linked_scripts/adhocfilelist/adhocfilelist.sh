#!/bin/sh
##
## \ProvidesFile{adhocfilelist.sh}
##              [2012/11/19 v0.71 command line -> \string\listfiles 
##                                                      \space(UL)]
##
## (C) 2012 Uwe Lück LaTeX Project Public License 
##
## essential part of the `myfilist' script:
#adhoc_head='mylshead'
## original idea: read standard file; now contained here instead:
adhoc_head='\\RequirePackage{myfilist}[2012/10/11]\\EmptyFileList';
## standard settings for `find':
adhoc_find_std='-name \*.tex'
add_type () {
    adhoc_find_std="$adhoc_find_std -o -name \*.$1"
}
add_type sty; add_type cfg; add_type cls; 
add_type dtx; add_type def; add_type fd;
adhoc_find_std="-L -maxdepth 1 ( $adhoc_find_std )" ## -L 2012-11-19 
#echo adhoc_find_std = $adhoc_find_std
## standard action (v0.7):
adhoc_fmt_std=',%f'
adhoc_fmt=$adhoc_fmt_std
##
## reduce code for help screen:
opt_help () { 
    echo \ \ \ \ \ \ \ \ \ \ "$@" 
};
## reporting detected options:
opt_log () {
    echo + adhocfilelist + "$@" +
}
## looking for options:
#while getopts "fF:hl:n:s:t:" optname
## one-letter options only, for distinguishing from `find' criteria
## (learnt from "Optionen ermitteln" by Jürgen Plate on 
##  http://www.netzmafia.de/skripten/unix/unix8.html)
optname='hh';        ## somewhat misused for controlling
while [ $optname ] ; ## the loop that processes options
  do
    #case $optname in## original code for getopts
    case $1 in
      -h)            ## help screen
        optname='h'  ## flag for avoiding main function
        echo; echo; echo; echo;
        echo =\
===============================================================================;
        echo
        #echo "\`adhocfilelist.sh' varies LaTeX's \\listfiles to produce"\
        #     a stand-alone
        #echo list of \\Provides... contents on screen\
        #     'and optionally in a text file,'
        echo "\`adhocfilelist.sh' varies LaTeX's \\listfiles to produce"\
             a stand-alone list of
        echo \\Provides... contents \for \files typed explicitly\
             or found by the \`\find\' utility.
        echo USAGE: \ \ \ [PATH]adhocfilelist.sh [-OPTIONS] [FILES]
        echo options: ' -0   files modified today,'\
                      'in addition to -g';                            ## v0.5
        opt_help -a '$ files modified $ days ago (or like -mtime $,'\
                       'cf. -0)'                                      ## v0.5
        opt_help -c '  compare info with modification dates (filedate.sty)'
                                                                      ## v0.7
        opt_help -f '  read FILES as `find'"' options and tests  "\
          '            ('"'"'\\*.tex'"')"
                 #'    \(\\\\\\*.tex\)'
        opt_help -F '$ read (first) `find'"' options/tests from file $"\
              '        (\\*.tex)'
        opt_help -g '  add FILES to standard `find'"' restrictions (cf. -f)"
        opt_help -h '  help display (what you are seeing)'
        opt_help -i '$ input TeX file $ (maybe for more'\
                       "\`nicefilelist' settings,"      ## was -m before v0.5
        opt_help '              '\
              '        cf.' \`myfilist\' 'v0.5 documentation)'
        opt_help -l '$ maximum (base) filename length for'\
                       "\`longnamefilelist.sty'"
        opt_help -n '$ longest (base) filename for `nicefilelist.sty'\'\
                      '(or ".")'
        opt_help -o '$ write list to plain text file $'
        opt_help -x '  use `xelatex'\'
        echo Without '`find'\'', FILES is a comma-separated'\
             list of \file names.
        echo Without -l or -n, \`myfilist.sty\' is used. For background\
             '(e.g., mentioned'
        echo '*.sty files), see latexfileinfo_pkgs.htm'\
             'from package latexfileinfo-pkgs,'
        echo
        opt_help http://ctan.org/pkg/latexfileinfo-pkgs
        echo
        echo =\
===============================================================================;
        echo;
        break;
	;;
      ## options for real output:	
      -0)                               ## was -2 in v0.5
        adhoc_day='0' 
        adhoc_find='\UseFindUtility'
        opt_log looking \for files modified today, using standard settings
        ## <- plural missing 2012-10-17 
        shift;;
      -a)                               ## was -d in v0.4
        adhoc_day="$2" 
        adhoc_find='\UseFindUtility'
        opt_log looking \for files modified $2 days ago,\
               using standard settings
        shift 2;;
      -c)   ## v0.7
        adhoc_find='\UseFindUtility'
        adhoc_fmt='\\ReadCheckDateOf{%f}{%TY/%Tm/%Td}'
        shift;;
      -f)
        adhoc_find='\UseFindUtility'
        adhoc_find_std=''
        opt_log expecting \find utility, adding nothing
        shift;;
      -F)
        adhoc_find='\UseFindUtility'
        adhoc_find_file="$2"
        opt_log reading \find criteria from $adhoc_find_file
        shift 2;;
      -g)                               ##v0.5
        adhoc_find='\UseFindUtility'
        opt_log expecting \find utility, using standard settings
        shift;;
      -i)				## was -m) before v0.5
        adhoc_head="\\input{$2}"        ## \input allows -l/-n
        opt_log formatting by $adhoc_head
        shift 2;;
      -l)
        adhoc_head="\\RequirePackage{longnamefilelist}
                    \listfiles[$2]
                    $adhoc_head"            ## default or -i
        ## v0.6 not using \MaxLengthEmptyList thinking of -i ...
        opt_log formatting with \`longnamefilelist\',\
                  max. name length $2
        shift 2;;
      -n)
        if [ $2 = . ]; then 
            set z abcdefgh; 
        fi  ## v0.5
        adhoc_head="\\RequirePackage{nicefilelist}
                    \\MFfieldtemplate{f-base}{$2}
                    $adhoc_head"
        ## v0.6 not using \MaxBaseEmptyList thinking of -i ...
        opt_log formatting with \`nicefilelist\',\
                  template \`$2\'
        shift 2;;
      -o)
        adhoc_target="[$2]"
        adhoc_head="${adhoc_head} 
                    \\def\\ListGenerator{^^J by adhocfilelist.sh^^J}"
        opt_log writing list to $2      ## still was $OPTARG before v0.7
        shift 2;;
      -x)                               ## v0.7
        adhoc_xe='xe'
        opt_log using xelatex
        shift;;
      *) optname=''                     ## terminate loop
    esac
  done
## options parsed, now proceed accordingly:
if [ "$optname" != "h" ]; ## on -h, nothing more to do
  then
    ## processing remaining arguments:
    if [ $adhoc_find ]; 
    then    ## as `find' input
        if [ $adhoc_find_file ]; then
            ## bashref command substitution:
            #adhoc_file_find=$(< $adhoc_find_file)   ## bad
            #adhoc_file_find=$(cat $adhoc_find_file) ## good
            adhoc_file_find=`cat $adhoc_find_file`  ## good
            ## <- Jürgen Plate 8.3.1:
            ##    http://www.netzmafia.de/skripten/unix/unix8.html
            #echo In file: $adhoc_file_find
        else
            adhoc_file_find="$adhoc_find_std"
        fi
        #echo adhoc_file_find = $adhoc_file_find; ## rm. 2012/10/15
        if [ $adhoc_day ]; then
            #adhoc_file_find="$adhoc_file_find    ## bad before $@
            #                 -daystart
            #                 -mtime $adhoc_day"
            adhoc_day="-daystart -mtime $adhoc_day"
        fi
        echo \find criteria: $adhoc_file_find $@ $adhoc_day; ## in 2012/10/15
        adhoc_files="$(find $adhoc_file_find $@ $adhoc_day\
                       -printf "$adhoc_fmt" )"
    else    ## as list of file names
        #adhoc_files="$@" ## works too
        adhoc_files="$*"
    fi
    echo Files: $adhoc_files
    ## run latex with generated list of files, unless empty:
    #if [ $adhoc_files ]; then ## bad with $@/$*
    if [ ! -z "$adhoc_files" ]; then
        if [ "$adhoc_fmt" = "$adhoc_fmt_std" ]; then 
            adhoc_list="\\ReadListFileInfos$adhoc_target{$adhoc_files}"
        else
	    adhoc_list="\\RequirePackage{filedate}\\EmptyFileList\\ModDates
	                $adhoc_files \\ListInfos$adhoc_target\\\\relax"
        fi
        #echo TeX Code:
        #echo $adhoc_head $adhoc_find \\NoBottomLines $adhoc_list
        #echo $adhoc_head $adhoc_find \\NoBottomLines $adhoc_list > adhoclsi.tex
        #latex adhoclsi.tex
        ${adhoc_xe}latex $adhoc_head $adhoc_find \\NoBottomLines $adhoc_list
    else
        echo No files. Type adhocfilelist.sh -h \for usage.
    fi                ## <- v0.5
fi
