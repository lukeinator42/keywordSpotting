#include <keywordSpotter/lib/features/Spectrogram.h>
#include <keywordSpotter/lib/features/FftRealPair.h>
#include <limits>
#include <cmath>
#include <algorithm>

Spectrogram::Spectrogram(int nFFT, int windowLen, int hopSize, double minFreq, double maxFreq, int sampleRate, bool logScale, bool pcen)
: nFFT(nFFT), windowLen(windowLen), hopSize(hopSize), minFreq(minFreq), maxFreq(maxFreq), sampleRate(sampleRate), logScale(logScale), pcen(pcen) {
    
    rplan = make_rfft_plan(windowLen);
    cplan = make_cfft_plan(windowLen);

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
            combined.push_back(0);
        }

        //do fft
        cfft_forward(cplan, combined.data(), 1.);

        std::vector<double> realComp;
        std::vector<double> imagComp;

        for(int j=0; j<combined.size(); j+=2) {
            realComp.push_back(combined[j]);
            imagComp.push_back(combined[j+1]);
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

            for(int k=0; k < (windowLen/2)+1; k++) {
                spectrogramCol[j] += fftRes[k] * fftRes[k] * fbank[j][k];     
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
    //return 2595*std::log10(1+(f/700));

    double f_min = 0.0;
    double f_sp = 200.0 / 3;

    double mel = (f - f_min) / f_sp;

    double min_log_hz = 1000.0;
    double min_log_mel = (min_log_hz - f_min) / f_sp;
    double logstep = log(6.4) / 27.0;

    if(f > min_log_hz)
        mel = min_log_mel + log(f / min_log_hz) / logstep;
    
    return mel;
}

double Spectrogram::melToHz(double mel) {
    //return 700*(pow(10, mel/2595)-1);
    double f_min = 0.0;
    double f_sp = 200.0 / 3;
    double freq = f_min + f_sp * mel;


    double min_log_hz = 1000.0;
    double min_log_mel = (min_log_hz - f_min) / f_sp;
    double logstep = log(6.4) / 27.0;


    if(mel >= min_log_mel)
        freq = min_log_hz * exp(logstep * (mel - min_log_mel));

    return freq;
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
        
        
        val = sqrt(val);
        
        res.push_back(val);
    }

    return res;
}

void Spectrogram::initFilterbank() {
    std::vector<double> fftFreqs;

    int numFreqs = (windowLen/2)+1;

    for(int i=0; i<numFreqs; i++)
        fftFreqs.push_back(((sampleRate/2))*(i/((double) numFreqs-1)));

    std::vector<double> melFreqs;

    double minMel = hzToMel(minFreq);
    double maxMel = hzToMel(maxFreq);
    
    for(int i=0; i<nFFT+2; i++)
        melFreqs.push_back(melToHz((maxMel-minMel)*(i/(nFFT+1.0))));

    std::vector<double> fdiff;
    
    for(int i=1; i<melFreqs.size(); i++)
        fdiff.push_back(melFreqs[i]-melFreqs[i-1]);

    std::vector< std::vector<double> > outer;

    for(int i=0; i<melFreqs.size(); i++) {
        std::vector< double > v(fftFreqs.size());
        outer.push_back(v);
    }

    for(int i=0; i<melFreqs.size(); i++) {
       for(int j=0; j<fftFreqs.size(); j++) {
            outer[i][j] = melFreqs[i]-fftFreqs[j];
       } 
    }

   for(int i=0; i<nFFT; i++) {

       std::vector< double > row;

       for(int j=0; j<numFreqs; j++) {
            double lw = -outer[i][j] / fdiff[i];
            double up = outer[i+2][j] / fdiff[i+1];

            double w = std::max(0.0, std::min(lw, up));

            double enorm = 2.0 / (melFreqs[i+2] - melFreqs[i]);

            w *= enorm;

            row.push_back(w);
       }

       fbank.push_back(row);
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


