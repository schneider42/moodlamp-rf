#include "config.h"
#include "interfaces.h"


struct interface ifaces[] = 
{
#if SERIAL_UART
    {IFACE_SERIAL, &serial_packetOut,  &serial_packetIn,   &serial_nextHeader, &serial_ready,    &serial_tick,  1,  1, 10},
#endif
{IFACE_RS485,  &rs485_packetOut,   &rs485_packetIn,   &rs485_nextHeader,  &rs485_ready,     &rs485_tick, 0,0,20},
{IFACE_RF,     &rf12packet_packetOut,      &rf12packet_packetIn,       &rf12packet_nextHeader,     &rf12packet_ready,        &rf12packet_tick,  0,  0, 30}

};

uint8_t iface_num = sizeof(ifaces)/sizeof(struct interface);

/*void interfaces_init(void)
{
    iface_num = sizeof(ifaces)/sizeof(struct interface);
}*/

uint8_t interfaces_getMetric(uint8_t iface)
{ 
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface)
            return ifaces[i].metric;
    return 255;
}

void interfaces_tick(void){
    uint8_t i;
    for(i=0;i<iface_num;i++)
        ifaces[i].tick();
}

uint8_t interfaces_isReady(uint8_t iface)
{
    uint8_t i;
    if(iface == IFACE_LOCAL || iface == IFACE_NONE)     //always ready hack
        return 1;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface)
            return ifaces[i].isReady();
    return 0;
}

uint8_t interfaces_getPacket(uint8_t iface, struct packet_t * p)
{
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].id == iface){
            i = ifaces[i].packetIn(p);
            p->iface = iface;
            /*if(p->iface == 1)
                while(1);*/

            return i;
        }
    return 1;
}

uint8_t interfaces_gotPacket(struct packet_t * p)
{
    uint8_t i;
    for(i=0;i<iface_num;i++)
        if(ifaces[i].nextHeader(p)){
            p->iface = ifaces[i].id;
            //if(ifaces[i].id == 1)
            //    return IFACE_NONE;
            return ifaces[i].id;
        }
    return IFACE_NONE;
}

uint8_t interfaces_packetOut(uint8_t id, struct packet_t * p)
{
    uint8_t i;
    for(i=0;i<iface_num;i++){
        if(id == ifaces[i].id){
            return ifaces[i].packetOut(p);
        }
    }
    return 1;
}

uint8_t interfaces_broadcast(uint8_t originid, struct packet_t * p, uint8_t force)
{
    uint8_t i;
    uint8_t temp = 0;
    for(i=0;i<iface_num;i++){
        if(ifaces[i].id == originid && ifaces[i].dobroadcast)
            temp = 1;
    }
    if(temp || force){
        for(i=0;i<iface_num;i++){
            if(originid != ifaces[i].id){
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
        return 0;
    }
    //uart1_puts("acDMab");
    for(i=0;i<iface_num;i++){
        if(originid != ifaces[i].id && ifaces[i].getbroadcast)
            ifaces[i].packetOut(p);
    }
    return 0;
}

