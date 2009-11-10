use Test;
use Encode::TECkit;

%tests = (
    'silipa93.tec' => [
        ["D\"\xe2s i\xf9z ?\xab tHEstH", 
            "\x{00F0}i\x{0303}s i\x{02D0}z \x{0294}\x{0259} t\x{02B0}\x{025B}st\x{02B0}"],
        ],
    'academy.tec' => [
        ["upkdu|Gm:", "\x{1000}\x{1005}\x{102F}\x{102D}\x{1000}\x{101B}\x{1039}\x{101D}\x{102C}\x{1038}"],
        ],
    );

foreach $k (keys %tests)
{
    $numtests++;
    $numtests += scalar (@{$tests{$k}}) * 2;
}

plan tests => $numtests;    

foreach $k (keys %tests)
{
    $enc = Encode::TECkit->new("../../test/$k");
    ok ($enc);
    foreach $t (@{$tests{$k}})
    {
        $str = $enc->decode($t->[0]);
        ok ($str, $t->[1]);
        $str = $enc->encode($t->[1]);
        ok ($str, $t->[0]);
    }
}
