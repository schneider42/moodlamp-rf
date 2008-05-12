#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#include "global.h"
#include "rf12config.h"
#include "rf12.h"
#include "uart.h"

struct RF12_stati
{
	unsigned char Rx:1;
	unsigned char Tx:1;
	unsigned char New:1;
};

#ifdef RF12_UseNoneBlockingCalls
struct RF12_stati RF12_status;
volatile unsigned char RF12_Index = 0;
unsigned char RF12_Data[RF12_DataLength+10];	// +10 == paket overhead
#endif

unsigned int crcUpdate(unsigned int crc, unsigned char serialData)
{
        unsigned int tmp;
        unsigned char j;

	tmp = serialData << 8;
        for (j=0; j<8; j++)
	{
                if((crc^tmp) & 0x8000)
			crc = (crc<<1) ^ 0x1021;
                else
			crc = crc << 1;
                tmp = tmp << 1;
        }
	return crc;
}
#ifdef RF12_UseNoneBlockingCalls
SIGNAL(SIG_INTERRUPT0)
{
    sei();
	if(RF12_status.Rx)
	{
		if(RF12_Index < RF12_DataLength){
		    unsigned char d  = rf12_trans(0xB000) & 0x00FF;
            if(RF12_Index == 0 && d > RF12_DataLength)
                d = RF12_DataLength;
	        RF12_Data[RF12_Index++]=d;
		}else
		{
			rf12_trans(0x8208);
			RF12_status.Rx = 0;
		}
		if(RF12_Index >= RF12_Data[0] + 3)		//EOT
		{
			rf12_trans(0x8208);
			RF12_status.Rx = 0;
			RF12_status.New = 1;
			//GICR &= ~(1<<INT0);		//disable int0
		}
	}
	else if(RF12_status.Tx)
	{
		rf12_trans(0xB800 | RF12_Data[RF12_Index]);
		if(!RF12_Index)
		{
			RF12_status.Tx = 0;
			rf12_trans(0x8208);		// TX off
		}
		else
		{
			RF12_Index--;
		}
	}
	else
	{
		rf12_trans(0x0000);			//dummy read
							//TODO: what happend
	}
}
#endif
#if 1
unsigned short rf12_trans(unsigned short wert)
{	
	unsigned short werti = 0;
	unsigned char i;
//cbi(RF_PORT, SCK);	
	cbi(RF_PORT, CS);
/* asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/
	for (i=0; i<16; i++)
	{	
		if (wert&32768)
			sbi(RF_PORT, SDI);
		else
			cbi(RF_PORT, SDI);
		werti<<=1;
		if (RF_PIN&(1<<SDO))
			werti|=1;
/**        asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/

		sbi(RF_PORT, SCK);
		wert<<=1;
		asm("nop");
/*        asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/
		cbi(RF_PORT, SCK);
/*        asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/

	}
/**            asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/

	sbi(RF_PORT, CS);
	return werti;
}
#else

unsigned short rf12_trans(unsigned short wert)
{	
	unsigned short werti = 0;
	unsigned char i;
cbi(RF_PORT, SCK);	
	cbi(RF_PORT, CS);
 asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);

	for (i=0; i<16; i++)
	{	
		werti<<=1;
		if (RF_PIN&(1<<SDO))
			werti|=1;
        cbi(RF_PORT,SCK);
		if (wert&32768)
			sbi(RF_PORT, SDI);
		else
			cbi(RF_PORT, SDI);

        asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);


		sbi(RF_PORT, SCK);
		wert<<=1;
//		asm("nop");
        asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);

/*        asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/

	}
/*            asm volatile("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        ::);
*/

		cbi(RF_PORT, SCK);
	sbi(RF_PORT, CS);
	return werti;
}
#endif
void rf12_init(void)
{
	unsigned char i;

	RF_DDR |= (1<<SDI)|(1<<SCK)|(1<<CS);
	RF_DDR &= ~(1<<SDO);
	RF_PORT |= (1<<CS);
    RF_PORT |= (1<<SDO);
    //RF_PORT = 0xFF;
	for (i=0; i<100; i++)
		_delay_ms(10);			// wait until POR done

	rf12_trans(0xC0E0);			// AVR CLK: 10MHz
	rf12_trans(0x80D7);			// Enable FIFO
	rf12_trans(0xC2AB);			// Data Filter: internal
	rf12_trans(0xCA81);			// Set FIFO mode
	rf12_trans(0xE000);			// disable wakeuptimer
	rf12_trans(0xC800);			// disable low duty cycle
	rf12_trans(0xC4F7);			// AFC settings: autotuning: -10kHz...+7,5kHz

	rf12_trans(0x0000);

#ifdef RF12_UseNoneBlockingCalls
	RF12_status.Rx = 0;
	RF12_status.Tx = 0;
	RF12_status.New = 0;

	RF_IRQDDR &= ~(1<<IRQ);
//    RF_IRQDDR |= (1<<IRQ);
    RF_IRQPORT |= (1<<IRQ);
#ifdef __AVR_ATmega16__
    MCUCR |= (1<<ISC01);
	GICR |= (1<<INT0);
#endif
#ifdef __AVR_ATmega644__
    EICRA |= (1<<ISC01);
    EIMSK |= (1<<INT0);
#endif

#ifdef __AVR_ATmega168__
     EICRA |= (1<<ISC01);
     EIMSK |= (1<<INT0);
#endif

#endif

}

void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi)
{
	rf12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}

