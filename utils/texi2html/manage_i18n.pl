#! /usr/bin/perl -w
#+##############################################################################
#
# manage_i18n.pl: manage translation files
#
#    Copyright (C) 2003  Free Software Foundation, Inc.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#-##############################################################################

# This requires perl version 5 or higher
require 5.0;

use strict;
#use vars qw(
#           $T2H_LANGUAGES
#);

use File::Copy;
use Data::Dumper;

select(STDERR);
$| = 1;
select(STDOUT);
$| = 1;

my $language;
my $i18n_dir = 'i18n'; # name of the directory containing the per language files
my $translation_file = 'translations.pl'; # file containing all the translations
my @known_languages = ('en', 'de', 'nl', 'es', 'no', 'pt', 'fr'); # The supported
                                               # languages
#my $template = 'template';
my $template = 'en';
my $template_file = "$i18n_dir/$template";
my @source_files = ('texi2html.pl', 'texi2html.init', 'T2h_i18n.pm', 
 'examples/roff.init', 'examples/noheaders.init');

use vars qw(
$LANGUAGES 
$T2H_OBSOLETE_STRINGS
);

# Strings not in code
my $template_strings =
{
 'January' => '', 
 'February' => '',
 'March' => '', 
 'April' => '',
 'May' => '',
 'June' => '',
 'July' => '',
 'August' => '',
 'September' => '',
 'October' => '',
 'November' => '',
 'December' => '', 
 'T2H_today' => '%s, %d %d',
};

# Handle per language files
$Data::Dumper::Sortkeys = 1;

if (@ARGV < 1)
{
    die "Need at least one arg\n";
}

my $command = shift @ARGV;

sub update_language_file($);

die "No suitable $i18n_dir directory\n" unless (-d $i18n_dir and -r $i18n_dir);

sub get_languages
{
    unless (opendir DIR, $i18n_dir)
    {
         die "Cannot open dir $i18n_dir: $!\n";
    }
    my @languages = grep {
         ! /^\./ &&
         ! /\.(bak|orig|old|dpkg-old|rpmnew|rpmsave)$/ &&
         ! /~$/ &&
         ! /^#.*#$/ &&
         -f $i18n_dir . '/' . $_
    } readdir DIR;
    closedir DIR;
    my @known = @known_languages;
    foreach my $lang (@languages)
    {
         if (grep {$_ eq $lang} @known)
         {
              @known = grep {$_ ne $lang} @known;
         }
         else
         {
             warn "Remark: you could update the known languages array for `$lang'\n";
         }
    }
    warn "Remark: the following known languages have no corresponding file: @known\n" if (@known);
    return @languages;
}

sub manage_i18n_files($)
{
    my $command = shift;
    if ($command eq 'update')
    {
        update_i18n_files();
    }
    elsif ($command eq 'merge')
    {
        merge_i18n_files();
    }
    elsif ($command eq 'template')
    {
        update_template(@source_files);
    }
    elsif ($command eq 'all')
    {
        update_template(@source_files);
        update_i18n_files();
        merge_i18n_files();
    }
    else
    {
        warn "Unknown i18n command: $command\n";
    }
    exit 0;
}

sub merge_i18n_files
{
    my @languages = get_languages();
    die "No languages found\n" unless (@languages);
    if (-f $translation_file)
    {
        unless (File::Copy::copy ($translation_file, "$translation_file.old"))
        {
            die "Error copying $translation_file to $translation_file.old\n";
        }
    }
    #foreach my $lang ($template, @known_languages)
    die "open $translation_file failed" unless (open (TRANSLATIONS, ">$translation_file"));
    foreach my $lang (@languages)
    {
         my $file = "$i18n_dir/$lang";
         next unless (-r $file);
         unless (open (FILE, $file))
         {
              warn "open $file failed: $!\n";
              return;
         }
         while (<FILE>)
         {
              print TRANSLATIONS;
         }
         close FILE;
    }
}
        
sub update_language_hash($$$)
{
    my $file = shift;
	my $lang = shift;
	my $reference = shift;
    if (-f $file)
    {
        eval { require($file) ;};
        if ($@)
        {
            warn "require $file failed: $@\n";
            return;
        }
        unless (File::Copy::copy ($file, "$file.old"))
        {
            warn "Error copying $file to $file.old\n";
            return;
        }
        if (!defined($LANGUAGES->{$lang}))
        {
            warn "LANGUAGES->{$lang} not defined in $file\n";
            return;
        }
    }
    if (!defined($T2H_OBSOLETE_STRINGS->{$lang}))
    {
        $T2H_OBSOLETE_STRINGS->{$lang} = {};
    }
    if (!defined($LANGUAGES->{$lang}))
    {
        $LANGUAGES->{$lang} = {};
    }
	
	foreach my $string (keys %{$LANGUAGES->{$lang}})
	{
        $T2H_OBSOLETE_STRINGS->{$lang}->{$string} = $LANGUAGES->{$lang}->{$string}
            if (defined($LANGUAGES->{$lang}->{$string}) and ($LANGUAGES->{$lang}->{$string} ne ''));
    }
	
    $LANGUAGES->{$lang} = {};
    
    foreach my $string (keys (%{$reference}))
    {
        if (exists($T2H_OBSOLETE_STRINGS->{$lang}->{$string}) and
            defined($T2H_OBSOLETE_STRINGS->{$lang}->{$string}) and
            ($T2H_OBSOLETE_STRINGS->{$lang}->{$string} ne ''))
        {
             $LANGUAGES->{$lang}->{$string} = $T2H_OBSOLETE_STRINGS->{$lang}->{$string};
             delete $T2H_OBSOLETE_STRINGS->{$lang}->{$string};
        }
        else
        {
             $LANGUAGES->{$lang}->{$string} = '';
        }
    }
	return 1;
}

