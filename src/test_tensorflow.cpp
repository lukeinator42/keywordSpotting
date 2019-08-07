#include <iostream>
#include <fstream>
#include <string>
#include <keywordSpotter/lib/EmbeddingNetwork.h>
#include <sndfile.h>
#include <keywordSpotter/lib/utils/WavUtils.h>
#include <keywordSpotter/lib/sound/Sound.h>
#include <keywordSpotter/lib/features/Spectrogram.h>
#include <tensorflow/core/protobuf/meta_graph.pb.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/public/session_options.h>

using namespace std;

int main(int argc, char ** argv) {
    const string graph_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/speech_commands_audio_lstm_embedding_model.meta";
    const string checkpoint_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/speech_commands_audio_lstm_embedding_model";
    
    if (argc != 3) {
        printf("\nUsage :\n\n    <executable name>  <input file>\n") ;
        exit(0);
    }

    string inpath(argv[1]);
    string outpath(argv[2]);

    WavUtils wavUtils;

    Sound buffer;

    int nFFT = 128;
    int winLen = 400;
    int hopSize = 160;
    double minFreq = 50;
    double maxFreq = 8000;
    double sampleRate = 16000;
    bool logScale = true;

    Spectrogram spectrogram(nFFT, winLen, hopSize, minFreq, maxFreq, sampleRate, logScale);

    wavUtils.readFile(inpath, buffer);

    EmbeddingNetwork network(graph_fn, checkpoint_fn);

    Embedding res = network.getSpectrogramEmbedding(buffer.getAudio());

    ofstream fout;
    fout.open(outpath);

    for(float el : res)
      fout << el << " ";

    fout << endl;

    fout.close();

    return 0;
}