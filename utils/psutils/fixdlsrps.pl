@PERL@
# fixdlsrps: fix DviLaser/PS document to work with PSUtils
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$nesting = 0;
$page = 1;
$infont = 0;

@fonts = ();
@body = ();
$header = 1;

while (<>) {
   if (/^XP/) {
      $infont++;
      push(@fonts, $_);
      $infont-- if /PXL.*RP/ || /DN?F.*RP/;
   } elsif ($infont) {
      push(@fonts, $_);
      $infont-- if /PXL.*RP/ || /DN?F.*RP/;
   } elsif ((/^%%EndSetup/ || /^%%Page:/) && $header) {
      print @body;
      @body = ("%%EndSetup\n");
      $header = 0;
   } elsif (/^%%EndProlog/ && !$nesting) {
      push(@body,
	   "\$DviLaser begin/GlobalMode{}bdef/LocalMode{}bdef/XP{}def/RP{}def",
	   "/DoInitialScaling{72.0 Resolution div dup scale}bdef end\n", $_);
   } elsif (/^%%BeginPageSetup/ && !$nesting) {
      push(@body, "%%Page: $page $page\n", $_,
	    "Resolution 72 div dup scale Magnification 1000 div dup scale\n",
	    "/DocumentInitState where {\n",
	    "/DocumentInitState [ matrix currentmatrix currentlinewidth",
	    " currentlinecap currentlinejoin currentdash currentgray",
	    " currentmiterlimit] cvx put}if\n");
      $page++;
   } elsif (/^%%BeginDocument:/ || /^%%BeginBinary:/ || /^%%BeginFile:/) {
      push(@body, $_);
      $nesting++;
   } elsif (/^%%EndDocument/ || /^%%EndBinary/ || /^%%EndFile/) {
      push(@body, $_);
      $nesting--;
   } elsif (!/^%%PageBoundingBox:/ && !/^%%Page:/) {
      push(@body, $_);
   }
}

print @fonts;
print @body;

exit 0;
@END@
