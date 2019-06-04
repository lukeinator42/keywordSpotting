// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  Spectrogram.cpp
 * @brief For generating spectrogram features.
 * =========================================================================== */

#ifndef _SPECTROGRAM_H_
#define _SPECTROGRAM_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>
#include <vector>
#include <cmath>

class Spectrogram {

  private:
    int nFFT;
    int windowLen;
    int hopSize;
    double minFreq;
    double maxFreq;
    bool logScale;
    int sampleRate;
    const double PI = std::acos(-1.0);
    std::vector<int> fftCenterBins;
    std::vector< std::vector<double> > fbank;

  public:

	/* ===========================================================================
	 *  Default Constructor.
     */
	Spectrogram(int nFFT=128, int windowLen=400, int hopSize=160, double minFreq=300.0, double maxFreq=8000.0, int sampleRate=16000, bool logScale=true);

    
    

    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~Spectrogram();

    std::vector< std::vector<double> > computeSpectrogram(std::vector<double> audio);

    std::vector< std::vector<double> > getFBank() { return fbank; }

  private:
    void hann(std::vector<double>& data);
    void hamm(std::vector<double>& data);
    double hzToMel(double f);
    double melToHz(double mel);
    void initFFTCenterBins();
    void initFilterbank();
    std::vector<double> fftAbs(std::vector<double> r, std::vector<double> c);

};


#endif  //_SPECTROGRAM_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
