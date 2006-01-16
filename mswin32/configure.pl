#
# Configure tool for Web2C & supp
# Author: F. Popineau
# Date  : 22/12/97
# Time-stamp: <04/02/15 14:47:24 popineau>
#
# Usage: 
#        configure --import       : will import Makefiles into the Win32 repository
#        configure --prefix <texpath> --tools_prefix <toolspath>
#                  --verbose
#        configure --help         : gives some help usage

use lib 'c:/source/TeXLive/Master/texmf-dist/tpm';

require "newgetopt.pl";
# use diagnostics;
use FileUtils qw(dirname basename normalize walk_dir sort_uniq
				  member push_uniq remove_list
				  build_tree print_tree walk_tree cleandir
				  is_dirsep is_absolute canon_dir build_path 
				  basename look_for );
use cauto qw(configure_cauto $kpsever $web2cver);
use Cwd;

# Repository for Makefiles and maybe others too
$win32_config_dir = "./win32/config/";

# Repository for .mak files
$win32_mk_dir = "./make/";
#Repository for .rc files
$win32_rc_dir = "./rc/";

# List of configuration variables, to be looked for and replaced.
@config_vars = ('kpsemaj', 'kpsemin', 'kpsebeta',
				'web2cmaj', 'web2cmin', 'web2cbeta',
				'buildmaj', 'buildmin', 'buildbeta', 'buildgamma');

# Path under which we will run
# BEWARE : path size is limited, even under NT
@path_list = (
	      # ensure that binaries from the current directory are used
	      '"./\$(objdir)"',
	      # kpathsea.dll from the build tree
	      '"\$(top_srcdir)/kpathsea/\$(objdir)"',
	      # current zlib? pnglib, tifflib and joglib for pdftex
	      '"\$(ZLIBDIR)/\$(objdir)"',
	      '"\$(PNGLIBDIR)/\$(objdir)"',
	      '"\$(TIFFLIBDIR)/\$(objdir)"',
	      '"\$(JPGLIBDIR)/\$(objdir)"',
	      # current tex engines
	      '"\$(top_srcdir)/web2c/\$(objdir)"',
	      # current makeinfo, install-info
	      '"\$(root_srcdir)/supp/texinfo/makeinfo/\$(objdir)"',
	      '"\$(root_srcdir)/supp/texinfo/util/\$(objdir)"',
	      # we still need to run tex & co at some point during the installation
#	      '$new_paths{prefix} . "/bin/win32"',
	      '"\$(prefix)/bin/win32"',
	      # GNU tools
#	      '$new_paths{toolsprefix} . "/bin"',
	      '"\$(toolsprefix)/bin"',
	      # compiler, because the PATH in win32.mak will override the
	      # one in the environment
	      '$ENV{"MSDEVDIR"} . "/../../VC98/Bin"',
	      '$ENV{"MSDEVDIR"} . "/Bin"',
	      'dirname($new_paths{prefix}) . "/perl/bin"',
	      # Windows by itself !
	      '$ENV{"SYSTEMROOT"}. "/System32"',
	      '$ENV{"SYSTEMROOT"}',
	      #			  '$ENV{"NTRESKIT"}',	      
	     );

$vernumreg = "-(v)?[0-9]+(\.[0-9]+)*([a-z0-9]*)?";

