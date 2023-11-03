#include "../include/common.h"
#include <cstring>
#include <iostream>

struct pkt create_ack_packet(int seq_number){

     pkt packet;

     packet.acknum = seq_number;

     strcpy(packet.payload , "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

     packet.seqnum = -1;

     packet.checksum = get_checksum(packet);

     return packet;

}


struct pkt create_msg_packet(int seqnum, char data[20]){


    pkt packet;

     for(int i=0; i <20; i++){

        packet.payload[i] = data[i];
    }

    packet.seqnum = seqnum;
    packet.acknum = -1;


    packet.checksum = get_checksum(packet);

    return packet;

}

int get_checksum(struct pkt packet){

    int hashcode = 0;

    for(int i = 19 ; i >= 0; i--){

        hashcode = 31 * hashcode + packet.payload[i];

    }

     hashcode = 31 * hashcode + packet.seqnum;

     hashcode = 31 * hashcode + packet.acknum;


    return hashcode;

}

bool is_pkt_corrupt(struct pkt packet){

    int expected_chksum = get_checksum(packet);

    int actual_chksum = packet.checksum;

    return expected_chksum != actual_chksum;

}

bool is_within_window( int seqnum, w_buffer* buffer){

    int base = buffer->base;
    int win_size = buffer->w_size;

    return base < seqnum && seqnum <= base + win_size;

}

void push_to_buffer(w_buffer* buffer, msg message){




    buffer ->index++;

    for(int i=0; i < 20; i++){

        buffer->arr[buffer->index].data[i] = message.data[i];
    }

    buffer->ack[buffer->index] = NOTSENT;


    std::cout<< "new msg " << message.data<<" added to buffer. curr status  " << buffer->ack[buffer->index] <<std::endl ;



}

void send_unsent_messages(w_buffer* buffer){


    int base = buffer -> base;

    int win_size = buffer ->w_size;


    for(int i = base + 1; i <= base + win_size; i++){

        if( buffer->ack[i] == NOTSENT) {

            //if it is the first message of the window, set a timer.
            if(i == base + 1){
                starttimer(0, 35.0);
            }


            pkt packet = create_msg_packet(i, buffer ->arr[i].data);

            tolayer3(0, packet);

            buffer->ack[i] = UNACK;


            std::cout<< "sent msg " << packet.payload<<" seq number "<< packet.seqnum << " chksum "<< packet.checksum<< " buffer status " << buffer->ack[i] <<std::endl ;

        }

    }


}


void send_unack_messages(w_buffer* buffer){


    int base = buffer-> base;

    int win_size = buffer ->w_size;


    for(int i = base + 1; i <= base + win_size; i++){

        if( buffer->ack[i] == NOTSENT || buffer->ack[i] == UNACK) {

            //if it is the first message of the window, set a timer.
            if(i == base + 1){
                starttimer(0, 35.0);
            }


            pkt packet = create_msg_packet(i, buffer ->arr[i].data);

            tolayer3(0, packet);

            buffer->ack[i] = UNACK;

            std::cout<< "sent msg " << packet.payload<<" seq number "<< packet.seqnum << " chksum "<< packet.checksum<< " buffer status " << buffer->ack[i] <<std::endl ;


        }

    }


}


void send_all_messages(w_buffer* buffer){


    int base = buffer -> base;

    int win_size = buffer ->w_size;


    for(int i = base + 1; i <= base + win_size; i++){

        if( buffer->ack[i] == NOTSENT || buffer->ack[i] == UNACK || buffer->ack[i] == ACK) {

            //if it is the first message of the window, set a timer.
            if(i == base + 1){
                starttimer(0, 35.0);
            }


            pkt packet = create_msg_packet(i, buffer ->arr[i].data);

            tolayer3(0, packet);

            buffer->ack[i] = UNACK;

            std::cout<< "sent msg " << packet.payload<<" seq number "<< packet.seqnum << " chksum "<< packet.checksum<< " buffer status " << buffer->ack[i] <<std::endl ;


        }

    }


}