void rf12_setfreq(unsigned short freq)
{	if (freq<96)				// 430,2400MHz
		freq=96;
	else if (freq>3903)			// 439,7575MHz
		freq=3903;
	rf12_trans(0xA000|freq);
}

void rf12_setbaud(unsigned short baud)
{
	if (baud<663)
		return;
	if (baud<5400)					// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		rf12_trans(0xC680|((43104/baud)-1));
	else
		rf12_trans(0xC600|((344828UL/baud)-1));
}

void rf12_setpower(unsigned char power, unsigned char mod)
{	
	rf12_trans(0x9800|(power&7)|((mod&15)<<4));
}

#ifdef RF12_UseBlockingCalls
/* blocking methods */
void rf12_ready(void)
{
	cbi(RF_PORT, CS);
	while (!(RF_PIN & (1<<SDO))); // wait until FIFO ready
}

void rf12_txdata(unsigned char *data, unsigned char number)
{	unsigned char i;
	unsigned int crc;
	rf12_trans(0x8238);			// TX on

	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB82D);
	rf12_ready();
	rf12_trans(0xB8D4);
	rf12_ready();
	rf12_trans(0xB800 | number);	
	crc = crcUpdate(0, number);
	for (i=0; i<number; i++)
	{	
		rf12_ready();
		rf12_trans(0xB800 | data[i]);
		crc = crcUpdate(crc, data[i]);
	}
	rf12_ready();
	rf12_trans(0xB800 | (crc & 0x00FF));
	rf12_ready();
	rf12_trans(0xB800 | (crc >> 8));
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();

	rf12_trans(0x8208);			// TX off
}


unsigned char rf12_rxdata(unsigned char *data)
{	unsigned char i, number;
	unsigned int crc, crc_chk;
	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA81);			// set FIFO mode
	rf12_trans(0xCA83);			// enable FIFO
	
	rf12_ready();
	number = rf12_trans(0xB000) & 0x00FF;
	crc_chk = crcUpdate(0, number);

	for (i=0; i<number; i++)
	{
		rf12_ready();
		data[i] = (unsigned char) (rf12_trans(0xB000) & 0x00FF);
		crc_chk = crcUpdate(crc_chk, data[i]);
	}

	rf12_ready();
	crc = rf12_trans(0xB000) & 0x00FF;
	rf12_ready();
	crc |=  rf12_trans(0xB000) << 8;

	rf12_trans(0x8208);			// RX off

	if (crc != crc_chk)
		number = 0;

	return number;
}
/* ------------------------- */
#endif

#ifdef RF12_UseNoneBlockingCalls
/* none blocking methods */
unsigned char rf12_rxstart(void)
{
	if(RF12_status.New)
		return(1);			//buffer not yet empty
	if(RF12_status.Tx)
		return(2);			//tx in action
	if(RF12_status.Rx)
		return(3);			//rx already in action

	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA81);			// set FIFO mode
	rf12_trans(0xCA83);			// enable FIFO
	
	//rf12_trans(0x0000);			//clear int

	RF12_Index = 0;
	RF12_status.Rx = 1;

	//MCUCR |= (1<<ISC01);
	//GICR |= (1<<INT0);
	return(0);				//all went fine
}

unsigned char rf12_rxfinish(unsigned char *data)
{
	unsigned int crc, crc_chk = 0;
	unsigned char i;
	if(RF12_status.Rx)
		return(255);				//not finished yet
	if(!RF12_status.New)
		return(254);				//old buffer
	for(i=0; i<RF12_Data[0] +1 ; i++)
		crc_chk = crcUpdate(crc_chk, RF12_Data[i]);

	crc = RF12_Data[i++];
	crc |= RF12_Data[i] << 8;
	RF12_status.New = 0;
	if(crc != crc_chk)
		return(0);				//crc err -or- strsize
	else
	{
		unsigned char i;
		for(i=0; i<RF12_Data[0]; i++)
			data[i] = RF12_Data[i+1];
		return(RF12_Data[0]);			//strsize
	}
}

unsigned char rf12_txstart(unsigned char *data, unsigned char size)
{
	unsigned char i, l;
	unsigned int crc;
	if(RF12_status.Tx)
		return(2);			//tx in action
	if(RF12_status.Rx)
		return(3);			//rx already in action
	if(size > RF12_DataLength)
		return(4);			//str to big to transmit

	RF12_status.Tx = 1;
	RF12_Index = size + 9;			//act -10 

	i = RF12_Index;				
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0x2D;
	RF12_Data[i--] = 0xD4;
	RF12_Data[i--] = size;
	crc = crcUpdate(0, size);
	for(l=0; l<size; l++)
	{
		RF12_Data[i--] = data[l];
		crc = crcUpdate(crc, data[l]);
	}	
	RF12_Data[i--] = (crc & 0x00FF);
	RF12_Data[i--] = (crc >> 8);
	RF12_Data[i--] = 0xAA;
	RF12_Data[i--] = 0xAA;

	rf12_trans(0x8238);			// TX on

	return(0);				//all went fine
}

unsigned char rf12_txfinished(void)
{
	if(RF12_status.Tx)
		return(255);			//not yet finished
	return(0);
}

void rf12_allstop(void)
{
	//GICR &= ~(1<<INT0);		//disable int0	
	RF12_status.Rx = 0;
	RF12_status.Tx = 0;
	RF12_status.New = 0;
	rf12_trans(0x8208);		//shutdown all
	rf12_trans(0x0000);		//dummy read
}
/* ---------------------- */
#endif
