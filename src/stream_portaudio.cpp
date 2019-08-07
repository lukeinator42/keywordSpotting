#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "portaudio.h"

using namespace std;


#define SAMPLE_RATE  (16000)
#define FRAMES_PER_BUFFER (512)
#define NUM_SECONDS     (1)
#define NUM_CHANNELS    (2)
#define DITHER_FLAG     (0) /**/

#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"

typedef struct
{
    int write;  /* Index into sample array. */
    int read;
}
paTestData;

const int CIRCULAR_BUFFER_SIZE = (NUM_SECONDS * SAMPLE_RATE);

float buffer[CIRCULAR_BUFFER_SIZE];

void processAlgorithm(paTestData* data) {
    SAMPLE              max, val;
    double              average;

    max = 0;
    average = 0.0;
    for(int i=0; i<FRAMES_PER_BUFFER; i++ )
    {
        val = buffer[(data->read+i) % CIRCULAR_BUFFER_SIZE];
        if( val < 0 ) val = -val; /* ABS */
        if( val > max )
        {
            max = val;
        }
        average += val;
    }

    average = average / (double)FRAMES_PER_BUFFER;

    printf("sample max amplitude = "PRINTF_S_FORMAT"\n", max );
    printf("sample average = %lf\n", average );
}

static int paStreamCallback(const void* input, void* output,
                            unsigned long samplesPerFrame,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData) {

    paTestData *data = (paTestData *)userData;
    // Write input buffer to our buffer: (memcpy function is fine, just a for loop of writing data
    memcpy(&buffer[data->write], input, samplesPerFrame); // Assuming samplesPerFrame = FRAME_SIZE

    // Increment write/wraparound
    data->write = (data->write + FRAMES_PER_BUFFER) % CIRCULAR_BUFFER_SIZE;

    // dummy algorithm for processing blocks:
    processAlgorithm(data);

    // Write output
    float *dummy_buffer = &buffer[data->read]; // just for easy increment

    // Increment read/wraparound
    data->read = (data->read + FRAMES_PER_BUFFER) % CIRCULAR_BUFFER_SIZE;

    return paContinue;
}

int main(void)
{
    PaStreamParameters  inputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 numSamples;
    int                 numBytes;
    
    

    printf("patest_record.c\n"); fflush(stdout);
    data.write = 0, data.read = 0;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = 2;                    /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paStreamCallback,
              &data );
    if( err != paNoError ) goto done;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    printf("\n=== Now recording!! Please speak into the microphone. ===\n"); fflush(stdout);

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
        printf("index = %d\n", data.read ); fflush(stdout);
    }
    if( err < 0 ) goto done;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

    /* Measure maximum peak amplitude. */
    

done:
    Pa_Terminate();

    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return err;
}
