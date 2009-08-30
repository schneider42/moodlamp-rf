#include "config.h"
#include "interfaces.h"


struct interface ifaces[] = 
{
#if SERIAL_UART
{IFACE_SERIAL, &serial_packetOut,       &serial_packetIn,       &serial_nextHeader,         &serial_ready,      &serial_tick,       1, 1, 10, 1},
#endif
{IFACE_RS485,  &rs485_packetOut,        &rs485_packetIn,        &rs485_nextHeader,          &rs485_ready,       &rs485_tick,        0, 0,20, 1},
{IFACE_RF,     &rf12packet_packetOut,   &rf12packet_packetIn,   &rf12packet_nextHeader,     &rf12packet_ready,  &rf12packet_tick,   0, 0, 30, 1}

};

uint8_t iface_num = sizeof(ifaces)/sizeof(struct interface);

/*void interfaces_init(void)
{
    iface_num = sizeof(ifaces)/sizeof(struct interface);
}*/

void interfaces_setEnabled(uint8_t iface, uint8_t enabled)
{
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface)
            ifaces[i].enabled = enabled;
}

uint8_t interfaces_isEnabled(uint8_t iface)
{
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface)
            return ifaces[i].enabled;
    return 0;
}
uint8_t interfaces_getMetric(uint8_t iface)
{ 
    
//    uart1_puts("acDWab");
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface){
            
 //   uart1_puts("acDZab");
            return ifaces[i].metric;
        }

 //   uart1_puts("acDYab");
    return 255;
}

void interfaces_tick(void){
    uint8_t i;

    for(i=0;i<iface_num;i++){
 //       uart1_puts("acDUab");
        if(ifaces[i].enabled){
 //           uart1_puts("acDuab");
            ifaces[i].tick();
        }
    }
 //   uart1_puts("acDVab");
}

uint8_t interfaces_isReady(uint8_t iface)
{
    uint8_t i;
 //   uart1_puts("acDPab");
    if(iface == IFACE_LOCAL || iface == IFACE_NONE){     //always ready hack
        
 //   uart1_puts("acDQab");
        return 1;
    }
    for(i=0;i<iface_num;i++){
        if(ifaces[i].id == iface){
            if(ifaces[i].enabled){
 //   uart1_puts("acDRab");
                return ifaces[i].isReady();
            }else{
                
 //   uart1_puts("acDSab");
                return 1;
            }
        }
    }
 //   uart1_puts("acDTab");
    return 0;
}

uint8_t interfaces_getPacket(uint8_t iface, struct packet_t * p)
{
 //   uart1_puts("acDLab");
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface){
            i = ifaces[i].packetIn(p);
            p->iface = iface;
            /*if(p->iface == 1)
                while(1);*/

 //   uart1_puts("acDMab");
            return i;
        }
 //   uart1_puts("acDOab");
    return 1;
}

uint8_t interfaces_gotPacket(struct packet_t * p)
{
    uint8_t i;

 //   uart1_puts("acDIab");
    for(i=0;i<iface_num;i++)
        if(ifaces[i].enabled && ifaces[i].nextHeader(p)){
            p->iface = ifaces[i].id;
            //if(ifaces[i].id == 1)
            //    return IFACE_NONE;
            //
 //           uart1_puts("acDJab");
            return ifaces[i].id;
        }
 //   uart1_puts("acDKab");
    return IFACE_NONE;
}

uint8_t interfaces_packetOut(uint8_t id, struct packet_t * p)
{
 //   uart1_puts("acDEab");
    uint8_t i;
    for(i=0;i<iface_num;i++){
        if(id == ifaces[i].id){
            if(ifaces[i].enabled){
 //   uart1_puts("acDFab");
                return ifaces[i].packetOut(p);
            }else{
 //   uart1_puts("acDGab");
                return 0;
            }
        }
    }
 //   uart1_puts("acDHab");
    return 1;
}

uint8_t interfaces_broadcast(uint8_t originid, struct packet_t * p, uint8_t force)
{
    uint8_t i;
    uint8_t temp = 0;
 //   uart1_puts("acDAab");
    for(i=0;i<iface_num;i++){
        if(ifaces[i].id == originid && ifaces[i].dobroadcast)
            temp = 1;
    }

 //   uart1_puts("acDBab");
    if(temp || force){
        for(i=0;i<iface_num;i++){
            if(originid != ifaces[i].id && ifaces[i].enabled){
    //            do{    
    //                uart1_puts("acDW");uart1_putc(i);uart1_putc(state);uart1_puts("ab");
    //            }while(!ifaces[i].isReady());
                //if(!(ifaces[i].isReady()))
                //    uart1_puts("acNRab");
                //if(ifaces[i].packetOut(p))
                //    return 1;
                ifaces[i].packetOut(p);
            }
        }

 //       uart1_puts("acDCab");
        return 0;
    }
    //uart1_puts("acDMab");
    for(i=0;i<iface_num;i++){
        if(originid != ifaces[i].id && ifaces[i].getbroadcast && ifaces[i].enabled)
            ifaces[i].packetOut(p);
    }

 //   uart1_puts("acDDab");
    return 0;
}

