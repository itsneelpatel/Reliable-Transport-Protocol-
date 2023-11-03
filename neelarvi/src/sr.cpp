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


w_buffer sender_buffer, receiver_buffer;


void handle_receive(struct pkt packet);


/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    push_to_buffer(&sender_buffer, message);

    send_unsent_messages(&sender_buffer);

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{

    int acknum = packet.acknum;

        std::cout<< "received ack for "<< acknum<<std::endl ;


    if(!is_pkt_corrupt(packet) && is_within_window(acknum, &sender_buffer) ){


        sender_buffer.ack[acknum] = ACK;

        if(sender_buffer.ack[sender_buffer.base + 1] == ACK){

            stoptimer(0);

            std::cout<< "base moved from "<< sender_buffer.base;

            while(sender_buffer.ack[sender_buffer.base + 1] == ACK){
                sender_buffer.base++;
            }

            std::cout<< " to "<< sender_buffer.base<< std::endl;


            starttimer(0, 35.0);

        }

    }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{

    std::cout<< "timer interrupt. resending msgs"<< std::endl;

    send_unack_messages(&sender_buffer);

}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

    sender_buffer.w_size = getwinsize();
    sender_buffer.index = -1;

    sender_buffer.base = -1;


    //fill the buffere with empty state initially
    for(int i = 0; i < BUFFER_SIZE; i++){
        sender_buffer.ack[i] = EMPTY;
    }





}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    int seqnum = packet.seqnum;

    std::cout<< "received new msg"<< packet.payload << " is corrupt " << is_pkt_corrupt(packet) << " is within window "<< is_within_window(seqnum, &receiver_buffer)  <<std::endl ;


    if(!is_pkt_corrupt(packet) && is_within_window(seqnum, &receiver_buffer)){


        handle_receive(packet);

        pkt ack = create_ack_packet(seqnum);

        tolayer3(1, ack);

    }


    if(!is_within_window(seqnum, &receiver_buffer)){


        int base = receiver_buffer.base;
        int wsize = receiver_buffer.w_size;

        if( base - wsize < base  && seqnum <= base){

            pkt ack = create_ack_packet(seqnum);

            tolayer3(1, ack);

        }


    }

}

void handle_receive(struct pkt packet){


    int seqnum = packet.seqnum;

    for(int i=0; i < 20; i++){

        receiver_buffer.arr[seqnum].data[i] = packet.payload[i];
    }

    receiver_buffer.ack[seqnum] = ACK;


    while(receiver_buffer.ack[receiver_buffer.base + 1] == ACK){

        char msg[20];

        for(int i=0; i < 20; i++){

            msg[i] =  receiver_buffer.arr[receiver_buffer.base + 1].data[i];

        }

        std::cout<< "delivered to upper layer. msg "<< msg <<std::endl ;

        tolayer5(1, msg);


        receiver_buffer.base++;

    }


}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    receiver_buffer.w_size = getwinsize();
    receiver_buffer.base = -1;

    for(int i=0; i < BUFFER_SIZE; i++){

        receiver_buffer.ack[i] = EMPTY;
    }


}
