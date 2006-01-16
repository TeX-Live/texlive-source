#
# Install a minimal perl based on a list of files generated
# by the PerlApp compiler
# Author: F. Popineau
# Date  : 04/11/98
# Time-stamp: <03/08/20 15:24:57 popineau>
#
#

BEGIN {
push @INC, 'c:/source/TeXLive/Master/Tools';
}

require "newgetopt.pl";
use FileUtils;
use Tpm;
use Cwd;

my $source_perl_dir = "c:/Local/Perl";
my $source_tcl_dir = "c:/Local/tcl";
# my $dest_perl_dir = ${Tpm::MasterDir} . "/xemtex/perl";
my $dest_bin_dir = "c:/Program Files/TeXLive/bin/win32";
my $dest_perl_dir = "c:/Program Files/TeXLive/xemtex/perl";
my $perlfilesname = "c:/source/fptex/mswin32/perlfiles.lst";
my $perlfilesnode;

my @perlfiles;

my %binaries = (
		"$source_perl_dir/bin" => [ "perl.exe", "perl58.dll" ],
		"$source_tcl_dir/bin" => [ "tk83.dll" ]
	       );

&main;

1;

sub restrict_perl {

  my ($dir, $dst, @files) = @_;
  my $node = $perlfilesnode;

  $dir =~ s@^\.@@;
  $dir =~ s@\\@/@g;
  $dir =~ s@^/@@;

  my @path = split "/", $dir;

  while (@path) {
    $node = $node->{shift @path};
  }

  @files = keys %{$node};
#  print "files = @files\n";
  return @files;
}

sub main {

  for my $dir (keys %binaries) {
    print "dir = $dir\n";
    for my $f (@{$binaries{$dir}}) {
      print "f = $f\n";
      if (&FileUtils::newer("$dir/$f", "${dest_bin_dir}/$f")) {
	&FileUtils::copy ("$dir/$f", "${dest_bin_dir}/$f");
	&FileUtils::set_file_time("$dir/$f", "${dest_bin_dir}/$f");

      }
    }
  }

  open IN, "<${perlfilesname}";
  while (<IN>) {
    chomp;
    if (/^  file:\s*c:\\Local(\\ActiveState)?\\Perl/) {
      $_ =~ s/^  file:\s+c:\\Local(\\ActiveState)?\\Perl\\//;
      $perlfiles{$_} = $_;
    }
  }
  close IN;
  @perlfiles = sort(keys %perlfiles);
  $perlfilesnode = &FileUtils::build_tree(@perlfiles);

  &FileUtils::sync_dir($source_perl_dir, $dest_perl_dir, \&restrict_perl, "", 0, 0, 0);

}