sub update_i18n_files
{
    die "No suitable $i18n_dir directory\n" unless (-d $i18n_dir and -w $i18n_dir);
    #my @languages = @known_languages;
    my @languages = get_languages();
    if (@ARGV)
    {
        @languages = ();
        foreach my $lang (@ARGV)
        {
            unless (grep {$lang eq $_} @known_languages)
            {
                #warn "Unsupported language `$lang'\n";
                #next;
                warn "Remark: you could update the known languages array for `$lang'\n";
            }
            push (@languages, $lang) unless (grep {$lang eq $_} @languages);
        }
    }
    unless (@languages)
    {
        warn "No languages to update\n" ;
        return;
    }
    die "$template_file not readable\n" unless (-r $template_file);
    eval { require($template_file) ;};
    if ($@)
    {
        die "require $template_file failed: $@\n";
    }
    die "LANGUAGE->{'en'} undef after require $template_file\n" unless
         (defined($LANGUAGES) and defined($LANGUAGES->{'en'}));
    foreach my $string (keys(%$template_strings))
    {
        die "template string $string undef" unless (defined($LANGUAGES->{'en'}->{$string}));
    }
    foreach my $lang (@languages)
    {
        update_language_file($lang);
    }
    return 1;
}

sub update_language_file($)
{
    my $lang = shift;
    #unless (grep {$lang eq $_} @known_languages)
    #{
    #    print STDERR "Unsupported language `$lang'\n";
    #    return;
    #}
    my $file = "$i18n_dir/$lang";

    return unless (update_language_hash($file, $lang, $LANGUAGES->{'en'}));
	
    unless (open (FILE, ">$file"))
    {
         warn "open $file failed: $!\n";
         return;
    }

	
    print FILE "" . Data::Dumper->Dump([$LANGUAGES->{$lang}], [ "LANGUAGES->{'$lang'}" ]);
    print FILE "\n";
    print FILE Data::Dumper->Dump([$T2H_OBSOLETE_STRINGS->{$lang}], [ "T2H_OBSOLETE_STRINGS->{'$lang'}"]);
    print FILE "\n";
    print FILE "\n";
    close FILE;
}

sub update_template (@)
{
    my $source_strings = {};
    foreach my $source_file (@_)
    {
        unless (-r $source_file) 
        {
            warn "$source_file not readable\n";
            next;
        }
        unless (open (FILE, "$source_file"))
        {
            warn "open $source_file failed: $!\n";
            next;
        }
        my $line_nr = 0;
        while (<FILE>)
        {
             $line_nr++;
             my $string;
             next if /^\s*#/;
             while ($_)
             {
                  if (defined($string))
                  {
                       if (s/^([^\\']*)(\\|')//)
                       {
                            $string .= $1 if (defined($1));
                            if ($2 eq "'")
                            {
                                 $source_strings->{$string} = '' ;
                                 $string = undef;
                            }
                            else
                            {
                                 if (s/^(.)//)
                                 {
                                      #$string .= '\\' . $1;
                                      $string .= $1;
                                 }
                                 else
                                 {
                                      warn "\\ at end of line, file $source_file, line nr $line_nr\n";
                                      $source_strings->{$string} = '' ;
                                      $string = undef;
                                 }
                            }
                       }
                       else
                       {     
                            warn "string not closed file $source_file, line nr $line_nr\n";
                            $source_strings->{$string} = '' ;
                            $string = undef;
                       }
                  }
                  elsif (s/^.*?&\$I\s*\('//)
                  {
                       $string = '';
                  }
                  else
                  {
                       last;
                  }
             }
        }
        close FILE;
    }
    foreach my $string (keys (%$template_strings))
    {
        $source_strings->{$string} = $template_strings->{$string};
    }
    die unless(update_language_hash($template_file, 'en', $source_strings));
	foreach my $string (keys(%$template_strings))
	{ # use values in template_srings if it exists
        $LANGUAGES->{'en'}->{$string} = $template_strings->{$string} if ($LANGUAGES->{'en'}->{$string} eq '');
    }
    unless (open (TEMPLATE, ">$template_file"))
    {
        die "open $template_file failed: $!\n";
    }
    print TEMPLATE "" . Data::Dumper->Dump([$LANGUAGES->{'en'}], [ "LANGUAGES->{'en'}" ]);
    print TEMPLATE "\n";
	if (keys(%{$T2H_OBSOLETE_STRINGS->{'en'}}))
	{
        print TEMPLATE Data::Dumper->Dump([$T2H_OBSOLETE_STRINGS->{'en'}], [ "T2H_OBSOLETE_STRINGS->{'en'}"]);
        print TEMPLATE "\n";
        print TEMPLATE "\n";
    }
    close TEMPLATE;
}

manage_i18n_files($command);

1;