# List of directories to exclude from installation
@config_exclude_paths = ( '"./gnu"', 
						   # '"./InstallShield/etex-win32"',
						   # '"./pdftex-win32"',
						   # '"./xdvi-win32"',
						   '"./Native"',
						   '"./dialog"',
						   '"./InstallShield"',
						   '"./doc/tug99"',
						   '"./doc/gut99"',
						   '"./doc/mpost"',
						   '"./supp/Setup"',
						   '"./libs/libttfVERNUM/arch/amigaos"',
						   '"./libs/libttfVERNUM/arch/msdos"',
						   '"./libs/libttfVERNUM/arch/os2"',
						   '"./libs/libttfVERNUM/arch/unix"',
						   '"./libs/libwwwVERNUM/cvs2sql"',
						   '"./libs/libwwwVERNUM/examples"',
						   '"./libs/libwwwVERNUM/External"',
						   '"./libs/libwwwVERNUM/User"',
						   '"./libs/libwwwVERNUM/vms"',
						   '"./libs/libwwwVERNUM/windows"',
						   '"./libs/w3c-libwwwVERNUM"',
						   '"./supp/gzipVERNUM/amiga"',
						   '"./supp/gzipVERNUM/atari"',
						   '"./supp/gzipVERNUM/msdos"',
						   '"./supp/gzipVERNUM/nt"',
						   '"./supp/gzipVERNUM/os2"',
						   '"./supp/gzipVERNUM/vms"',
						   '"./supp/ttf2pk/data"',
						   '"./supp/ttf2pk/scripts"',
						   '"./supp/ttfdump/docs"',
						   '"./supp/ttfdump/include"',
						   '"./supp/french"',
						   '"./supp/installVERNUM"',
						   '"./supp/texconfig"',
						   '"./supp/xindyk"',
						   '"./supp/texinfo/info"', 
						   '"./supp/texinfo/emacs"',
						   '"./supp/texinfo/po"',
						   '"./supp/texinfo/intl"',
						   '"./supp/tiffVERNUM"',
						   '"./supp/tiffVERNUM/contrib"',
						   '"./supp/tiffVERNUM/html"',
						   '"./supp/tiffVERNUM/ports"',
						   '"./supp/tiffVERNUM/man"',
						   '"./supp/tiffVERNUM/contrib/acorn"',
						   '"./supp/xdvikVERNUM"',
						   '"./supp/xdvik.new"',
						   '"./libs/gifreaderVERNUM"',
						   '"./libs/jpegVERNUM"',
						   '"./libs/zlibVERNUM/amiga"',			  
						   '"./libs/zlibVERNUM/contrib"',		  
						   '"./libs/zlibVERNUM/msdos"',			  
						   '"./libs/zlibVERNUM/nt"',			  
						   '"./libs/zlibVERNUM/os2"',			  
						   '"./libs/xpdfVERNUM/ltk"',			  
						   '"./todo"', 
						   '"./upgrades"', 
						   '"./share"',
						   '"./win32"',
						   '"./win32/config"',
						   '"./win32/winmake"',
						   '"./win32/def"',
						   '"./win32/rc"',
						   '"./win32/sed"',
						   '"./win32/perl"',
						   'basename($web2cdir) . "/djgpp"',
						   'basename($web2cdir) . "/contrib/glob"',
						   'basename($web2cdir) . "/contrib/regex"',
						   'basename($web2cdir) . "/dviljk/fonts"',
						   'basename($web2cdir) . "/dvipdfm/latex-support"',
						   'basename($web2cdir) . "/dvipsk/vms"',
						   'basename($web2cdir) . "/dvipsk/vmcms"',
						   'basename($web2cdir) . "/libwww"',
						   'basename($web2cdir) . "/ps2pk16beta1a"',
						   'basename($web2cdir) . "/oxdvik"',
						   'basename($web2cdir) . "/share"',
						   'basename($web2cdir) . "/tetex"',
						   'basename($web2cdir) . "/texlive"',
						   'basename($web2cdir) . "/web2c/tiedir"',
						   'basename($web2cdir) . "/xdvik"',
						   '"dynamic"', '"dynamic-debug"', 
						   '"static"', '"static-debug"');

# A-List of {variable, pathname} to be replaced.
%config_paths = ('root_srcdir' => 'version_make',
				  'prefix' => 'paths_make',
				  'toolsprefix' => 'version_make',
				  'texmf' => 'paths_make',
				  'BOUNDSCHECKER' => 'version_make',
				  'PATH' => 'version_make',
				  'SystemDrive' => 'version_make',
				  # 	  'MSDevDir' => 'version_make',
				  #		  'TEXMFMAIN' => 'version_make',
				  #		  'TEXMF' => 'version_make'
				 );

