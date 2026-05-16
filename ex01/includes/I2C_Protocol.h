#ifndef I2C_PROTOCOL_H
#define I2C_PROTOCOL_H

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#define AHT20_ADDR        (0x38 << 1)//AHT20 adress
#define AHT20_STATUS_CALIBRATED 0x08

#define BAUD_RATE 115200
#define ACK		0
#define NACK	1
#define PRINT	1

//aht20
void aht20_print_status(uint8_t _print);
void aht20_trigger_measure(void);
uint8_t aht20_get_status(void);
void aht20_read(uint8_t *data);
void aht20_calibrate(void);
void aht20_wait_ready(void);


//i2c
void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_write(unsigned char data);
char i2c_read(uint8_t mode);

#endif