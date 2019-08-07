#include <iostream>
#include <fstream>
#include <sndfile.h>
#include <dirent.h>
#include <keywordSpotter/lib/EmbeddingNetwork.h>
#include <keywordSpotter/lib/EmbeddingDatabase.h>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {

    if (argc != 3) {
        printf("\nUsage :\n\n    <executable name> <input folder> <input file>\n") ;
        exit(0);
    }

    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    dp = opendir(argv[1]);

    string path(argv[1]);

    string wavPath(argv[2]);

    const string graph_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/speech_commands_audio_lstm_embedding_model.meta";
    const string checkpoint_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/speech_commands_audio_lstm_embedding_model";
    

    EmbeddingDatabase database(path, graph_fn, checkpoint_fn);

    cout << "audio: " << database.getEmbedding("audio").size() << endl;
    cout << "robot: " << database.getEmbedding("robot").size() << endl;
    cout << "intelligence: " << database.getEmbedding("intelligence").size() << endl;

    Embedding test = database.getEmbeddingFromWav(wavPath);

    cout << "wav: " << test.size() << endl;

    cout << endl;

    database.testEmbedding(test);
    
    return 0;
}