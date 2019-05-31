#include <keywordSpotter/features/Spectrogram.h>

Spectrogram::Spectrogram(int nFFT, int windowLen, int hopSize, int minFreq, int maxFreq, bool logScale)
: nFFT(nFFT), windowLen(windowLen), hopSize(hopSize), minFreq(minFreq), maxFreq(maxFreq), logScale(logScale) {

}

Spectrogram::~Spectrogram() {
    
}

