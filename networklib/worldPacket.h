#ifndef WORLDPACKET_H
#define WORLDPACKET_H

#include "cmsgpack.h"
#include <string>

class WorldPacket
{
public:
	WorldPacket() {
		m_mpbuf = &m_data;
		mp_buf_constructor(m_mpbuf, 0, 0);
	}

	WorldPacket(uint16_t opcode, size_t size = 256) {
		m_mpbuf = &m_data;
		mp_buf_constructor(m_mpbuf, opcode, size + 12);
	}

	// copy constructor
	WorldPacket(const WorldPacket &packet) {
		size_t size		= packet.size();
		uint16_t opcode = packet.GetOpcode();

		m_mpbuf = &m_data;
		mp_buf_constructor(m_mpbuf, opcode, size + 12);
		mp_encode_buf_content(getMpBuf(), packet.getMpBuf());
	}

	~WorldPacket() {
		mp_buf_destructor(m_mpbuf);
	}

	void Initialize(uint16_t opcode) {
		mp_buf_reset(m_mpbuf, opcode);
	}

	mp_buf* getMpBuf() const { return m_mpbuf; }
	void preConsume(size_t len) { mp_buf_pre_consume(m_mpbuf, len);}
	const uint8_t* contents() const { return mp_buf_get_content(m_mpbuf); }
	size_t size() const { return mp_buf_get_size(m_mpbuf); }
	void rpos(size_t pos) { mp_buf_set_read_pos(m_mpbuf, pos); }
	size_t wpos() const { return mp_buf_get_write_pos(m_mpbuf); }
	uint16_t GetOpcode() const { return mp_buf_get_opcode(m_mpbuf); }
	void SetOpcode(uint16_t opcode) { mp_buf_set_opcode(m_mpbuf, opcode); }

	void packUint8(uint8_t value) {
		unsigned char b[mp_encode_8_len];
		b[0] = mp_uint8;        /* uint 8 */
		mp_encode_8(b, value);
		mp_buf_append(m_mpbuf, b, mp_encode_8_len);
	}

	void packUint16(uint16_t value) {
		unsigned char b[mp_encode_16_len];
		b[0] = mp_uint16;        /* uint 16 */
		mp_encode_16(b, value);
		mp_buf_append(m_mpbuf, b, mp_encode_16_len);
	}

	void packUint32(uint32_t value) {
		unsigned char b[mp_encode_32_len];
		b[0] = mp_uint32;        /* uint 32 */
		mp_encode_32(b, value);
		mp_buf_append(m_mpbuf, b, mp_encode_32_len);
	}

	WorldPacket& operator<<(bool value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(uint8_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(uint16_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(uint32_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(uint64_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(int8_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(int16_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(int32_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(int64_t value) {
		mp_encode_int(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(float value) {
		mp_encode_double(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(double value) {
		mp_encode_double(m_mpbuf, value);
		return *this;
	}

	WorldPacket& operator<<(const char* value) {
		mp_encode_bytes(m_mpbuf, (const unsigned char*)value, strlen(value));
		return *this;
	}

	WorldPacket& operator<<(const std::string & value) {
		mp_encode_bytes(m_mpbuf, (const unsigned char*)value.c_str(), value.size());
		return *this;
	}

	WorldPacket& operator>>(bool &value) {
		value = mp_decode_int(m_mpbuf) ? true: false;
		return *this;
	}

	WorldPacket& operator>>(uint8_t &value) {
		value = (uint8_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(uint16_t &value) {
		value = (uint16_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(uint32_t &value) {
		value = (uint32_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(uint64_t &value) {
		value = (uint64_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(int8_t &value) {
		value = (int8_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(int16_t &value) {
		value = (int16_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(int32_t &value) {
		value = (int32_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(int64_t &value) {
		value = (int64_t)mp_decode_int(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(float &value) {
		value = (float)mp_decode_double(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(double &value) {
		value = (double)mp_decode_double(m_mpbuf);
		return *this;
	}

	WorldPacket& operator>>(std::string & value) {
		size_t len;
		char* str = NULL;
		
		mp_decode_string(m_mpbuf, &str, &len);
		if ( str ) {
			value.assign(str, len);
		} else {
			value.clear();
		}
		return *this;
	}

	void AppendBinary(const void *str, size_t len) {
		mp_encode_bytes(m_mpbuf, (const unsigned char *)str, len);
	}

	void UnsafeReadBinary(void *buf, uint16_t bufsize) {
		size_t len;
		char* str = NULL;
		
		mp_decode_string(m_mpbuf, &str, &len);

		if( len <= bufsize ) {
			memcpy(buf, str, len);
		} else {
			fprintf( stderr, "UnsafeReadBinary not enought memory!.");
			MSGPACK_ASSERT(0);
		}
	}

	void append(const void* data, size_t len) {
		mp_buf_append(m_mpbuf, (const unsigned char*)data, len);
	}

	void AppendPacket(const WorldPacket& data) {
		mp_encode_buf(m_mpbuf, data.getMpBuf());
	}

	void AppendPacketData(const WorldPacket& data) {
		mp_encode_buf_content(m_mpbuf, data.getMpBuf());
	}

	void ReadPacket(WorldPacket& data) {
		mp_decode_buf(m_mpbuf, data.getMpBuf());
	}

	void prePutInt8() {
		mp_pre_encode_int8(m_mpbuf);
	}

	void prePutUint16() {
		mp_pre_encode_uint16(m_mpbuf);
	}

	void prePutUint32() {
		mp_pre_encode_uint32(m_mpbuf);
	}

	void putInt8(size_t pos, int8_t value) {
		mp_replace_int8(m_mpbuf, pos, value);
	}

	void putUint16(size_t pos, uint16_t value) {
		mp_replace_uint16(m_mpbuf, pos, value);
	}

	void putUint32(size_t pos, uint32_t value) {
		mp_replace_uint32(m_mpbuf, pos, value);
	}
private:
	mp_buf* m_mpbuf;
	mp_buf m_data;
};

#endif
