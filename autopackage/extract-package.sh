#!/bin/bash
# Inspect the content of an autopackage file

# Copyright (C) 2007  Sylvain Beucler

# This file is part of GNU FreeDink

# GNU FreeDink is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# GNU FreeDink is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.

package=$1

skip_lines=0
while read line; do
    case $line in
        '## END OF HEADER')
            break
            ;;
        skip_lines=* | skipLines=*)
            skip_lines=`echo $line | grep -o '[0-9]\+'`
            ;;
        'export meta_size='* | 'export metaSize='*)
            meta_size=`echo $line | grep -o '[0-9]\+'`
            ;;
    esac
done < "$package"

echo skip_lines=$skip_lines 2>&1
echo meta_size=$meta_size 2>&1
type=$(tail -n +$skip_lines "$package" | tail -c +$(($meta_size+1)) | head -c 100 | file - | cut -d' ' -f2)

case $type in
    bzip2)
	ext=bz2
	;;
    gzip)
	ext=gz
	;;
    *)
	# file doesn't detect it yet
	ext=lzma
	;;
esac
tail -n +$skip_lines "$package" | head -c $meta_size > package-metadata.tar.gz
tail -n +$skip_lines "$package" | tail -c +$(($meta_size+1)) > package.tar.$ext
