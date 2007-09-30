#!/bin/bash
# Copyright (C) 2006  Sylvain Beucler

# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

# Updates config.guess and config.sub in the target directory only if
# the autotools-dev version is newer

# http://bugs.debian.org/380481

dest="$1"
# strip trailing slash for nicer output
dest=$(echo $dest | sed 's:/$::')

if [ -z $dest ]; then
    echo "Usage: $0 destination"
    exit 1;
fi

for ext in sub guess; do
    # --version output looks like "GNU config.sub (2006-02-23)"
    v_autotools=$(/usr/share/misc/config.$ext --version | head -1 | grep -o -E '[0-9]{4}-[0-9]{2}-[0-9]{2}')
    v_upstream=$($dest/config.$ext --version | head -1 | grep -o -E '[0-9]{4}-[0-9]{2}-[0-9]{2}')
    if dpkg --compare-versions "$v_autotools" '>>' "$v_upstream"; then
	echo "Updating $dest/config.$ext"
	cp -f /usr/share/misc/config.$ext $dest
    fi
done
