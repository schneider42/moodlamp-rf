#ifndef __RFM12CONFIG_H
#define __RFM12CONFIG_H

/* config */
#define RF_PORT		PORTD
#define RF_DDR		DDRD
#define RF_PIN		PIND
#define SDI		7
#define SCK		6
#define CS		5
#define SDO		4

#define RF_IRQDDR	DDRD
#define RF_IRQPIN	PIND
#define RF_IRQPORT  PORTD
#define IRQ		2		//only used if async mode. Had to be INT0
					//or INT1/2 when change something in rf12.c
#define RF12DEBUGBIN
#endif
