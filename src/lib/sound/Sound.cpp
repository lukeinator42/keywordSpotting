#include <keywordSpotter/lib/sound/Sound.h>

Sound::Sound() {
}

void Sound::appendAudio(float audio[], int len) {
    for(int i=0; i<len; i++) {
        buffer.push_back(audio[i]);
    }
}

Sound::~Sound() {
    
}

