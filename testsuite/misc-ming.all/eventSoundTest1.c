/* 
 *   Copyright (C) 2007, 2009 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */ 

/*
 * Test for sounds being started and stopped
 * 
 * The _root movie has 20 frames. 
 *
 * In uneven frames a sound is started, and in even frames it is stopped.
 */


#include <stdlib.h>
#include <stdio.h>
#include <ming.h>

#include "ming_utils.h"

#define OUTPUT_VERSION 6
#define OUTPUT_FILENAME "eventSoundTest1.swf"

void setupMovie(SWFMovie mo, const char* srcdir);
SWFSound setupSounds(const char* filename);
void runMultipleSoundsTest(SWFMovie mo, SWFSound so, int* frame);
void runNoMultipleSoundsTest(SWFMovie mo, SWFSound so, int* frame);
void pauseForNextTest(SWFMovie mo);
void printFrameInfo(SWFMovie mo, int i, const char* desc);

void pauseForNextTest(SWFMovie mo)
{
  add_actions(mo, "_root.onMouseDown = function() {"
                  "play(); Mouse.removeListener(_root); };"
                  "Mouse.addListener(_root);"
                  );
  add_actions(mo, "note('Click and "
                  "wait for the test.');"
		  "testReady = true; stop();");
}

void setupMovie(SWFMovie mo, const char* srcdir)
{
  SWFDisplayItem it;
  char fdbfont[256];
  SWFFont font;
  SWFMovieClip dejagnuclip;
  FILE* font_file;
  
  sprintf(fdbfont, "%s/Bitstream-Vera-Sans.fdb", srcdir);
  font_file = fopen(fdbfont, "r");
  if (font_file == NULL)
  {
    perror(fdbfont);
    exit(1);
  }

  font = loadSWFFontFromFile(font_file);


  /* Add output textfield and DejaGnu stuff */
  dejagnuclip = get_dejagnu_clip((SWFBlock)font, 10, 0, 0, 800, 400);
  it = SWFMovie_add(mo, (SWFBlock)dejagnuclip);
  SWFDisplayItem_setDepth(it, 200); 
  //SWFDisplayItem_move(it, 0, 0); 

}

SWFSound setupSounds(const char* filename)
{
  FILE *soundfile;
  SWFSound so;
  
  printf("Opening sound file: %s\n", filename);

  soundfile = fopen(filename, "r");

  if (!soundfile) {
    perror(filename);
    exit(EXIT_FAILURE);
  }

  so = newSWFSound(soundfile,
     SWF_SOUND_NOT_COMPRESSED |
     SWF_SOUND_22KHZ |
     SWF_SOUND_16BITS |
     SWF_SOUND_STEREO);
     
  return so;
}


void
printFrameInfo(SWFMovie mo, int i, const char* desc)
{
    char descBuf[200];
    char frameBuf[50];

    sprintf(descBuf, "note('%s');", desc);
    sprintf(frameBuf, "note('Frame: ' + %d);", i);

    /* Display frame number and description
    at the beginning of each frame */
    add_actions(mo, frameBuf);
    add_actions(mo, descBuf);
}

void
runMultipleSoundsTest(SWFMovie mo, SWFSound so, int* frame)
{
    const char* frameDesc[5];
    int i;

    SWFMovie_nextFrame(mo);
    add_actions(mo,
              "note('Multiple Sound Test.\n"
              "The notes should start exactly at the beginning of a frame "
              "(to coincide with the appearance of the description text).\n"
              "Test should start in two seconds.');");

    /* This is what is supposed to happen in each frame */
    frameDesc[0] = "Two notes (C, E)";
    frameDesc[1] = "Two notes (G-C, E)";
    frameDesc[2] = "Two notes (G-C, E)";
    frameDesc[3] = "Two notes (G-C, E)";
    frameDesc[4] = "Nothing";

    for (i = 0; i < 4; i++)
    {
        SWFMovie_nextFrame(mo);

        (*frame)++;

        printFrameInfo(mo, i, frameDesc[i]);

        SWFMovie_startSound(mo, so);
    }

    SWFMovie_nextFrame(mo);

    printFrameInfo(mo, i, frameDesc[i]);
    SWFMovie_stopSound(mo, so);

}


void
runNoMultipleSoundsTest(SWFMovie mo, SWFSound so, int* frame)
{
  const char* frameDesc[5];
  int i;

  SWFMovie_nextFrame(mo);
  add_actions(mo, "note('Non-multiple Sound Test\n"
              "The notes should start exactly at the beginning "
              "of a frame (to coincide with the appearance of the description "
              "text). Test should start in two seconds.');");
            

  /* This is what is supposed to happen in each frame */
  frameDesc[0] = "Two notes (C, E)";
  frameDesc[1] = "One note (G)";
  frameDesc[2] = "Two notes (C, E) ";
  frameDesc[3] = "One note (G)";
  frameDesc[4] = "Nothing";


    for (i = 0; i < 4; i++)
    {
        SWFMovie_nextFrame(mo);

        (*frame)++;

        printFrameInfo(mo, i, frameDesc[i]);

        SWFSoundInstance so_in = SWFMovie_startSound(mo, so);
        SWFSoundInstance_setNoMultiple(so_in);
    }

    SWFMovie_nextFrame(mo);

    printFrameInfo(mo, i, frameDesc[i]);
    SWFMovie_stopSound(mo, so);

}


int
main(int argc, char** argv)
{
  SWFMovie mo;
  SWFSound so;
  const char* soundFile;
  const char* srcdir;
  int frame;

  if (argc > 1) {
    soundFile = argv[1];
  }
  else {
    soundFile = "brokenchord.wav";
  }
  if (argc > 2) {
    srcdir = argv[2];
  }
  else {
    srcdir = ".";
  }

  /* setup ming and basic movie properties */
  Ming_init();  
  Ming_useSWFVersion(OUTPUT_VERSION);

  mo = newSWFMovie();
  SWFMovie_setDimension(mo, 800, 600);
  SWFMovie_setRate(mo, 0.5);

  setupMovie(mo, srcdir);
  so = setupSounds(soundFile);

  add_actions(mo, "c = 0;");

  SWFMovie_nextFrame(mo);

  add_actions(mo,
          "note('You will hear several short tests with a succession of sounds.\n"
		  "Each frame is two seconds long.\n"
          "The movie will describe what you should hear at the beginning of the frame.');"
		  );
		  
  frame = 0;

  pauseForNextTest(mo);
  runMultipleSoundsTest(mo, so, &frame);
  
  pauseForNextTest(mo);
  runNoMultipleSoundsTest(mo, so, &frame);

  pauseForNextTest(mo);
  //Output movie
  puts("Saving " OUTPUT_FILENAME );
  SWFMovie_save(mo, OUTPUT_FILENAME);

  return 0;
}
