class AudioGeneratorMP3;
class AudioFileSourceID3;
class AudioFileSourceSPIFFS;
class AudioOutputI2S;
class playmp3 {
public:
    playmp3();
    
    bool play(int fileNo);
    bool isPlaying();
    void loop();
    int getFileCount();

private:
    void getMP3FileList();

    AudioGeneratorMP3 *mp3;
    AudioFileSourceID3 *id3;
    AudioFileSourceSPIFFS *file;
    AudioOutputI2S *out;

    char fileName[16][32];
    int fileCount = 0;
    int filePos = 0;
};
