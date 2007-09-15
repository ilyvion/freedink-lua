#ifndef _BGM_H
#define _BGM_H

extern int last_cd_track;

int cdplaying();
int killcd();
int PlayCD(int cd_track);
int something_playing(void);
int playMIDIFile(char *midi_filename);
int PlayMidi(char *sFileName);
int PauseMidi();
int ResumeMidi();
int StopMidi();
int ReplayMidi();
void check_midi();

#endif
