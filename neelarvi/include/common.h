#ifndef COMMON_H_
#define COMMON_H_

#define BUFFER_SIZE 10000
#define ACK 0
#define UNACK 1
#define NOTSENT 2
#define EMPTY 3


#


int get_checksum(struct pkt packet);

struct w_buffer{

    msg arr[BUFFER_SIZE];
    int ack[BUFFER_SIZE];

    int index;

    int w_size;

    int base;
    int nextseqnum;

};


#endif
