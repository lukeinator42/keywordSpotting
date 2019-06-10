#include <iostream>
#include <fstream>
#include <sndfile.h>
#include <dirent.h>
#include <keywordSpotter/lib/EmbeddingNetwork.h>
#include <keywordSpotter/lib/EmbeddingDatabase.h>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {

    if (argc != 2) {
        printf("\nUsage :\n\n    <executable name>  <input file>\n") ;
        exit(0);
    }

    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    dp = opendir(argv[1]);

    string path(argv[1]);

    const string graph_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/librispeech_lstm_embedding_model.meta";
    const string checkpoint_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/librispeech_lstm_embedding_model";
    

    EmbeddingDatabase database(path, graph_fn, checkpoint_fn);

    if(path[path.size()-1] != '/')
        path += '/';
    
    if (dp != nullptr) {
        while ((entry = readdir(dp))) {
            string subfolder(entry->d_name);

            if(subfolder[0] == '.')
                continue;

            cout << subfolder << ": ";

            Embedding res = database.averageFolderEmbeddings(path+subfolder);

            cout << res.size() << endl;
        }
    }
    
    return 0;
}