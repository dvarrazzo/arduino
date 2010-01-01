/*
 * Looking into the Nunchuk cable:
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
 * 5    2    Clock   Blue
 * 4    5    Data    Brown
 * 3    1    +3V     Black/White
 * 2    6    Ground  Black
 * ---- ---- ------- ------------
 */
#ifndef NUNCHUK_H
#define NUNCHUK_H

#include "ardulib/utils.h"

// adapt to your hardware config
#ifndef POWER_VIA_PORT_C2_C3
#define POWER_VIA_PORT_C2_C3 1 // use port pins port C2 and C3 as power supply of the Nunchuk (direct plug using wiichuck adapter)
#endif

//#define DEBUG_RCV_TEL 1
#define USE_NEW_WAY_INIT 1 // use "The New Way" of initialization <http://wiibrew.org/wiki/Wiimote#The_New_Way>
#define WII_IDENT_LEN ((uint8_t)6)
#define WII_TELEGRAM_LEN ((uint8_t)6)
#define WII_NUNCHUK_TWI_ADR ((uint8_t)0x52)

class Nunchuk
{
    public:
        void init(void);
        int read(void);
        uint8_t readControllerIdent(uint8_t *outbuf);

        uint8_t joyX(void) const;
        uint8_t joyY(void) const;

        int accX(void) const;
        int accY(void) const;
        int accZ(void) const;

        uint8_t btnZ(void) const;
        uint8_t btnC(void) const;

    private:
        uint8_t buffer[WII_TELEGRAM_LEN]; // array to store nunchuk output

        uint8_t nunchukInit(unsigned short timeout);
        void sendZero(void);
        char decodeByte(uint8_t x);
        void clearTwiInputBuffer(void);
};

inline uint8_t Nunchuk::joyX(void) const
{
    return buffer[0];
}

inline uint8_t Nunchuk::joyY(void) const
{
    return buffer[1];
}

inline int Nunchuk::accX(void) const
{
    return (buffer[2] << 2) | ((buffer[5] >> 2) & 3);
}

inline int Nunchuk::accY(void) const
{
    return (buffer[3] << 2) | ((buffer[5] >> 4) & 3);
}

inline int Nunchuk::accZ(void) const
{
    return (buffer[4] << 2) | ((buffer[5] >> 6) & 3);
}


inline uint8_t Nunchuk::btnZ(void) const
{
    return 1 ^ (buffer[5] & 1);
}

inline uint8_t Nunchuk::btnC(void) const
{
    return 1 ^ ((buffer[5] >> 1) & 1);
}


#endif // NUNCHUK_H
