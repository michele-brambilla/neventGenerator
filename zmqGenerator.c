/**
 * This is an n-event generator which reads a NeXus file, makes 
 * an event list from it and sends that using 0MQ. 
 *
 * Mark Koennecke, June 2015
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <zmq.h>

#include "nexus2event.h"

/* #include "config.h" */

static unsigned long pulseID = 0L; 

void timer_func() 
{
  pulseID++;
}

int main(int argc, char *argv[]) {
  char dataHeader[1024*1024];
  pNEventArray data = NULL, tmp = NULL;
  void *zmqContext = NULL;
  void *pushSocket = NULL;
  char sockAddress[255];
  unsigned long nCount = 0,  pulseCount = 0;
  int64_t byteCount = 0;
  time_t statTime;
  unsigned int multiplier = 1;
  int rc;
  int hwm_value = 2;

  if(argc < 3) {
    printf("usage:\n\tzmqGenerator nexusfile port [multiplier]\n");
    return 1;
  }

  /*
    load NeXus file into en event list
  */
  data = loadNeXus2Events(argv[1]);
  if(data == NULL){
    printf("Failed to load NeXus data to events\n");
    return -1;
  }
  if(sizeof(data) < 10) {
    data = createNEventArray(1024);
  }
  
  /*
    handle multiplier
  */
  if(argc > 3){
    multiplier = atoi(argv[3]);
    tmp = multiplyNEventArray(data,multiplier);

    if(tmp == NULL){
      printf("Failed to multiply event array by %d\n", multiplier);
    } else {
      /* killNEventArray(&data); */
      data = tmp;
    }
  }
  printf("Sending %ld n-events per message\n", data->count);

  /*
    initialize 0MQ
  */
  zmqContext = zmq_ctx_new();
  pushSocket = zmq_socket(zmqContext,ZMQ_PUSH);
  snprintf(sockAddress,sizeof(sockAddress),"tcp://127.0.0.1:2911");
  rc = zmq_bind(pushSocket,sockAddress);
  rc = zmq_setsockopt(pushSocket,ZMQ_SNDHWM, &hwm_value, sizeof(hwm_value));
 
  statTime = time(NULL);

  while(1) {

    /*
      create dataHeader
    */
    snprintf(dataHeader,sizeof(dataHeader),"{\"htype\":\"sinq-1.0\",\"pid\":%lu,\"st\":1469096950.708,\"ts\":1706054815,\"tr\":10000,\"ds\":[{\"ts\":32,\"bsy\":1,\"cnt\":1,\"rok\":1,\"gat\":1,\"evt\":4,\"id1\":12,\"id0\":12},%lu],\"hws\":{\"bsy\":0,\"cnt\":1,\"rok\":1,\"gat\":1,\"error\":0,\"full\":0,\"zmqerr\":0,\"lost\":[0,0,0,0,0,0,0,0,0,0]}}",pulseID,data->count);
    /*
      send the stuff away 
    */

    if(data->count) {
      byteCount += zmq_send(pushSocket,dataHeader,strlen(dataHeader),ZMQ_SNDMORE);
      byteCount += zmq_send(pushSocket,data->event,data->count*sizeof(int64_t),0);
    }
    else {
      byteCount += zmq_send(pushSocket,dataHeader,strlen(dataHeader),0);
    }
    /*
      handle statistics
    */
    nCount += data->count;
    pulseCount++;
    if(time(NULL) >= statTime + 10){
      printf("byteCount = %lld, nCount = %ld, pulseCount = %ld\n", byteCount, nCount, pulseCount); 
      printf("Sent %f MB/sec , %f n* 10^6/sec, %ld pulses\n", byteCount/(1024.*1024.*10.), nCount/10000000., pulseCount);
      pulseCount = 0;
      byteCount = 0;
      nCount = 0;
      statTime = time(NULL);
    }
  }


  killNEventArray(&data);
  zmq_close(pushSocket);
  zmq_ctx_destroy(zmqContext);

  return 0;
}
 
