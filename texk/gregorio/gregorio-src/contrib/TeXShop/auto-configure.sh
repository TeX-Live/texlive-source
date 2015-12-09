#!/usr/bin/env bash

# This script is designed to automatically configure a TeXShop distribution.

ENGINEDIR="$HOME/Library/TeXShop/Engines"
if [ ! -d "$ENGINEDIR" ]; then
    mkdir -p "$ENGINEDIR"
fi
SOURCE="/Users/Shared/Gregorio/contrib/TeXShop/LuaLaTeX+se.engine"
if [ -e "$SOURCE" ]; then
    cp "$SOURCE" "$ENGINEDIR"
else
    echo "Cannot find LuaLaTeX+se.engine"
    echo "Please try running the Gregorio intaller again"
    exit 1
fi
defaults write TeXShop OtherTeXExtensions -array-add "gabc"
