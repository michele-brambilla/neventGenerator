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
#include "posix_timers.h"
#include "md5.h"

/* #include "config.h" */

static unsigned long pulseID = 0L; 

void timer_func() 
{
  pulseID++;
}

int main(int argc, char *argv[])
{
  static unsigned int oldPulseID = 0;
  char dataHeader[1024];
  pNEventArray data = NULL, tmp = NULL;
  MD5_CTX md5Context;
  unsigned char md5Hash[16];
  struct timespec tPulse;
  void *zmqContext = NULL;
  void *pushSocket = NULL;
  char sockAddress[255];
  unsigned long nCount = 0,  pulseCount = 0;
  int64_t byteCount = 0;
  time_t statTime;
  int64_t rtimestamp[2];
  unsigned int multiplier = 1;
  int rc;
  int hwm_value = 2;

  if(argc < 3) {
    printf("usage:\n\tzmqGenerator nexusfile portNo [multiplier]\n");
    return 1;
  }

  /*
    load NeXus file into en event list
  */
  data = loadNeXus2Events(argv[1]);
  if(data == NULL){
    printf("Failed to load NeXus data to events\n");
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
    create dataHeader
  */

  snprintf(dataHeader,sizeof(dataHeader),"{\"htype\":\"sinq-1.0\",\"pid\":925,\"st\":1469096950.708,\"ts\":1706054815,\"tr\":10000,\"ds\":[{\"ts\":32,\"bsy\":1,\"cnt\":1,\"rok\":1,\"gat\":1,\"evt\":4,\"id1\":12,\"id0\":12},%lu],\"hws\":{\"bsy\":0,\"cnt\":1,\"rok\":1,\"gat\":1,\"error\":0,\"full\":0,\"zmqerr\":0,\"lost\":[0,0,0,0,0,0,0,0,0,0]}}",data->count);

  
  MD5Init(&md5Context);
  MD5Update(&md5Context,dataHeader,strlen(dataHeader));
  MD5Final(md5Hash,&md5Context);
  /* printf("%s, hash =%x\n",dataHeader,md5Hash);  */


  /*
    initialize 0MQ
  */
  zmqContext = zmq_ctx_new();
  pushSocket = zmq_socket(zmqContext,ZMQ_PUSH);
  snprintf(sockAddress,sizeof(sockAddress),"tcp://127.0.0.1:%s",argv[2]);
  zmq_bind(pushSocket,sockAddress);
  rc = zmq_setsockopt(pushSocket,ZMQ_SNDHWM, &hwm_value, sizeof(hwm_value));

  /*
    start timer
  */
  init_timer();
  set_periodic_timer(71420); /* 71,42 milliseconds == 14HZ */
  
  statTime = time(NULL);

  while(1){
    if(oldPulseID != pulseID){
      if(pulseID - oldPulseID > 1) {
	printf("Timer miss at pulseID %lu\n", pulseID);
      }
      oldPulseID = pulseID;

      /*
	send the stuff away 
      */
      byteCount += zmq_send(pushSocket,dataHeader,strlen(dataHeader),ZMQ_SNDMORE);
      byteCount += zmq_send(pushSocket,data->event,data->count*sizeof(int64_t),ZMQ_SNDMORE);
      
      /*
	handle statistics
      */
      nCount += data->count;
      pulseCount++;
      if(time(NULL) >= statTime + 10){
	printf("byteCount = %ld, nCount = %ld, pulseCount = %ld\n", byteCount, nCount, pulseCount); 
	printf("Sent %f MB/sec , %f n* 10^6/sec, %ld pulses\n", byteCount/(1024.*1024.*10.), nCount/10000000., pulseCount);
	pulseCount = 0;
	byteCount = 0;
	nCount = 0;
	statTime = time(NULL);
      }
    }
  }

  killNEventArray(&data);
  zmq_close(pushSocket);
  zmq_ctx_destroy(zmqContext);

  return 0;
}
 
