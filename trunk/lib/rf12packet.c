#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "avr/io.h"

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
    }else{
#ifdef RF12DEBUGBIN
        if(ret == 0)
            printf("acEcrc errorab");
#endif
    }
/*    if(rf12_txfinished() == 0){
        rf12_rxstart();
        
    }else
        return;
        */
    if(rf12_txfinished()){
        return;
    }
    switch(state){
        case STATE_IDLE:
            rf12_rxstart();
        break;
        case STATE_SENDPACKET:
            if(retries--)
                state = STATE_WAITFREE;
            else{
                rf12packet_status |= RF12PACKET_TIMEOUT;
                state = STATE_IDLE;
            }
        break;
        case STATE_WAITFREE:
            ret = ((rf12_trans(0x0000)>>8) & (1<<0));
            if(!ret){
            //if(RF12_Index == 0){
                count = 3; //+ binexprnd[0...10];
                state = STATE_WAITRND;
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
            if(--count == 0){
                ret = ((rf12_trans(0x0000)>>8) & (1<<0));
                if(ret){
                //if(RF12_Index){
                    state = STATE_WAITFREE;
                }else{
                    //printf("send\r\n");
                    rf12packet_sendpacket();
                    state = STATE_SEND;
                }
            }
        break;
        case STATE_SEND:
            if(rf12_txfinished() == 0){
                rf12_rxstart();
                count = 20;
                state = STATE_WAITACK;
            }
        break;
        case STATE_WAITACK:
            if(acked){
                acked = 0;
                rf12packet_status |= RF12PACKET_PACKETDONE;
                rf12_rxstart();
                state = STATE_IDLE;
            }else{
                if(--count == 0){
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

void rf12packet_process(unsigned char * packet, unsigned char len)
{
    //static int remoteseq = -1;
    unsigned char buf[10];
    //UART_Tx_Str("recv:",5);
    if(packet[0] == 'A' && packet[1] == (unsigned char)(seq+1) && packet[2] == localadr){
        acked = 1;
        seq++;
        //printf("acked\r\n");
    }

    if(packet[0] == 'P' && (packet[2] == localadr || rf12packet_sniff)){
        if(packet[2] == localadr){
            buf[0]='A';
            buf[1]=packet[1]+1;
            buf[2]=packet[3];
            buf[3]=localadr;
#ifdef RF12DEBUGBIN
//            printf("acDaacking\r\nab");
#endif
            rf12_allstop();
            rf12_txstart(buf,4);
        }
        memcpy(rf12packet_data,packet,len);
        rf12packet_datalen = len;
        rf12packet_status |= RF12PACKET_NEWDATA;

        //if(packet[1] != remoteseq){
            /*if(len > 4){
                printf("Packet data: ");
                packet[len] = 0;
                printf((char*)packet+4);
                printf("\r\n");
            }
            */
        //    remoteseq = packet[1];
        //}
    }
    return;
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
