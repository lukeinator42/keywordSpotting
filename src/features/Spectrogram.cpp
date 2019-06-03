#include <keywordSpotter/features/Spectrogram.h>
#include <cmath>

Spectrogram::Spectrogram(int nFFT, int windowLen, int hopSize, double minFreq, double maxFreq, int sampleRate, bool logScale)
: nFFT(nFFT), windowLen(windowLen), hopSize(hopSize), minFreq(minFreq), maxFreq(maxFreq), sampleRate(sampleRate), logScale(logScale) {
    initFFTCenterBins();
}

Spectrogram::~Spectrogram() {
    
}

double Spectrogram::hzToMel(double f) {
    return 2595*std::log10(1+(f/700));
}

double Spectrogram::melToHz(double mel) {
    return 700*(pow(10, mel/2595)-1);
}

void Spectrogram::hann(double data[], int winLen) {
    for (int i = 0; i < winLen; i++) {
        double multiplier = 0.5 * (1 - std::cos(2*(this->PI)*i/(winLen-1)));
        data[i] *= multiplier;
    }
}

void Spectrogram::initFFTCenterBins() {
    double minMel = hzToMel(this->minFreq);
    double maxMel = hzToMel(this->maxFreq);

    for(int i=0; i<this->nFFT+2; i++) {
        double mel = minMel + (i*1.0/(this->nFFT+1))*(maxMel-minMel);
        double freq = melToHz(mel);
        int bin = std::floor(((this->windowLen)+1)*freq/this->sampleRate);
        
        this->fftCenterBins.push_back(bin);
    }
}

