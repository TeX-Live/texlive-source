$dd = '/';

&main;

exit 0;

sub main {
    local (@files);
    @files = @ARGV;
    $temp = &get_tmp_dir() . "ait.$$";
    open(OUT, ">$temp") || exit 1;
    print OUT "Contents:\n\n";
    while (<>) {
	$prevline = $line;
	$line = $_;
	if ($line =~ m/^\*+$/) {
	    print OUT "$prevline";
	}
	elsif ($line =~ m/^=+$/) {
	    print OUT "  $prevline";
	}
	elsif ($line =~ m/^-+$/) {
	    print OUT "    $prevline";
	}
	elsif ($line =~ m/^\.+$/) {
	    print OUT "      $prevline";
	}
    }
    print OUT "\n\n";
    @ARGV = @files;
    while (<>) {
	print OUT $_;
    }
    close (OUT);
    open (IN, $temp);
    open (OUT, ">$files[0]");
    binmode(IN); binmode(OUT);
    local($/) = undef;
    print OUT <IN>;
    close(OUT);
    close(IN);    
}

sub get_tmp_dir {
    if (!($TMP)) {	
	local (@l) = ("TMPDIR", "TEMP", "TMP", "c${dd}tmp");
	local ($var, $dir);
	foreach $var (@l) {
	    $dir = $ENV{$var};
	    if (-d $dir) { 
		$TMP = $dir;
		last;
	    }
	}
    }
    $TMP =~ s/\\/$dd/g;
	
    if ($TMP =~ /$dd$/) {
	} else { $TMP .= $dd }
}