# List of tools to be checked. Each tool has a make variable with
# the same name capitalized (SED = c:\bin\sed.exe).
# If a name has an association, use it (builtin commands).
%tools = ('tr' => '', 
		  'sed' => '', 
		  'grep' => '', 
		  'diff -u' => '',
		  'makedepend -Y' => '', 
		  'perl' => '', 
		  'echo' => 'echo',
		  'cp' => 'copy',
		  'touch' => '',
		  'rm' => 'del /f/q',
		  'mv' => 'copy',
		 );

&main;

exit 0;


sub dir_match
  {
    local ($d1, $d2) = @_;
    local ($e1, $e2, $ret);
    $e1 = quotemeta(&canon_dir($d1));
    $e1 =~ s/VERNUM/$vernumreg/g;
	#    $e1 = &canon_dir($d1);
    $e2 = &canon_dir($d2);
    $ret = ($e2 =~ m/$e1/);
	#    if ($ret) { print "dir_match : $d1 & $d2\n-> $e1 & $e2 = $ret\n"; }
    return $ret;
  }

# Rename source into destination, backuping the source.
sub rename
  {
    local ($src, $dest) = @_;
    print STDERR "Renaming $src -> $dest\n" if $opt_verbose;
    # under Win32, better to ensure that $dest does not exist anymore.
    if (-f $dest) {
	  unlink ("$dest.backup");
	  rename ($dest, "$dest.backup");
    }
    rename ($src, $dest);
  }

# Move source to dest, dest may be a directory
sub move
  {
    local ($src, $dest) = @_;
    print STDERR "Moving $src -> $dest\n" if $opt_verbose;
    die "Copy : source file $src does not exist.\n" 
	  unless (-f $src);
    die "Copy : destination file $dest does not exist.\n" 
	  unless (-f $src || -d $src);
    if (-d $dest) {
	  $dest = &build_path($dest, &basename($src));
    }
    if (-f $dest) {
	  &rename($dest, "$dest.backup");
    }
    &copy($src, $dest);
    unlink($src);
  }

# Copy source to dest
sub copy
  {
    local ($src, $dest) = @_;
	
    print "Copying $src to $dest\n" if ($opt_verbose);
	
    die "Copy : source file $src does not exist.\n" 
	  unless (-f $src);
	#    die "Copy : destination file $dest does not exist.\n" 
	#	unless (-f $dest || -d $dest);
    if (-d $dest) {
	  $dest = &build_path($dest, &basename($src));
    }
    open IN, "<$src";
    open OUT, ">$dest";
    binmode(IN);
    binmode(OUT);
    print OUT <IN>;
    close(IN);
    close(OUT);
  }

# Copy a text file filtering lines as stated by the filter function
sub copy_filter
  {
    # these variables must have a lexical scope, so 
    # will be hopefully available in the filter function
    local ($src, $dest, $filter) = @_;
    local ($line, $num);
	
    print "Copying $src to $dest with filtering\n" if ($opt_verbose);
	
    die "Copy : source file $src does not exist.\n" 
	  unless (-f $src);
    if (-d $dest) {
	  $dest = &build_path($dest, &basename($src));
    }
    open IN, "<$src";
    open OUT, ">$dest";
    $num = 0;
    while (<IN>) {
	  $line = &{$filter}($_);
	  print OUT $line;
	  $num = $num + 1;
    }
    close(IN);
    close(OUT);
	
  }

# Test if f1 is newer than f2
sub newer
  {
    local ($f1, $f2) = @_;
    local ($d1, $d2, $res);
    @d1 = stat($f1); $d1 = $d1[9];
    @d2 = stat($f2); $d2 = $d2[9];
    $res = 0;
    if ($#d1 == 0) {
	  $res = 0;
    }
    if ($#d2 == 0 ) {
	  $res = 1;
    }
    $res = $d1 > $d2;
	
    if ($res) {
	  print "Warning: $f1 is newer than $f2.\n";
    }
    return($res);
  }

