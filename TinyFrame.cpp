#include "TinyFrame.h"

const uint8_t FLAG = 0x7e;
const uint8_t SCAPE = 0x7d;
const uint8_t INVERT_BIT = 0x20;
const uint8_t CONTROL = 0x03;
const uint8_t BROADCAST = 0xff;

bool scaped = false;

TinyFrame::TinyFrame(handler_response handler): TinyFrame(handler, BROADCAST) {}
TinyFrame::TinyFrame(handler_response handler, uint8_t dire) {
	this->_handler = handler;
	this->_dire = dire;
	this->_maxLength = 64; // buffer Arduino
}

#ifdef TINYFRAME_DEBUG_TEENSY
bool TinyFrame::begin(HardwareSerial* port, usb_serial_class* DEBUG, unsigned long baud) {
	this->begin(port, baud);
	this->_DEBUG = DEBUG;
	(*_DEBUG).println("~ [ DEBUG MODE TINY FRAME ] ~");
	return true;
}
#endif

#ifdef TINYFRAME_DEBUG
bool TinyFrame::begin(HardwareSerial* port, HardwareSerial* DEBUG, unsigned long baud) {
	this->begin(port, baud);
	this->_DEBUG = DEBUG;
	(*_DEBUG).println("~ [ DEBUG MODE TINY FRAME ] ~");
	return true;
}
#endif

bool TinyFrame::begin(HardwareSerial* port, unsigned long baud) {
	this->_port = port;
	(*port).begin(baud);
	while (!(*port));
	return true;
}

void TinyFrame::collect() {
	if (!this->_port->available()) return;

	#if defined(TINYFRAME_DEBUG_TEENSY) || defined(TINYFRAME_DEBUG)
	this->_DEBUG->print(" ");
	this->_DEBUG->print(this->_port->peek(), HEX);
	#endif

	uint8_t data = this->_port->read();

	if (data == FLAG || this->_position == this->_maxLength - 1) {

		// validate an pass a handler
		if (this->_position >= 4 &&
			(this->_buffer[0] == 0xff || this->_buffer[0] == this->_dire ) &&
			this->_buffer[1] == CONTROL &&
			true ){
			#if defined(TINYFRAME_DEBUG_TEENSY) || defined(TINYFRAME_DEBUG)
			this->_DEBUG->println(" [CALL HANDLER] ");
			#endif
			this->_handler(&this->_buffer[2], this->_position - 4);
		}

		this->_scaped = false;
		this->_buffer = (uint8_t *) malloc(this->_maxLength * sizeof(uint8_t));
		this->_position = 0;
		this->collect();
		return;
	}

	if(this->_scaped){
		this->_scaped = false;
		data ^= INVERT_BIT;
	}

	if (data == SCAPE) {
		this->_scaped = true;
		this->collect();
		return;
	}

	this->_buffer[this->_position] = data;
	this->_position++;
	this->collect();
	return;
}

bool TinyFrame::write(const uint8_t* data, uint16_t length) {
	#if defined(TINYFRAME_DEBUG_TEENSY) || defined(TINYFRAME_DEBUG)
	this->_DEBUG->print(" [ SEND DATA ] ");
	#endif
	uint16_t count = 0;
	this->_port->write(FLAG);
	this->_port->write(BROADCAST);
	this->_port->write(CONTROL);
	while(length--) this->_scape(data[count++]);
	this->_port->write(0xCC);
	this->_port->write(0xCC);
	this->_port->write(FLAG);
	return true;
}

void TinyFrame::_scape(uint8_t data) {
	if (data == CONTROL || data == FLAG || data == SCAPE){
		this->_port->write(SCAPE);
		data ^= INVERT_BIT;
	}
	this->_port->write(SCAPE);
}


