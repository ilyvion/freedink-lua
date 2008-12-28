#!/bin/bash
# Dirty-convert a hard-coded translation to a proper external .po file
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

(
    echo 'msgid ""'
    echo 'msgstr ""'
    echo '"Project-Id-Version: dmod 1.0\n"'
    echo '"POT-Creation-Date: 2008-12-07 15:19+0100\n"'
    echo '"PO-Revision-Date: 2008-12-07 15:18+0100\n"'
    echo '"Last-Translator: you <email@domain.tld>\n"'
    echo '"Language-Team: XX <xx@li.org>\n"'
    echo '"MIME-Version: 1.0\n"'
    echo '"Content-Type: text/plain; charset=UTF-8\n"'
    echo '"Content-Transfer-Encoding: 8bit\n"'
    echo
    
    (
	a=0
	b=0
	i=0
	j=0
	diff -bwBur islandstory islandstoryfr/ \
	    | grep -E '^(\+|-)[^+-]' \
	    | grep -vE '//' \
	    | grep -vE '^(\+|-)[[:space:]]*$' \
	    | grep -ivE 'debug\("' \
	    | sed 's/\r//' \
	    | iconv -f latin1 -t utf-8 \
	    | while read line; do
	    if echo "$line" | grep -i -E 'say[a-zA-Z_]*\(' > /dev/null; then
		if [ -n "$msgids" ]; then
		    k=0
		    while [ $k -lt $i ]; do
			echo "${msgids[$k]}"
			echo "${msgstrs[$k]}"
			echo
			((k++))
		    done
		    i=0
		    j=0
		fi
		str=$( echo -n "$line" \
		    | sed -e 's/.*say[a-zA-Z_]*("\?\(.*\)".*/\1/i' -e 's/^`.//' );
		echo "$line" | grep '^-' > /dev/null
		if [ $? = 0 ]; then # "-"
		    k=0
		    if [ $b -gt 0 ]; then
			while [ $k -lt $a ]; do
			    echo "${smsgids[k]}"
			    echo "msgstr \"${smsgstrs[k]}\""
			    echo 
			    ((k++))
			done
			a=0
			b=0
		    fi
		    smsgids[a]="msgid \"$str\""
		    ((a++))
		else # "+"
		    if [ $b -eq $a ]; then
			smsgstrs[b-1]="${smsgstrs[a]} $str"
		    else
			smsgstrs[b]="$str"
			((b++))
		    fi
		fi
	    else
		# dialogs
		str=$(echo "$line" | sed -e 's/^.//' -e 's/^[[:space:]]*\(.*\)[[:space:]]*$/\1/' \
		                         -e 's/^([^"]*)[[:space:]]*"/"/' -e 's/^[^"].*$/"&"/')
		if echo "$line" | grep '^-' > /dev/null; then
		    msgids[i]="msgid $str"
		    ((i++))
		else
		    msgstrs[j]="msgstr $str"
		    ((j++))
		fi
	    fi
	done
    )  | tail -n+3  # skip first empty entry
) > t.po

# Remove duplicates
msguniq t.po > t2.po

msgmerge t2.po island.pot > fr.po
msgfmt -c --statistics fr.po

exit


TODO: dialog choices:

  choice_start();
  title_start();

  My title
  title_end();
  "Freak out"
  "Stay calm"
  choice_end();


Sample .po:

  msgid "Error: Invalid .dmod file selected!"
  msgstr "Erreur: fichier .dmod sélectionné invalide!"