# What this script does...
sub help
  {
    print "Usage: configure [options]\n";
    print "\n";
    print "    --prefix <path>               sets the root tree at <path>;\n";
    print "                                  assumes TeX will be in \n";
    print "                                  <path>\TeX\{bin,include,lib,share}\n";
    print "    --tools_prefix <tools-path>   sets the tools tree at <tools-path>\n";
    print "                                  assumes the other tools will be in \n";
    print "                                  <tools-path>\{bin,include,lib,share}\n";
    print "    It is expected that flex, bison, sed, gawk, makedepend and groff will\n";
    print "    be found in <tools-path>. If you do not have them, check the following\n";
	print "    adress :\n";
	print "    <CTAN>/systems/win32/fptex/gnutools-1.1-win32.zip\n";
	print "    The whole package can only be build reliably under NT (because of\n";
	print "    batch commands) and using Visual Studio .Net\n";
	print "    This is the currrent release and I won't make any effort supporting\n";
	print "    previous versions.\n";
	print "    Be careful that some global optimizations of `cl' are removed by hand\n";
	print "    in `convert.bat' and are specific to each version of `cl'\n";
	print "    Last but not least, it is expected that a some texmf tree be found\n";
	print "    in <path>/texmf.\n";
    print "    \n";
    print "    Using this normal mode will copy/update/check the configuration files\n";
    print "    to make the distribution ready for compilation.\n";
    print "    Kpathsea and Web2C  version numbers  are  guessed  from their\n";
    print "    configure.in files, looked for in the highest web2c-xx.yyzz directory\n";
    print "    found. There should be no problem if the distribution\n";
    print "    is consistent.\n";
    print "    After that, type :\n";
	print "    perl ./build.pl --install --log=install.log\n";
	print "    and wait...\n";
    print "\n";
    print "    Support commands:\n";
    print "    --help                          displays this help text and exits\n";
	#     print "    --makedist                      will assemble a new release\n";
  }

# Read current config variables from the file
sub read_current
  {
    local($file) = @_;
    open VER, "<$file";
    while (<VER>) {
	  $line = $_;
	  foreach $v (@config_vars) {
	    if ($line =~ m/^$v\s*=\s*(.*)$/) {
		  $current_vars{$v} = $1;
	    }
	  }
    }
    close VER;
  }

# Read current paths from their files (path.mak, w2cwin32.mak)
sub read_paths
  {
    local ($k,$p);
    foreach $k (keys %config_paths) {
	  @p = ($k, eval "\$$config_paths{$k}");
	  open IN, "<$p[1]";
	  while (<IN>) {
	    if (/^$p[0]\s*=\s*(.*)$/) {
		  $current_paths{$p[0]} = $1;
	    }
	  }
	  close IN;
    }
  }

# Display current configuration variables
sub print_current
  {
    foreach $v (@config_vars) {
	  print "$v = $current_vars{$v}\n";
    }
    foreach $k (keys %config_paths) {
	  @p = ($k, eval "\$$config_paths{$k}");
	  print "$k = $current_paths{$k}\n";
    }
  }

# Write current configuration variables (into w2cwin32.mak, argument)
sub write_current
  {
    local($file) = @_;
    open VERIN, "<$file";
    open VEROUT, ">$file.tmp";
	
    while (<VERIN>) {
	  $line = $_;
	  foreach $v (@config_vars) {
	    if ($line =~ s,^$v\s*=\s*(\S*)$,$v = $current_vars{$v},) {
		  print "$v was $1, is $current_vars{$v}\n" if ($opt_verbose);
	    }
	  }
	  print VEROUT $line;
	  if ($line !~ m/\n$/) { print VEROUT "\n"; }
    }
	
    close VERIN;
    close VEROUT;
    &rename("$file.tmp", "$file");
  }

