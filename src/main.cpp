#include <iostream>
#include <keywordSpotter/EmbeddingNetwork.h>
#include <sndfile.h>
using namespace std;

#define ARRAY_LEN(x)    ((int) (sizeof (x) / sizeof (x [0])))

int main(int argc, char ** argv) {

    EmbeddingNetwork e("Hello World!");

    cout << e.getMsg() << endl;


    SNDFILE *infile, *outfile ;
    SF_INFO sfinfo ;
    double buffer [1024] ;
    sf_count_t count ;
    
    if (argc != 3) {
        printf("\nUsage :\n\n    <executable name>  <input file> <output file>\n") ;
        exit(0);
    }
    
    memset (&sfinfo, 0, sizeof (sfinfo)) ;
    if ((infile = sf_open (argv [1], SFM_READ, &sfinfo)) == NULL)     {     
        printf ("Error : Not able to open input file '%s'\n", argv [1]);
        sf_close (infile);
        exit (1) ;
    } 
    
    if ((outfile = sf_open (argv [2], SFM_WRITE, &sfinfo)) == NULL) { 
        printf ("Error : Not able to open output file '%s'\n", argv [argc - 1]);
        sf_close (infile);
        exit (1);
    }
        
    while ((count = sf_read_double (infile, buffer, ARRAY_LEN (buffer))) > 0) {
        for (int i = 0; i < 1024; i++)
        buffer[i] *= 4;
        sf_write_double (outfile, buffer, count);
    }
    
    sf_close (infile) ;
    sf_close (outfile) ;

    return 0;
}