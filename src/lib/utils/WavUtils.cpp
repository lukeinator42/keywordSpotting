#include <keywordSpotter/lib/utils/WavUtils.h>

WavUtils::WavUtils() {
}

void WavUtils::readFile(std::string path, Sound& sound_buffer) {
    memset (&sfinfo, 0, sizeof (sfinfo)) ;
    if ((infile = sf_open (path.c_str(), SFM_READ, &sfinfo)) == NULL)     {     
        printf ("Error : Not able to open input file '%s'\n", path.c_str());
        sf_close (infile);
        exit (1) ;
    } 

    double raw_buffer[1024];

    while ((count = sf_read_double (infile, raw_buffer, ARRAY_LEN (raw_buffer))) > 0) {
        
        sound_buffer.appendAudio(raw_buffer, 1024);
    }
}

WavUtils::~WavUtils() {
    sf_close (infile);
}

