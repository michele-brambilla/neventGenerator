/**
 * This is a litte data structure to hold n-events for the n-event generator.
 *
 * Mark Koennecke, June 2015
 */

#ifndef __NEVENTARRAY
#define __NEVENTARRAY
#include <stdint.h>

typedef struct {
  unsigned long count;
  int64_t *event;
} neventArray, *pNEventArray;


pNEventArray createNEventArray(unsigned long count);
void killNEventArray(pNEventArray *self);
pNEventArray multiplyNEventArray(pNEventArray source, unsigned int factor);

#endif
