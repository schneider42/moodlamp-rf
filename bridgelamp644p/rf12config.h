#ifndef __RFM12CONFIG_H
#define __RFM12CONFIG_H

/* config */
#define RF_PORT		PORTB
#define RF_DDR		DDRB
#define RF_PIN		PINB
#define SDI		5
#define SCK		7
#define CS		4
#define SDO		6

#define RF_IRQDDR	DDRD
#define RF_IRQPIN	PIND
#define RF_IRQPORT  PORTD
#define IRQ		2

#define RESET_PORT  PORTA
#define RESET_DDR   DDRA
#define RESET       PA3

//define RF12DEBUG
#endif
