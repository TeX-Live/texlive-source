use ExtUtils::MakeMaker;
use Config;
use Getopt::Std;

$name = 'Encode::TECkit';

getopts('d');

@scripts = grep {-f } glob("scripts/*.*");
@cpps = qw(../Engine);
$c_ext = '.cpp';
$o_ext = ($^O =~ m/win32/o) ? '.obj' : '.o';
push(@ExtUtils::MakeMaker::Overridable, qw(pm_to_blib)) if ($^O eq 'MSWin32');

$define = '-I.. "-I../Public-headers" -I../../zlib-1.2.11 -I../..';
$lddlflags = $Config{'lddlflags'};
if ($Config{'byteorder'} eq '4321')
{ $define .= ' -DTARGET_RT_BIG_ENDIAN'; }
elsif (!$Config{'byteorder'} eq '1234')
{ die "Byteorder of $Config{'byteorder'} not currently supported by TECkit"; }
# $libs = $Config{'libs'};
$extras{OPTIMIZE} = '-g' if ($opt_d);
$define .= " -DTRACING" if ($opt_d);


if ($^O =~ m/win32/oi)
{
    $lddlflags =~ s/:x86/:I386/o;
    $lddlflags .= " ../../zlib-1.2.11/zlib.lib";
#    $libs = 'strmbase.lib msvcprt.lib';
    $define .= ' -DWIN32 -GX -Fo$@';
    $ccflags = $Config{'ccflags'};
    $objects = join(' ', map {$_ . $o_ext} (@cpps, 'TECkit'));
#    $ccflags =~ s/-O1//o;
#    foreach $i (split(/;/, $ENV{'INCLUDE'}))
#    { $define .= " -I\"$i\""; }
}
elsif ($^O eq 'linux' || $^O eq 'darwin')
{
    $ccflags = $Config{'ccflags'};
    $objects = join(' ', map {$_ . $o_ext} (@cpps, 'TECkit'));
    $ldfrom = $objects;
    $ldfrom =~ s{\.\./}{}og;
    $extras{LDFROM} = $ldfrom;
    $define .= " -DHAVE_CONFIG_H";
    $libs = '-lstdc++ -lz';
#    $lddlflags .= ' ' . join(' ', map {"-L$_"} split(' ', $Config{'libsdirs'}));
}


WriteMakefile(
    'NAME' => $name,
    'VERSION_FROM' => 'lib/Encode/TECkit.pm',
    'XS' => {'TECkit.xs' => 'TECkit.cpp'},
    'OBJECT' => $objects,
    'C' => [map {"$_" . $c_ext} @cpps],
    'CCFLAGS' => $ccflags,
    'DEFINE' => $define,
    'LDDLFLAGS' => $lddlflags,
    'XSPROTOARG' => '-C++ -nolinenumbers -noprototypes',
    'EXE_FILES' => \@scripts,
    'AUTHOR' => "martin_hosken\@sil.org",
    'ABSTRACT' => "Perl TECkit interface",
    'PMLIBDIRS' => ['lib'],
#    'OPTIMIZE' => '-GZ -Gs',
    'LIBS' => $libs,
    'dist' => { 'TO_UNIX' => 'perl -Mtounix -e "tounix(\"$(DISTVNAME)\")"' },
    %extras
);

if ($^O eq 'MSWin32') {
# incantation to solve the problem of everyone's $Config{make} being 'nmake'
# when we want 'pmake'. And $Config{} is read only.
# actually, this is just a copy of the code from ExtUtiles::MM_Win32 tidied
# up (to expose tabs) and the dependency on Config removed
sub MY::pm_to_blib
{
    my $self = shift;
    my($autodir) = $self->catdir('$(INST_LIB)','auto');
    return <<"EOT";

pm_to_blib: \$(TO_INST_PM)
\t$self->{NOECHO}\$(PERL) \"-I\$(INST_ARCHLINE)\" \"-I\$(INST_LIB)\" \\
\t\"-I\$(PERL_ARCHLIB)\" \"-I\$(PERL_LIB)\" -MExtUtils::Install \\
\t-e \"pm_to_blib({ qw[\$(PM_TO_BLIB)] }, '$autodir')
\t$self->{NOECHO}\$(TOUCH) \$@

EOT
}

}

sub MY::xs_c 
{
    package MY;
    if ($_[-1] eq 'pass_thru')
    {
        pop @_;
        shift->SUPER::xs_c(@_);
    }
    else
    { return ''; }
}


sub MY::xs_o
{
    package MY;
    my ($str) = shift->xs_c(@_, 'pass_thru');
#    $str =~ s/^(\s*)\.xs\.c/$1.xs.cpp/o;
    $str =~ s/\.c/\.cpp/og;
    $str;
}

1;
