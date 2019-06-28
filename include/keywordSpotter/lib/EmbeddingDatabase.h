// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  EmbeddingDatabase.h
 * @brief Class for matching query embeddings with previous examples.
 * =========================================================================== */

#ifndef _EMBEDDING_DATABASE_H_
#define _EMBEDDING_DATABASE_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>
#include <vector>
#include <utility>
#include <string>
#include <keywordSpotter/lib/EmbeddingNetwork.h>
#include <keywordSpotter/lib/features/Spectrogram.h>
#include <keywordSpotter/lib/utils/WavUtils.h>
#include <keywordSpotter/lib/sound/Sound.h>


class EmbeddingDatabase {

  private:
	
	/* ===========================================================================
	 *  Variables received from the constructor.
	 * =========================================================================== */
	std::string folderPath;
	std::map<std::string, Embedding> database;
	WavUtils* wavUtils;

    int nFFT = 40;
    int winLen = 400;
    int hopSize = 160;
    double minFreq = 50;
    double maxFreq = 8000;
    double sampleRate = 16000;
    bool logScale = true;

	Spectrogram* spectrogram;

	EmbeddingNetwork* network;


  public:

	/* ===========================================================================
	 *  Default Constructor.
	 * =========================================================================== */
	EmbeddingDatabase(const std::string folderPath, std::string graph_fn, std::string checkpoint_fn);

	Embedding averageEmbeddings(Embedding a, Embedding b);

	Embedding averageFolderEmbeddings(std::string folderPath);

	float cosineDistance(Embedding a, Embedding b);

	Embedding getEmbedding(std::string k) { return database[k]; }

	void testEmbedding(Embedding a);

	Embedding getEmbeddingFromWav(std::string wavPath);

    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~EmbeddingDatabase();


  private:
	void initDB();

};


#endif  //_EMBEDDING_DATABASE_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