# Write current paths into their files.
# Beware to the logic: each file must be open only once.
sub write_paths
  {
    local ($k,$p);
    
    @lv = &sort_uniq(values %config_paths);
    # foreach file (values of the hash config_paths)
    foreach $v (@lv) {
	  $filename = eval("\$$v");
	  open IN, "<$filename";
	  open OUT, ">$filename.tmp";
	  @p = ();
	  # which keys $k have value $v ?
	  foreach $k (keys %config_paths) {
	    if ($config_paths{$k} eq $v) {
		  push @p, ($k);
	    }
	  }
	  # foreach line in the file
	  while (<IN>) {
	    $line = $_;
	    foreach $k (@p) {
		  if ($line =~ s,^$k\s*=\s*(.*)$,$k = $current_paths{$k},) {
		    print "$k was $1, is $current_paths{$k}\n" if ($opt_verbose);
		  }
	    }
	    print OUT $line;
	  }
	  close IN;
	  close OUT;
	  &rename ("$filename.tmp", "$filename");
    }
  }

# Rewrite the resources common include file, using a template
# (w2cversion.h built using w2cversion.in)
sub write_res
  {
    local($fout) = @_;
    local($fin, $line);
    $fin = $fout;
    $fin =~ s/^(.*)\.h$/$1.in/;
	#    print "Res: $fin -> $fout\n";
    open IN, "<$fin";
    open OUT, ">$fout";
    while (<IN>) {
	  $line = $_;
	  foreach $v (@config_vars) {
	    $line =~ s/\@$v\@/$current_vars{$v}/g;
	  }
	  print OUT $line;
    }
    close IN;
    close OUT;
  }

# Put win32 configuration files where expected
sub export_config
  {
    local ($dest) = @_;
    $d = cwd();
    chdir $dest;
    # FIXME: add a pointer to the block to execute
    &walk_dir (".", \&export_files, \&prune_make);
    chdir $d;
  }

# Test if $dir is matched by one of the list' element.
# Also, remove any directory with some version number (-xx.yy)
sub prune_make
  {
    local ($d) = @_;
    local ($e, $ret);
    foreach $e (@config_exclude_paths) {
	  if (&dir_match(eval($e), $d)) {
	    print "Pruning $d because of $e\n" if ($opt_verbose);
	    return 1;
	  }
    }
    return 0;
  }

sub export_files
  {
    local ($dir, @l) = @_;
    local ($src, $f, $make_done, $cauto_done);
	
	#    $make_done = 1;
    $cauto_done = 0;
    
	foreach $f (@l) {	  
	  # There are also c-auto.h to export
	  if (! $cauto_done 
		  && $dir !~ m@^\.(\\|/)win32$@ 
		  && $f =~ /^(c-auto|config)(\.h)?\.in$/) {
		$cauto_done = 1;
		&configure_cauto("$dir/$f", "$dir/$1.h");
	  }
	}
  }

##
## Packages handling
##

# Zip one package
sub makepkg
  {
	
  }

