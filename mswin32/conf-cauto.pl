#
# Generate .h file from .in file
# Define or undefine features for the WIN32 platform
#

use cauto;

&main;

exit(0);

sub main {
    local ($src, $dst);

    if  (-f "$ARGV[0]" && $ARGV[0] =~ /(.*)\.in$/) {
      $src = $ARGV[0];
      $dst = $1;
      if ($ARGV[1] =~ /(.*)\.h$/) {
	$dst = $ARGV[1];
      }
      &cauto::configure_cauto($src, $dst);
    }
    else {
      if (-f "./cauto.h.in") {
	$src = "./cauto.h.in";
	$dst = "./cauto.h";
	&cauto::configure_cauto($src, $dst);
      }
      elsif (-f "./c-auto.in") {
	$src = "./c-auto.in";
	$dst = "./c-auto.h";
	&cauto::configure_cauto($src, $dst);
      }
      
      if (-f "./config.h.in") {
	$src = "./config.h.in";
	$dst = "./config.h";
	&cauto::configure_cauto($src, $dst);
      }
      elsif (-f "./config.hin") {
	$src = "./config.hin";
	$dst = "./config.h";
	&cauto::configure_cauto($src, $dst);
      }
      elsif (-f "./config.in") {
	$src = "./config.in";
	$dst = "./config.h";
	&cauto::configure_cauto($src, $dst);
      }
      else {
	print STDERR "No file to configure found!\n";
      }
    }
}
