#include <iostream>
#include <keywordSpotter/EmbeddingNetwork.h>
#include <sndfile.h>
#include <keywordSpotter/utils/WavUtils.h>
#include <keywordSpotter/sound/Sound.h>

using namespace std;

int main(int argc, char ** argv) {

    EmbeddingNetwork e("Hello World!");

    //cout << e.getMsg() << endl;
    
    if (argc != 2) {
        printf("\nUsage :\n\n    <executable name>  <input file>\n") ;
        exit(0);
    }

    string path(argv[1]);

    WavUtils wavUtils;

    Sound buffer;

    cout << buffer.size() << endl;

    wavUtils.readFile(path, buffer);

    cout << buffer.size() << endl;

    

    
    return 0;
}