/**
 * This is a utility component which creates a n-event array from
 * the content of a NeXus file.
 *
 * Supported: AMOR
 *
 * Mark Koennecke, June 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdf5.h"
#include "hdf5_hl.h"
#include "neventArray.h"

/* see histogram_memory::process_rita2 */
#define EV_TYPE_POS_1D 0x02000000
#define RITA2_COUNTER_MASK 0xf0000000

typedef union {
  uint32_t s[2];
  uint64_t l;
} event_t;

static unsigned long countNeutrons(int32_t *data, unsigned int size) {
  unsigned long count = 0;
  int i;

  for (i = 0; i < size; i++) {
    count += data[i];
  }
  return count;
}

static pNEventArray loadRITA2(const char *filename) {
  hid_t file_id;
  hsize_t dim[3], size;
  uint32_t i, x, y, temp, timestamp, cts;

  event_t event;
  pNEventArray events;
  uint64_t nEvents, counter;

  int32_t *counts;
  file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);

  /* get the dimensions of the dataset */
  H5LTget_dataset_info(file_id, "/entry1/RITA-2/detector/counts", dim, NULL,
                       NULL);

  /* read dataset */
  for (i = 1, size = dim[0]; i < 3; i++) {
    size *= dim[i];
  }
  counts = calloc(size, sizeof(int32_t));

  H5LTread_dataset_int(file_id, "/entry1/RITA-2/detector/counts", counts);

  /* allocate event array */
  nEvents = countNeutrons(counts, size);
  events = createNEventArray(nEvents);

  /* fill event array */
  counter = 0;
  for (i = 0; i < size; ++i) {
    if (counts[i] == 0) {
      continue;
    }
    y = i % dim[2];
    temp = i / dim[2];
    timestamp = temp / dim[1];
    x = temp % dim[1];
    event.s[1] = 0xffffffff / dim[0] * timestamp;
    event.s[0] = x * dim[2] + y;
    event.s[0] += EV_TYPE_POS_1D + RITA2_COUNTER_MASK;
    for (cts = 0; cts < counts[i]; ++cts) {
      events->event[counter] = event.l;
      counter++;
    }
  }
  printf("done\n");
  free(counts);
  return events;
}

static pNEventArray loadAMOR(char *filename) {
  hid_t file_id;
  hsize_t dim[3], size;
  uint32_t i, x, y, temp, timestamp, cts;

  event_t event;
  pNEventArray events;
  uint64_t nEvents, counter;

  int32_t *counts, *tof;
  file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);

  /* get the dimensions of the dataset */
  H5LTget_dataset_info(file_id, "/entry1/AMOR/area_detector/data", dim, NULL,
                       NULL);

  /* read dataset */
  for (i = 1, size = dim[0]; i < 3; i++) {
    size *= dim[i];
  }
  counts = calloc(size, sizeof(int32_t));
  tof = calloc(dim[2], sizeof(int32_t));
  if (counts == NULL || tof == NULL) {
    printf("failed to allocate memory for NeXus data\n");
    return NULL;
  }

  printf("%s :\n\tsize = %d\n", filename, size);
  H5LTread_dataset_int(file_id, "/entry1/AMOR/area_detector/data", counts);
  H5LTread_dataset_int(file_id, "/entry1/AMOR/area_detector/time_binning", tof);

  nEvents = countNeutrons(counts, size);
  events = createNEventArray(nEvents);
  if (events == NULL) {
    return NULL;
  }
  printf("\tcounts = %d\n", nEvents);

  /* fill event array */
  counter = 0;
  for (i = 0; i < size; ++i) {
    if (counts[i] == 0) {
      continue;
    }
    y = i % dim[2];
    temp = i / dim[2];
    timestamp = temp / dim[1];
    x = temp % dim[1];
    event.s[1] = 0xffffffff / dim[0] * timestamp;
    event.s[0] = 32 * x * dim[2] + 32 * y;
    event.s[0] += EV_TYPE_POS_1D + RITA2_COUNTER_MASK;
    for (cts = 0; cts < counts[i]; ++cts) {
      events->event[counter] = event.l;
      counter++;
    }
  }

  //   /* nEvents = 0; */
  //   /* for(i = 0; i < dim[0]; i++){ */
  //   /*   for(j = 0; j < dim[1]; j++){ */
  //   /*     detID++; */
  //   /*     offset = i*dim[1]*dim[2] + j*dim[2]; */
  //   /*     for(k = 0; k < dim[2]; k++){ */
  //   /* 	nCount = data[offset+k]; */
  //   /* 	iTof = round(tof[k]/10.); */
  //   /* 	for(l = 0; l < nCount; l++){ */
  //   /* 	  evData->detectorID[nEvents] = detID; */
  //   /* 	  evData->timeStamp[nEvents] = iTof; */
  //   /* 	  nEvents++; */
  //   /* 	} */
  //   /*     } */
  //   /*   } */
  //   /* } */
  //
  free(counts);
  free(tof);

  return events;
}

