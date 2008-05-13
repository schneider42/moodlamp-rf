#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "avr/io.h"

#include "rf12config.h"
#include "rf12.h"
#include "rf12packet.h"
//#include "uart.h"

unsigned char state = STATE_IDLE;
unsigned char rf12packet_status = 0;
unsigned char count = 0;
unsigned char retries = 0;
unsigned char outpacket[50];
unsigned char outlen = 0;
unsigned char inpacket[50];
unsigned char acked = 0;
unsigned char seq = 0;
unsigned char localadr;
unsigned char rf12packet_data[50];
unsigned char rf12packet_datalen = 0;
unsigned char rf12packet_sniff = 0;

unsigned char multiadr = 1;

void rf12packet_init(unsigned char adr)
{
    localadr = adr;
}

unsigned char rf12packet_getstatus(void)
{
    return rf12packet_status;
}

void rf12packet_sendpacket(void)
{
    rf12_allstop();
    unsigned char c = rf12_txstart(outpacket,outlen);
    if(c){
#ifdef RF12DEBUG
        printf("sendpacket ret: %u\r\n", c);
#endif
#ifdef RF12DEBUGBIN
        printf("acDsendpaacket ret: %uab", c);
#endif
    }
}

void rf12packet_tick(void)      //every 1ms ~ 2bytes
{
    unsigned char ret = rf12_rxfinish(inpacket);
    if(ret != 255 && ret != 254 && ret != 0){
        rf12packet_process(inpacket,ret);
    }else{                                  //len== 0 || no new data ||
                                            //rx not finished || crc error
#ifdef RF12DEBUGBIN
        if(ret == 0)           //len or crc
            printf("acEcrc errorab");       //propably crc
#endif
    }
/*    if(rf12_txfinished() == 0){
        rf12_rxstart();
        
    }else
        return;
        */
    if(rf12_txfinished()){          //!=0 when transmitting
        return;
    }
    switch(state){
        case STATE_IDLE:
            rf12_rxstart();         //aborts if already receiving
        break;
        case STATE_SENDPACKET:
            if(retries--)
                state = STATE_WAITFREE;
            else{
                rf12packet_status |= RF12PACKET_TIMEOUT;
                state = STATE_IDLE;
            }
        break;
        case STATE_WAITFREE:        //wait for clean air
            ret = ((rf12_trans(0x0000)>>8) & (1<<0));   //clock recovery bit
            if(!ret){
            //if(RF12_Index == 0){
                count = 3; //+ binexprnd[0...10];   //Todo: random exponential
                                                    //backoff needed
                state = STATE_WAITRND;              //wait some time(csma)
#ifdef RF12DEBUG
                printf("free\r\n");
#endif
#ifdef RF12DEBUGBIN
                printf("acDfreeab");
#endif
            }else{
#ifdef RF12DEBUG
                printf("busy\r\n");
#endif
#ifdef RF12DEBUGBIN
                printf("acDbusyab");
#endif
            }
        break;
        case STATE_WAITRND:
            if(--count == 0){                   //when waiting check again
                ret = ((rf12_trans(0x0000)>>8) & (1<<0));
                if(ret){
                //if(RF12_Index){
                    state = STATE_WAITFREE;     //busy air ...
                }else{
                    //printf("send\r\n");
                    rf12packet_sendpacket();    //go kids, go
                    state = STATE_SEND;
                }
            }
        break;
        case STATE_SEND:
            if(rf12_txfinished() == 0){         //finished?
                rf12_rxstart();                 //recv ack
                count = 20;                     //max. 20ms pour le ack
                acked = 0;
                if(retries != 0)                //bb mc set this to zero
                    state = STATE_WAITACK;
                else
                    state = STATE_IDLE;
            }
        break;
        case STATE_WAITACK:
            rf12_rxstart();             //we might have sent an ack
                                        //for a packet and turned the tx on
            if(acked){
                rf12packet_status |= RF12PACKET_PACKETDONE;     //tell the app
                rf12_rxstart();         //start rx again
                state = STATE_IDLE;
            }else{
                if(--count == 0){       //no ack after 20ms
                    state = STATE_SENDPACKET;
#ifdef RF12DEBUG
                    printf("nack\r\n");
#endif
#ifdef RF12DEBUGBIN
                    printf("acDnaackab");
#endif
                }
            }
        break;
    }
}
#define MAX_HOSTS   10
static unsigned char seqs[MAX_HOSTS][2];

