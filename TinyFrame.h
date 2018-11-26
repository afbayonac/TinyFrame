#include "Arduino.h"
// #define TINYFRAME_DEBUG
// #define TINYFRAME_DEBUG_TEENSY

#ifdef TINYFRAME_DEBUG_TEENSY
#include "usb_serial.h"
#endif


// max length of data 65535 characters
typedef void (* handler_response)(const uint8_t *data, uint16_t length);

class TinyFrame {
	private:
	 	Stream* _port;
	    handler_response _handler;
	    uint8_t _dire;
	    uint16_t _maxLength;
	    bool _scaped;
	    uint8_t* _buffer;
	    uint16_t _position;
	    void _scape(uint8_t fata);
		#ifdef TINYFRAME_DEBUG
	    HardwareSerial* _DEBUG;
	    #endif
		#ifdef TINYFRAME_DEBUG_TEENSY
	    usb_serial_class* _DEBUG;
		#endif

	public:
		TinyFrame(handler_response handler);
		TinyFrame(handler_response handler, uint8_t dire);
	    void collect(void);
		bool write(const uint8_t* data, uint16_t length);
		bool begin(HardwareSerial* port, unsigned long baud, uint8_t format);
		bool begin(HardwareSerial* port, unsigned long baud);

		#ifdef TINYFRAME_DEBUG
		bool begin(HardwareSerial* port, HardwareSerial* DEBUG, unsigned long baud, uint8_t format);
		bool begin(HardwareSerial* port, HardwareSerial* DEBUG, unsigned long baud);
		#endif
		#ifdef TINYFRAME_DEBUG_TEENSY
		bool begin(HardwareSerial* port, usb_serial_class* DEBUG, unsigned long baud, uint8_t format);
		bool begin(HardwareSerial* port, usb_serial_class* DEBUG, unsigned long baud);
		#endif
};
