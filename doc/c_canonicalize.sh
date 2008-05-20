#!/bin/bash
# Canonical C source transformation for easy diffing
# Copyright (C) 2008  Sylvain Beucler
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

# Replace all new lines by a single space, and indent the result to
# get a "canonical" form. Some newlines are significant: preprocessor
# directives and '//' comments; for those, we first replace newlines
# by \r and convert them back to \n after all spaces are squeezed.
# 
# A comment can be on a line by itself (documents the code below) or
# placed at the right of a code line (documents code on its left). To
# avoid replacing the former by the later and introducing confusion, I
# also insert a significant newline before a comment when it's on a
# line by itself.

cat $1 | sed 's/\r//' | sed 's,\(^#.*\),\1\r,' | sed 's,^[[:space:]]*\(//.*$\),\r\1\r,' \
  | sed 's,\(//.*$\),\1\r,' \
  | tr '\t' ' ' | tr -s '\n' ' ' | tr -s ' ' ' ' | tr '\r' '\n' \
  | indent - > $1_canonical
