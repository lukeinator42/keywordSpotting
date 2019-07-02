#include <keywordSpotter/lib/features/Spectrogram.h>
#include <keywordSpotter/lib/features/FftRealPair.h>
#include <limits>
#include <cmath>

Spectrogram::Spectrogram(int nFFT, int windowLen, int hopSize, double minFreq, double maxFreq, int sampleRate, bool logScale, bool pcen)
: nFFT(nFFT), windowLen(windowLen), hopSize(hopSize), minFreq(minFreq), maxFreq(maxFreq), sampleRate(sampleRate), logScale(logScale), pcen(pcen) {
    
    rplan = make_rfft_plan(windowLen);
    cplan = make_cfft_plan(windowLen);

    initFFTCenterBins();
    initFilterbank();
}


Spectrogram::~Spectrogram() {
    
}

SpectrogramResult Spectrogram::computeSpectrogram(std::vector<double> audio) {
    while(audio.size() % windowLen != hopSize)
        audio.push_back(0);

    SpectrogramResult sp;

    for(int i=0; i<audio.size(); i += hopSize) {
        //get subsection of audio
        std::vector<double> subvec(audio.begin()+i, audio.begin()+i+windowLen);

        //hanning window section
        hann(subvec);

        //create complex array

        std::vector<double> combined;

        for(double el : subvec) {
            combined.push_back(el);
            combined.push_back(el);
        }

        //do fft
        cfft_forward(cplan, combined.data(), 1.);

        std::vector<double> realComp;
        std::vector<double> imagComp;

        for(int i=0; i<combined.size(); i+=2) {
            realComp.push_back(combined[i]);
            imagComp.push_back(combined[i+1]);
        }

        //absolute val of fft
        std::vector<double> fftRes = fftAbs(realComp, imagComp);

        //convert to power spectral estimate
        // for(int j=0; j<fftRes.size()/2; j++)
        //     fftRes[j] = (1.0/(fftRes.size()/2))*fftRes[j]*fftRes[j];

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
                spectrogramCol[j] = 10.0*log10(std::max(1e-5, spectrogramCol[j]));
                spectrogramCol[j] -= 10.0*log10(1.0);
            }
        }

        //append col to spectrogram
        sp.push_back(spectrogramCol);
    }

    if(pcen)
        applyPcen(sp);

    return sp;
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

    double minMel = hzToMel(this->minFreq);
    double maxMel = hzToMel(this->maxFreq);
    
    for(int i=0; i<this->nFFT; i++) {
        double mel1 = minMel + ((i+2)*1.0/(this->nFFT+1))*(maxMel-minMel);
        double freq1 = melToHz(mel1);

        double mel2 = minMel + ((i)*1.0/(this->nFFT+1))*(maxMel-minMel);
        double freq2 = melToHz(mel2);
        
        double slaney = 2.0/(freq1-freq2);

        for(int j=0; j<this->windowLen/2; j++) {
            fbank[i][j] *= slaney;
        }
    }
}

void Spectrogram::applyPcen(SpectrogramResult& s) {
    double T = 0.4 * sampleRate / hopSize;
    // double b = (sqrt(1 + 4* T*T) - 1) / (2.0 * T*T);
    double b = 0.025;

    double gain=0.98, bias=2, power=0.5, eps=1e-06;

    SpectrogramResult m(s);

    SpectrogramResult smooth(m);

    for(int i=1; i<m.size(); i++) {
        for(int j=0; j< m[0].size(); j++) {
            m[i][j] = (1-b)*m[i-1][j]+b*s[i][j];
        }
    }

       for(int i=0; i<m.size(); i++) {
        for(int j=0; j< m[0].size(); j++) {
            smooth[i][j] = exp(-gain * (log(eps) + log1p(m[i][j] / eps)));
        }
    }

    for(int i=0; i< s.size(); i++) {
        for(int j=0; j<s[0].size(); j++) {
            s[i][j] = pow((s[i][j] * smooth[i][j] + bias), power) - pow(bias, power);
        }
    }
}

void Spectrogram::initFFTCenterBins() {
    double minMel = hzToMel(this->minFreq);
    double maxMel = hzToMel(this->maxFreq);

    for(int i=0; i<this->nFFT+2; i++) {
        double mel = minMel + (i*1.0/(this->nFFT+2))*(maxMel-minMel);
        double freq = melToHz(mel);

        int bin = std::floor(((this->windowLen))*freq/this->sampleRate);
        
        this->fftCenterBins.push_back(bin);
        
    }

}

