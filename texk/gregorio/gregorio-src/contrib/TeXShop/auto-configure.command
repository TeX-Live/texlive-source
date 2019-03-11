#!/usr/bin/env bash

# This script is designed to automatically configure a TeXShop distribution.
# You should be able to direct it to run by double clicking on it.

#This trap combination allows the window to linger long enough for the user to
#inspect the output, but still get closed when all is said and done.
function quit {
    read -n1 -r -p "Press any key to close window." key
    osascript -e 'tell application "Terminal" to close front window' > /dev/null 2>&1 &
}

trap quit EXIT


#Copy the engine file from its instalation directory to the TeXShop Engines directory
ENGINEDIR="$HOME/Library/TeXShop/Engines"
if [ ! -d "$ENGINEDIR" ]; then
    echo "Cannot Find TeXShop configuration directory!"
    echo "Please open and close TeXShop and try running this script again."
    exit 1
fi
SOURCE="/Users/Shared/Gregorio/contrib/TeXShop/LuaLaTeX+se.engine"
if [ ! -e "$SOURCE" ]; then
    SOURCE="$PWD/LuaLaTeX+se.engine"
    if [ ! -e "$SOURCE" ]; then
        DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
        SOURCE="$DIR/LuaLaTeX+se.engine"
    else
        echo "Cannot find LuaLaTeX+se.engine"
        echo "Please try running the Gregorio intaller again"
        exit 1
    fi
fi
echo "Copying LuaLaTeX+se.engine into TeXShop configuration"
cp "$SOURCE" "$ENGINEDIR"

SOURCE="/Users/Shared/Gregorio/contrib/TeXShop/LuaTeX+se.engine"
if [ ! -e "$SOURCE" ]; then
    SOURCE="$PWD/LuaTeX+se.engine"
    if [ ! -e "$SOURCE" ]; then
        DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
        SOURCE="$DIR/LuaTeX+se.engine"
    else
        echo "Cannot find LuaTeX+se.engine"
        echo "Please try running the Gregorio intaller again"
        exit 1
    fi
fi
echo "Copying LuaTeX+se.engine into TeXShop configuration"
cp "$SOURCE" "$ENGINEDIR"

#double check the execution bits
chmod +x "$ENGINEDIR/LuaLaTeX+se.engine"
chmod +x "$ENGINEDIR/LuaTeX+se.engine"

echo "Configuration complete"
exit 0