# builds the distribution
sub makedist
  {
    local($pkg) = @_;
    if ($pkg eq "") {
	  # make all packages
	  foreach $pkg (<pkg/*.pkg>) {
	    &makepkg($pkg);
	  }
    }
    else {
	  &makepkg($pkg);
    }
  }

#
# Guess web2cver and kpsever by looking at configure.in 
# in web2c-xx.yyzz/kpathsea and web2c-xx.yyzz/web2c
#

sub guess_web2c_version
  {
    local($ret);
    $ret = 0;
    $kpsever = &look_for('KPSEVERSION', "$rootdir/texk/kpathsea/configure.in");
    if ($kpsever =~ /^([0-9]+)\.([0-9])+(\.?)(.*)$/) {
	  $new_vars{'kpsemaj'} = $1;
	  $new_vars{'kpsemin'} = $2;
	  $new_vars{'kpsebeta'} = $4;
	  print "$0: Using kpathsea version $1.$2$3$4\n";
	  $ret = 1;
    }
    else {
	  print "$0: can't guess what Kpathsea version you are using.\
Please set the version numbers for Kpathsea in $win32_mk_dir/w2cwin32.mak\n";
    }
    $web2cver = &look_for('WEB2CVERSION', "$rootdir/texk/web2c/configure.in");    
    if ($web2cver =~ /^([0-9]+)\.([0-9])+(\.?)(.*)$/) {
	  $new_vars{'web2cmaj'} = $1;
	  $new_vars{'web2cmin'} = $2;
	  $new_vars{'web2cbeta'} = $4;
	  print "$0: Using Web2C version $1.$2$3$4.\n";
	  $ret = 1;
    }
    else {
	  print "$0: can't guess what Web2C version you are using.\
Please set the version numbers for Web2C in $win32_mk_dir/w2cwin32.mak\n";
    }
    return $ret;
  }

##
## Main program
##	    
sub main
  {
    unless (&NGetOpt ("help", 
					  #	"import", 
					  #	"kpsever=s", "web2cver=s",
					  #	"newbuild=s", 
					  "makedist:s", "prefix=s", 
					  "tools_prefix=s", "verbose"))
	  {
		print STDERR "Try `configure --help'";
		exit 1;
	  }
    # Give help, exit immediately.
    if ($opt_help)
	  {
		help;
		exit 0;
	  }
	
    # We need it early
    $rootdir = dirname(cwd());

    # Actual configuration thing.
    $version_make = "$win32_mk_dir/w2cwin32.mak";
    $paths_make = "$win32_mk_dir/paths.mak";
    $version_res = "$win32_rc_dir/w2cversion.h";
    
    # Update the version numbers using arguments.
	
    # read the current version numbers
    $current_vars = &read_current($version_make);
    # flag for checking arguments have been modified.
    $current_modified = 0;
		
    $current_modified = &guess_web2c_version;
	
    $new_vars{'buildmaj'} = $new_vars{'web2cmaj'};
    $new_vars{'buildmin'} = $new_vars{'web2cmin'};
    if ($new_vars{'web2cbeta'} =~ /^alpha([0-9]+)$/) {
	  $new_vars{'buildmin'} = $new_vars{'buildmin'} - 1;
	  $new_vars{'buildbeta'} = 9800 + $1;
    }
    elsif ($new_vars{'web2cbeta'} =~ /^beta([0-9]+)$/) {
	  $new_vars{'buildmin'} = $new_vars{'buildmin'} - 1;
	  $new_vars{'buildbeta'} = 9900 + $1;
    }
    else {
	  $new_vars{'buildbeta'} = $new_vars{'web2cbeta'};
	  if ($new_vars{'buildbeta'} eq '') {
	    $new_vars{'buildbeta'} = 0;
	  }
    }
    if ($new_vars{'buildmaj'} ne $current_vars{'buildmaj'}
		|| $new_vars{'buildmin'} ne $current_vars{'buildmin'}
		|| $new_vars{'buildbeta'} ne $current_vars{'buildbeta'}) {
	  $new_vars{'buildgamma'} = 0;
    }
	
    $new_vars{'buildgamma'} = $new_vars{'buildgamma'} + 1;
    $current_modified = 1;
	
    if ($current_modified != 0) {
	  # replace current version numbers by the new ones
	  foreach $v (@config_vars) {
		print "$v, $new_vars{$v}\n" if ($opt_verbose);
		$current_vars{$v} = $new_vars{$v};
	  }
    }

    $web2cdir = &build_path($rootdir, "texk");
    if (! -d $web2cdir) {
	  print "$0: no $web2cdir web2c directory !\n";
	  exit 1;
    }
	
    # Build the distribution.
    if ($opt_makedist) {
	  &makedist($opt_makedist);
	  exit 0;
    }
	
    # write the new version numbers
    &write_current($version_make);
    # and into the res file too. This one is taken from a
    # template file (.in)
    &write_res($version_res);
    # While we are at it, change version numbers in c-auto.h too.
    # &write_res($c_auto_h);
    
    # Paths
    $current_modified = 0;
    # TeX destiantion directory prefix
    if ($opt_prefix) {
	  die "Prefix $opt_prefix must be absolute.\n" 
	    unless &is_absolute($opt_prefix);
	  $new_paths{'prefix'} = &canon_dir($opt_prefix);
	  $current_modified = 1;
    }
    # prefix of the destination dir for other tools
    if ($opt_tools_prefix) {
	  die "Tools prefix $opt_tools_prefix must be absolute.\n" 
	    unless &is_absolute($opt_tools_prefix);
	  $new_paths{'toolsprefix'} = &canon_dir($opt_tools_prefix);
	  $current_modified = 1;
    }
    $new_paths{'root_srcdir'} = &canon_dir($rootdir);
    # read the current value for configuration paths
    &read_paths;
    # check the root source directory
    if ($current_paths{'root_srcdir'} cmp $rootdir) {
	  $current_modified = 1;
    }
    # TEXMF and TEXMCNF for documentation and formats
    $prefix = $new_paths{'prefix'};
    if (! $prefix) {
	  $prefix = $current_paths{'prefix'};
    }
	# look for a plausible texmf
	$current_modified = 1;
	# This is TeX Live && fpTeX
	if (-d "$prefix/texmf") {
	  $new_paths{'TEXMF'} = &canon_dir("$prefix/texmf", '/');
	  $new_paths{'texmf'} = &canon_dir('$(datadir)/texmf', '\\');
	}
	# In case of ...
	elsif (-d "$prefix/share/texmf") {
	  $new_paths{'TEXMF'} = &canon_dir("$prefix/share/texmf", '/');
	  $new_paths{'texmf'} = &canon_dir('$(datadir)/share/texmf', '\\');
	}
	else {
	  die "Can't find a plausible TEXMF.\n";
	}
	
    $new_paths{'SystemDrive'} = $ENV{"SYSTEMDRIVE"};
    # BoundsChecker, default location.
    $new_paths{'BOUNDSCHECKER'} = $ENV{"SYSTEMDRIVE"} . "\\Program Files\\Numega\\BoundsChecker";
    # PATH: We need to change it because there is a limitation on size,
    # and simply adding everything would overflow
    $new_paths{'PATH'} = join ( ";", map {&canon_dir(eval($_))} @path_list );
    print "PATH = $new_paths{'PATH'}\n";
    $current_modified = 1;
    if (-d "$prefix/share/texmf") {
	  $new_paths{'TEXMFMAIN'} = "$prefix/share/texmf";
	  $current_modified = 1;
    }
    elsif (-d "$prefix/texmf") {
	  $new_paths{'TEXMFMAIN'} = "$prefix/texmf";
	  $current_modified = 1;
    }
    else {
	  $new_paths{'TEXMFMAIN'} = $current_paths{'TEXMFMAIN'};
	  $current_modified = 1;
    }
    if ($current_modified) {
	  # replace current paths by the new ones
	  foreach $p (keys %config_paths) {
	    if ($new_paths{$p}) {
		  print "$p, $new_paths{$p}\n" if ($opt_verbose);
		  $current_paths{$p} = $new_paths{$p};
	    }
	  }
	  # rewrite the paths
	  &write_paths;
    }

    # Generate c-auto.h/config.h/whatever in each directory
    &export_config($rootdir);
	
    # install any newer version of texinfo.tex
    $new_texinfo = "$web2cdir/etc/texinfo.tex";
    $installed_texinfo = $new_paths{'TEXMFMAIN'} . "/tex/texinfo/texinfo.tex";
    if (!( -f $installed_texinfo) || &newer($new_texinfo, $installed_texinfo)) {
      &copy($new_texinfo, $installed_texinfo);
      print "Copying $new_texinfo to $installed_texinfo.\n";
    }

    # We could also verify that all the tools needed may be found
    # &check_tools($version_make)

    print "The distribution is ready for compiling.\n";
    print "You still have compilation options :\n";
    print "The prefered way is 'nmake NODEBUG=1 install'\n";
    print "but you may have a look at $version_make for other options.\n";

}
