/**
 * Translate game texts

 * Copyright (C) 2008  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"

#include "paths.h"
#include "str_util.h"

/**
 * Copy a translation for 'latin1_source' in 'utf8_dest' or, if there
 * isn't, convert 'latin1_source' to UTF-8. 'utf8_dest' will always be
 * NULL-terminated, and won't be longer than max_size bytes (including
 * trailing '\0').
 * 
 * The reason the text is always converted to UTF-8 is because Dink
 * (freeware version) was used in European countries where some
 * non-ASCII characters were used (e.g. 0xB4 or "single acute accent"
 * which is present in Finnish keyboards and used instead of the more
 * common single quote "'" - check the Milderrr! series for instance).
 */
void i18n_translate(char* text, int buf_size)
{
  /* Don't translate the empty string, which has a special meaning for
     gettext */
  if (strlen(text) == 0)
    return;

  char* latin1_source = strdup(text);
  char* utf8_dest = text;

  char* translated_text = dgettext(paths_getdmodname(), latin1_source);
  if (translated_text == latin1_source)
    {
      /* No translation, let's manually convert from Latin-1 to UTF-8,
	 so that 'TTF_RenderUTF8_Solid' can parse it correctly. */
      latin1_to_utf8(latin1_source, utf8_dest, buf_size);
    }
  else
    {
      /* Copy the translation */
      /* TODO: 'max_size' chars max for now */
      strncpy(utf8_dest, translated_text, buf_size-1);
      utf8_dest[buf_size-1] = '\0';
    }
  free(latin1_source);
}
