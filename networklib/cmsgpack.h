#ifndef C_MSGPAKK_H
#define C_MSGPAKK_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


// fixme
#ifdef PUB_RELEASE
#define MSGPACK_ASSERT(x) ((void)0)
#else
#define MSGPACK_ASSERT(x) assert(x)
#endif


#ifdef __cplusplus
extern "C" {
#endif

// fix me
#define MP_BUF_DEFAULT_SIZE 32

void mp_print_new_ratio();

typedef struct mp_buf_header {
	uint16_t size;
	uint16_t opcode;
} mp_buf_header, MsgPacketHeader;

//所有接口都采用了大端模式
#define mp_header_get_size(header) ((((unsigned char*)&header)[0] << 8)   \
								 | ((unsigned char*)&header)[1]) - sizeof(uint16_t)

#define mp_header_get_opcode(header) (((unsigned char*)&header)[2] << 8) \
								   | (((unsigned char*)&header)[3])

#define mp_header_set_size(header, size) {			  \
	uint16_t n = size + sizeof(uint16_t);			  \
	((unsigned char*)&header)[0] = (n & 0xff00) >> 8; \
	((unsigned char*)&header)[1] = n & 0xff;		  \
}

#define mp_header_set_opcode(header, n) {			  \
	((unsigned char*)&header)[2] = (n & 0xff00) >> 8; \
	((unsigned char*)&header)[3] = n & 0xff;		  \
}

typedef struct mp_buf {
	unsigned char *b, *p;
	size_t len, free, left;
	uint16_t opcode;
	uint8_t err;
} mp_buf, MsgPacket;

#define c_msgpack_error(msg) fprintf(stderr, "[MsgPack error]: %s\n", msg); MSGPACK_ASSERT(0)

enum MP_ERROR_OPCODE
{
	MP_CUR_ERROR_NONE = 0,
	MP_CUR_ERROR_EOF  = 1,		/* Not enough data to complete the opereation. */
	MP_CUR_ERROR_BADFMT = 2,	/* Bad data format */
	MP_ERROR_COUNT	  = 2
};

#define mp_buf_set_p(buf) buf->p = buf->b
#define mp_buf_get_content(buf) buf->b
#define mp_buf_set_size(buf, size) buf->len = size
#define mp_buf_get_size(buf) buf->len
#define mp_buf_set_opcode(buf, opcode) buf->opcode = opcode
#define mp_buf_get_opcode(buf) buf->opcode

#define mp_buf_set_read_pos(buf, pos) \
	buf->left = buf->len - pos;		  \
	mp_buf_set_p(buf)

#define mp_buf_get_read_pos(buf) buf->len - buf->left
#define mp_buf_get_write_pos(buf) buf->len

void mp_buf_constructor(mp_buf* buf, uint16_t opcode, size_t size);
void mp_buf_destructor(mp_buf* buf);
mp_buf* mp_buf_new(uint16_t opcode, size_t size);
void mp_buf_free(mp_buf* buf);
void mp_buf_resize(mp_buf* buf, size_t size);
void mp_buf_append(mp_buf *buf, const unsigned char *s, size_t len);
void mp_buf_pre_consume(mp_buf *buf, size_t len);
void mp_buf_init(mp_buf *buf, uint16_t opcode, size_t size);
#define mp_buf_reset(buf, opcode) mp_buf_init(buf, opcode, buf->len + buf->free)
mp_buf* mp_buf_pool_pop(uint16_t opcode, size_t size);
void mp_buf_pool_push(mp_buf* buf);
void mp_buf_pool_clear();

#define mp_pre_encode_int8(buf) { \
	unsigned char b[1] = {0};	  \
	mp_buf_append(buf, b, 1);     \
}

#define mp_replace_int8(buf, pos, n) { \
	unsigned char* b = buf->b + pos; \
	if ( (b[0] & 0x80) == 0 ) {		 \
		b[0] = n;					 \
	} else {						 \
		c_msgpack_error("Bad data format<int8> in replace pos."); \
	}								 \
}
	
#define mp_pre_encode_uint16(buf) {		\
	unsigned char b[3] = {0xcd, 0, 0};	\
	mp_buf_append(buf, b, 3);			\
}

