package Getopt::MySimple;

# Name:
#	Getopt::MySimple.
#
# Documentation:
#	POD-style (incomplete) documentation is in file MySimple.pod
#
# Tabs:
#	4 spaces || die.
#
# Author:
#	Ron Savage	rpsavage@ozemail.com.au.
#	1.00	19-Aug-97	Initial version.
#	1.10	13-Oct-97	Add arrays of switches (eg '=s@').
#	1.20	 3-Dec-97	Add 'Help' on a per-switch basis.
#	1.30	11-Dec-97	Change 'Help' to 'verbose'. Make all hash keys lowercase.
#	1.40	10-Nov-98	Change width of help report. Restructure tests.
#               1-Jul-00        Modifications for Texi2html

# --------------------------------------------------------------------------
# Locally modified by obachman (Display type instead of env, order by cmp)
# $Id: MySimple.pm,v 1.4 2004/02/10 00:12:42 pertusus Exp $

# use strict;
# no strict 'refs';

use vars qw(@EXPORT @EXPORT_OK @ISA);
use vars qw($fieldWidth $opt $VERSION);

use Exporter();
use Getopt::Long;

@ISA		= qw(Exporter);
@EXPORT		= qw();
@EXPORT_OK	= qw($opt);	# An alias for $self -> {'opt'}.

# --------------------------------------------------------------------------

$fieldWidth	= 20;
$VERSION	= '1.41';

# --------------------------------------------------------------------------

sub byOrder
{
	my($self) = @_;
	
	return uc($a) cmp (uc($b));
}

# --------------------------------------------------------------------------

sub dumpOptions
{
	my($self) = @_;

	print 'Option', ' ' x ($fieldWidth - length('Option') ), "Value\n";

	for (sort byOrder keys(%{$self -> {'opt'} }) )
	{
	  print "-$_", ' ' x ($fieldWidth - (1 + length) ), "${$self->{'opt'} }{$_}\n";
	}

	print "\n";

}	# End of dumpOptions.

# --------------------------------------------------------------------------
# Return:
#	0 -> Error.
#	1 -> Ok.

