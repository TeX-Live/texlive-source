#!/usr/bin/perl -w
##-----------------------------------------------------------------------------
## This file is part of ctan-o-mat.
## This program is distributed under BSD-like license. See file LICENSE
##
## (c) 2016-2017 Gerd Neugebauer
##
## Net: gene@gerd-neugebauer.de
##
## This program is free software; you can redistribute it and/or modify it
## under the terms of a 3-clause BSD-like license as stated in the file
## LICENSE contained in this distribution.
##
## You should have received a copy of the LICENSE along with this program; if
## not, see the repository under https://github.com/ge-ne/ctan-o-mat.
##
##-----------------------------------------------------------------------------

=head1 NAME

ctan-o-mat - Validate and upload a package for CTAN

=head1 SYNOPSIS

ctan-o-mat [options] [<package configuration>]

=head1 DESCRIPTION

This program can be used to automate the upload of a package to the
Comprehensive TeX Archive Network (https://www.ctan.org). The description
of the package is taken from a configuration file. Thus it can be updated
easily without the need to fill a Web form with the same old information
again and again.

The provided information is validated in any case. If the validation
succeeds and not only the validation is requested then the provided
archive file is placed in the incoming area of the CTAN for further
processing by the CTAN team.

In any case any finding during the validation is reported at the end
of the processing. Note that the validation is the default and a
official submission has to be requested by the an appropriate command
line option.

B<ctan-o-mat> requires an Internet connection to the CTAN server. Even the
validation retrieves the known attributes and the basic constraints
from the server.


=head1 CONFIGURATION

The default configuration is read from a file with the same name as
the current directory an the extension .pkg. This file name can be
overwritten on the command line.

The configuration depends on the features currently supported by the
CTAN server.
Since these features can change over time the configuration is not
hard-coded in B<ctan-o-mat>. You can request an empty template of the
configuration via the command line parameter C<--init>.


=head1 OPTIONS

=over 4

=item -h

=item --help

Print this short summary about the usage and exit the program.

=item -i

=item --init

Create an empty template for a configuration.

=item --list licenses

List the known licenses of CTAN to the standard output stream.
Each license is represented as one line. The line contains the fields
key, name, free indicator. Those fields are separated by tab characters.
Afterwards the program terminates without processing any further arguments.

=item --config <package configuration>

=item --pkg <package configuration>

=item --package <package configuration>

Set the package configuration file.

=item -s

=item --submit

Upload the submission, validate it and officially submit it to CTAN it the 
validation succeeds.

=item -v

=item --verbose

Print some more information during the processing (verbose mode).

=item --version

Print the version number of this program and exit.

=item --validate

=item -n

=item --noaction

Do not perform the final upload. The package is validated and the
resulting messages are printed. 

=item <package>

This parameter is the name of a package configuration
(see section CONFIGURATION) contained in a file.
If not set otherwise the package configuration defaults to the
name of the current directory with C<.pkg> appended.

=back


=head1 ENVIRONMENT

The following environment variables are recognized by B<ctan-o-mat>.

=over 4

=item CTAN_O_MAT_URL

The value is the URL prefix for the CTAN server to be contacted. The default
is C<https://ctan.org/submit>. The complete URL is constructed by appending
C<validate>, C<upload>, or C<fields> to use the respective CTAN REST API.

=back


=head1 CONNECTING VIA PROXIES

If you need to connect to the Internet via a proxy then this can be achieved
by setting some environment variables before running B<ctan-o-mat>.
To redirect the request via the proxy simply define an environment variable
C<http_proxy> to point to the proxy host -- including protocol and port as
required. Note that the name of the environment variable is supposed to be in
B<lower> case.


=head1 AUTHOR

Gerd Neugebauer (gene@gerd-neugebauer.de)

=head1 BUGS

=over 4

=item *

The program can not be used without a working connection to the
Internet.

=back

=cut

use strict;
use FileHandle;
use File::Basename;
use Cwd;

use constant VERSION => '1.2';

#------------------------------------------------------------------------------
# Function:		usage
# Arguments:	none
# Returns:		nothing
# Description:	Print the POD to stderr and exit
#
sub usage {
	use Pod::Text;
	Pod::Text->new()
	  ->parse_from_filehandle( new FileHandle( $0, 'r' ), \*STDERR );
	exit(0);
}

#------------------------------------------------------------------------------
# Variable:		$verbose
# Description:	The verbosity indicator.
#
my $verbose = 0;

#------------------------------------------------------------------------------
# Variable:		$debug
# Description:	The debug indicator.
#
my $debug = 0;

#------------------------------------------------------------------------------
# Variable:		$submit
# Description:	The validation or submit indicator.
#
my $submit = undef;

#------------------------------------------------------------------------------
# Variable:		$cfg
# Description:	The name of the configuration file.
#
my $cfg = undef;

#------------------------------------------------------------------------------
# Variable:		$CTAN_URL
# Description:  The base URL for requesting information from the CTAN server.
#
my $CTAN_URL = $ENV{'CTAN_O_MAT_URL'} || 'https://ctan.org';
$CTAN_URL .= '/' if not $CTAN_URL =~ m/\/$/;

use Getopt::Long;
GetOptions(
	"config=s" => \$cfg,
	"debug"    => \$debug,
	"h|help"   => \&usage,
	"i|init:s" => sub {
		local $_ = pkg_name_or_fallback( $_[1], '' );
		( new CTAN::Pkg() )->add( pkg => $_ )
		  ->write( new CTAN::Upload::Fields() );
		exit(0);
	},
	"list=s" => sub {
		if ( $_[1] eq 'licenses' ) {
			new CTAN::Licenses()->print();
		}
		else {
			print STDERR "*** Unknown entity $_[1]\n";
		}
		exit(0);
	},
	"n|noaction"    => sub { $submit = undef; },
	"pkg=s"         => \$cfg,
	"package=s"     => \$cfg,
	"submit|upload" => sub { $submit = 1; },
	"validate"      => sub { $submit = undef; },
	"v|verbose"     => \$verbose,
	"version"       => sub {
		print STDOUT VERSION, "\n";
		exit(0);
	},
);

new CTAN::Pkg()
  ->read( pkg_name_or_fallback( $ARGV[0] || $cfg, '.pkg' ) )
  ->upload($submit);

#------------------------------------------------------------------------------
# Function:		pkg_name_or_fallback
# Arguments:	$value the value
#				$ext the extension to append
# Description:	Construct a fallback from the current directory if $value is
#				not defined.
#
sub pkg_name_or_fallback {
	my ( $value, $ext ) = @_;
	if ( not defined $value or $value eq '' ) {
		$value = cwd();
		$value =~ s|.*[/\\]||;
		$value = $value . $ext;
	}
	return $value;
}

###############################################################################

package JSON::Parser;

#------------------------------------------------------------------------------
# Constructor:	new
# Description:	This is the constructor
#
sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $this = {};

	return bless $this, $class;
}

