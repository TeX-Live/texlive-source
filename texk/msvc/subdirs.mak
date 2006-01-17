################################################################################
#
# Makefile  : subdirs, rules to recursively enter directories
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/02/19 02:52:04 popineau>
#
################################################################################

default:: all

all::
	-@echo $(verbose) & for %%d in ($(subdirs)) do \
		echo Entering %%d for $@ \
		& pushd %%d & $(make) $@ & popd

lib::
	-@echo $(verbose) & for %%d in ($(subdirs)) do \
		echo Entering %%d for $@ \
		& pushd %%d & $(make) $@ & popd

install::
	-@echo $(verbose) & for %%d in ($(subdirs)) do \
		echo Entering %%d for $@ \
		& pushd %%d & $(make) $@ & popd

depend::
	-@echo $(verbose) & for %%d in ($(subdirs)) do \
		echo Entering %%d for $@ \
		& pushd %%d & $(make) $@ & popd

# Other standard targets for everything.
clean distclean::
	-@echo $(verbose) & for %%d in ($(subdirs)) do \
		echo Entering %%d for $@ \
		& pushd %%d & $(make) $@ & popd

# End of subdirs.mak
#
# Local Variables:
# mode: makefile
# End:
