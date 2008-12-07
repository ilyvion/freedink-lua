#!/bin/bash
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
	diff -ur islandstory islandstoryfr/ \
	    | grep -E '^(\+|-)[^+-]' \
	    | grep -vE '^(\+|-)[[:space:]]*//' \
	    | grep -vE '^(\+|-)[[:space:]]*$' \
	    | sed 's/\r//' \
	    | iconv -f latin1 -t utf-8 \
	    | grep -i -E 'say[a-zA-Z_]+\(' \
	    | while read saycall; do
	    str=$(  echo -n "$saycall" \
		| sed -e 's/.*say[a-zA-Z_]*("\(.*\)".*/\1/i' -e 's/^`.//'  );
	    echo $saycall | grep '^-' > /dev/null
	    if [ $? = 0 ]; then
		echo msgid \""$msgid"\"
		echo msgstr \""$trans"\"
		echo 
		msgid=$str; trans="";
	    else
		if [ -z "$trans" ]
		then
		    trans="$str"
		else
		    trans="$trans $str"
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