sub getOptions
{
	push(@_, 0) if ($#_ == 2);	# Default for $ignoreCase is 0.
	push(@_, 1) if ($#_ == 3);	# Default for $helpThenExit is 1.

	my($self, $default, $helpText, $versionText, 
	   $helpThenExit, $versionThenExit, $ignoreCase) = @_;
	
	$helpThenExit = 1 unless (defined($helpThenExit));
	$versionThenExit = 1 unless (defined($versionThenExit));
	$ignoreCase = 0 unless (defined($ignoreCase));

	$self -> {'default'}		= $default;
	$self -> {'helpText'}		= $helpText;
	$self -> {'versionText'}        = $versionText;
	$Getopt::Long::ignorecase	= $ignoreCase;

	unless (defined($self -> {'default'}{'help'}))
	{
	  $self -> {'default'}{'help'} = 
	  { 
	   type => ':i', 
	   default => '',
	   linkage => sub {$self->helpOptions($_[1]); sleep 5;exit (0) if $helpThenExit;},
	   verbose => "print help and exit"
	  };
	}

	unless (defined($self -> {'default'}{'version'}))
	{
	  $self -> {'default'}{'version'} = 
	  { 
	   type => '', 
	   default => '',
	   linkage => sub {print $self->{'versionText'};  exit (0) if $versionThenExit;},
	   verbose => "print version and exit"
	  };
	}

	for (keys(%{$self -> {'default'} }) )
	{
	  my $type = ${$self -> {'default'} }{$_}{'type'};
	  push(@{$self -> {'type'} }, "$_$type");
	  $self->{'opt'}->{$_} =  ${$self -> {'default'} }{$_}{'linkage'}
            if ${$self -> {'default'} }{$_}{'linkage'};
	}

	my($result) = &GetOptions($self -> {'opt'}, @{$self -> {'type'} });

        return $result unless $result;

	for (keys(%{$self -> {'default'} }) )
	{
 	   if (! defined(${$self -> {'opt'} }{$_})) #{
            {
 	     ${$self -> {'opt'} }{$_} = ${$self -> {'default'} }{$_}{'default'};
            }
	}

	$result;
}	# End of getOptions.

# --------------------------------------------------------------------------

sub helpOptions
{
	my($self) = shift;
	my($noHelp) = shift;
	$noHelp = 0 unless $noHelp;
	my($optwidth, $typewidth, $defaultwidth, $maxlinewidth, $valind, $valwidth) 
	  = (10, 5, 9, 78, 4, 11);

	print "$self->{'helpText'}" if ($self -> {'helpText'});

	print ' Option', ' ' x ($optwidth - length('Option') -1 ),
		'Type', ' ' x ($typewidth - length('Type') + 1),
		'Default', ' ' x ($defaultwidth - length('Default') ),
	        "Description\n";

	for (sort byOrder keys(%{$self -> {'default'} }) )
	{
	  my($line, $help, $option, $val);
	  $option = $_;
	  next if ${$self->{'default'} }{$_}{'noHelp'} && ${$self->{'default'} }{$_}{'noHelp'} > $noHelp;
          #$line = " -$_" . ' ' x ($optwidth - (2 + length) ) .
          #      	"${$self->{'default'} }{$_}{'type'} ".
          #      	' ' x ($typewidth - (1+length(${$self -> {'default'} }{$_}{'type'}) ));
		$line = " --$_" . "${$self->{'default'} }{$_}{'type'}".
			' ' x ($typewidth - (1+length(${$self -> {'default'} }{$_}{'type'}) ));

                 $val = ${$self->{'default'} }{$_}{'linkage'};
                if ($val)
                {
                  if (ref($val) eq 'SCALAR')
		  {
		    $val = $$val; 
		  }
		  else
		  {
		    $val = '';
		  }
                }
		else
		{
		  $val = ${$self->{'default'} }{$_}{'default'};
		}
	        $line .= "$val  ";
		$line .= ' ' x ($optwidth + $typewidth + $defaultwidth + 1 - length($line));
		
		if (defined(${$self -> {'default'} }{$_}{'verbose'}) &&
		  ${$self -> {'default'} }{$_}{'verbose'} ne '')
	      {
		$help = "${$self->{'default'} }{$_}{'verbose'}";
	      }
	      else
	      {
		$help = ' ';
	      }
	      if ((length("$line") + length($help)) < $maxlinewidth)
	      {
		print $line , $help, "\n";
	      }
	      else
	      {
		print $line, "\n", ' ' x $valind, $help, "\n";
	      }
	      for $val (sort byOrder keys(%{${$self->{'default'}}{$option}{'values'}}))
	      {
	        print ' ' x ($valind + 2);
                print $val, '  ', ' ' x ($valwidth - length($val) - 2);
	        print ${$self->{'default'}}{$option}{'values'}{$val}, "\n";
	      }
	}

	print <<EOT;
Note: 'Options' may be abbreviated. 'Type' specifications mean:
 <none>| !    no argument: variable is set to 1 on -foo (or, to 0 on -nofoo)
    =s | :s   mandatory (or, optional)  string argument
    =i | :i   mandatory (or, optional)  integer argument
EOT
}	# End of helpOptions.

#-------------------------------------------------------------------

sub new
{
	my($class)				= @_;
	my($self)				= {};
	$self -> {'default'}	= {};
	$self -> {'helpText'}	= '';
	$self -> {'opt'}		= {};
	$opt					= $self -> {'opt'};	 # An alias for $self -> {'opt'}.
	$self -> {'type'}		= ();

	return bless $self, $class;

}	# End of new.

# --------------------------------------------------------------------------

1;

# End MySimple.pm
