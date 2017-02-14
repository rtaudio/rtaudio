/******************************************/
/*
  record.cpp
  by Gary P. Scavone, 2007

  This program records audio from a device and writes it to a
  header-less binary file.  Use the 'playraw', with the same
  parameters and format settings, to playback the audio.
*/
/******************************************/

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include <sndfile.hh>

/*
typedef char MY_TYPE;
#define FORMAT RTAUDIO_SINT8
*/

//typedef signed short MY_TYPE;
//#define FORMAT RTAUDIO_SINT16

/*
typedef S24 MY_TYPE;
#define FORMAT RTAUDIO_SINT24

typedef signed long MY_TYPE;
#define FORMAT RTAUDIO_SINT32
*/

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

/*
typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
*/

// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications
  std::cout << "\nuseage: record N fs <duration> <device> <channelOffset>\n";
  std::cout << "    where N = number of channels,\n";
  std::cout << "    fs = the sample rate,\n";
  std::cout << "    duration = optional time in seconds to record (default = 2.0),\n";
  std::cout << "    device = optional device to use (default = 0),\n";
  std::cout << "    and channelOffset = an optional channel offset on the device (default = 0).\n\n";
  exit( 0 );
}

struct InputData {
	SndfileHandle *wavHandle;
};

// Interleaved buffers
int input( void * /*outputBuffer*/, void *inputBuffer, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  InputData *iData = (InputData *) data;

  if (iData->wavHandle->write((const MY_TYPE*)inputBuffer, nBufferFrames* iData->wavHandle->channels()) <= 0)
	  return 1;
  return 0;
}

int main( int argc, char *argv[] )
{
  unsigned int channels, fs, bufferFrames, device = 0, offset = 0;
  double time = 2.0;
  FILE *fd;

  // minimal command-line checking
  if ( argc < 3 || argc > 6 ) usage();

  RtAudio adc;
  if ( adc.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 1 );
  }

  channels = (unsigned int) atoi( argv[1] );
  fs = (unsigned int) atoi( argv[2] );
  if ( argc > 3 )
    time = (double) atof( argv[3] );
  if ( argc > 4 )
    device = (unsigned int) atoi( argv[4] );
  if ( argc > 5 )
    offset = (unsigned int) atoi( argv[5] );

  // Let RtAudio print messages to stderr.
  adc.showWarnings( true );

  // Set our stream parameters for input only.
  bufferFrames = 512;
  RtAudio::StreamParameters iParams;
  if ( device == 0 )
    iParams.deviceId = adc.getDefaultInputDevice();
  else
    iParams.deviceId = device;
  iParams.nChannels = channels;
  iParams.firstChannel = offset;

  SndfileHandle fi("record.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, channels, fs);
  InputData data;
  data.wavHandle = &fi;

  try {
    adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, &input, (void *)&data );
	adc.startStream();
  }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    goto cleanup;
  }


  std::cout << "\nRecording ... writing file 'record.wav' (buffer frames = " << bufferFrames << ")." << std::endl;
  std::cout << "\nPress any key to stop." << std::endl;
  getchar();


 cleanup:
  if ( adc.isStreamOpen() ) adc.closeStream();

  return 0;
}
