#include <stdio.h>
#include <string.h>

#include "protocol.h"
#include "datalink.h"

#define MAX_SEQ 31
#define DATA_TIMER 3800//calculagraph timer
#define ACK_TIMER 1100
#define NR_BUFS 16   //windows number
#define inc(k)if(k<MAX_SEQ)k++;else k=0

typedef enum{false,true}bool;
typedef unsigned char seq_nr;//sequence or ack number
typedef struct{
	unsigned char info[PKT_LEN];
}packet;//packet definition

static int phl_ready = 0;

bool no_nak=true;

typedef struct FRAME { 
    unsigned char kind; // FRAME_DATA 
    seq_nr ack;//acknowledgement number
    seq_nr  seq;//sequence number
    packet data; //the network layer packet
    unsigned int  padding;
}Frame;

static int between(seq_nr a,seq_nr b,seq_nr c){
      if((a <= b && b < c) || (c < a && a <= b) || (b < c && c < a))
          return true;
      else
          return false; 
}
//add crc
static void put_frame(unsigned char *frame, int len)
{
    *(unsigned int *)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = 0;
}
//send data to physical layer
static void send_data(unsigned char fk,seq_nr frame_nr,seq_nr frame_expected,packet buffer[])
{
    Frame s;
    s.kind = fk;
    s.seq = frame_nr;//insert sequence number into frame
    s.ack = (frame_expected+ MAX_SEQ)%(MAX_SEQ+1);//piggyback ack

	if(fk == FRAME_DATA)
		memcpy(s.data.info, buffer[frame_nr%NR_BUFS].info, PKT_LEN);//insert packet into frame
	
	if(fk == FRAME_NAK)
		no_nak = false;  //one nak per frame,please

    dbg_frame("Send DATA %d %d, ID %d\n", s.seq, s.ack, *(short *)s.data.info);
	if (fk == FRAME_DATA)
     	put_frame((unsigned char *)&s, 3 + PKT_LEN);//put CRC following the frame	
	if(fk == FRAME_NAK||fk==FRAME_ACK)
		put_frame((unsigned char *)&s, 2);//put CRC following the frame
    if(fk == FRAME_DATA)
		start_timer(frame_nr%NR_BUFS, DATA_TIMER);
	stop_ack_timer();//no need for separate ack frame
}

int main(int argc, char **argv){
    
	int event,arg,len=0;
    int i;//contol the number of circle of arrived
    Frame r;//scratch variable
    seq_nr next_frame_to_send;//the upper of the send's window
    seq_nr ack_expected;//the lower of the send's window
    seq_nr frame_expected;//the lower of the reciver's window
    seq_nr too_far;//the upper of the reciver's window
    seq_nr nbuffered;//currently window
    packet out_buf[NR_BUFS];//buffer for the outbound stream
    packet in_buf[NR_BUFS];//buffer for the in stream
    bool arrived[NR_BUFS];
    
    enable_network_layer();
    //disable_network_layer();//allow netword_layer_ready events
    ack_expected = 0;
    next_frame_to_send = 0;
    frame_expected = 0;
    too_far = NR_BUFS;
    nbuffered = 0;  
    for(i = 0;i < NR_BUFS;i++)
        arrived[i] = false;

	protocol_init(argc, argv); 
    lprintf("Designed by chenchen_07415_071419, build: " __DATE__"  "__TIME__"\n\n");

    while(true){
        event = wait_for_event(&arg);
        switch(event){
            case NETWORK_LAYER_READY:
				   nbuffered++;//expand the sender's window
                   get_packet(out_buf[next_frame_to_send%NR_BUFS].info);//fetch new packet
                   send_data(FRAME_DATA,next_frame_to_send,frame_expected,out_buf);//transmit the packet
			       inc(next_frame_to_send);//advance the upper edge of sender's window
                   break;
            case PHYSICAL_LAYER_READY:
                 phl_ready = 1;
                 break;
            case FRAME_RECEIVED:
                 len = recv_frame((unsigned char *)&r, sizeof r);//from physical frame's len
                 if (len < 5 || crc32((unsigned char *)&r, len) !=0){//CRC error
                     dbg_event("**** Receiver Error, Bad CRC Checksum\n");
                     if(no_nak)
                         send_data(FRAME_NAK,0,frame_expected,out_buf);
                     break;
                 }
                 if(r.kind == FRAME_DATA){
                    if((r .seq != frame_expected)&&no_nak)//not arrival in the right order
                         send_data(FRAME_NAK,0,frame_expected,out_buf);//send nac and send again
                    else
                         start_ack_timer(ACK_TIMER);
                    if(between(frame_expected,r.seq,too_far)&&(arrived[r.seq%NR_BUFS]==false)){
                         arrived[r.seq%NR_BUFS] = true;//the frame arrival
					     //for(j=0;j<NR_BUFS;j++)
                          in_buf[r.seq%NR_BUFS] = r.data;//keep in the in_buf 
                         while(arrived[frame_expected%NR_BUFS]){
                             put_packet(in_buf[frame_expected%NR_BUFS].info, len-7);
                             no_nak = true;
                             arrived[frame_expected%NR_BUFS]=false;
                             inc(frame_expected);//advance upper edge of ceiver's windowsge
                             inc(too_far);
                             start_ack_timer(ACK_TIMER);
                         }  
                    }
                 }
                 if((r.kind == FRAME_NAK)&&between(ack_expected,(r.ack+1)%(MAX_SEQ+1),next_frame_to_send))
                 //receive nak &&between the windows 
                     send_data(FRAME_DATA,(r.ack+1)%(MAX_SEQ+1),frame_expected,out_buf);
                 while(between(ack_expected,r.ack,next_frame_to_send)){
                       nbuffered--;
                       stop_timer(ack_expected%NR_BUFS);
                       inc(ack_expected);
                 }
                 break;
            case ACK_TIMEOUT:
				 dbg_event("---- DATA %d timeout\n", arg);
                 send_data(FRAME_ACK,0,frame_expected,out_buf);
                 break;
            case DATA_TIMEOUT:
                 dbg_event("---- DATA %d timeout\n", arg); 
		         if(! between(ack_expected,arg,next_frame_to_send))
                    arg = arg + NR_BUFS;
                 send_data(FRAME_DATA,arg,frame_expected,out_buf);//select the bad frame and resend it 
                 break;
        }
        if (nbuffered <NR_BUFS&& phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
    }
}            
