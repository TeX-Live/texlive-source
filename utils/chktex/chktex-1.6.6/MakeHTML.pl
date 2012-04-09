# Remove parts of code enclosed in %latex/%endlatex tags

$latexonly = 0;

while(<>)
{
    $latexonly = 1 if /^%latex/i;
    $latexonly = 0 if /^%endlatex/i;
    

    if($latexonly) {
	if(/^%!(.*)/)
	{
	    print $1;
	}
    }
    else {
	print;
    }
}
