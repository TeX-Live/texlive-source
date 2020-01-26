#!/bin/sh

tty -s && {
  echo
  echo "Warning: the ps2frag script is not needed with this version of psfrag."
  echo "Please read the manpage ps2frag(1) and the documentation of the"
  echo "psfrag package."
  echo
} >&2

exit 0
