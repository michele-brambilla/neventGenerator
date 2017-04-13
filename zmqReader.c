/**
 * This is a simple test reader for the n-events sent by the zmqGenerator. 
 * Just a proof of concept and testing thinsg and a template for implementing 
 * more serious readers.
 *
 * Mark Koennecke, June 2015
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <zmq.h>

#include "neventArray.h"
/* #include "config.h" */

void timer_func() 
{
  //  pulseID++;
}

int main(int argc, char *argv[])
{
  unsigned long nCount = 0, pulseCount = 0, oldPulseID = 0, pulseID = 0, nEvents = 0;
  int64_t byteCount = 0;
  int bytesRead, status;
  void *zmqContext = NULL;
  void *pullSocket = NULL;
  time_t statTime;
  char headerData[1024*1024];
  char *pPtr, *pEnd;
  int32_t rtimestamp[2];
  int64_t *dataBuffer = NULL, *dataTimeStamp = NULL;

  unsigned int dataBufferSize = 0, dataTimeStampSize = 0;

  if(argc < 2) {
    printf("usage:\n\tzmqReader endpoint\n\twith endpoint being in the format: tcp://host:port\n");
    return 1;
  }
  
  /*
   * initialize 0mq
   */
  zmqContext = zmq_ctx_new();
  pullSocket = zmq_socket(zmqContext,ZMQ_PULL);

  status = zmq_connect(pullSocket,argv[1]);
  assert(status == 0);
  /* status = zmq_setsockopt (pullSocket, ZMQ_SUBSCRIBE, */
  /*                          "", 0); */
  
  statTime = time(NULL);

  
  while(1) {

    bytesRead = zmq_recv(pullSocket, headerData, sizeof(headerData),0);
    if(bytesRead >= sizeof(headerData)) {
      headerData[sizeof(headerData)-1] = '\0';
    } else {
      headerData[bytesRead] = '\0';
    }
    printf("header: %s\n",headerData);

    nEvents = atoi(pPtr);
    /* printf("nEvents = %d\n", nEvents); */
    nCount += nEvents;

    /*
      make sure that we have enough data buffer size
    */
    if(nEvents *sizeof(int64_t) != dataBufferSize){
      if(dataBuffer != NULL){
	free(dataBuffer);
      }
      if(dataTimeStamp != NULL){
	free(dataTimeStamp);
      }
      //      dataBufferSize = nEvents*sizeof(int64_t);
      dataBufferSize = nEvents*sizeof(int64_t);
      dataBuffer = malloc(dataBufferSize);

      dataTimeStampSize = nEvents*sizeof(int32_t);
      dataTimeStamp = malloc(dataTimeStampSize);

      if(dataBuffer == NULL ||  dataTimeStamp == NULL){
	printf("Out of memory allocating data buffers\n");
	return 1;
      }
    }

    /*
      read the two data elements
    */
    byteCount += zmq_recv(pullSocket,dataBuffer,dataBufferSize,0);

    /*
      do the statistics
    */
    pulseCount++;
    if(time(NULL) >= statTime + 10){
      /* printf("byteCount = %ld, nCount = %ld, pulseCount = %ld\n", byteCount, nCount, pulseCount);  */
      printf("Received %f MB/sec , %f n* 10^6/sec, %ld pulses\n", byteCount/(1024.*1024.*10.), nCount/10000000., pulseCount);
      pulseCount = 0;
      byteCount = 0;
      nCount = 0;
      statTime = time(NULL);
    }
    
  }
      
  /*
    never get here, but close it anyway
  */
  zmq_close(pullSocket);
  zmq_ctx_destroy(zmqContext);


  return 0;

}
