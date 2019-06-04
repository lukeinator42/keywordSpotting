#include <iostream>
#include <keywordSpotter/EmbeddingNetwork.h>
#include <sndfile.h>
#include <keywordSpotter/utils/WavUtils.h>
#include <keywordSpotter/sound/Sound.h>
#include <keywordSpotter/features/Spectrogram.h>

using namespace std;

int main(int argc, char ** argv) {

    EmbeddingNetwork e("Hello World!");

    //cout << e.getMsg() << endl;
    
    if (argc != 2) {
        printf("\nUsage :\n\n    <executable name>  <input file>\n") ;
        exit(0);
    }

    string path(argv[1]);

    WavUtils wavUtils;

    Sound buffer;

    int nFFT = 128;
    int winLen = 400;
    int hopSize = 160;
    double minFreq = 50;
    double maxFreq = 8000;
    double sampleRate = 16000;
    bool logScale = false;

    Spectrogram spectrogram(nFFT, winLen, hopSize, minFreq, maxFreq, sampleRate, logScale);

    // cout << buffer.size() << endl;

    wavUtils.readFile(path, buffer);

    // cout << buffer.size() << endl;

    vector< vector<double> > s = spectrogram.computeSpectrogram(buffer.getAudio());

    for(int i=0; i<s.size(); i++) {
        for(int j=0; j<s[i].size(); j++)
            cout << s[i][j] << " ";
        cout << endl;
    }
    
    return 0;
}