static pNEventArray loadFOCUS(char *filename) {
  //   NXhandle handle;
  //   int32_t dim[2 + 1];
  //   int status, i, j, /* k,  */ l, nCount, rank, type;
  //   unsigned int size, offset;
  //   int32_t *data = NULL;
  //   float *tof = NULL;
  //   int32_t *tof_monitor = NULL;
  //   int32_t iTof;
  //   /* int32_t mTof; */
  //   unsigned long nEvents;
  pNEventArray evData = NULL;
  //   unsigned int detID = -1;
  //
  //   status = NXopen(filename, NXACC_READ, &handle);
  //   if (status != NX_OK) {
  //     printf("Failed to open NeXus file %s\n", filename);
  //     return NULL;
  //   }
  //
  //   status = NXopenpath(handle, "/entry1/FOCUS/merged/counts");
  //   if (status != NX_OK) {
  //     printf("NeXus file %s in wrong format\n", filename);
  //     return NULL;
  //   }
  //   NXgetinfo(handle, &rank, dim, &type);
  //
  //   for (i = 1, size = dim[0]; i < 2; i++) {
  //     size *= dim[i];
  //   }
  //   dim[rank] = dim[rank - 1];
  //
  //   data = malloc(size * sizeof(int32_t));
  //   tof = malloc(dim[1] * sizeof(float));
  //   tof_monitor = malloc(dim[1] * sizeof(int32_t));
  //   if (data == NULL || tof == NULL || tof_monitor == NULL) {
  //     printf("failed to allocate memory for NeXus data\n");
  //     return NULL;
  //   }
  //   NXgetdata(handle, data);
  //
  //   status = NXopenpath(handle, "/entry1/FOCUS/merged/time_binning");
  //   if (status != NX_OK) {
  //     printf("NeXus file %s in wrong format\n", filename);
  //     return NULL;
  //   }
  //   NXgetdata(handle, tof);
  //
  //   status = NXopenpath(handle, "/entry1/FOCUS/tof_monitor");
  //   if (status != NX_OK) {
  //     printf("NeXus file %s in wrong format\n", filename);
  //     return NULL;
  //   }
  //   NXgetdata(handle, tof_monitor);
  //
  //   NXclose(&handle);
  //
  //   nEvents = countNeutrons(data, size) + countNeutrons(tof_monitor,
  //   dim[2]);
  //
  //   evData = createNEventArray(nEvents);
  //   if (evData == NULL) {
  //     return NULL;
  //   }
  //   nEvents = 0;
  //
  //   /* for(i = 0; i < dim[0]; i++){ */
  //   /*   detID++; */
  //   /*   offset = i*dim[1]; */
  //   /*   for(j = 0; j < dim[1]; j++){ */
  //   /*     nCount = data[offset+j]; */
  //   /*     iTof = round(tof[j]/10.); */
  //
  //   /*     for(l = 0; l < nCount; l++){ */
  //   /*       evData->detectorID[nEvents] = detID; */
  //   /*       evData->timeStamp[nEvents] = iTof; */
  //
  //   /*       nEvents++; */
  //   /*     } */
  //   /*   } */
  //   /* } */
  //
  //   /* detID++; */
  //   /* for(j = 0; j < dim[2]; j++){ */
  //   /*   nCount = tof_monitor[j]; */
  //   /*   iTof = round(tof[j]/10.); */
  //
  //   /*   for(l = 0; l < nCount; l++){ */
  //   /*     evData->detectorID[nEvents] = detID; */
  //   /*     evData->timeStamp[nEvents] = iTof; */
  //
  //   /*     nEvents++; */
  //   /*   } */
  //   /* } */
  //
  //   free(data);
  //   free(tof_monitor);
  //   free(tof);
  //
  return evData;
}

pNEventArray loadNeXus2Events(char *filename) {
  pNEventArray nxData = NULL;

  if (strstr(filename, "amor") != NULL) {
    nxData = loadAMOR(filename);
  } else if (strstr(filename, "focus") != NULL) {
    nxData = loadFOCUS(filename);
  } else if (strstr(filename, "rita2") != NULL) {
    nxData = loadRITA2(filename);
  } else
    printf("Filetype not supported\n");

  return nxData;
}
