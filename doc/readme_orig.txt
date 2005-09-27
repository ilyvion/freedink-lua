Greetings fellow game programmers!

This is the Dink Smallwood source code, a pretty decent RPG/Adventure game basically
untouched since it was created in 1997 except for a few bugfixes. 

I always planned to go through and renovate it myself, but as time goes on, this seems
less and less likely.  I would just rather re-write then fix-up, especially when the project
was my first ever windows game.

After seeing the 250 KB .h file you are probably about to poke out your
eyes and bathe in lye to cleanse yourself.

Before doing so, let me explain a few things about this source code
as I vaguely remember it.

* dinkvar.h is shared between the editor and the main game.
* compress.c doesn't have a project, but it is the source for the util
  that makes .d files from .c files.
* ffcreate is the util that packs a dirs contents into one giant file.
* If you change #define __ENGLISH to #define __GERMAN the game text in the .EXE will
  be in german.
* Amazing and sad fact: There is not a single instance of a float, or sin/cos in this source.


*** HOW TO COMPILE AND TEST ***

Use Microsoft Visual C++ 6 and open the dink.dsw.  This houses all the projects.

It probably works with VC7, but it hasn't been tested.

Getting this to compile should be very easy, just make sure your DirectX paths
are added to your Tools->Options->Directories.  That's pretty much all you need. Next
you'll want to check the output paths in the project setting so you can find the
.exe's that are built.

To actually test the .exe, install Dink Smallwood v1.06 and apply the 1.07 beta patch,
it's about a 24 MB file you can get from my site. (it's freeware)  Just replace the
dink.exe with the one you've compiled.

*** GETTING HELP ***

This source is provided "as is" and unfortunately (well, fortunately for me) I won't
be able to offer technical support on using it.  If you have a question I recommend
posting on my forums somewhere, myself or someone else will most likely help out if
we can.  This is a much better use of my time then answering 1-on-1 emails as the
information is available to all.

*** ABOUT THE DINK SOURCE ***

I'm sure you'll be overjoyed to know that I have chosen to keep the historic value of this
source intact by choosing NOT to go through and fix the hundreds of compiler warnings,
remove the 39 instances of the word "shit" in the code or make the code even slightly
understandable.	 (as if I could - It's too late for that now...)

If you are thinking about using this source for your own RPG, you are welcome to it,
this is a very open license and if you think it's easier to fix up this code, add high color,
windowed mode and whatever else than simply starting from scratch, I will humbly disagree
but wish you luck.

Donations:  If you find this source helpful or it had you LOL'ing for weeks at its magical
horribleness you are gently asked to donate a couple bucks via codedojo.com.  This cash
will be used to maintain the site and hopefully inspire me to get off my duff and release
more (hopefully better) source.

Also, if you could drop me a line letting me know what software you used it in so I can keep a
list, that would be cool.

Special thanks to the guys who helped make Dink (Justin Martin, Shawn Teal and Greg Smith)
and the guys over at the Dink Network for keeping the Dink community active and helping people
get files and help.  New DMOD's are still being made, it's a pretty amazing.

-Seth A. Robinson (seth@rtsoft.com)

Useful links:

www.rtsoft.com <-- my company, you can find screenshots and a link to download Dink here.
www.codedojo.com <-- my programming/free source site
www.dinksmallwood.net <-- the Dink Network (not run by me)