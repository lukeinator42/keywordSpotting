// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  
 * WavUtils.cpp
 * @brief An object for loading sound buffers from wav files.
 * =========================================================================== */

#ifndef _WAV_LOADER_H_
#define _WAV_LOADER_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>
#include <sndfile.h>
#include <keywordSpotter/sound/Sound.h>

#define ARRAY_LEN(x)    ((int) (sizeof (x) / sizeof (x [0])))

class WavUtils {

  private:
	
	/* ===========================================================================
	 *  Variables received from the constructor.
	 * =========================================================================== */
    SNDFILE *infile;
    SF_INFO sfinfo;
    sf_count_t count;

  public:

	/* ===========================================================================
	 *  Default Constructor.
	 * =========================================================================== */
	
    WavUtils();


void readFile(std::string path, Sound& sound_buffer);


    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~
    WavUtils();


  private:

};


#endif  //_WAV_LOADER_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
