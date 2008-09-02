#ifndef __RFM12CONFIG_H
#define __RFM12CONFIG_H

/* config */
#define RF_PORT		PORTB
#define RF_DDR		DDRB
#define RF_PIN		PINB
#define SDI		5
#define SCK		7
#define CS		0
#define SDO		6

#define RF_IRQDDR	DDRD
#define RF_IRQPIN	PIND
#define RF_IRQPORT  PORTD
#define IRQ		2		//only used if async mode. Had to be INT0
					//or INT1/2 when change something in rf12.c
#define RF12DEBUGBIN
#endif
