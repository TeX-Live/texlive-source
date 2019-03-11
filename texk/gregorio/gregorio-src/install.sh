#!/bin/bash

# Copyright (C) 2015-2019 The Gregorio Project (see CONTRIBUTORS.md)
# 
# This file is part of Gregorio.
#
# Gregorio is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Gregorio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

# This script installs Gregorio and GregorioTeX as cloned from GitHub and built
# using build.sh for a GNU system (like GNU/Linux), using default options.
#
# For now, it passes its argument to install-gtex.sh, defaulting to system.
#
# If you need something more complex, you probably know more than we do, so run
# the commands manually.

function die {
	echo "Failed to $1.  Did you forget to run as root?"
	exit 1
}

echo "Installing Gregorio"
make install || die "install Gregorio"
echo

echo "Installing GregorioTeX"
./install-gtex.sh "${1:-system}" || die "install GregorioTeX"
echo

echo "Installation complete."

exit 0
