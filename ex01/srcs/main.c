#include "I2C_Protocol.h"

void uart_init()
{
	UCSR0A |= (1 << U2X0);//activate double speed

	uint16_t ubrr = (16000000 / (8 * BAUD_RATE)) - 1;//set uart speed p146
	//ubrr must be right rounded but in code 7.67 is rounded as 7 instead as 8, so we must double speed or make round function

	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;
	
	UCSR0B = (1 << TXEN0);//activate tx p148/160
	UCSR0C = (1 << UCSZ01 )| (1 << UCSZ00);//8 bit char p162, other ucsr0c must stay 0 p161

}

void uart_tx(char c)
{
	while(!(UCSR0A & (1 << UDRE0)))// udre0 is 1 if udr0 is empty, so ready to send something new
		;
	UDR0 = c;
}

void uart_printstr(const char *str)
{
	while(*str)
	{
		uart_tx(*str);
		str ++;
	}
}

void aht20_print_status(uint8_t _print)
{
    uint8_t status = TW_STATUS;

    const char *i2c_msg[0x60] = {0};

	i2c_msg[TW_START]            = "START sent";          // START condition sent successfully
	i2c_msg[TW_REP_START]        = "REPEATED START";      // Repeated START condition sent

	i2c_msg[TW_MT_SLA_ACK]       = "SLA+W ACK";           // Slave address + WRITE sent, ACK received
	i2c_msg[TW_MT_SLA_NACK]      = "SLA+W NACK";          // Slave address + WRITE sent, NACK received

	i2c_msg[TW_MT_DATA_ACK]      = "DATA ACK";            // Data byte sent, ACK received
	i2c_msg[TW_MT_DATA_NACK]     = "DATA NACK";           // Data byte sent, NACK received

	i2c_msg[TW_MT_ARB_LOST]      = "ARBITRATION LOST";    // Lost bus arbitration while transmitting

	i2c_msg[TW_MR_SLA_ACK]       = "SLA+R ACK";           // Slave address + READ sent, ACK received
	i2c_msg[TW_MR_SLA_NACK]      = "SLA+R NACK";          // Slave address + READ sent, NACK received

	i2c_msg[TW_MR_DATA_ACK]      = "DATA RX ACK";         // Data received, ACK sent (more bytes expected)
	i2c_msg[TW_MR_DATA_NACK]     = "DATA RX NACK";        // Data received, NACK sent (last byte)

	i2c_msg[TW_SR_SLA_ACK]       = "SLAVE SLA+W ACK";     // Slave mode: address matched, WRITE request
	i2c_msg[TW_SR_DATA_ACK]      = "SLAVE DATA ACK";      // Slave received data, ACK sent
	i2c_msg[TW_SR_STOP]          = "SLAVE STOP";          // STOP or repeated START detected in slave mode

	i2c_msg[TW_ST_SLA_ACK]       = "SLAVE SLA+R ACK";     // Slave mode: address matched, READ request
	i2c_msg[TW_ST_DATA_ACK]      = "SLAVE TX ACK";        // Slave transmitted data, ACK received
	i2c_msg[TW_ST_DATA_NACK]     = "SLAVE TX NACK";       // Slave transmitted data, NACK received

	i2c_msg[TW_BUS_ERROR]        = "BUS ERROR";           // Illegal START/STOP condition detected

	i2c_msg[TW_NO_INFO]          = "NO INFO";             // No relevant state information

    if (i2c_msg[status] && _print) 
    {
		uart_printstr("I2C status 0x");
		uart_tx("0123456789ABCDEF"[(status >> 4) & 0x0F]);
        uart_tx("0123456789ABCDEF"[status & 0x0F]);
		uart_tx(' ');
        uart_printstr(i2c_msg[status]);
        uart_printstr("\r\n");
    }
}

void i2c_wait()
{
	while (!(TWCR & (1 << TWINT)))//wait for activation
		;
}

void i2c_init()
{
	TWSR = 0; // Sets prescaler to 0
	TWBR = 72; // Sets TWBR val
	_delay_ms(40);
}

void i2c_start()
{
	// SREG |= (1 << 7);
	TWCR =  (1 << TWINT) | (1 << TWEN) | (1 <<TWSTA); // Send message as master
	//TWEN active I2c, TWSTA genere start, TWINT doit etre a 1 pour "valider" l'action, sera a 0 quand finis
	i2c_wait();
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);// Stop condition (End the transmission)
	while (TWCR & (1 << TWSTO))
		;
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);  // Need to be cleared to pass at the next action and activates the TWI interface
	i2c_wait();
}

char i2c_read(uint8_t mode)
{
	if (mode == ACK)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	else
		TWCR = (1 << TWINT) | (1 << TWEN);   // Need to be cleared to pass at the next action Activates the TWI interface
	i2c_wait();
	return TWDR;
}

void print_hex_value(char *aht20_str)
{
    char hex[] = "0123456789ABCDEF";

    for (int i = 0; i < 7; i++)
    {
        uart_tx(hex[(aht20_str[i] >> 4) & 0x0F]); // nibble haut
        uart_tx(hex[aht20_str[i] & 0x0F]);        // nibble bas

        if (i < 6)
            uart_tx(' '); // espace entre chaque byte
    }
    uart_printstr("\n\r");
}

int main()
{
	uart_init();
	i2c_init();
	_delay_ms(100);
	aht20_calibrate();
	uint32_t raw_humidity;
	uint32_t raw_temperature;
	
	uint8_t data[6];


	while(1)
	{
		aht20_trigger_measure();
		aht20_wait_ready();
		aht20_read(data);
		
		print_hex_value(data);
		_delay_ms(2000);
	}
}