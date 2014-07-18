package Encode::TECkit;

=head1 NAME

Encode::TECkit - TECkit Encode interface

=head1 DESCRIPTION

This module interfaces with the TECkit processor to provide a Perl interface for
data conversion.

TECkit is a binary encoding converter designed to handle complex encoding conversions
requiring multiple passes over the data and contextual data conversion. See the module
Encode::UTR22 for a module that handles a textual language for this kind of conversion.
That module contains a compiler that takes an extended UTR22 description and creates
a binary control file for TECkit. Equally, TECkit contains its own language and compiler,
but these are not written in Perl.

There are two forms of Encode::TECkit (this is probably a bug). The first is a Perl
object which passes methods along to the Encode::TECkit XS code. The difference is
that the Perl object usually contains two binary Encode::TECkit objects. So, don't
go calling XS methods on the pure Perl object (as returned by new).

Notice that at this stage the interface is not there to use TECkit is a pure Unicode
normalizer or encoding form converter. Use C<Unicode::Normalize> and (un)pack for that.

=head1 METHODS

=cut

require DynaLoader;
@ISA = qw(DynaLoader);

$VERSION = 0.06;

bootstrap Encode::TECkit;

my (%forms) = ('nfc' => 0x100, 'nfd' => 0x200);

=head2 Encode::TECkit->new($fname, %opts)

This creates a new TECkit object. The usual form of the method call is to pass in 
the filename of the TECkit binary control file to use. In addition, the option:
C<-form> may be used to specify which normal form to create when converting to
UTF-8. This can take the values: C<nfc> or C<nfd>.

It is possible to get an XS Encode::TECkit object using new(). To get this, use
the following required options:

=over 4

=item -raw

Set this to a non-zero value to get a pure XS object

=item -forward

if set, then mapping of this object is in the direction of forwards as specified
in the TECkit binary file. By default this is assumed to by bytes to Unicode. if
cleared, then the direction is the opposite (Unicode to bytes).

=item -style

This specifies what form the data should be converted to. The only sensible values
are: 1 for bytes, 2 for UTF-8 and 3 for Unicode to Unicode translation.

=back

There are other non-required options to new:

=over 4

=item -form

Takes the value C<nfc> or C<nfd> according to which form the data to be converted
is in or should be in.

=back

=cut

sub new
{
    my ($class, $fname, %opts) = @_;
    my ($res) = {};
    my ($ref, $hr, $form);
    
    if ($opts{'-form'})
    { $form = $forms{lc($opts{'-form'})}; }
    else
    { $form = 0; }

    if ($opts{'-raw'})
    { 
        ($res, $hr) = new_conv($fname, $opts{'-forward'}, $opts{'-style'} + $form);
        return undef if $hr;
    }
    else
    {
        ($ref, $hr) = new_conv($fname, 1, 2 + $form);
        return undef if $hr;
        $res->{'decoder'} = $ref;
        ($ref, $hr) = new_conv($fname, 0, 1);
        return undef if $hr;
        $res->{'encoder'} = $ref;
        $res->{'form'} = $form;
    }
    bless $res, ref $class || $class;
}


sub new_scalar
{
    my ($class, $fdat, %opts) = @_;
    my ($res) = {};
    my ($ref, $hr, $form);
    
    if ($opts{'-form'})
    { $form = $forms{lc($opts{'-form'})}; }
    if ($opts{'-raw'})
    { 
        ($res, $hr) = new_conv_scalar($fdat, $opts{'-forward'}, $opts{'-style'} + $form);
        return undef if $hr;
    }
    else
    {
        ($ref, $hr) = new_conv_scalar($fdat, 1, 2 + $form);
        return undef if $hr;
        $res->{'decoder'} = $ref;
        ($ref, $hr) = new_conv_scalar($fdat, 0, 1);
        return undef if $hr;
        $res->{'encoder'} = $ref;
        $res->{'form'} = $form;
    }
    bless $res, ref $class || $class;
}


=head2 $enc->decode($str, $check)

Converts $str from bytes to Unicode. $check does nothing in this implementation.

=cut

sub decode
{
    my ($self, $str, $check) = @_;
    my ($res, $hr);
    
    $hr = 1;
    $res = $self->{'decoder'}->convert($str, 2, $hr);
    return $res;
}


=head2 $enc->encode($str, $check)

Converts $str from Unicode to bytes. $check does nothing in this implementation
and has no meaning (ignore it).

=cut

sub encode
{
    my ($self, $str, $check) = @_;
    my ($res, $hr);
    
    $hr = 1;
    $res = $self->{'encoder'}->convert($str, 1, $hr);
    return $res;
}


=head2 ($xs_enc, $hr) = Encode::TECkit::new_conv($fname, $forward, $style)

XS function to create a new Encode::TECkit object. $fname specifies the filename of
the TECkit binary control file to use. $forward indicates which direction to use
the control file. $style is the encoding form of the output when using this mapping.
The only sensible values are: 1 - bytes, 2 - UTF-8, and 0x102 for UTF-8 NFC 
and 0x202 for UTF-8 NFD.

$hr is a result code which is 0 for success and non-zero for failure. See
TECkit_Engine.h in the source for details of the meaning of this value


=head2 $res = $xs_enc->convert($str, $style, $isComplete)

XS function that converts a string according to the way the converter was setup. $str is the
string to convert. $style indicates the resulting encoding format: 1 - bytes,
2 - UTF-8. $style is used to set the appropriate bits in the string to 
indicate the encoding to Perl. $isComplete indicates whether the string is
a complete string and so no further flushing is needed. It also acts as a return
value (and so must be a valid lvalue). The return value is the $hr for the
conversion.


=head2 $res = $xs_enc->flush($style, $hr)

XS function that finishes off a conversion with the given $style value. Notice that $hr is
merely a place holder for the returned $hr, so must be a valid lvalue. It's
value has no meaning.

=cut
