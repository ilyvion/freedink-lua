#!/usr/bin/env perl
# Tag translatable strings in DinkC story files
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


# This script alters a set of DinkC files to make them look like C, so
# that 'xgettext' can extract translatable strings from them.
# It attempts to reproduces the behavior of the DinkC parser.

use strict;
foreach my $file (@ARGV) {
    open(FILE, "<$file");
    open(TMP, '>tmp.tag');
    while (my $line = <FILE>) {
	if ($line =~ /^ *choice_start[ (]/i) {
	    print TMP $line;
	    # dialog mode
	    while (my $lined = <FILE>) {
		if ($lined =~ /^ *title_start\(/i) {
		    print TMP $lined;
		    # dialog title - I attempted merging all lines,
		    # but the Dink engine is pretty messy there,
		    # replaces \r\n by \n\n, etc., making it quite
		    # difficult to match the msgid, so I falled back
		    # to line-by-line msgids.
		    my $title = '';
		    while (my $linet = <FILE>) {
			if ($linet =~ /^ *title_end\(/i) {
			    print TMP $linet;
			    last;
			} elsif ($linet =~ /./) {
			    chomp($linet);
			    $linet =~ s/"/\\"/g;
			    print TMP '_("' . $linet . '");' . "\n";
			} else {
			    print TMP $linet;
			}
		    }
		} elsif (
		    $lined =~ /^ *set_y( |$)/i or
		    $lined =~ /^ *set_title_color( |$)/i or
		    $lined =~ /^ *$/) {
		    # non translatable dialog directives
		    print TMP $lined;
		} elsif ($lined =~ /^ *choice_end\(/i) {
		    print TMP $lined;
		    last;
		} else {
		    # Optional conditional statementS followed by a
		    # quoted string - but skip &savegameinfo lines
		    if ($lined !~ /"&savegameinfo"/i) {
			my $nb_repl = ($lined =~ s/^ *(\([^)]*\) *)*("[^"]*"?)/_($2);/);
			if ($nb_repl == 0)
			{
			    # Probably a parser problem
			    chomp($lined);
			    print STDERR "Unexpected line during choice_start...choice_end: [$lined]\n";
			}
		    }
		    print TMP $lined;
		}
	    }
	} else {
	    $line =~ s/(.*say[a-z_]*\()"?(`.)?(.+)",(.*)$/$1_("$3"),$4/i;
	    print TMP $line;
	}
    }
    close(FILE);
    close(TMP);
    rename('tmp.tag', $file);
}
