# I2C Driver – ATmega328P + AHT20

Low-level implementation of the I2C (TWI) protocol on an ATmega328P to communicate with an AHT20 temperature and humidity sensor.

This project focuses on register-level embedded programming using the microcontroller peripheral directly, without abstraction layers.

## Technical Highlights

- Manual I2C configuration at 100kHz using AVR registers
- Start / Stop / Write / Read implementation with TWI peripheral
- Communication with AHT20 temperature and humidity sensor
- Raw 7-byte sensor frame acquisition
- Temperature and humidity conversion using datasheet formulas
- Rolling average over the last 3 measurements
- UART serial output for monitoring and debugging

## Development Steps

### Phase 1 — TWI Initialization

- I2C peripheral initialization
- Bus frequency configuration at 100kHz
- Start and Stop condition handling
- Transmission status checking using TWSR

### Phase 2 — Raw Sensor Communication

- Sensor trigger command transmission
- Required measurement delay management
- Reading the 7-byte raw measurement frame
- Hexadecimal output for validation and debugging

Example:

```txt
0C 79 9A A6 4E 3C F2
```

Phase 3 — Data Processing
Extraction of humidity and temperature raw values from sensor bytes
Conversion using AHT20 datasheet formulas
Precision handling and proper rounding
Rolling average over the last 3 measurements
Formatted UART output for terminal display

Example:

```txt
Temperature: 18.0°C
Humidity: 43.5%
```

Hardware
ATmega328P
AHT20 Temperature / Humidity Sensor
Why this project matters

This project demonstrates:

datasheet-driven development
low-level peripheral configuration
embedded communication protocols
sensor integration
debugging on constrained systems
precision handling in embedded applications

Build:
```txt
make flash
```