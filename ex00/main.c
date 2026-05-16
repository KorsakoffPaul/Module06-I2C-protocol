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


/*
TWBR	bit rate
bit        7   	 6     5     4     3     2     1     0
         TWBR7 TWBR6 TWBR5 TWBR4 TWBR3 TWBR2 TWBR1 TWBR0

every bit define prescaler


TWSR	status register
bit         7    6    5    4    3    2     1     0
          TWS7 TWS6 TWS5 TWS4 TWS3   -   TWPS1 TWPS0

bits TWPS1/TWPS0 = prescaler I2C
TWSx = état du bus (ACK, START, etc.)


TWCR	control register
bit        7      6     5      4      3     2     1     0
         TWINT  TWEA  TWSTA  TWSTO  TWWC  TWEN    -   TWIE

TWINT = flag "action terminée"
TWSTA = START condition
TWSTO = STOP condition
TWEN = enable I2C hardware
TWEA = ACK enable
TWIE = interrupt enable (optionnel)

TWDR	data register
bit        7    6    5    4    3    2    1    0
         TWD7 TWD6 TWD5 TWD4 TWD3 TWD2 TWD1 TWD0

is I2C's adress or data send/received

*/


/*
SCL = F_CPU / (16 + 2 * TWBR * TWPSs)
100kHz = 16 000 000 / (16 + 2 * TWBR * 1)
TWBR = 72
*/


void ath20_check_status(uint8_t _print)
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
		uart_printstr("I2C status :0x");
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
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);// Stop condition (End the transmission)
	while (TWCR & (1 << TWSTO))
		;
}

void i2c_start(void)
{
	SREG |= (1 << 7);
	TWCR =  (1 << TWINT) | (1 << TWEN) | (1 <<TWSTA); // Send message as master
	//TWEN active I2c, TWSTA genere start, TWINT doit etre a 1 pour "valider" l'action, sera a 0 quand finis
	i2c_wait();
}


int main()
{
	uart_init();
	i2c_init();
	i2c_start();
	ath20_check_status(PRINT);
	i2c_stop();
	while(1)
	{

	}
}