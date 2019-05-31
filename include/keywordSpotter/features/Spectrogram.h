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

class Spectrogram {

  private:
    int nFFT;
    int windowLen;
    int hopSize;
    bool logScale;

  public:

	/* ===========================================================================
	 *  Default Constructor.
     */
	Spectrogram(int nFFT=128, int windowLen=400, int hopSize=160, bool logScale=true);


    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~Spectrogram();


  private:

};


#endif  //_SPECTROGRAM_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
