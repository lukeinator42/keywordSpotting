// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  EmbeddingNetwork.cpp
 * @brief Neural network that returns embeddings for audio.
 * =========================================================================== */

#ifndef _EMBEDDING_NETWORK_H_
#define _EMBEDDING_NETWORK_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>


class EmbeddingNetwork {

  private:
	
	/* ===========================================================================
	 *  Variables received from the constructor.
	 * =========================================================================== */
  std::string msg;


  public:

	/* ===========================================================================
	 *  Default Constructor.
	 * =========================================================================== */
	EmbeddingNetwork(std::string msg);


	/* ===========================================================================
	 *  test func.
	 * =========================================================================== */
  std::string getMsg();


    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~EmbeddingNetwork();


  private:

};


#endif  //_EMBEDDING_NETWORK_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
