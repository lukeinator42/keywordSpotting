#include <keywordSpotter/features/Spectrogram.h>

Spectrogram::Spectrogram(int nFFT, int windowLen, int hopSize, bool logScale)
: nFFT(nFFT), windowLen(windowLen), hopSize(hopSize), logScale(logScale) {

}

Spectrogram::~Spectrogram() {
    
}

