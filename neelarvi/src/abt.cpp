#include "../include/simulator.h"
#include "./common.cpp"

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

#define BUFFER_SIZE 1000

#define SS_SEND0 0
#define SS_ACK0 1
#define SS_SEND1 2
#define SS_ACK1 3

#define RS_WAIT0 0
#define RS_WAIT1 1


int server_state, receiver_state;

bool send_flag;

struct buffer{

    msg arr[BUFFER_SIZE];
    int front;
    int back;

} server_buffer;

void add_to_buffer(struct msg message){


    server_buffer.back++;


    for(int i=0; i < 20; i++){

        server_buffer.arr[server_buffer.back].data[i] = message.data[i];
    }


}

struct msg get_current_message(){

    return server_buffer.arr[server_buffer.front];

}

void update_success(){

    server_buffer.front++;

}


void send_abt_message(int is_retry){


    msg message = server_buffer.arr[server_buffer.front];

    int seqnum;

    if( server_state == SS_SEND0 || server_state == SS_ACK0){
            seqnum = 0;
    } else{
            seqnum = 1;
    }

    struct pkt packet = create_msg_packet(seqnum, message.data);

    tolayer3(0, packet);
    starttimer(0, 15.0);

    if(server_state == SS_SEND0){
        server_state = SS_ACK0;
    } else if(server_state == SS_SEND1){
        server_state = SS_ACK1;
    }

    std::cout<< "sent " << packet.payload<<" with checksum " << packet.checksum << " seq num "<< packet.seqnum <<std::endl ;
}


/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{



std::cout<< "new msg " << message.data<<std::endl ;

    add_to_buffer(message);



    if(send_flag){

        send_abt_message(0);

        send_flag = false;

    }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{



    int checksum = get_checksum(packet);

    std::cout << "received ack with checksum "<< packet.checksum << " pn "<< packet.acknum<< " cal checksum "<< checksum<< std::endl;

    if(checksum == packet.checksum &&
        (server_state == SS_ACK0 && packet.acknum == 0 || server_state == SS_ACK1 && packet.acknum == 1)){

        stoptimer(0);
        update_success();

        if(server_state == SS_ACK0){
            server_state = SS_SEND1;
        } else {
            server_state = SS_SEND0;
        }

        if(server_buffer.front <= server_buffer.back){

            send_abt_message(0);

        } else {
            send_flag  = true;
        }


    }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{

    std::cout << "timer interrupt";
    send_abt_message(1);

}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

server_buffer.back = -1;
server_buffer.front = 0;

server_state = SS_SEND0;

send_flag = true;



}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{


    int checksum = get_checksum(packet);

    std::cout<< "received "<< packet.payload<<" with checksum " << checksum << " seq num "<< packet.seqnum<< " curr state "<< receiver_state<< std::endl ;


    if(checksum == packet.checksum && (receiver_state == RS_WAIT0 && packet.seqnum == 0 || receiver_state == RS_WAIT1 && packet.seqnum == 1) ){

        char msg[20];

        for(int i=0; i <20; i++){

            msg[i] = packet.payload[i];
        }


        tolayer5(1, msg);

        int seq_num = packet.seqnum;

        pkt packet = create_ack_packet(seq_num);

        tolayer3(1, packet);

        if(receiver_state == RS_WAIT0){

            receiver_state = RS_WAIT1;

        } else {

            receiver_state = RS_WAIT0;
        }


    } else {

        pkt packet;

        if(receiver_state == RS_WAIT0){

            packet = create_ack_packet(1);

         } else {

            packet = create_ack_packet(0);
         }

         tolayer3(1, packet);



    }


}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

    receiver_state = RS_WAIT0;


}




