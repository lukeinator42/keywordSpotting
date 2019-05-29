// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  EmbeddingNetwork.cpp
 * @brief Neural network that returns embeddings for audio.
 * =========================================================================== */

#include <keywordSpotter/EmbeddingNetwork.h>

EmbeddingNetwork::EmbeddingNetwork(std::string msg): msg(msg) {
}

EmbeddingNetwork::~EmbeddingNetwork() {

}

std::string EmbeddingNetwork::getMsg() {
    return this->msg;
}

