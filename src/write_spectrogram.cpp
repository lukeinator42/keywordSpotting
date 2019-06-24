#include <iostream>
#include <fstream>
#include <keywordSpotter/lib/EmbeddingNetwork.h>
#include <sndfile.h>
#include <keywordSpotter/lib/utils/WavUtils.h>
#include <keywordSpotter/lib/sound/Sound.h>
#include <keywordSpotter/lib/features/Spectrogram.h>

using namespace std;

int main(int argc, char ** argv) {

    //cout << e.getMsg() << endl;
    
    if (argc != 3) {
        printf("\nUsage :\n\n    <executable name>  <input file>\n") ;
        exit(0);
    }

    string inpath(argv[1]);

    string outpath(argv[2]);

    WavUtils wavUtils;

    Sound buffer;

    int nFFT = 128;
    int winLen = 400;
    int hopSize = 160;
    double minFreq = 0;
    double maxFreq = 8000;
    double sampleRate = 16000;
    bool logScale = true;

    Spectrogram sp(nFFT, winLen, hopSize, minFreq, maxFreq, sampleRate, logScale);

    // cout << buffer.size() << endl;

    wavUtils.readFile(inpath, buffer);

    // cout << buffer.size() << endl;

    SpectrogramResult s = sp.computeSpectrogram(buffer.getAudio());


    ofstream fout;
    fout.open(outpath);

    for(int i=0; i<s.size(); i++) {
        for(int j=0; j<s[i].size(); j++)
            fout << s[i][j] << " ";
        fout << endl;
    }

    fout.close();
    
    return 0;
}