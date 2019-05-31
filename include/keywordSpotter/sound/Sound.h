// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  Sound.cpp
 * @brief An object for holding audio data.
 * =========================================================================== */

#ifndef _SOUND_H_
#define _SOUND_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>
#include <vector>

class Sound {

  private:
	
	/* ===========================================================================
	 *  Variables received from the constructor.
	 * =========================================================================== */
    std::vector<double> buffer;
    int samplingRate;

  public:

	/* ===========================================================================
	 *  Default Constructor.
	 * =========================================================================== */
	Sound();

    void appendAudio(double audio[], int len);


	/* ===========================================================================
	 *  Returns sample at index.
	 * =========================================================================== */
    double getSample(int index) { return buffer[index]; }

    void setSamplingRate(int sr) { samplingRate = sr; }

    int getSamplingRate() { return samplingRate; }

	/* ===========================================================================
	 *  Returns size of sound object.
	 * =========================================================================== */

    int size() { return buffer.size(); }

    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~Sound();


  private:

};


#endif  //_SOUND_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
