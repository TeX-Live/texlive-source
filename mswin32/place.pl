#! /usr/bin/perl

# Sebastian Rahtz 2000/02/05
# Process a tree in the texlive "cooked" directory and put
# in the main texmf tree, with an RDF catalogue entry

use lib 'c:/source/TeXLive/Master/texmf/tpm';

use English;
use File::Basename;
use File::Find;
use Time::gmtime;
use Cwd;
use FileUtils qw(walk_dir rec_rmdir);

require "newgetopt.pl";

$IsWin32 = $^O =~ /win32/i;

$newpackage=0;
$Inidir = getcwd();
$Master="d:/local/texlive";
$package=$ARGV[0];
$collection=$ARGV[1];
$Runsize=0;
$Docsize=0;
$Sourcesize=0;
@Docfiles=();
@Sourcefiles=();
@Runfiles=();
@zapped=();

# zap_empty_dirs
$zap_noaction = 1;
$zap_regexp = "^(\\.zipped|\\.cache|\\.cache\\+|00Contents|00Description)\$";

&main;

exit 0;

# checks whether only files matching regexp are contained in
#   dir-name and removes the directory if this is the case.
# -debug prints out what it does
# -noaction suppresses the deletion

sub zap_empty_dirs {

  my($dir_name, @all_files) = @_;
  my (@files);

  @files = grep(! /$zap_regexp/,@all_files);
  
  if ($#files < 0) {
    print "Empty directory: $dir_name\n";
    &rec_rmdir($dir_name) if (! $zap_noaction);
  }
}

sub main {


  unless (&NGetOpt ("package=s", "master=s",
		    "verbose", "help")) {
    print STDERR "Try `$0 --help'";
    exit 1;
  }

  if ($opt_help) {
    &help;
    exit 0;
  }

  $package = $opt_package if ($opt_package ne "");
  $Master = $opt_master if ($opt_master ne "");

  if (!-d "$package") { 
    die "No package to install: $package\n";  
  }
  
  # delete existing files
  chdir("$Master") || die ("directory $Master does not exist");

  if (-f "$Master/texmf/tpm/$package.tpm") {
    open(LIST,"$Master/texmf/tpm/$package.tpm");
    while (<LIST>) { 
      chop; 
      if (/^[A-z]/) { $Old{$_}=1;		  }
    }
    close(LIST);
  }
  # remove empty directories from new package
  # finddepth(\&dirs,"$Inidir/$package");
  
  # build catalogue file
  $_=$collection;
  ($coll,$level) = /([A-z]*)([0-9]*)/;
  chdir("$Master");
  if (-f "$Master/texmf/lists/$collection/$package") {
    system "p4 edit $Master/texmf/lists/$collection/$package";
    system "p4 edit $Master/texmf/tpm/$package.tpm";
  } else {
    $newpackage=1;
  }
  open(TPM,">$Master/texmf/tpm/$package.tpm");
  
#  $Date=`date +%Y/%m/%d`;
  $gm = gmtime();
  $Date = sprintf("%02d/%02d/%02d", $gm->year() + 1900, $gm->mon(), $gm->wday());
  if ($IsWin32) {
    $Who = $ENV{"USERNAME"};
    if ($Who eq "") {
      $Who = "TeXLive Team";
    }
  }
  else {
    $Who=`whoami`;
    chop($Who);
  }

  &walk_dir("$Inidir/$package", \&zap_empty_dirs);

#  chdir("$Inidir/$package");
#  system("find  . -depth -type d -exec /texlive/Build/cdbuild/zap-empty-dirs -debug  {} ;");
  find(\&files,".");
  print TPM <<EOF;
<?xml version="1.0"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" 
 xmlns:TPM="http://texlive.dante.de/">
  <rdf:Description  about="http://texlive.dante.de/$1">
    <TPM:Name>$package</TPM:Name>
    <TPM:Collection>$coll</TPM:Collection>
    <TPM:Level>$level</TPM:Level>
    <TPM:Date>$Date</TPM:Date>
    <TPM:Creator>$Who</TPM:Creator>
    <TPM:DocFiles size="$Docsize">
EOF
  foreach $file (@Docfiles) {print TPM "$file\n";}
  print TPM "    </TPM:DocFiles>\n";
  print TPM "    <TPM:SourceFiles size=\"$Sourcesize\">\n";
  foreach $file (@Sourcefiles) {print TPM "$file\n";}
  print TPM "    </TPM:SourceFiles> \n";
  print TPM "    <TPM:RunFiles size=\"$Runsize\">\n";
  foreach $file (@Runfiles) {print TPM "$file\n";}
  print TPM "texmf/tpm/$package.tpm\n";
  print TPM "    </TPM:RunFiles>\n";
  if (-f "$Master/texmf/doc/html/catalogue/entries/$package.xml") {
    system("xt $Master/texmf/doc/html/catalogue/entries/$package.xml /texlive/Build/cdbuild/cat2rdf.xsl $$.xml") ;
    open(XX,"$$.xml");
    while (<XX>) { print TPM; }
    close XX;
    unlink "$$.xml";
  }
  else
    {
      print TPM "<TPM:Title>\n\n</TPM:Title>\n<TPM:Description>\n\n</TPM:Description>\n";
    }
  print TPM <<EOF;
  </rdf:Description>
</rdf:RDF>
EOF
  close(TPM);
  system "xt $Master/texmf/tpm/$package.tpm /texlive/Build/cdbuild/tpm2list.xsl";
  $New{"texmf/tpm/$package.tpm"}=1;
  $Old{"texmf/tpm/$package.tpm"}=1;
  foreach $file (keys %Old) {
    if ($New{$file}) { 
      system "p4 edit  $Master/$file"; 
    } else { 
      system "p4 delete  $Master/$file"; 
    }
  }
  # copy files
  $Job="tar cf - . | (cd $Master ; tar xf - )";
  system($Job);
  
  foreach $file (keys %New) {
    if ($Old{$file}) { }  else { system "p4 add  $Master/$file"; }
  }
  if ($newpackage) 
    {system "p4 add $Master/texmf/lists/$collection/$package";
     system "p4 add $Master/texmf/tpm/$package.tpm";
   }
  chdir("$Inidir");
  system("mv $package $package.done");
}  

