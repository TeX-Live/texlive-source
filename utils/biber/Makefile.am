# See ./README for our basic strategy here.
#
#   Copyright 2011 Karl Berry <tex-live@tug.org>
#
#   This file is free software; the copyright holder
#   gives unlimited permission to copy and/or distribute it,
#   with or without modifications, as long as this notice is preserved.

if TL_HAVE_BIBER_BINARY
#
bin_SCRIPTS = biber
tl_platform = $(TL_PLATFORM)
#
biber: biber.$(tl_platform)
	rm -f biber
	cp "$(srcdir)/biber.$(tl_platform)" biber

# if the biber binary for the current platform is not present,
# nothing to do.
endif
