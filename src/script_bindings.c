/**
 * Dink Script Bindings

 * Copyright (C) 2013  Alexander Krivács Schrøder

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

#include "game_engine.h"
#include "log.h"
#include "sfx.h"
#include "dinkvar.h"
#include "bgm.h"

void scripting_load_sound(const char *wav_file, int sound_index)
{
  if (sound_on)
  {
    log_info("getting %s..", wav_file);
    CreateBufferFromWaveFile(wav_file, sound_index);
  }
}

int scripting_sp_script(int sprite, const char *script)
{
  // (sprite, direction, until, nohard);
  if (sprite <= 0 || (sprite >= MAX_SPRITES_AT_ONCE && sprite != 1000))
  {
    sprintf(scripting_error, "cannot process sprite %d", sprite);
    return -1;
  }
  scripting_kill_scripts_owned_by(sprite);
  if (scripting_load_script(script, sprite, /*true*/1) == 0)
  {
    return 0;
  }

  int tempreturn = 0;
  if (sprite != 1000)
  {
    if (no_running_main == /*true*/1)
      log_info("Not running %s until later..", sinfo[spr[sprite].script]->name);
    else if (no_running_main == /*false*/0)
    {
      scripting_run_proc(spr[sprite].script, "MAIN");
      tempreturn = spr[sprite].script;
    }
  }
    
  return tempreturn;
}

int scripting_playmidi(const char* midi_file)
{
  //StopMidi();
  int regm = atol(midi_file);
  log_debug("Processing playmidi command.");
  if (regm > 1000)
  //cd directive
  {
    int cd_track = regm - 1000;
    log_info("playmidi - cd play command detected.");

    if (cd_inserted)
    {
      if (cd_track == last_cd_track
          && cdplaying())
      {
        return 1;
      }

      log_info("Playing CD track %d.", cd_track);
      if (PlayCD(cd_track) >= 0)
        return 0;
    }
    else
    {
      //cd isn't instered, can't play CD song!!!
      char buf[10+4+1];
      sprintf(buf, "%d.mid", cd_track);
      log_info("Playing midi %s.", buf);
      PlayMidi(buf);
      // then try to play 'midi_file' as well:
      // (necessary for START.c:playmidi("1003.mid"))
    }
  }
  log_info("Playing midi %s.", midi_file);
  PlayMidi(midi_file);
  return 0;
}