#--------------------------------------
sub dirs {
  local @filenames;
  if (-d) { 
    @filenames=&buildfilelist($File::Find::name,$_);
    if (!@filenames) { 
      rmdir $File::Find::name;
    }
  }
}

# sub cleanup {
#   local ($msg) = @_;
#   print STDERR $msg;
#   exit 1;
# }

sub buildfilelist {
  local($me,$name) = @_;
  my (@files);
  opendir(DIR,$me) || &cleanup ("cannot open directory $me");
  @files =grep(!/^\.\.?/,readdir(DIR));
  closedir(DIR);  
  return @files;
}

sub files {
  if (-f) {
    $This=$File::Find::name;
    $This =~ s/^\.\///;
    $New{$This}=1;
    if ($This =~ /^texmf\/doc/) {
      #   $_=$This;
      #  if (/\.ps$/) { system("gzip $_"); $This="$_.gz" ; }
      ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
       $atime,$mtime,$ctime,$blksize,$blocks)
	= stat($This);
      $Docsize+=$size;    push @Docfiles,$This; 
    }
    elsif ($This =~ /^Books/)           {  
      ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
       $atime,$mtime,$ctime,$blksize,$blocks)
	= stat($This);
      $Docsize+=$size;    push @Docfiles,$This; }
    elsif ($This =~ /^man/)           {  
      ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
       $atime,$mtime,$ctime,$blksize,$blocks)
	= stat($This);
      $Docsize+=$size;    push @Docfiles,$This; }
    elsif ($This =~ /^texmf\/source/) { 
      $_=$This;
      #if (/\.ps$/) { system("gzip $_"); $This="$_.gz" ; }
      $Sourcesize+=$size; push @Sourcefiles,$This; }
    else                         {  
      ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
       $atime,$mtime,$ctime,$blksize,$blocks)
	= stat($This);
      $Runsize+=$size;    
      push @Runfiles,$This; 
    }
  }
}