#------------------------------------------------------------------------------
# Method:		parse
# Arguments:
#	$json		the JSON list with the messages
# Description:	Parse the input string for a JSON object and retrun the Perl
#				representation of it.
#
sub parse {
	my ( $this, $json ) = @_;

	my ( $result, $remainder ) = $this->scan($json);
	chomp $remainder;
	if ( $remainder ne '' ) {
		die "*** Unprocessed JSON: $remainder\n";
	}
	return $result;
}

#------------------------------------------------------------------------------
# Method:		scan
# Arguments:
#	$json		the JSON list with the messages
# Description:	Scan the input string for the next token
#
sub scan {
	my ( $this, $json ) = @_;
	local $_ = $json;

	s/^\s+//;
	if ( m/^\[\s*/ ) {
		my @a = ();
		$_ = $';
		while ( not m/^\]/ ) {
			my ( $el, $remainder ) = $this->scan($_);
			push @a, $el;
			$_ = $remainder;
			s/^\s*,\s*//;
		}
		$_ = substr( $_, 1 );
		return ( \@a, $_ );
	}
	elsif ( m/^\{\s*/ ) {
		my %a = ();
		$_ = $';
		while ( not m/^\}/ ) {
			my ( $key, $remainder ) = $this->scan($_);
			$_ = $remainder;
			s/^\s*:\s*//;
			my ( $val, $remainder2 ) = $this->scan($_);
			$_ = $remainder2;
			$a{$key} = $val;
			s/^\s*,\s*//;
		}
		$_ = substr( $_, 1 );
		return ( \%a, $_ );
	}
	elsif ( $_ =~ m/^"/ ) {
		$_ = $';
		my $s = '';
		while ( m/(\\.|")/ ) {
			$s .= $`;
			$_ = $';
			if ( $& eq '"' ) {
				return ( $s, $_ );
			}
			if ( $& eq '\\n' ) {
				$s .= "\n";
			}
			elsif ( $& eq '\\"' ) {
				$s .= '"';
			}
			elsif ( $& eq '\\t' ) {
				$s .= "\t";
			}
			elsif ( $& eq '\\\\' ) {
				$s .= "\\";
			}
			elsif ( $& eq '\\r' ) {
				$s .= "\r";
			}
			elsif ( $& eq '\\b' ) {
				$s .= "\b";
			}
			else {
				$s .= "\\";
			}
		}
		die "*** Missing end of string\n";
	}
	elsif ( m/^([0-9]+|[a-z]+)/i ) {
		$_ = $';
		$_ = $&;
		return ( $_, $_ );
	}

	die "*** Parse error at: $_\n";
}

###############################################################################
package CTAN::Upload::Fields;

use LWP::UserAgent;
use LWP::Protocol::https;
use HTTP::Request::Common;

#------------------------------------------------------------------------------
# Variable:		@parameter
# Description:	The list of fields.
#
my @parameter = ();    # FIXME

#------------------------------------------------------------------------------
# Constructor:	new
# Description:	This is the constructor
#
sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $this  = {};
	bless $this, $class;
	return $this->_load();
}

#------------------------------------------------------------------------------
# Method:		_load
# Arguments:	none
# Description:	Retrieve a list of currently supported fields from the
#				CTAN server.
#
sub _load {
	my $this = shift;
	my $url  = $CTAN_URL . 'submit/fields';

	print STDERR "--- Retrieving fields from CTAN..." if $::verbose;
	print STDERR $url, "\n" if $debug;
	my $response;
	eval {
		my $ua      = LWP::UserAgent->new();
		my $request = GET $url;
		print STDERR "done\n" if $::verbose;
		$response = $ua->request($request);
	};

	die CTAN::ErrorHandler::format(
		$response->decoded_content, $response->status_line
	  ),
	  "\n"
	  if not $response->is_success;

	local $_ = $response->decoded_content;
	print STDERR $response->decoded_content, "\n\n" if $debug;
	while (m/\"([a-z0-9]+)\":\{([^{}]*)\}/i) {
		my $f = $1;
		my %a = ();
		$_ = $';
		my $attr = $2;
		while ( $attr =~ m/\"([a-z0-9]+)\":([a-z0-9]+|"[^"]*")/i ) {
			$attr = $';
			$a{$1} = $2;
			$a{$1} =~ s/(^"|"$)//g;
		}
		$this->{$f} = \%a;
		push @CTAN::Upload::Fields::parameter, $f;
	}
	return $this;
}

###############################################################################
package CTAN::Licenses;

use LWP::UserAgent;
use LWP::Protocol::https;
use HTTP::Request::Common;

use Data::Dumper;

#------------------------------------------------------------------------------
# Constructor:	new
# Description:	This is the constructor
#
sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $this  = [];
	bless $this, $class;
	return $this->_load();
}

#------------------------------------------------------------------------------
# Method:		_load
# Arguments:	none
# Description:	Retrieve a list of currently supported licenses from the
#				CTAN server.
#
sub _load {
	my $this = shift;
	my $url  = $CTAN_URL . 'json/1.3/licenses';

	print STDERR "--- Retrieving licenses from CTAN..." if $verbose;
	print STDERR $url, "\t" if $debug;
	my $response;
	eval {
		my $ua      = LWP::UserAgent->new();
		my $request = GET $url;
		print STDERR "done\n" if $verbose;
		$response = $ua->request($request);
	};

	die CTAN::ErrorHandler::format(
		$response->decoded_content, $response->status_line
	  ),
	  "\n"
	  if not $@ and not $response->is_success;

	print STDERR "done\n" if $verbose;
	local $_ = $response->decoded_content;

	eval {
		$this->[0] = new JSON::Parser()->parse($_);
	};
	if ($@) {
		s/^[0-9]+ */*** /;
		die $_;
	}

	return $this;
}

#------------------------------------------------------------------------------
# Method:		print
# Arguments:	none
# Description:	Print the licenses to stdout.
#
sub print {
	my $this = shift;
	local $_ = $this->[0];
	my @a = @$_;

	foreach (@a) {
		print $_->{key}, "\t", $_->{name}, "\t";
		if ( $_->{free} eq 'true' ) {
			print "free\n";
		}
		else {
			print "non-free\n";
		}
	}
}

###############################################################################
package CTAN::ErrorHandler;

#------------------------------------------------------------------------------
# Method:		format
# Arguments:
#	$json		the JSON list with the messages
#   $fallback	the fallback message if the first parameter is empty
# Description:	format the JSON error message
#
sub format {
	local $_ = shift;
	if ( $_ eq '' ) {
		return shift;
	}
	if (m/^(<!DOCTYPE html>|<html)/i) {
		return "Unexpected HTML response found under $CTAN_URL";
	}

	my $json;
	eval {
		$json = new JSON::Parser()->parse($_);
	};
	if ($@) {
		s/^[0-9]+ */*** /;
		die $_;
	}
	return join( "\n", map { join( ': ', @$_ ) } @$json );
}

###############################################################################
package CTAN::Pkg;

use LWP::UserAgent;
use LWP::Protocol::https;
use HTTP::Request::Common;

#------------------------------------------------------------------------------
# Constructor:	new
# Description:	This is the constructor
#
sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $this  = [];
	return bless $this, $class;
}

#------------------------------------------------------------------------------
# Method:		add
# Arguments:	arbitrary many key/value pairs
# Description:
#	This function adds a key/value pair to the object.
#
sub add {
	my $this = shift;
	my ( $key, $val );
	$key = shift;
	$val = shift;
	while ( defined $key and defined $val ) {
		if ( $key eq 'file' ) {
			push @$this, $key => [$val];
		}
		else { push @$this, $key => $val; }
		$key = shift;
		$val = shift;
	}
	return $this;
}

#------------------------------------------------------------------------------
# Method:		read
# Arguments:	$file the file name to be read
# Description:
#	This function parses a configuration file in (La)TeX form and returns
#   it as hash-like list.
#
sub read {
	my ( $this, $file ) = @_;
	die "*** Configuration file missing.\n" if not defined $file;

	my $fields = new CTAN::Upload::Fields();
	my $fd     = new FileHandle($file)
	  || die "*** Configuration file `$file' could not be read.\n";
	local $_;

	while (<$fd>) {
		s/^[ \t]*%.*//;
		s/([^\\])%.*/$1/;
		while (m/\\([a-z]+)/i) {
			$_ = $';
			my $keyword = $1;
			if ( $keyword eq 'begin' ) {
				die "$file:$.: missing {environment} instead of $_\n"
				  if not m/^[ \t]*\{([a-z]*)\}/i;
				$keyword = $1;
				my $val = '';
				$_ = $';
				while ( not m/\\end\{$keyword\}/ ) {
					$val .= $_;
					$_ = <$fd>;
					die "$file:$.: "
					  . "unexpected end of file while searching end of $keyword\n"
					  if not defined $_;
				}
				m/\\end\{$keyword\}/;
				$_ = $';
				$val .= $`;
				$val =~ s/^[ \t\n\r]*//m;
				$val =~ s/[ \t\n\r]*$//m;
				$this->add( $keyword => $val );
			}
			elsif ( $keyword eq 'endinput' ) {
				last;
			}
			elsif ( defined $fields->{$keyword} ) {
				die "$file:$.: missing {environment} instead of $_\n"
				  if not m/^[ \t]*\{([^{}]*)\}/i;
				$_ = $';
				$this->add( $keyword => $1 );
			}
			else {
				die "$file:$.: undefined keyword $keyword\n";
			}
			s/^[ \t]*%.*//;
		}
	}
	$fd->close();
	return $this;
}

#------------------------------------------------------------------------------
# Method:		upload
# Arguments:	Upload a file and the parameters
# Description:	Connect to the CTAN server to upload or validate the package.
#
sub upload {
	my $this   = shift;
	my $submit = shift;

	my $service_url;
	if ($submit) {
		print STDERR "--- Sending to CTAN for submission..." if $verbose;
		$service_url = $CTAN_URL . 'submit/upload';
	}
	else {
		print STDERR "--- Uploading to CTAN for validation..." if $verbose;
		$service_url = $CTAN_URL . 'submit/validate';
	}
	my $ua      = LWP::UserAgent->new();
	my $request = POST(
		$service_url,
		'Content_Type' => 'multipart/form-data',
		'Content'      => $this
	);
	my $response = $ua->request($request);
	print STDERR "done\n" if $verbose;

	die CTAN::ErrorHandler::format( $response->decoded_content,
		$response->status_line )
	  . "\n"
	  if not $response->is_success;

	if ( not $submit and $response->decoded_content eq '[]' ) {
		print "ok\n";
		print STDERR "--- The validation has succeeded.\n",
		  "--- You can now submit your package to CTAN for publication.\n"
		  if $verbose;
	}
	else {
		print CTAN::ErrorHandler::format( $response->decoded_content, 'ok' ),
		  "\n";
	}
	return $this;
}

#------------------------------------------------------------------------------
# Method:		write
# Arguments:	none
# Description:	Write a new configuration to stdout.
#
sub write {
	my $this   = shift;
	my %this   = @$this;
	my $fields = shift;

	print <<__EOF__;
% This is a description file for ctan-o-mat.
% It manages uploads of a package to 
% CTAN -- the Comprehensive TeX Archive Network.
%
% The syntax is roughly oriented towards (La)TeX.
% Two form of the macros are used. The simple macros take one argument
% in braces. Here the argument may not contain embedded macros.
%
% The second form uses an environment enclosed in \\begin{}/\\end{}.
% In the long text fields logo macros can be used.
%
% You should enter your values between the begin and the end of the
% named type.
__EOF__
	local $_;
	foreach (@CTAN::Upload::Fields::parameter) {
		print <<__EOF__;
% -------------------------------------------------------------------------
% This field contains the $fields->{$_}->{'text'}.
__EOF__
		if ( defined $fields->{$_}->{'nullable'} ) {
			print "% The value is optional.\n";
		}
		if ( defined $fields->{$_}->{'url'} ) {
			print "% The value is a URL.\n";
		}
		if ( defined $fields->{$_}->{'email'} ) {
			print "% The value is an email address.\n";
		}
		if ( defined $fields->{$_}->{'file'} ) {
			print
			  "% The value is the file name of the archive to be uploaded.\n";
			print "% It may have a relative or absolute directory.\n";
		}
		if ( defined $fields->{$_}->{'maxsize'} ) {
			print "% The value is restricted to ", $fields->{$_}->{'maxsize'},
			  " characters.\n";
		}
		if ( defined $fields->{$_}->{'list'} ) {
			print "% Multiple values are allowed.\n\\$_\{}\n";
		}
		elsif ( defined $fields->{$_}->{'maxsize'}
			and $fields->{$_}->{'maxsize'} ne 'null'
			and $fields->{$_}->{'maxsize'} < 256 )
		{
			my $v = $this{$_};
			$v = '' if not defined $v;
			print "\\$_\{$v\}\n";
		}
		elsif ( defined $fields->{$_}->{'file'}
			and $fields->{$_}->{'file'} eq 'true' )
		{
			my $v = $this{$_};
			$v = '' if not defined $v;
			print "\\$_\{$v\}\n";
		}
		else {
			my $v = $this{$_};
			if ( defined $v ) {
				$v = "\n  " + $v + "\n";
			}
			else {
				$v = '';
			}

			print "\\begin{$_}$v\\end{$_}\n";
		}
	}
}

#------------------------------------------------------------------------------
# Local Variables:
# mode: perl
# End:
