#
# Perl script for processing modes.mf and build 2 char** 
# initialized with modes and bpdi values.
#

&main;

1;

sub main {
  local($modes_file, $m, $dpi);

  $modes_file = `kpsewhich modes.mf`;
  if ($modes_file eq "") {
    print "Can't find modes.mf !\nAborting...";
    exit 1;
  }
  open IN, "<$modes_file";
  $m = "";
  while (<IN>) {
    if ($_ =~ m/^mode_def\s+([^\s]+)\s+=\s+.*\\\[\s*(.*)\s*$/) {
      $m = $1;
      push @list_modes, $m; 
      push @list_desc, $2;
      $count ++;
    }
    elsif ($_ =~ m/mode_param\s*\(\s*pixels_per_inch\s*,\s*([0-9\.]+)\s*\)/) {
      $bdpi = $1;
      push @list_bdpi, $bdpi;
    }
  }
  close IN;

  # print lists
  open OUT, ">mfmodes.h";
  print OUT "typedef struct _mfmode {\n\tchar *name;\n\tchar *bdpi;\n\tchar *desc;\n\t} mfmode;\n\n";
  print OUT "mfmode avail_modes[$count] = {\n";

  for ($i = 0; $i < $count - 1; $i++) {
    print OUT "\t{\"$list_modes[$i]\", \"$list_bdpi[$i]\", \"$list_desc[$i]\"},\n";
  }
  print OUT "\t{\"$list_modes[$i]\", \"$list_bdpi[$i]\", \"$list_desc[$i]\"}\n\t};\n";

}
