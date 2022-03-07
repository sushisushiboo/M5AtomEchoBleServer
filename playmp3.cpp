#include "playmp3.h"
#include <M5Atom.h>
#include "FS.h"
#include "SPIFFS.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#define CONFIG_I2S_BCK_PIN      19
#define CONFIG_I2S_LRCK_PIN     33
#define CONFIG_I2S_DATA_PIN     22

playmp3::playmp3()
{
  SPIFFS.begin();
  Serial.printf("MP3 playback begins...\n");
  audioLogger = &Serial;
  out = new AudioOutputI2S();
  out->SetPinout(CONFIG_I2S_BCK_PIN, CONFIG_I2S_LRCK_PIN, CONFIG_I2S_DATA_PIN);
  out->SetGain(1.5);
  mp3 = new AudioGeneratorMP3();

  getMP3FileList();

}

bool playmp3::play(int fileNo)
{
  if (this->mp3->isRunning())
  {
    Serial.println("still playing");
    return false;
  }

  if (fileNo < this->fileCount)
  {
    Serial.print("Play:");
    Serial.print(this->fileName[fileNo]);
    this->file = new AudioFileSourceSPIFFS(this->fileName[fileNo]);
    this->id3 = new AudioFileSourceID3(file);
    mp3->begin(this->id3, this->out);
    return true;
  } else {
    Serial.println("invalid fileNo");
    return false;
  }
}

void playmp3::getMP3FileList()
{
  File root = SPIFFS.open("/");
  if (!root) 
  {
    Serial.println("failed open root");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("root is not directory");
    return;
  }
  File file = root.openNextFile();
  while(file)
  {
    if (!file.isDirectory())
    {
      sprintf(fileName[fileCount], file.name());
      int len = strlen(fileName[fileCount]);
      Serial.println(fileName[fileCount]);
      if (0 == strcmp(fileName[fileCount] + len - 4, ".mp3"))
      {
        Serial.println("mp3!");
        fileCount ++;
        if (16 <= fileCount)
        {
          Serial.println("file count to max");
          break;
        }
      }
    }
    file = root.openNextFile();
  }
  return;
}

bool playmp3::isPlaying()
{
  return this->mp3->isRunning();
}

void playmp3::loop()
{
  if (this->mp3->isRunning())
  {
    if (!this->mp3->loop())
    {
      this->mp3->stop();
    }
  }
}
