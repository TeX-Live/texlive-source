#+##############################################################################
#
# T2h_i18n.pm: Internationalization for texi2html
#
#    Copyright (C) 1999, 2000  Free Software Foundation, Inc.
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

package Texi2HTML::I18n;

use strict;

use vars qw(
@ISA
@EXPORT
);

use Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(pretty_date);

my $language;
my $i18n_dir = 'i18n'; # name of the directory containing the per language files
#my $translation_file = 'translations.pl'; # file containing all the translations
#my @known_languages = ('de', 'nl', 'es', 'no', 'pt', 'fr'); # The supported
                                               # languages

########################################################################
# Language dependencies:
# To add a new language extend the WORDS hash and create $T2H_<...>_WORDS hash
# To redefine one word, simply do:
# $T2h_i18n::T2H_LANGUAGES->{<language>}->{<word>} = 'whatever' in your personal init file.
#
my $T2H_WORDS_EN =
{
 # titles  of pages
 #'Table of Contents'       => 'Table of Contents',
 #'Short Table of Contents'  => 'Short Table of Contents',
 #'Index'     => 'Index',
 #'About This Document'     => 'About This Document',
 #'Footnotes' => 'Footnotes',
 #'See'             => 'See',
 #'see'             => 'see',
 #'section'         => 'section',
 'About This Document'       => '',
 'Table of Contents'         => '',
 'Short Table of Contents',  => '',
 'Index'                     => '',
 'Footnotes'                 => '',
 'See'                       => '',
 'see'                       => '',
 'section'                   => '',
 'Top'                       => '',
 'Untitled Document'         => '',
 # If necessary, we could extend this as follows:
 #  # text for buttons
 #  'Top_Button' => 'Top',
 #  'ToC_Button' => 'Contents',
 #  'Overview_Button' => 'Overview',
 #  'Index_button' => 'Index',
 #  'Back_Button' => 'Back',
 #  'FastBack_Button' => 'FastBack',
 #  'Prev_Button' => 'Prev',
 #  'Up_Button' => 'Up',
 #  'Next_Button' => 'Next',
 #  'Forward_Button' =>'Forward',
 #  'FastWorward_Button' => 'FastForward',
 #  'First_Button' => 'First',
 #  'Last_Button' => 'Last',
 #  'About_Button' => 'About'
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

my $T2H_WORDS_DE =
{
 'Table of Contents'       => 'Inhaltsverzeichniss',
 'Short Table of Contents'  => 'Kurzes Inhaltsverzeichniss',
 'Index'     => 'Index',
 'About This Document'     => '&Uuml;ber dieses Dokument',
 'Footnotes' => 'Fu&szlig;noten',
 'See'             => 'Siehe',
 'see'             => 'siehe',
 'section'         => 'Abschnitt',
 'January' => 'Januar', 
 'February' => 'Februar',
 'March' => 'M&auml;rz', 
 'April' => 'April',
 'May' => 'Mai',
 'June' => 'Juni',
 'July' => 'Juli',
 'August' => 'August',
 'September' => 'September',
 'October' => 'Oktober',
 'November' => 'November',
 'December' => 'Dezember', 
};

my $T2H_WORDS_NL =
{
 'Table of Contents'       => 'Inhoudsopgave',
 'Short Table of Contents'  => 'Korte inhoudsopgave',
 'Index'     => 'Index',      #Not sure ;-)
 'About This Document'     => 'No translation available!', #No translation available!
 'Footnotes' => 'No translation available!', #No translation available!
 'See'             => 'Zie',
 'see'             => 'zie',
 'section'         => 'sectie',
 'January' => 'Januari', 
 'February' => 'Februari',
 'March' => 'Maart', 
 'April' => 'April',
 'May' => 'Mei',
 'June' => 'Juni',
 'July' => 'Juli',
 'August' => 'Augustus',
 'September' => 'September',
 'October' => 'Oktober',
 'November' => 'November',
 'December' => 'December', 
};

my $T2H_WORDS_ES =
{
 'Table of Contents'       => '&iacute;ndice General',
 'Short Table of Contents'  => 'Resumen del Contenido',
 'Index'     => 'Index',      #Not sure ;-)
 'About This Document'     => 'No translation available!', #No translation available!
 'Footnotes' => 'Fu&szlig;noten',
 'See'             => 'V&eacute;ase',
 'see'             => 'v&eacute;ase',
 'section'         => 'secci&oacute;n',
 'January' => 'enero', 
 'February' => 'febrero',
 'March' => 'marzo', 
 'April' => 'abril',
 'May' => 'mayo',
 'June' => 'junio',
 'July' => 'julio',
 'August' => 'agosto',
 'September' => 'septiembre',
 'October' => 'octubre',
 'November' => 'noviembre',
 'December' => 'diciembre', 
};

my $T2H_WORDS_NO =
{
 'Table of Contents'       => 'Innholdsfortegnelse',
 'Short Table of Contents'  => 'Kort innholdsfortegnelse',
 'Index'     => 'Indeks',     #Not sure ;-)
 'About This Document'     => 'No translation available!', #No translation available!
 'Footnotes' => 'No translation available!',
 'See'             => 'Se',
 'see'             => 'se',
 'section'         => 'avsnitt',
 'January' => 'januar', 
 'February' => 'februar',
 'March' => 'mars', 
 'April' => 'april',
 'May' => 'mai',
 'June' => 'juni',
 'July' => 'juli',
 'August' => 'august',
 'September' => 'september',
 'October' => 'oktober',
 'November' => 'november',
 'December' => 'desember', 
};

my $T2H_WORDS_PT =
{
 'Table of Contents'       => 'Sum&aacute;rio',
 'Short Table of Contents'  => 'Breve Sum&aacute;rio',
 'Index'     => '&Iacute;ndice', #Not sure ;-)
 'About This Document'     => 'No translation available!', #No translation available!
 'Footnotes' => 'No translation available!',
 'See'             => 'Veja',
 'see'             => 'veja',
 'section'         => 'Se&ccedil;&atilde;o',
 'January' => 'Janeiro', 
 'February' => 'Fevereiro',
 'March' => 'Mar&ccedil;o', 
 'April' => 'Abril',
 'May' => 'Maio',
 'June' => 'Junho',
 'July' => 'Julho',
 'August' => 'Agosto',
 'September' => 'Setembro',
 'October' => 'Outubro',
 'November' => 'Novembro',
 'December' => 'Dezembro', 
};

my $T2H_WORDS_FR =
{
 'Table of Contents'       => 'Table des mati&egrave;res',
 'Short Table of Contents'  => 'R&eacute;sum&eacute;e du contenu',
 'Index'     => 'Index',
 'About This Document'     => 'A propos de ce document',
 'Footnotes' => 'Notes de bas de page',
 'See'             => 'Voir',
 'see'             => 'voir',
 'section'         => 'section',
 'January' => 'Janvier', 
 'February' => 'F&eacute;vrier',
 'March' => 'Mars', 
 'April' => 'Avril',
 'May' => 'Mai',
 'June' => 'Juin',
 'July' => 'Juillet',
 'August' => 'Ao&ucirc;t',
 'September' => 'Septembre',
 'October' => 'Octobre',
 'November' => 'Novembre',
 'December' => 'D&eacute;cembre', 
 'T2H_today' => 'le %2$d %1$s %3$d'
};

#$T2H_LANGUAGES =
#{
# 'en' => $T2H_WORDS_EN,
# 'de' => $T2H_WORDS_DE,
# 'nl' => $T2H_WORDS_NL,
# 'es' => $T2H_WORDS_ES,
# 'no' => $T2H_WORDS_NO,
# 'pt' => $T2H_WORDS_PT,
# 'fr' => $T2H_WORDS_FR,
#};

sub set_language($)
{
    my $lang = shift;
    if (defined($lang) && exists($Texi2HTML::Config::LANGUAGES->{$lang}) && defined($Texi2HTML::Config::LANGUAGES->{$lang}))
    {
         $language = $lang;
         return 1;
    }
    else
    {
         return 0;
    }
}


my @MONTH_NAMES =
    (
     'January', 'February', 'March', 'April', 'May',
     'June', 'July', 'August', 'September', 'October',
     'November', 'December'
    );

my $I = \&get_string;

sub pretty_date($) 
{
    my $lang = shift;
    my($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst);

    ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
    $year += ($year < 70) ? 2000 : 1900;
    # obachman: Let's do it as the Americans do
    #return($MONTH_NAMES->{$lang}[$mon] . ", " . $mday . " " . $year);
	#return(sprintf(&$I('T2H_today'), (get_string($MONTH_NAMES[$mon]), $mday, $year)));
	return &$I('%{month}, %{day} %{year}', { 'month' => get_string($MONTH_NAMES[$mon]),
          'day' => $mday, 'year' => $year });
}

my $error_no_en = 0;
sub get_string($;$$)
{
    my $string = shift;
    my $arguments = shift;
    my $state = shift;
    my $T2H_LANGUAGES = $Texi2HTML::Config::LANGUAGES;
    if (! exists($T2H_LANGUAGES->{'en'}))
    {
        unless($error_no_en)
        {
            print STDERR "i18n: no LANGUAGES->{'en'} hash\n";
            $error_no_en = 1;
        }
    }
    else
    {
        print STDERR "i18n: missing string $string\n" unless (exists ($T2H_LANGUAGES->{'en'}->{$string}));
        if (defined ($T2H_LANGUAGES->{$language}->{$string}) and
           ($T2H_LANGUAGES->{$language}->{$string} ne ''))
        {
            $string = $T2H_LANGUAGES->{$language}->{$string};
        }
        elsif (defined ($T2H_LANGUAGES->{'en'}->{$string}) and
            ($T2H_LANGUAGES->{'en'}->{$string} ne ''))
        {
            $string = $T2H_LANGUAGES->{'en'}->{$string};
        }
    }
    return main::substitute_line($string, $state) unless (defined($arguments) or !keys(%$arguments));
    my $result = '';
    while ($string)
    {
        if ($string =~ s/^([^%]*)%//)
        {
            $result .= $1 if (defined($1));
            $result .= '%';
            if ($string =~ s/^%//)
            {
                 $result .= '%';
            }
            elsif ($string =~ /^\{(\w+)\}/ and exists($arguments->{$1}))
            {
                 $string =~ s/^\{(\w+)\}//;
                 if (!$state->{'keep_texi'})
                 {
                      $result .= "\@\{$1\@\}";
                 }
                 else
                 {
                      $result .= "\{$1\}";
                 }
            }
            else
            {
                 $result .= '%';
            }
            next;
        }
        else 
        {   
            $result .= $string;
            last;
        }
    }
    if (!$state->{'keep_texi'})
    {
         $string = main::substitute_line($result, $state);
    }
    else
    {
         $string = $result;
    }
    $result = '';
    while ($string)
    {
        if ($string =~ s/^([^%]*)%//)
        {
            $result .= $1 if (defined($1));
            if ($string =~ s/^%//)
            {
                 $result .= '%';
            }
            elsif ($string =~ /^\{(\w+)\}/ and exists($arguments->{$1}))
            {
                 $string =~ s/^\{(\w+)\}//;
                 $result .= $arguments->{$1};
            }
            else
            {
                 $result .= '%';
            }
            next;
        }
        else 
        {   
            $result .= $string;
            last;
        }
    }
    return $result;
}

1;