#define mp_replace_uint16(buf, pos, n) { \
	unsigned char* b = buf->b + pos;\
	if ( b[0] == 0xcd ) {			\
		mp_encode_16(b, n);			\
	} else {						\
		c_msgpack_error("Bad data format<uint16> in replace pos."); \
	}								\
}

#define mp_pre_encode_uint32(buf) {			\
	unsigned char b[5] = {0xce, 0, 0, 0, 0};\
	mp_buf_append(buf, b, 5);				\
}

#define mp_replace_uint32(buf, pos, n) { \
	unsigned char* b = buf->b + pos;   \
	if ( b[0] == 0xce ) {			   \
		mp_encode_32(b, n);			   \
	} else {						   \
		c_msgpack_error("Bad data format<uint32> in replace pos."); \
	}								   \
}

#define mp_encode_buf_content(buf, subbuf) \
	mp_buf_append(buf, mp_buf_get_content(subbuf), mp_buf_get_size(subbuf))
void mp_encode_buf(mp_buf *buf, mp_buf *subbuf);
void mp_encode_double(mp_buf *buf, double d);
void mp_encode_bool(mp_buf *buf, int n);
void mp_encode_int(mp_buf *buf, int64_t n);
void mp_encode_bytes(mp_buf *buf, const unsigned char *s, size_t len);
#define mp_encode_string(buf, s) mp_encode_bytes(buf, (const unsigned char *)s, strlen(s)+1)

void mp_decode_string(mp_buf* c, char** str, size_t* len);
double mp_decode_double(mp_buf* c);
uint8_t mp_decode_bool(mp_buf* buf) ;
int64_t mp_decode_int(mp_buf* c);
void mp_decode_buf(mp_buf *buf, mp_buf *destbuf);


/* ------------------------------ String cursor ----------------------------------
 * This simple data structure is used for parsing. Basically you create a cursor
 * using a string pointer and a length, then it is possible to access the
 * current string position with cursor->p, check the remaining length
 * in cursor->left, and finally consume more string using
 * mp_cur_consume(cursor,len), to advance 'p' and subtract 'left'.
 * An additional field cursor->error is set to zero on initialization and can
 * be used to report errors. */
#define mp_cur_consume(_c,_len) do { _c->p += _len; _c->left -= _len; } while(0)

/* When there is not enough room we set an error in the cursor and return, this
 * is very common across the code so we have a macro to make the code look
 * a bit simpler. */
#define mp_cur_need(_c,_len) do {   \
	if (_c->left < _len) {		    \
		_c->err = MP_CUR_ERROR_EOF; \
		c_msgpack_error("Unpacks missing bytes in input."); \
		return; \
	} \
} while(0)

#define mp_over_floor(_c, len) do {    \
	if( len > 65535 ) {                \
		_c->err = MP_CUR_ERROR_EOF;    \
		c_msgpack_error("Unpacks lenght overflow!"); \
		return;                        \
	}                                  \
} while(0)

#define mp_isnan(_c, x) do {           \
	if( !((x)==(x)) ) {                \
		_c->err = MP_CUR_ERROR_EOF;    \
		c_msgpack_error("Unpacks number is NaN!"); \
		return;                        \
	}                                  \
} while(0)

#define mp_cur_need_num(_c,_len) do {  \
	if (_c->left < _len) {			   \
	_c->err = MP_CUR_ERROR_EOF;		   \
	c_msgpack_error("Unpacks missing bytes in input."); \
	return num;						   \
	} \
} while(0)

#define mp_decode_true(p) 1
#define mp_decode_false(p) 0

#define mp_decode_fixraw_len(p) p[0] & 0x1f
#define mp_decode_fixraw_pointer(p) (char*)p+1
#define mp_decode_raw16_len(p) (p[1] << 8) | p[2]
#define mp_decode_raw16_pointer(p) (char*)p+3
#define mp_decode_raw32_len(p) (p[1] << 24) | (p[2] << 16) | (p[3] << 8) | p[4]
#define mp_decode_raw32_pointer(p) (char*)p+5
#define mp_decode_array16_len(p) (p[1] << 8) | p[2]
#define mp_decode_array32_len(p) (p[1] << 24) | (p[2] << 16) | (p[3] << 8) | p[4]
#define mp_decode_map16_len(p) (p[1] << 8) | p[2]
#define mp_decode_map32_len(p) (p[1] << 24) | (p[2] << 16) | (p[3] << 8) | p[4]
#define mp_decode_fixarray_len(p) p[0] & 0xf
#define mp_decode_fixmap_len(p) p[0] & 0xf

