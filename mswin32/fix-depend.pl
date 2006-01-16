#
# Split makedepend generated lines
# to get at most one dependency per line
# to facilitate diffs
#
my $line;
my $depline;
my $target;
my @dep;
my %dependencies;
my @preamble;
my @postamble;
my @unmatched;

while (<>) {
  chomp;
  $line = $_;
  if ($line =~ m/^#/) {
    if (keys %dependencies) {
      push @postamble, $line;
    }
    else {
      push @preamble, $line;
    }
    break;
  }
  elsif ($line =~ m/^([^\s].*):\s+(.*)$/) {
    # new dependency
    $target = $1;
    push @{$dependencies{$target}}, $2;
    break;
  }
  elsif ($line =~ m/^\s+(.*)\s+\\$/) {
    # continuation
    push @{$dependencies{$target}}, $1;
    break;
  }
  elsif ($line =~ m/^\s+(.*)$/) {
    # final
    push @{$dependencies{$target}}, $1;
    break;
  }
  else {
    if ($line =~ m/[^\s]/) {
      push @unmatched, $line;
    }
  }

}

END {
  map { print "$_\n"; } @preamble;
  map {
    print "$_: \\\n";
    $depline = join (" ", @{$dependencies{$_}});
    my @dep = split("[ \t]", $depline);
    @dep = grep { $_ !~ m/^(\s*|\s*\\)$/ } @dep;
    $depline = join " \\\n\t", @dep;
    $depline = "\t" . $depline . "\n";
    print $depline;
    # skip one line
    print "\n";
  } (keys %dependencies);
  map { print "$_\n"; } @postamble;
  map { print STDERR "unmatched: $_\n"; } @unmatched;
}
