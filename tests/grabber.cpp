/******************************************/
/*
  grabber.cpp
  by Fabian Schlieper, 2017

  This program opens a playback device for loopback 
  and passes the grabbed audio to the output device
*/
/******************************************/

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

/*
typedef char MY_TYPE;
#define FORMAT RTAUDIO_SINT8
*/

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16

/*
typedef S24 MY_TYPE;
#define FORMAT RTAUDIO_SINT24

typedef signed long MY_TYPE;
#define FORMAT RTAUDIO_SINT32

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
*/

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications
  std::cout << "\nuseage: grabber gDevice <oDevice> <N> <fs> <iChannelOffset> <oChannelOffset>\n";
  std::cout << "    gDevice = device to grab audio frome (loopback),\n";
  std::cout << "    oDevice = optional output device to use (default = 0),\n";
  std::cout << "    N = optional number of channels (default 2),\n";
  std::cout << "    fs = optional the sample rate (default 48000),\n"; 
  std::cout << "    iChannelOffset = an optional grabbing channel offset (default = 0),\n";
  std::cout << "    and oChannelOffset = optional output channel offset (default = 0).\n\n";
}

int inout( void *outputBuffer, void *inputBuffer, unsigned int /*nBufferFrames*/,
           double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
  // Since the number of input and output channels is equal, we can do
  // a simple buffer copy operation here.
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;

  unsigned int *bytes = (unsigned int *) data;
  memcpy( outputBuffer, inputBuffer, *bytes );
  return 0;
}

int main( int argc, char *argv[] )
{
  unsigned int channels = 2, fs = 48000, bufferBytes, oDevice = -1, gDevice = -1, iOffset = 0, oOffset = 0;

  RtAudio adac;

  if (adac.getDeviceCount() < 1) {
	  std::cout << "\nNo audio devices found!\n";
	  exit(1);
  }

  // Minimal command-line checking
  if (argc < 2 || argc > 7) {
	  usage();

	  int devices = adac.getDeviceCount();

	  std::cout << "Available devices:" << std::endl;
	  for (int i = 0; i < devices; i++) {
		  auto inf = adac.getDeviceInfo(i);
		  std::cout << i << ": " << inf.name << std::endl;
	  }
	  char input;
	  std::cout << "\npress <enter> to quit .\n";
	  std::cin.get(input);
	  exit(0);
  }


  gDevice = (unsigned int)atoi(argv[1]);
  if ( argc > 2 )
    oDevice = (unsigned int)atoi(argv[2]);
  if ( argc > 3 )
    channels = (unsigned int)atoi(argv[3]);
  if ( argc > 4 )
    fs = (unsigned int)atoi(argv[4]);
  if ( argc > 5 )
    iOffset = (unsigned int) atoi(argv[5]);
  if ( argc > 6 )
    oOffset = (unsigned int) atoi(argv[6]);

  // Let RtAudio print messages to stderr.
  adac.showWarnings( true );

  // Set the same number of channels for both input and output.
  unsigned int bufferFrames = 512;
  RtAudio::StreamParameters gParams, oParams;
  gParams.deviceId = gDevice;
  gParams.nChannels = channels;
  gParams.firstChannel = iOffset;
  oParams.deviceId = oDevice;
  oParams.nChannels = channels;
  oParams.firstChannel = oOffset;

  if ( gDevice == -1 )
    gParams.deviceId = adac.getDefaultOutputDevice();

  // fallback to another device for playback
  if ( oDevice == -1 )
    oParams.deviceId = (unsigned int)!gParams.deviceId;

  std::cout << "Grabbing audio from " << adac.getDeviceInfo(gParams.deviceId).name << std::endl;
  std::cout << "and passing it to " << adac.getDeviceInfo(oParams.deviceId).name << std::endl;
	

  RtAudio::StreamOptions options;
  //options.flags |= RTAUDIO_NONINTERLEAVED;

  bufferBytes = bufferFrames * channels * sizeof( MY_TYPE );
  try {
    adac.openStream( &oParams, &gParams, FORMAT, fs, &bufferFrames, &inout, (void *)&bufferBytes, &options );
  }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    exit( 1 );
  }

  // Test RtAudio functionality for reporting latency.
  std::cout << "\nStream latency = " << adac.getStreamLatency() << " frames" << std::endl;

  try {
    adac.startStream();

    char input;
    std::cout << "\nRunning ... press <enter> to quit (buffer frames = " << bufferFrames << ").\n";
    std::cin.get(input);

    // Stop the stream.
    adac.stopStream();
  }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    goto cleanup;
  }

 cleanup:
  if ( adac.isStreamOpen() ) adac.closeStream();

  return 0;
}