void mp_memrevifle(void *ptr, size_t len);
#define mp_decode_f(p, f) memcpy(&f,p+1,4); mp_memrevifle(&f,4)
#define mp_decode_d(p, d) memcpy(&d,p+1,8); mp_memrevifle(&d,8)

#define mp_decode_positive_fixnum(p) p[0]
#define mp_decode_negative_fixnum(p) (signed char)p[0]
#define mp_decode_uint8(p) p[1]
#define mp_decode_int8(p) (char)p[1]
#define mp_decode_uint16(p) (p[1] << 8) | p[2]
#define mp_decode_int16(p) (int16_t)(p[1] << 8) | p[2]

#define mp_decode_uint32(p) ((uint32_t)p[1] << 24) | ((uint32_t)p[2] << 16) \
						| ((uint32_t)p[3] << 8) | (uint32_t)p[4]

#define mp_decode_int32(p) ((int32_t)p[1] << 24) | ((int32_t)p[2] << 16)	 \
						| ((int32_t)p[3] << 8) | (int32_t)p[4]

#define mp_decode_uint64(p) ((uint64_t)p[1] << 56) | ((uint64_t)p[2] << 48)  \
						| ((uint64_t)p[3] << 40) | ((uint64_t)p[4] << 32) \
						| ((uint64_t)p[5] << 24) | ((uint64_t)p[6] << 16) \
						| ((uint64_t)p[7] << 8) | (uint64_t)p[8]

#define mp_decode_int64(p) ((int64_t)p[1] << 56) | ((int64_t)p[2] << 48)	\
						| ((int64_t)p[3] << 40) | ((int64_t)p[4] << 32) \
						| ((int64_t)p[5] << 24) | ((int64_t)p[6] << 16) \
						| ((int64_t)p[7] << 8) | (int64_t)p[8]

#define mp_encode_8(b, n) b[1] = n & 0xff;

#define mp_encode_16(b, n) b[1] = (n & 0xff00) >> 8; b[2] = n & 0xff

#define mp_encode_32(b, n) b[1] = (n & 0xff000000) >> 24; \
						b[2] = (n & 0xff0000) >> 16;   \
						b[3] = (n & 0xff00) >> 8;      \
						b[4] = n & 0xff

#define mp_encode_64(b, n) b[1] = (n & 0xff00000000000000LL) >> 56; \
						b[2] = (n & 0xff000000000000LL) >> 48;   \
						b[3] = (n & 0xff0000000000LL) >> 40;     \
						b[4] = (n & 0xff00000000LL) >> 32;       \
						b[5] = (n & 0xff000000) >> 24;           \
						b[6] = (n & 0xff0000) >> 16;             \
						b[7] = (n & 0xff00) >> 8;                \
					 	b[8] = n & 0xff

enum mp_encode_len {
	mp_encode_fix_len = 1,
	mp_encode_8_len   = 2,
	mp_encode_16_len  = 3,
	mp_encode_32_len  = 5,
	mp_encode_64_len  = 9,
};

enum mp_type {
	mp_fixmaplow = 0x80,
	mp_fixarrlow = 0x90,
	mp_fixrawlow = 0xa0,
	
	mp_nil   = 0xc0,
	mp_false = 0xc2,
	mp_true  = 0xc3,
	mp_tbuf	 = 0xc4,

	mp_float  = 0xca,
	mp_double = 0xcb,

	mp_uint8  = 0xcc,
	mp_uint16 = 0xcd,
	mp_uint32 = 0xce,
	mp_uint64 = 0xcf,

	mp_int8  = 0xd0,
	mp_int16 = 0xd1,
	mp_int32 = 0xd2,
	mp_int64 = 0xd3,
	
	mp_raw16 = 0xda,
	mp_raw32 = 0xdb,

	mp_array16 = 0xdc,
	mp_array32 = 0xdd,

	mp_map16 = 0xde,
	mp_map32 = 0xdf,

	mp_neglow = 0xe0,
};

#ifdef __cplusplus
}
#endif

#endif
