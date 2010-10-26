#ifndef __CONTROL_H_
#define __CONTROL_H_

//extern uint16_t timeoutmax;
extern unsigned int initadr;

void control_init(void);
void control_setServer(uint8_t s);
void control_setColor(uint8_t r, uint8_t g, uint8_t b);
void control_fade(uint8_t r, uint8_t g, uint8_t b, uint16_t speed);
void control_setTimeout(void);
void control_tick(void);
void control_gotAddress(void);
void control_setupOK(void);
#endif
