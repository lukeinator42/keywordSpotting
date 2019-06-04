#include <keywordSpotter/features/Spectrogram.h>
#include <keywordSpotter/features/FftRealPair.h>
#include <limits>

Spectrogram::Spectrogram(int nFFT, int windowLen, int hopSize, double minFreq, double maxFreq, int sampleRate, bool logScale)
: nFFT(nFFT), windowLen(windowLen), hopSize(hopSize), minFreq(minFreq), maxFreq(maxFreq), sampleRate(sampleRate), logScale(logScale) {
    
    initFFTCenterBins();
    initFilterbank();
}

Spectrogram::~Spectrogram() {
    
}

std::vector< std::vector<double> > Spectrogram::computeSpectrogram(std::vector<double> audio) {
    while(audio.size() % windowLen != hopSize)
        audio.push_back(0);

    std::vector< std::vector<double> > spectrogram;

    for(int i=0; i<audio.size(); i += hopSize) {
        //get subsection of audio
        std::vector<double> subvec(audio.begin()+i, audio.begin()+i+windowLen);

        //hanning window section
        hann(subvec);

        //create complex array
        std::vector<double> comp;
        comp.resize(subvec.size());

        //do fft
        Fft::transform(subvec, comp);

        //absolute val of fft
        std::vector<double> fftRes = fftAbs(subvec, comp);

        //convert to power spectral estimate
        for(int j=0; j<fftRes.size()/2; j++)
            fftRes[j] = (1.0/(fftRes.size()/2))*fftRes[j]*fftRes[j];

        //create spectrogram col
        std::vector<double> spectrogramCol;
        spectrogramCol.resize(nFFT);

        //filter based on filterbank
        for(int j=0; j<nFFT; j++) {
            spectrogramCol[j] = 0;

            for(int k=0; k < windowLen/2; k++) {
                spectrogramCol[j] += fftRes[k] * fbank[j][k];     
            }

            //log scale spectrogram
            if(logScale) {
                spectrogramCol[j] = 10.0*log10(std::max(1e-10, spectrogramCol[j]));
                spectrogramCol[j] -= 10.0*log10(1.0);
            }
        }

        double log_min = spectrogramCol[0];

        for(int j=0; j<nFFT; j++)
            log_min = std::min(log_min, spectrogramCol[i]);

        //append col to spectrogram
        spectrogram.push_back(spectrogramCol);
    }

    return spectrogram;
}

double Spectrogram::hzToMel(double f) {
    return 2595*std::log10(1+(f/700));
}

double Spectrogram::melToHz(double mel) {
    return 700*(pow(10, mel/2595)-1);
}

void Spectrogram::hann(std::vector<double>& data) {
    int winLen = data.size();

    for (int i = 0; i < winLen; i++) {
        double multiplier = 0.5 * (1 - std::cos(2*(this->PI)*i/(winLen-1)));
        data[i] *= multiplier;
    }
}

void Spectrogram::hamm(std::vector<double>& data) {
    int winLen = data.size();

    double omega = 2.0 * PI / (winLen-1);

    for (int i = 0; i < winLen; i++) {
        double multiplier = (0.54 - 0.46 * cos(omega*(i-1)));
        data[i] *= multiplier;
    }
}

std::vector<double> Spectrogram::fftAbs(std::vector<double> r, std::vector<double> c) {
    std::vector<double> res;

    for(int i=0; i<r.size(); i++) { 
        // r[i] /= r.size();
        // c[i] /= c.size();
        
        double val = r[i]*r[i]+c[i]*c[i];
        
        if(!logScale) 
            val = sqrt(val);
        
        res.push_back(val);
    }

    return res;
}

void Spectrogram::initFilterbank() {
    for(int i=1; i<=this->nFFT; i++) {
        std::vector<double> row;

        for(int j=0; j<this->windowLen/2; j++) {
            if(this->fftCenterBins[i-1] <= j && j < fftCenterBins[i]) {
                double val = (j-fftCenterBins[i-1])*1.0/(fftCenterBins[i]-fftCenterBins[i-1]);
                row.push_back(val);

            } else if(fftCenterBins[i] == j) {
                row.push_back(1);
            } else if(fftCenterBins[i] < j && j <= fftCenterBins[i+1]) {
                double val = (fftCenterBins[i+1]-j)*1.0/(fftCenterBins[i+1]-fftCenterBins[i]);
                row.push_back(val);

            } else {
                row.push_back(0);
            }
        }

        this->fbank.push_back(row);
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

