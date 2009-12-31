// From http://www.windmeadow.com/node/42#comment-28
//
// read out a Wii Nunchuck controller
// adapted to work with wireless Nunchuck controllers of third party vendors by Michael Dreher

/*
 * Looking into the Nunchuck cable:
 *
 *    /       /
 *   +-------+
 *   | 5 3 1 |
 *   | 6 4 2 |/
 *   +__---__+
 *
 * For arduino connection see photos on:
 *
 *   http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *
 * On our hacked cable:
 *
 * ---- ---- ------- ------------
 * Ard. Nun. Wire    Color
 * PIN  PIN
 * ---- ---- ------- ------------
 * 3    1    +3V     Black/White
 * 5    2    Clock   Blue
 * 4    5    Data    Brown
 * 2    6    Ground  Black
 * ---- ---- ------- ------------
 */

// adapt to your hardware config
#define POWER_VIA_PORT_C2_C3 1 // use port pins port C2 and C3 as power supply of the Nunchuck (direct plug using wiichuck adapter)
//#define DEBUG_RCV_TEL 1

#define USE_NEW_WAY_INIT 1 // use "The New Way" of initialization <http://wiibrew.org/wiki/Wiimote#The_New_Way>
#define WII_IDENT_LEN ((byte)6)
#define WII_TELEGRAM_LEN ((byte)6)
#define WII_NUNCHUCK_TWI_ADR ((byte)0x52)

void Serial_println(const char *s)
{
	Serial.println(s);
}

#include <Wire.h>
#include <string.h>
#include <utility/twi.h>
#undef int
#include <stdio.h>

uint8_t outbuf[WII_TELEGRAM_LEN]; // array to store arduino output
int cnt = 0;
int ledPin = 13;

#undef SHOW_ACC_RGB
#define SHOW_JOY_XY

// if i want to convert acc to rgb led
#ifdef SHOW_ACC_RGB
static const uint8_t PIN_LED_R = 10;
static const uint8_t PIN_LED_G = 11;
static const uint8_t PIN_LED_B = 12;
#endif

// if i want to display the joystick into 4 leds
#ifdef SHOW_JOY_XY
static const uint8_t PIN_LED_XP = 6;
static const uint8_t PIN_LED_XN = 5;
static const uint8_t PIN_LED_YP = 10;
static const uint8_t PIN_LED_YN = 11;
#endif

// Forward declarations
byte nunchuck_init(unsigned short timeout);
byte readControllerIdent(byte* pIdent);
void parse_nunchuck_data(uint8_t buffer[], struct s_nunchuck_state_t *out);
char nunchuk_decode_byte(char x);
void print_nunchuck(const struct s_nunchuck_state_t *state);

void setup ()
{
	Serial.begin (19200);
	Serial.println("Starting setup");

	#ifdef POWER_VIA_PORT_C2_C3 // power supply of the Nunchuck via port C2 and C3
	PORTC &=~ _BV(PORTC2);
	PORTC |= _BV(PORTC3);
	DDRC |= _BV(PORTC2) | _BV(PORTC3); // make outputs
	delay(100); // wait for things to stabilize
	#endif

	Serial.println("Wire.begin");
	Wire.begin(); // initialize i2c
	// we need to switch the TWI speed, because the nunchuck uses Fast-TWI
	// normally set in hardware\libraries\Wire\utility\twi.c twi_init()
	// this is the way of doing it without modifying the original files
	#define TWI_FREQ_NUNCHUCK 400000L
	TWBR = ((CPU_FREQ / TWI_FREQ_NUNCHUCK) - 16) / 2;

	Serial.println("n. init");
	nunchuck_init(0); // send the initialization handshake
	Serial.println("n. init ok");

	// display the identification bytes, must be "00 00 A4 20 00 00" for the Nunchuck
	byte i;
	if(readControllerIdent(outbuf) == 0)
	{
		Serial.print("Ident=");
		for (i = 0; i < WII_TELEGRAM_LEN; i++)
		{
			Serial.print(outbuf[i], HEX);
			Serial.print(' ');
		}
		Serial.println();
	}

	Serial.println("Finished setup");

#undef SHOW_ACC_RGB
#define SHOW_JOY_XY

#ifdef SHOW_ACC_RGB
	pinMode(PIN_LED_R, OUTPUT);
	pinMode(PIN_LED_G, OUTPUT);
	pinMode(PIN_LED_B, OUTPUT);
#endif

#ifdef SHOW_JOY_XY
	pinMode(PIN_LED_XP, OUTPUT);
	pinMode(PIN_LED_XN, OUTPUT);
	pinMode(PIN_LED_YP, OUTPUT);
	pinMode(PIN_LED_YN, OUTPUT);
#endif

}

typedef struct s_nunchuck_state_t
{
	uint8_t joy_x;
	uint8_t joy_y;
	int acc_x;
	int acc_y;
	int acc_z;
	uint8_t btn_c;
	uint8_t btn_z;
} nunchuck_state_t;


// params:
// timeout: abort when timeout (in ms) expires, 0 for unlimited timeout
// return: 0 == ok, 1 == timeout
byte nunchuck_init(unsigned short timeout)
{
	byte rc = 1;

	#ifndef USE_NEW_WAY_INIT
	// look at <http://wiibrew.org/wiki/Wiimote#The_Old_Way> at "The Old Way"
	Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
	Wire.send (0x40); // sends memory address
	Wire.send (0x00); // sends sent a zero.
	Wire.endTransmission (); // stop transmitting
	#else
	// disable encryption
	// look at <http://wiibrew.org/wiki/Wiimote#The_New_Way> at "The New Way"

	Serial.println("init new way");
	unsigned long time = millis();
	do
	{
		Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
		Wire.send (0xF0); // sends memory address
		Wire.send (0x55); // sends data.
		if(Wire.endTransmission() == 0) // stop transmitting
		{
			Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
			Wire.send (0xFB); // sends memory address
			Wire.send (0x00); // sends sent a zero.
			if(Wire.endTransmission () == 0) // stop transmitting
			{
				rc = 0;
			}
		}
	}
	while (rc != 0 && (!timeout || ((millis() - time) < timeout)));
	#endif

	return rc;
}


