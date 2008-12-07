# DOS2UNIX
find -name "*.[cC]" | xargs sed -i -e 's/\r//'
# say... commands, possibly with a missing leading '"'
find -name "*.[cC]" | xargs sed -i -e 's/\(.*say[a-zA-Z_]*(\)"\?\(`.\)\?\(.*\)",\(.*\)$/\1_("\3"),\4/i'
# dialog choices (on a line by themselves)
find -name "*.[cC]" | xargs sed -i -e 's/^[[:space:]]*\(".*"\)[[:space:]]*$/_(\1)/'
# dialog titles (between title_start() and title_end()
find -name "*.[cC]" | xargs sed -i -e '/title_start()/,/title_end()/s/^[[:space:]]*\(.\+\)[[:space:]]*$/_("\1")/'
# dirty fix "title_start()" and "title_end()" translations
find -name "*.[cC]" | xargs sed -i -e 's/_("title_start().*//' -e 's/_("title_end().*/title_end()/'
# extract matched strings
find -name "*.[cC]" | xargs xgettext -k_ -o island.pot
