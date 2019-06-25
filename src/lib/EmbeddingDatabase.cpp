// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  EmbeddingNetwork.cpp
 * @brief Neural network that returns embeddings for audio.
 * =========================================================================== */
#include <dirent.h>
#include <keywordSpotter/lib/EmbeddingDatabase.h>

EmbeddingDatabase::EmbeddingDatabase(std::string folderPath, std::string graph_fn, std::string checkpoint_fn): folderPath(folderPath) {
    wavUtils = new WavUtils();
    spectrogram = new Spectrogram(nFFT, winLen, hopSize, minFreq, maxFreq, sampleRate, logScale);
    network = new EmbeddingNetwork(graph_fn, checkpoint_fn);

    initDB();
}

void EmbeddingDatabase::initDB() {
    if(folderPath[folderPath.size()-1] != '/')
        folderPath += '/';

    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    dp = opendir(folderPath.c_str());
    
    if (dp != nullptr) {
        while ((entry = readdir(dp))) {
            std::string subfolder(entry->d_name);

            if(subfolder[0] == '.')
                continue;


            Embedding res = averageFolderEmbeddings(folderPath+subfolder);

            database[subfolder] = res;
        }
    }
}

Embedding EmbeddingDatabase::averageFolderEmbeddings(std::string folderPath) {
    if(folderPath[folderPath.size()-1] != '/')
        folderPath += '/';

    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    dp = opendir(folderPath.c_str());

    Embedding ret;
    
    if (dp != nullptr) {
        while ((entry = readdir(dp))) {
            std::string subfolder(entry->d_name);
            std::string filepath = folderPath + subfolder;

            if(subfolder[0] == '.')
                continue;

                Sound buffer;

                wavUtils->readFile(filepath, buffer);
                SpectrogramResult s = spectrogram->computeSpectrogram(buffer.getAudio());
                Embedding res = network->getSpectrogramEmbedding(s);

                ret = averageEmbeddings(ret, res);
        }
    }

    delete dp;

    return ret;
}

Embedding EmbeddingDatabase::getEmbeddingFromWav(std::string wavPath) {
    Sound buffer;
    wavUtils->readFile(wavPath, buffer);
    SpectrogramResult s = spectrogram->computeSpectrogram(buffer.getAudio());
    Embedding res = network->getSpectrogramEmbedding(s);

    return res;
}

void EmbeddingDatabase::testEmbedding(Embedding a) {
    for(auto el : database)
        std::cout << el.first << ": " << cosineDistance(a, el.second) << std::endl;
}

Embedding EmbeddingDatabase::averageEmbeddings(Embedding a, Embedding b) {
    if(a.size() == 0)
        return b;
    
    if(b.size() == 0)
        return a;

    int minlen = std::min(a.size(), b.size());

    Embedding ret;

    std::reverse(a.begin(), a.end());
    std::reverse(b.begin(), b.end());

    while(a.size() > minlen)
        a.pop_back();

    while(b.size() > minlen)
        b.pop_back();

    std::reverse(a.begin(), a.end());
    std::reverse(b.begin(), b.end());
    
    for(int i=0; i<minlen; i++)
        ret.push_back((a[i]+b[i])/2.0);

    return ret;
}

float EmbeddingDatabase::cosineDistance(Embedding a, Embedding b) {
    float dot=0, aMag=0, bMag=0;

    while(a.size() > b.size())
        a.erase(a.begin());

    while(b.size() > a.size())
        b.erase(b.begin());

    for(int i=0; i<a.size(); i++) {
        dot += a[i]*b[i];

        aMag += a[i]*a[i];
        bMag += b[i]*b[i];
    }

    aMag = sqrt(aMag);
    bMag = sqrt(bMag);

    return dot/(aMag * bMag);
}

EmbeddingDatabase::~EmbeddingDatabase() {
    delete wavUtils;
    delete spectrogram;
    delete network;
}
