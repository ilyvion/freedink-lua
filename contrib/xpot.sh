# /bin/bash
# Extract strings from DinkC story files
# 
# Copyright (C) 2008 Sylvain Beucler
# 
# This file is part of GNU FreeDink
# 
# GNU FreeDink is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# GNU FreeDink is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.


# dos2unix
find -name "*.[cC]" -print0 | xargs -0 sed -i -e 's/\r//'

# Tag strings
find -name "*.[cC]" -print0 | xargs -0 `dirname $0`/xpot-tag.pl

# extract marked strings
find -name "*.[cC]" -print0 | xargs -0 xgettext -k_ -o island.pot
