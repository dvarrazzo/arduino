#include "ardulib/Nunchuk.h"

#include <Wire.h>
#include <utility/twi.h> // For CPU_FREQ
#include "WProgram.h"  // For DDRC

void Nunchuk::init(void)
{
	// we need to switch the TWI speed, because the nunchuk uses Fast-TWI
	// normally set in hardware\libraries\Wire\utility\twi.c twi_init()
	// this is the way of doing it without modifying the original files
	#define TWI_FREQ_NUNCHUK 400000L
	TWBR = ((CPU_FREQ / TWI_FREQ_NUNCHUK) - 16) / 2;

	#if POWER_VIA_PORT_C2_C3 // power supply of the Nunchuk via port C2 and C3
	PORTC &=~ _BV(PORTC2);
	PORTC |= _BV(PORTC3);
	DDRC |= _BV(PORTC2) | _BV(PORTC3); // make outputs
	delay(100); // wait for things to stabilize
	#endif

	nunchukInit(0); // send the initialization handshake
}

// params:
// timeout: abort when timeout (in ms) expires, 0 for unlimited timeout
// return: 0 == ok, 1 == timeout
uint8_t Nunchuk::nunchukInit(unsigned short timeout)
{
	uint8_t rc = 1;

#if !USE_NEW_WAY_INIT
	// look at <http://wiibrew.org/wiki/Wiimote#The_Old_Way> at "The Old Way"
	Wire.beginTransmission(WII_NUNCHUK_TWI_ADR); // transmit to device 0x52
	Wire.send(0x40); // sends memory address
	Wire.send(0x00); // sends sent a zero.
	Wire.endTransmission(); // stop transmitting
#else
	// disable encryption
	// look at <http://wiibrew.org/wiki/Wiimote#The_New_Way> at "The New Way"

	unsigned long time = millis();
	do
	{
		Wire.beginTransmission(WII_NUNCHUK_TWI_ADR); // transmit to device 0x52
		Wire.send(0xF0); // sends memory address
		Wire.send(0x55); // sends data.
		if(Wire.endTransmission() == 0) // stop transmitting
		{
			Wire.beginTransmission(WII_NUNCHUK_TWI_ADR); // transmit to device 0x52
			Wire.send(0xFB); // sends memory address
			Wire.send(0x00); // sends sent a zero.
			if(Wire.endTransmission() == 0) // stop transmitting
			{
				rc = 0;
			}
		}
	}
	while (rc != 0 && (!timeout || ((millis() - time) < timeout)));
#endif // !USE_NEW_WAY_INIT

	return rc;
}

int Nunchuk::read(void)
{
	sendZero(); // send the request for next bytes
	delay(20);

	Wire.requestFrom(WII_NUNCHUK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuk

    int cnt;
	for (cnt = 0; cnt < WII_TELEGRAM_LEN && Wire.available(); cnt++)
	{
		buffer[cnt] = decodeByte(Wire.receive()); // receive byte as an integer
	}

	// debugging
	#ifdef DEBUG_RCV_TEL
	Serial.print("avail=");
	Serial.print(Wire.available());
	Serial.print(" cnt=");
	Serial.println(cnt);
	#endif

	clearTwiInputBuffer();

	// If we recieved the 6 bytes, then go print them
	return (cnt >= WII_TELEGRAM_LEN);
}

// params:
// ident [out]: pointer to buffer where 6 bytes of identification is stored. Buffer must be at least 6 bytes long.
// A list of possible identifications can be found here: <http://wiibrew.org/wiki/Wiimote#The_New_Way>
// return: 0 == ok, 1 == error
uint8_t Nunchuk::readControllerIdent(uint8_t *outbuf)
{
	byte rc = 1;

	// read identification
    delay(10);
	Wire.beginTransmission(WII_NUNCHUK_TWI_ADR); // transmit to device 0x52
	Wire.send(0xFA); // sends memory address of ident in controller
	if (Wire.endTransmission() == 0) // stop transmitting
	{
		byte i;
        delay(10);
		Wire.requestFrom(WII_NUNCHUK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuk
		for (i = 0; i < WII_TELEGRAM_LEN && Wire.available(); i++)
		{
			outbuf[i] = Wire.receive(); // receive byte as an integer
		}
		if (i == WII_TELEGRAM_LEN)
		{
			rc = 0;
		}
	}
	return rc;
}


void Nunchuk::sendZero(void)
{
	// I don't know why, but it only works correct when doing this exactly 3 times
	// otherwise only each 3rd call reads data from the controller (cnt will be 0 the other times)
	for(uint8_t i = 0; i < 3; i++)
	{
		Wire.beginTransmission(WII_NUNCHUK_TWI_ADR); // transmit to device 0x52
		Wire.send(0x00); // sends one byte
		Wire.endTransmission(); // stop transmitting
	}
}

// Decode data format that original Nunchuk uses with old init sequence. This never worked with
// other controllers (e.g. wireless Nunchuk from other vendors)
char Nunchuk::decodeByte(uint8_t x)
{
	#if !USE_NEW_WAY_INIT
	x = (x ^ 0x17) + 0x17;
	#endif
	return x;
}

// clear the receive buffer from any partial data
void Nunchuk::clearTwiInputBuffer(void)
{
	while (Wire.available())
	Wire.receive();
}

