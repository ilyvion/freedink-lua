#ifndef _BGM_H
#define _BGM_H

extern int last_cd_track;

extern int cdplaying();
extern int killcd();
extern int PlayCD(int cd_track);
extern int something_playing(void);
extern int PlayMidi(char *sFileName);
extern int PauseMidi();
extern int ResumeMidi();
extern int StopMidi();
extern int ReplayMidi();
extern void check_midi();
extern void bgm_init(void);
extern void bgm_quit(void);
extern void loopmidi(int loop_midi);

#endif
