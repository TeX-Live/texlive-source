## newbuild.pl: prepare a TeXLive build			-*- Perl -*-
##
## Copyright (C) 2002 Fabrice Popineau <Fabrice.Popineau@supelec.fr>
##                    derived from the MiKTeX newbuild.pl perl script
##
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
##
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

use strict;
use Cwd;
use File::Path;
use File::Basename;
use Getopt::Long;
use File::DosGlob 'glob';

my $texlivesrcdir;
my $major_version_number;
my $minor_version_number;
my $version_string;
my $patch_number_str;
my $mscver;
my $opt_prefix;
my $opt_help;
my $opt_alpha;
my $opt_beta;
my $opt_offical;
my $opt_debug;
my $opt_tmdir;
my $opt_snapshot;
my $opt_official;
my $release_motto;
my $build_label;
my $build_special_str;
my $build_number;
my $state;
my $beta_number;
my $alpha_number;
my $build_date_str;
my $builder_name_str;
my $computer_name_str;
my $build_date;
my $patch_number;

&main;
exit (0);

###############################################################################
## help
###############################################################################

sub help
{
  print <<EOF;
Usage: newbuild [OPTIONS]
  --alpha           initialize an alpha build
  --beta            initialize a beta build
  --debug           initialize a debug build
  --help            display this help text and exit
  --official        initialize an official build
  --prefix=DIR      install all files in DIR
  --tmdir=DIR       where Total Texlive is installed
  --snapshot        initialize a snapshot build
EOF
}

###############################################################################
## make_version_h
###############################################################################

sub make_version_h
{
  $version_string = $major_version_number;

  if ($minor_version_number > 0)
    {
      $version_string .= ".$minor_version_number";
    }

  $version_string .= $patch_number_str;

  if ($build_special_str)
    {
      $version_string .= " $build_special_str";
    }

  if ($state == 1)
    {
      $version_string .= " (snapshot build $build_number)";
    }
  elsif ($state == 2)
    {
      $version_string .= " beta $beta_number";
    }
  elsif ($state == 3)
    {
      $version_string .= " alpha $alpha_number";
    }
  elsif ($state == 4)
    {
      $version_string .= " (debug build $build_number)";
    }

  my $version_h_filename = $texlivesrcdir . "./texlive-version.h";

  open (VERSION_H, ">$version_h_filename");

  print VERSION_H <<EOF;
/* Made by newbuild.pl; DO NOT EDIT !!!   -*- C -*- */
#ifndef VERSION__9C077E32_49F9_4a99_9A1F_3E4C4DB17C7D__INCLUDED_H_
#define VERSION__9C077E32_49F9_4a99_9A1F_3E4C4DB17C7D__INCLUDED_H_
#define TEXLIVE_PRODUCTNAME_STR "TeXLive"
#define TEXLIVE_COMPANYNAME_STR "TeX Users Group"
#define TEXLIVE_BUILD_COMMENT_STR "Build $build_number ($build_label); build date: $build_date_str; machine: $computer_name_str"
#define TEXLIVE_BUILD_DATE $build_date
#define TEXLIVE_BUILDERNAME_STR "$builder_name_str"
#define TEXLIVE_PRIVATEBUILD_STR "Built by $builder_name_str"
#define TEXLIVE_MAJOR_VERSION $major_version_number
#define TEXLIVE_MINOR_VERSION $minor_version_number
#define TEXLIVE_BETA_NUMBER $beta_number
#define TEXLIVE_BETA_NUMBER_STR "$beta_number"
#define TEXLIVE_ALPHA_NUMBER $alpha_number
#define TEXLIVE_ALPHA_NUMBER_STR "$alpha_number"
#define TEXLIVE_PATCH_NUMBER $patch_number
#define TEXLIVE_PATCH_NUMBER_STR "$patch_number_str"
#define TEXLIVE_BUILD_LABEL "$build_label"
#define TEXLIVE_BUILD_NUMBER $build_number
#define TEXLIVE_BUILD_SPECIAL_STR "$build_special_str"
#define TEXLIVE_MAJOR_VERSION_STR "$major_version_number"
#define TEXLIVE_MINOR_VERSION_STR "$minor_version_number"
#define TEXLIVE_BETA_VERSION_STR "$beta_number"
#define TEXLIVE_ALPHA_VERSION_STR "$alpha_number"
#define TEXLIVE_BUILD_NUMBER_STR "$build_number"
#define TEXLIVE_RELEASE_STATE $state
#define TEXLIVE_RELEASE_MOTTO_STR "$release_motto"
#define TEXLIVE_PRODUCTVERSION_STR "$version_string"
#define TEXLIVE_LIBRARYPREFIX_STR "tl$major_version_number$minor_version_number"
#ifndef TEXLIVE_LEGALCOPYRIGHT_STR
#define TEXLIVE_LEGALCOPYRIGHT_STR "Copyright (C) 1996-2003 Fabrice Popineau and the TeX Users Group"
#endif
#ifndef TEXLIVE_LEGALTRADEMARKS_STR
#define TEXLIVE_LEGALTRADEMARKS_STR ""
#endif
#endif
EOF

  close (VERSION_H);
}

###############################################################################
## update_tpm_files
###############################################################################

sub update_tpm_files
{
#    my $x;
#    foreach $x (glob "tpm\\*.tpm")
#      {
#        open TPM, $x;
#        open TPM_NEW, ">$x.new";
#        while (<TPM>)
#  	{
#  	  s-<TPM:Version>.*</TPM:Version>-<TPM:Version>$version_string</TPM:Version>-;
#  	  s-<TPM:Date>.*</TPM:Date>-<TPM:Date>$build_date_str</TPM:Date>-;
#  	  print TPM_NEW $_;
#  	}
#        close TPM_NEW;
#        close TPM;
#        unlink $x;
#        rename "$x.new", $x;
#      }
}

