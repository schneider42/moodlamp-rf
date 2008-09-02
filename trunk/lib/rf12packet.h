#ifndef __RF12PACKET_H_
#define __RF12PACKET_H_

void rf12packet_tick(void);
void rf12packet_setadr(unsigned char adr);
void rf12packet_process(unsigned char * packet, unsigned char len);
unsigned char rf12packet_send(unsigned char adr, unsigned char * packet, unsigned char len);
void rf12packet_init(unsigned char adr);
unsigned char rf12packet_getstatus(void);
unsigned char rf12packet_incrseq(void);

extern unsigned char rf12packet_data[50];
extern unsigned char rf12packet_datalen;
unsigned char rf12packet_status;
extern unsigned char rf12packet_sniff;
unsigned char rf12packet_isidle(void);
unsigned char rf12packet_sendmc(unsigned char adr, unsigned char * packet, unsigned char len);
#define STATE_SENDPACKET    0
#define STATE_WAITFREE      1
#define STATE_WAITRND       2
#define STATE_SEND          3
#define STATE_WAITACK       4
#define STATE_IDLE          5


#define PACKET_ACK          0
#define PACKET_DATA         1
#define PACKET_DATAACK      2

#define RF12PACKET_NEWDATA  1
#define RF12PACKET_TIMEOUT  2
#define RF12PACKET_PACKETDONE 4
#endif
