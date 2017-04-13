/**
 * This is a litte data structure to hold n-events for the n-event generator.
 *
 * Mark Koennecke, June 2015
 */
#include <stdlib.h>
#include <string.h>
#include "neventArray.h"


pNEventArray createNEventArray(unsigned long count) {
  pNEventArray result = NULL;
  
  result = malloc(sizeof(neventArray));
  if(result == NULL){ return result; }
  memset(result,0,sizeof(neventArray));

  result->count = count;
  result->event = malloc(count*sizeof(int64_t));

  if(result->event == NULL) {
    killNEventArray(&result);
    return NULL;
  }
  return result;
}
/*--------------------------------------------*/
void killNEventArray(pNEventArray *pself) {
  pNEventArray self = *pself;
  if(self != NULL){
    if(self->event != NULL){
      free(self->event);
    }
    free(*pself);
  }
}

/*-----------------------------------------------------------*/

pNEventArray multiplyNEventArray(pNEventArray source, unsigned int factor)
{
  pNEventArray result;
  int i;

  result = createNEventArray(source->count*factor);
  if(result == NULL){ return NULL; }
  for(i = 0; i < factor; i++){
    memcpy(result->event + i*source->count,
  	   source->event, source->count*sizeof(int64_t));
  }
  return source;
}