###############################################################################
## mscver
###############################################################################

sub mscver
{
  open MSCVER, ">mscver.c";
  print MSCVER <<EOF;
#include <stdio.h>
void main (void) { printf ("%d", _MSC_VER); }
EOF
  close MSCVER;
  system "cl /nologo mscver.c";
  $mscver = `mscver`;
  unlink ("mscver.c");
  unlink ("mscver.obj");
  unlink ("mscver.exe");
}

###############################################################################
## write_paths_ini
###############################################################################

#  sub write_paths_ini
#  {
#    system "nmake -f texlive.mak configure"
#  }

###############################################################################
## main
###############################################################################

sub main
{
  unless (GetOptions("alpha" => \$opt_alpha,
		     "beta" => \$opt_beta,
		     "help" => \$opt_help,
		     "official" => \$opt_official,
		     "prefix=s" => \$opt_prefix,
		     "tmdir=s" => \$opt_tmdir,
		     "debug" => \$opt_debug,
		     "snapshot" => \$opt_snapshot))
    {
      print STDERR "Try `newbuild --help'";
      exit (1);
    }

  if ($opt_help)
    {
      help ();
      exit (0);
    }

  my($ss,$mm,$hh,$mday,$mon,$year) = localtime();

  $builder_name_str = $ENV{"USERNAME"};
  $computer_name_str = $ENV{"COMPUTERNAME"};

  my $new_state = -1;
  $new_state = 4 if ($opt_debug);
  $new_state = 3 if ($opt_alpha);
  $new_state = 2 if ($opt_beta);
  $new_state = 1 if ($opt_snapshot);
  $new_state = 0 if ($opt_official);

  $texlivesrcdir = &dirname(cwd()) . "/texk/msvc";
  
  $texlivesrcdir =~ s:/:\\:g;

  my $texlive_inc_filename = $texlivesrcdir . "/texlive.inc";

  die "$texlive_inc_filename does not exist" unless (-e $texlive_inc_filename);
  open (TEXLIVE_INC, $texlive_inc_filename);

  open (TMPFILE, ">xxx");
  $state = -1;
  my $last_state = -1;

  mscver ();

  my $timestamp = sprintf("%02d/%02d/%02d %02d:%02d:%02d %s",
			  $year, $mon+1, $mday, $hh, $mm, $ss,
			  $ENV{"USERNAME"});

  while (<TEXLIVE_INC>)
    {
      if (/^state = ([01234])/)
	{
	  $state = $1;
	  $state = $new_state if ($new_state >= 0);
	  print TMPFILE "state = $state\n";
	}
      elsif (/^alpha_number = ([0-9]+)/)
	{
	  $alpha_number = $1;
	  print TMPFILE "alpha_number = $alpha_number\n";
	}
      elsif (/^beta_number = ([0-9]+)/)
	{
	  $beta_number = $1;
	  print TMPFILE "beta_number = $beta_number\n";
	}
      elsif (/^release_motto = (.*)/)
	{
	  $release_motto = $1;
	  print TMPFILE $_;
	}
      elsif (/^build_label = ([A-Za-z0-9_-]+)/)
	{
	  $build_label = $1;
	  print TMPFILE $_;
	}
      elsif (/^build_special_str = ([A-Za-z0-9 ]+)/)
	{
	  $build_special_str = $1;
	  print TMPFILE $_;
	}
      elsif (/^(build_number = )([0-9]+)/)
	{
	  $build_number = $2 + 1;
	  print TMPFILE $1, $build_number, "\n";
	}
      elsif (/^build_date = /)
	{
	  $build_date = time ();
	  print TMPFILE $&, $build_date, "\n";
	}
      elsif (/^build_date_str = /)
	{
	  $build_date_str = localtime ();
	  print TMPFILE $&, $build_date_str . "\n";
	}
      elsif (/^texlivesrcdir = /)
	{
	  print TMPFILE $&, $texlivesrcdir . "\n";
	}
      elsif (/^prefix = /)
	{
	  if (length($opt_prefix) > 0)
	    {
	      print TMPFILE $&, $opt_prefix . "\n";
	    }
	  else
	    {
	      print TMPFILE $&, $ENV{"HOMEDRIVE"} . "\\texmf\n";
	    }
	}
      elsif (/^tmdir = /)
	{
	  if (length($opt_tmdir) > 0)
	    {
	      print TMPFILE $&, "$opt_tmdir\n";
	    }
	  else
	    {
	      print TMPFILE $&, "\$(prefix)\n";
	    }
	}
      elsif (/^mscver = /)
	{
	  print TMPFILE $&, $mscver . "\n";
	}
      else
	{
	  if (/^major_version_number = ([0-9]+)/)
	    {
	      $major_version_number = $1;
	    }
	  elsif (/^minor_version_number = ([0-9]+)/)
	    {
	      $minor_version_number = $1;
	    }
	  elsif (/^patch_number = ([0-9]+)/)
	    {
	      $patch_number = $1;
	    }
	  elsif (/^patch_number_str = ([a-z])/)
	    {
	      $patch_number_str = $1
	    }
	  else
	    {
	      s/Time-stamp:[ \t]+\"(.+)\"/Time-stamp: \"$timestamp\"/;
	    }
	  print TMPFILE;
	}
    }

  close (TEXLIVE_INC);
  close (TMPFILE);
  unlink ($texlive_inc_filename);
  rename ("xxx", $texlive_inc_filename);

  make_version_h ();

  update_tpm_files ();

#    if ($opt_tmdir && $opt_prefix)
#      {
#        write_paths_ini ();
#      }
}