// params:
// ident [out]: pointer to buffer where 6 bytes of identification is stored. Buffer must be at least 6 bytes long.
// A list of possible identifications can be found here: <http://wiibrew.org/wiki/Wiimote#The_New_Way>
// return: 0 == ok, 1 == error
byte readControllerIdent(byte* pIdent)
{
	byte rc = 1;

	// read identification
	Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
	Wire.send (0xFA); // sends memory address of ident in controller
	if(Wire.endTransmission () == 0) // stop transmitting
	{
		byte i;
		Wire.requestFrom (WII_NUNCHUCK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuck
		for (i = 0; (i < WII_TELEGRAM_LEN) && Wire.available (); i++)
		{
			pIdent[i] = Wire.receive(); // receive byte as an integer
		}
		if(i == WII_TELEGRAM_LEN)
		{
			rc = 0;
		}
	}
	return rc;
}

void clearTwiInputBuffer(void)
{
	// clear the receive buffer from any partial data
	while( Wire.available ())
	Wire.receive ();
}


void send_zero()
{
	// I don't know why, but it only works correct when doing this exactly 3 times
	// otherwise only each 3rd call reads data from the controller (cnt will be 0 the other times)
	for(byte i = 0; i < 3; i++)
	{
		Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
		Wire.send (0x00); // sends one byte
		Wire.endTransmission (); // stop transmitting
	}
}

void loop()
{
	Wire.requestFrom (WII_NUNCHUCK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuck

	for (cnt = 0; (cnt < WII_TELEGRAM_LEN) && Wire.available (); cnt++)
	{
		outbuf[cnt] = nunchuk_decode_byte (Wire.receive ()); // receive byte as an integer
		digitalWrite (ledPin, HIGH); // sets the LED on
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
	if (cnt >= WII_TELEGRAM_LEN)
	{
		struct s_nunchuck_state_t nch;
		parse_nunchuck_data(outbuf, &nch);
		print_nunchuck(&nch);
		// print ();

#ifdef SHOW_ACC_RGB
		analogWrite(PIN_LED_R, nch.acc_x >> 2);
		analogWrite(PIN_LED_G, nch.acc_y >> 2);
		analogWrite(PIN_LED_B, nch.acc_z >> 2);
#endif

#ifdef SHOW_JOY_XY
		if (nch.joy_x < 125) {
			analogWrite(PIN_LED_XN, (125 - nch.joy_x) * 2);
			analogWrite(PIN_LED_XP, 0);
		}
		else if (nch.joy_x > 140) {
			analogWrite(PIN_LED_XN, 0);
			analogWrite(PIN_LED_XP, (nch.joy_x - 140) * 2);
		}
		else
		{
			analogWrite(PIN_LED_XN, 0);
			analogWrite(PIN_LED_XP, 0);
		}

		if (nch.joy_y < 125) {
			analogWrite(PIN_LED_YN, (125 - nch.joy_y) * 2);
			analogWrite(PIN_LED_YP, 0);
		}
		else if (nch.joy_y > 140) {
			analogWrite(PIN_LED_YN, 0);
			analogWrite(PIN_LED_YP, (nch.joy_y - 140) * 2);
		}
		else {
			analogWrite(PIN_LED_YN, 0);
			analogWrite(PIN_LED_YP, 0);
		}
#endif
	}


	send_zero (); // send the request for next bytes
	delay (20);
}

void parse_nunchuck_data(uint8_t buffer[], struct s_nunchuck_state_t *out)
{
	out->joy_x = buffer[0];
	out->joy_y = buffer[1];

	out->acc_x = buffer[2] << 2;
	out->acc_y = buffer[3] << 2;
	out->acc_z = buffer[4] << 2;

	out->btn_z = 1 ^  (buffer[5]       & 1);
	out->btn_c = 1 ^ ((buffer[5] >> 1) & 1);

	out->acc_x |= (buffer[5] >> 2) & 3;
	out->acc_y |= (buffer[5] >> 4) & 3;
	out->acc_z |= (buffer[5] >> 6) & 3;
}

void print_nunchuck(const struct s_nunchuck_state_t *state)
{
	Serial.print (state->joy_x, DEC);
	Serial.print ("\t");

	Serial.print (state->joy_y, DEC);
	Serial.print ("\t");

	Serial.print (state->acc_x, DEC);
	Serial.print ("\t");

	Serial.print (state->acc_y, DEC);
	Serial.print ("\t");

	Serial.print (state->acc_z, DEC);
	Serial.print ("\t");

	Serial.print (state->btn_c, DEC);
	Serial.print ("\t");

	Serial.print (state->btn_z, DEC);

	Serial.print ("\r\n");
}


// Decode data format that original Nunchuck uses with old init sequence. This never worked with
// other controllers (e.g. wireless Nunchuck from other vendors)
char nunchuk_decode_byte(char x)
{
	#ifndef USE_NEW_WAY_INIT
	x = (x ^ 0x17) + 0x17;
	#endif
	return x;
}

// Michael
