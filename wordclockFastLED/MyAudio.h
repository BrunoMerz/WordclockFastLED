/**
   MyAudio.h
   @autor    Bruno Merz

   @version  1.0
   @created  06.03.2021

   Play Audiofile

*/

#pragma once


#ifdef WITH_AUDIO

#include "AudioFileSourceLittleFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"


class MyAudio {
  public:
    MyAudio(void);
    void play(String mp3file);
    void check(void);

  private:

    AudioGeneratorMP3 *mp3;
    AudioFileSourceLittleFS *file;
    AudioOutputI2S *out;
    AudioFileSourceID3 *id3;

};

#endif
