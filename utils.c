#include "cJSON.h"

char * default_hedaer = "{\"htype\":\"sinq-1.0\",\"pid\":0,\"st\":1469096950.708,\"ts\":1706054815,\"tr\":10000,\"ds\":[{\"ts\":32,\"bsy\":1,\"cnt\":1,\"rok\":1,\"gat\":1,\"evt\":4,\"id1\":12,\"id0\":12},0],\"hws\":{\"bsy\":0,\"cnt\":1,\"rok\":1,\"gat\":1,\"error\":0,\"full\":0,\"zmqerr\":0,\"lost\":[0,0,0,0,0,0,0,0,0,0]}}";

int set_header(char * output, int packet_id, struct timeval * system_time, int timestamp, int nevents) {

  cJSON * root = cJSON_Parse(default_hedaer);
  if (!root) {
    return -1;
  }
  cJSON_GetObjectItem(root,"pid")->valueint = packet_id;
  cJSON_GetObjectItem(root,"st")->valueint = system_time;
  cJSON_GetObjectItem(root,"ts")->valueint = timestamp;
  cJSON_GetObjectItem(root,"nevents")->valueint = nevents;
  
  
};
