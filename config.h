/* defines what kind of communications we want for the socket */
#define COMM_TYPE PULLPUSH

#if COMM_TYPE == PULLPUSH    /* PUSH PULL socket */

#define SEND_TYPE ZMQ_PUSH
#define RECV_TYPE ZMQ_PULL

#elif COMM_TYPE == PUBSUB   /* PUB SUB socket */

#define SEND_TYPE ZMQ_SUB
#define RECV_TYPE ZMQ_PUB

#endif
