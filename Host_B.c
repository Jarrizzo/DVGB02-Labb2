
//Host_B.c

#include "Sim_Engine.h"
#include <stdbool.h>
#include <string.h>
#include "Host_B.h"

#define A 0
#define B 1
int prevSeq;

bool isCorrupted(struct pkt pkt_A);

/* Called from layer 5, passed the data to be sent to other side */
void B_output( struct msg message) {
  /* DON'T IMPLEMENT */
}

/* Called from layer 3, when a pkt_A arrives for layer 4 */
void B_input(struct pkt pkt_A) {

  //printf("B_input: %s\n",pkt_A.payload);

  if(pkt_A.seqnum == 0){
    if(isCorrupted(pkt_A)){
      pkt_A.acknum = 1;
      //printf("Packet is corrupted -- Sending ACK %d",pkt_A.acknum);
      tolayer3(B, pkt_A);
    }
    else{
      pkt_A.acknum = 0;
      if(pkt_A.seqnum == prevSeq){
        //printf("Packet is a duplicate, throwing pkt_A\n");
      }
      else{
        tolayer5(B,pkt_A.payload);
        prevSeq = pkt_A.seqnum;
      }
      tolayer3(B,pkt_A);
    }
  }
  else{
    if(isCorrupted(pkt_A)){
      pkt_A.acknum = 0;
      //printf("Packet is corrupted -- Sending ACK %d\n",pkt_A.acknum);
      tolayer3(B,pkt_A);
    }
    else{
      pkt_A.acknum = 1;
      if(pkt_A.seqnum == prevSeq){
        //printf("Packet is a duplicate, throwing pkt_A\n");
      }
      else{
        tolayer5(B,pkt_A.payload);
        prevSeq = pkt_A.seqnum;
      }
    tolayer3(B,pkt_A);
    }
  }
}

/* Called when B's timer goes off */
void B_timerinterrupt() {
  /* TODO */
}  

/* The following routine will be called once (only) before any other */
/* Host B routines are called. You can use it to do any initialization */
void B_init() {
  prevSeq = 1;
}

bool isCorrupted(struct pkt pkt_A){
  int sum = 0;
  for(int i = 0; pkt_A.payload[i] != '\0' ;i++){
    sum += pkt_A.payload[i];
  }

  if(sum != pkt_A.checksum){
    return true;
  }
  return false;
}
