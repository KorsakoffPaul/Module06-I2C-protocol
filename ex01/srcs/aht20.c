#include "I2C_Protocol.h"

uint8_t aht20_get_status(void)
{
	uint8_t status;

	i2c_start();
	// aht20_print_status(1);

	// adresse + write
	i2c_write(0x70);
	// aht20_print_status(1);

	// commande status
	i2c_write(0x71);
	// aht20_print_status(1);

	// repeated start
	i2c_start();
	// aht20_print_status(1);

	// adresse + read
	i2c_write(0x71);
	// aht20_print_status(1);

	// lecture du status (dernier byte -> NACK)
	status = i2c_read(NACK);
	// aht20_print_status(1);

	i2c_stop();

	return status;
}

void aht20_calibrate(void)
{
	uint8_t status;

	// lire status
	status = aht20_get_status();

	// bit 3 = calibré
	if ((status & AHT20_STATUS_CALIBRATED) == 0)
	{
		i2c_start();
		i2c_write(AHT20_ADDR | TW_WRITE);

		i2c_write(0x1B);//those mus be initialized (p12)
		i2c_write(0x1C);
		i2c_write(0x1E);
		i2c_stop();
	}
}

void aht20_wait_ready(void)
{
	while (aht20_get_status() & 0x80)
		;
}

void aht20_trigger_measure(void)
{
	i2c_start();

	i2c_write(AHT20_ADDR | TW_WRITE);
	// aht20_print_status(1);

	i2c_write(0xAC);
	// aht20_print_status(1);

	i2c_write(0x33);
	// aht20_print_status(1);

	i2c_write(0x00);
	// aht20_print_status(1);

	i2c_stop();

	_delay_ms(80);
}

void aht20_read(uint8_t *data)
{
	i2c_start();
	// aht20_print_status(1);

	i2c_write(AHT20_ADDR | TW_READ);
	// aht20_print_status(1);

	for (int i = 0; i < 5; i++)
		data[i] = i2c_read(ACK);

	data[5] = i2c_read(NACK);

	i2c_stop();
}