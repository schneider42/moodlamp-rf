#if SERIAL_UART
/** process serial data received by uart */
void check_serial_input(uint8_t data)
/* {{{ */ {
    static uint8_t buffer[10];
    static int16_t fill = -1;
    static uint8_t escaped = 0;

    if(data == 0xAA){
        if(!escaped){
            escaped = 1;
            return;
        }
        escaped = 0;
    }else if(escaped){
        escaped = 0;
        if(data == 0x01){
            fill = 0;
            return;
        }
    }
    if(fill != -1){
        buffer[fill++] = data;
        if(fill >= 10)
            fill = -1;
    }
    uint8_t pos;
    if (buffer[0] == 0x01 && fill == 1) {  /* soft reset */

        jump_to_bootloader();
        
    } else if (buffer[0] == 0x02 && fill == 4) { /* set color */

        for ( pos = 0; pos < 3; pos++) {
            global_pwm.channels[pos].target_brightness = buffer[pos + 1];
            global_pwm.channels[pos].brightness = buffer[pos + 1];
        }

        fill = -1;

    } else if (buffer[0] == 0x03 && fill == 6) { /* fade to color */

        for (pos = 0; pos < 3; pos++) {
            global_pwm.channels[pos].speed_h = buffer[1];
            global_pwm.channels[pos].speed_l = buffer[2];
            global_pwm.channels[pos].target_brightness = buffer[pos + 3];
        }

        fill = -1;
    }