unsigned char checkseq(unsigned char host, unsigned char seq)
{
    unsigned char i;
    unsigned char rc;
    static unsigned char seqrr = 0;
    for(i=0;i<MAX_HOSTS;i++){
        if(seqs[i][0] == host){                     //host known
            rc = seqs[i][1]!=seq?1:0;               //check for old seq
            seqs[i][1] = seq;
            return rc;
        }
    }                                               //host unknown
    seqs[seqrr][0] = host;                          //add host to table
    seqs[seqrr++][1] = seq;
    if(seqrr == MAX_HOSTS)                          //round robin style
        seqrr = 0;
    return 1;
}

void rf12packet_process(unsigned char * packet, unsigned char len)
{
    unsigned char buf[10];
    if(packet[0] == 'A' && packet[1] == (unsigned char)(seq+1) && 
                           packet[2] == localadr){          //check for our ack
        acked = 1;
        seq++;
        //printf("acked\r\n");
    }
    if(packet[0] == 'M' && (packet[2] == multiadr ||    //this is a multicast
                            packet[2] == 255 ||         // do our domain
                            rf12packet_sniff)){
        //if(checkseq(packet[3],packet[1]) || rf12packet_sniff){  //check seq
            memcpy(rf12packet_data,packet,len);
            rf12packet_datalen = len;
            rf12packet_status |= RF12PACKET_NEWDATA;
        //}
    }

    if(packet[0] == 'P' && (packet[2] == localadr ||    //packet or broadcast
                            packet[2] == 255 ||
                            rf12packet_sniff)){
        if(packet[2] == localadr){          //is packet is for us ack it
            buf[0]='A';
            buf[1]=packet[1]+1;
            buf[2]=packet[3];
            buf[3]=localadr;
#ifdef RF12DEBUGBIN
//            printf("acDaacking\r\nab");
#endif
            rf12_allstop();                 //propably in rx ;)
            rf12_txstart(buf,4);
        }
        if(checkseq(packet[3],packet[1]) || rf12packet_sniff){
            memcpy(rf12packet_data,packet,len);
            rf12packet_datalen = len;
            rf12packet_status |= RF12PACKET_NEWDATA;
        }
    }
    return;
}

unsigned char rf12packet_incrseq(void)      //used for not mc and bc packets
{                                           //as they don't get acked
    seq++;
    return seq;
}


unsigned char rf12packet_send(unsigned char adr, unsigned char * packet, unsigned char len)
{
    if(state != STATE_IDLE)
        return 1;
    state = STATE_SENDPACKET;
    outpacket[0] = 'P';
    outpacket[1] = seq;
    outpacket[2] = adr;
    outpacket[3] = localadr;
    memcpy(outpacket+4,packet,len);
    outlen = len+4;
    retries = 10;
    return 0;
}

unsigned char rf12packet_isidle(void)
{
    return state==STATE_IDLE?1:0;
}

unsigned char rf12packet_sendmc(unsigned char adr, unsigned char * packet, unsigned char len)
{
    if(state != STATE_IDLE)
        return 1;
    state = STATE_SENDPACKET;
    outpacket[0] = 'M';
    outpacket[1] = seq;
    outpacket[2] = adr;
    outpacket[3] = localadr;
    memcpy(outpacket+4,packet,len);
    outlen = len+4;
    retries = 1;
    return 0;
}
