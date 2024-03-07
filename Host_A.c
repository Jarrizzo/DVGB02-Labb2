
//Host_A.c

#include "Sim_Engine.h"
#include "Host_A.h"
#include <stdbool.h>
#include <string.h>
#define MAXSIZE 100
#define A 0
#define B 1

static struct pkt pkt_A;
static struct msg Q[MAXSIZE];
static float waitingTime;
static bool isSending;
static int Qsize = 0;

void push(struct msg message);
struct msg pop();
int get_checksum(struct pkt packet);
int checksumA(struct msg message);

/* Called from layer 5, passed the data to be sent to other side */
void A_output( struct msg message) {

  if(isSending){
    push(message);
    return;
  }
  //printf("A_out: %s\n", message.data);

  memcpy(pkt_A.payload,message.data,sizeof(message.data));
  pkt_A.checksum = checksumA(message);
  if(pkt_A.seqnum == 0)
    pkt_A.seqnum = 1;
  else
    pkt_A.seqnum = 0;

  //printf("Pkt_A sequense number: %d\n",pkt_A.seqnum);

  isSending = true;
  tolayer3(A, pkt_A);
  starttimer(A,waitingTime);
}

/* Called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt pkt_B) {

  //printf("A_input: -- ACK: %d\n",pkt_B.acknum);
  stoptimer(A);

  if(pkt_B.acknum == pkt_A.seqnum || pkt_B.checksum == get_checksum(pkt_B)){
    //printf("\n\n\n\n\pkt_B.checksum : %d\nget_checksum(pkt_B) : %d\npkt_B.acknum: %d\npkt_A.seqnum: %d\n\n\n\n\n",pkt_B.checksum,get_checksum(pkt_B),pkt_B.acknum,pkt_A.seqnum);
    isSending = false;
    if(Qsize != 0){
      A_output(pop());
    }
  }
  else if(pkt_B.acknum != pkt_A.seqnum || pkt_B.checksum != get_checksum(pkt_B)){
    //printf("A_input: -- Packet corrupted -- resending packet...\nPayload: %s\n",pkt_A.payload);
    tolayer3(A,pkt_A);
    starttimer(A,waitingTime);
  }
}

/* Called when A's timer goes off */
void A_timerinterrupt() {
  //printf("TIMEINTERUP! -- Resending the packetnumber: %d --\n",pkt_A.seqnum);
  tolayer3(A,pkt_A);
  starttimer(A,waitingTime);
}  

/* The following routine will be called once (only) before any other */
/* Host A routines are called. You can use it to do any initialization */
void A_init() {
  pkt_A.acknum = 0;
  pkt_A.checksum = 0;
  pkt_A.seqnum = 1;
  waitingTime = 100;
  isSending = false;
}

// -- HELP FUNCTIONS --
void push(struct msg message){
  Q[Qsize] = message;
  Qsize++;
  return;
}

struct msg pop(){
  struct msg toBePoped = Q[0];
  for(int i = 0; i < Qsize-1; i++){
    Q[i] = Q[i+1];
  }
  Qsize--;
  return toBePoped;
}
//Returns the checksum of msg
int checksumA(struct msg message){
  int sum = 0;
  for(int i = 0; i < strlen(message.data); i++){
    sum += message.data[i];
  }
  return sum;
}
int get_checksum(struct pkt packet){
  int sum = 0;
  for(int i = 0; i < strlen(packet.payload); i++){
    sum += packet.payload[i];
  }
  sum += packet.acknum;
  sum += packet.seqnum;
  return sum;
}