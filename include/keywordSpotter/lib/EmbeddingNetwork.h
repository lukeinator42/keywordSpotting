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
#include <vector>
#include <utility>
#include <string>
#include <tensorflow/core/protobuf/meta_graph.pb.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/public/session_options.h>

typedef std::vector<float> Embedding;
typedef std::vector<std::pair<std::string, tensorflow::Tensor>> tensor_dict;

class EmbeddingNetwork {

  private:
	
	/* ===========================================================================
	 *  Variables received from the constructor.
	 * =========================================================================== */
  std::string msg;
  tensorflow::Session *sess;
  tensorflow::SessionOptions options;

  public:

	/* ===========================================================================
	 *  Default Constructor.
	 * =========================================================================== */
	EmbeddingNetwork(const std::string graph_fn, const std::string checkpoint_fn);

	Embedding getSpectrogramEmbedding( std::vector<float> s);


	/* ===========================================================================
	 *  test func.
	 * =========================================================================== */
  std::string getMsg();


    /* ===========================================================================
	 *  Destructor.
	 * =========================================================================== */
	~EmbeddingNetwork();


  private:
		tensorflow::Status LoadModel(tensorflow::Session *sess, std::string graph_fn, std::string checkpoint_fn = "");

};


#endif  //_EMBEDDING_NETWORK_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
