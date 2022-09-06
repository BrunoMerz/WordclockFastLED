/**
   MyAudio.cpp
   @autor    Bruno Merz

   @version  1.0
   @created  06.03.2021

*/

//#define myDEBUG
#include "Debug.h"

#include "Configuration.h"

#ifdef WITH_AUDIO

#include "MyAudio.h"

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  DEBUG_PRINTF("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    DEBUG_PRINTF("%c", a);
  }
  DEBUG_PRINTF("'\n");
  DEBUG_FLUSH();
}

MyAudio::MyAudio(void) {
  audioLogger = &Serial;
  //bool SetPinout(int bclkPin, int wclkPin, int doutPin);
  file = new AudioFileSourceLittleFS();
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();
}

void MyAudio::play(String mp3file) {
  String fn = mp3file;
  if(!fn.startsWith("/"))
    fn = "/" + fn;
  DEBUG_PRINTLN("Audio play: "+fn);
  file->open(fn.c_str());
  mp3->begin(id3, out);
  out->SetGain(0.1);
}

void MyAudio::check(void) {
  bool runstatus = mp3->isRunning();
  if (runstatus) {
    DEBUG_PRINTLN("Audio check: " + (String)runstatus);
    if (!mp3->loop()) {
      mp3->stop();          // out->stop();  id3->close() -> file->close();
      DEBUG_PRINTLN("Audio stopped");
    }
  }
}

#endif
