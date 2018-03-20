#include "cmsgpack.h"

#define LARGE_SIZE 1073741824
/* --------------------------- Endian conversion --------------------------------
 * We use it only for floats and doubles, all the other conversions are performed
 * in an endian independent fashion. So the only thing we need is a function
 * that swaps a binary string if the arch is little endian (and left it untouched
 * otherwise). */

/* Reverse memory bytes if arch is little endian. Given the conceptual
 * simplicity of the Lua build system we prefer to check for endianess at runtime.
 * The performance difference should be acceptable. */
void mp_memrevifle(void *ptr, size_t len) {
    unsigned char *p = (unsigned char *)ptr, *e = p+len-1, aux;
    int test = 1;
    unsigned char *testp = (unsigned char*) &test;

    if (testp[0] == 0) return; /* Big endian, nothign to do. */
    len /= 2;
    while(len--) {
        aux = *p;
        *p = *e;
        *e = aux;
        p++;
        e--;
    }
}

/* ----------------------------- String buffer ----------------------------------
 * This is a simple implementation of string buffers. The only opereation
 * supported is creating empty buffers and appending bytes to it.
 * The string buffer uses 2x preallocation on every realloc for O(N) append
 * behavior.  */
void mp_buf_init(mp_buf *buf, uint16_t opcode, size_t size) {
	mp_buf_set_size(buf, 0);
	mp_buf_set_opcode(buf, opcode);

	mp_buf_set_p(buf);
	buf->free = size;
	buf->left = 0;
	
	buf->err = MP_CUR_ERROR_NONE;
}

void mp_buf_constructor(mp_buf* buf, uint16_t opcode, size_t size) {
	size = size < MP_BUF_DEFAULT_SIZE? MP_BUF_DEFAULT_SIZE: size;
	buf->b = (unsigned char *)malloc(size);

	mp_buf_init(buf, opcode, size);
}

void mp_buf_destructor(mp_buf* buf) {
	buf->p = NULL;
	free(buf->b);
}

mp_buf* mp_buf_new(uint16_t opcode, size_t size) {
	mp_buf* buf = (mp_buf *)malloc(sizeof(*buf));

	size = size < MP_BUF_DEFAULT_SIZE? MP_BUF_DEFAULT_SIZE: size;
	buf->b = (unsigned char *)malloc(size);

	mp_buf_init(buf, opcode, size);
    return buf;
}

void mp_buf_free(mp_buf* buf) {
	buf->p = NULL;
	free(buf->b);
	free(buf);
}

void mp_buf_resize(mp_buf* buf, size_t size) {
	size_t oldsize = buf->len + buf->free;
	if ( oldsize >= size ) return;

	buf->b = (unsigned char *)realloc(buf->b,size);
	mp_buf_set_p(buf);
	buf->free = size - buf->len;
}

void mp_buf_pre_consume(mp_buf *buf, size_t len) {
	size_t nsize = buf->len + len;
	if (buf->free < len) {
		size_t size = buf->len + buf->free;

		if( LARGE_SIZE <= nsize ) {
			c_msgpack_error("resize too large!");
		}
		
		while( size < nsize ) size <<= 1;
		buf->b = (unsigned char *)realloc(buf->b, size);
		mp_buf_set_p(buf);
		buf->free = size - buf->len;
	}
	mp_buf_set_size(buf, nsize);
	buf->left = nsize;
	buf->free -= len;
}

void mp_buf_append(mp_buf *buf, const unsigned char *s, size_t len) {
	size_t oldlen = buf->len;
	mp_buf_pre_consume(buf, len);
	memcpy(buf->b+oldlen,s,len);
}

//void mp_buf_reset(mp_buf *buf, uint16_t opcode) {
//	size_t len = buf->len + buf->free;
//	//memset(buf->b, 0, len);
//	mp_buf_init(buf, opcode, len);
//}


/* --------------------------- Low level MP encoding -------------------------- */
void mp_encode_buf(mp_buf *buf, mp_buf *subbuf) {
	unsigned char type = mp_tbuf;
	size_t size = mp_buf_get_size(subbuf);
	uint16_t opcode = mp_buf_get_opcode(subbuf);
	//mp_buf_header header;
	//mp_header_set_size(header, size);
	//mp_header_set_opcode(header, opcode);

	mp_buf_append(buf, &type, 1);						// encode type
	//mp_buf_append(buf, (unsigned char*)&header, sizeof(mp_buf_header));  // encode header
	mp_encode_int(buf, opcode);
	mp_encode_int(buf, size);
	mp_buf_append(buf, subbuf->b, size);				// encode content
}

void mp_encode_bytes(mp_buf *buf, const unsigned char *s, size_t len) {
    unsigned char hdr[5];
    int hdrlen;

    if (len < 32) {
        hdr[0] = mp_fixrawlow | (len&0xff); /* fix raw */
        hdrlen = mp_encode_fix_len;
    } else if (len <= 0xffff) {
        hdr[0] = mp_raw16;
        mp_encode_16(hdr, len);
        hdrlen = mp_encode_16_len;
    } else {
        hdr[0] = mp_raw32;
		mp_encode_32(hdr, len);
        hdrlen = mp_encode_32_len;
    }
    mp_buf_append(buf,hdr,hdrlen);
    mp_buf_append(buf,s,len);
}

/* we assume IEEE 754 internal format for single and double precision floats. */
void mp_encode_double(mp_buf *buf, double d) {
    unsigned char b[9];
    float f = d;

    MSGPACK_ASSERT(sizeof(f) == 4 && sizeof(d) == 8);
    if (d == (double)f) {
        b[0] = mp_float;    /* float IEEE 754 */
        memcpy(b+1,&f,4);
        mp_memrevifle(b+1,4);
        mp_buf_append(buf,b,mp_encode_32_len);
    } else if (sizeof(d) == 8) {
        b[0] = mp_double;    /* double IEEE 754 */
        memcpy(b+1,&d,8);
        mp_memrevifle(b+1,8);
        mp_buf_append(buf,b,mp_encode_64_len);
    }
}

void mp_encode_bool(mp_buf *buf, int n) {
	unsigned char b = n ? mp_true : mp_false;
	mp_buf_append(buf,&b,mp_encode_fix_len);
}

void mp_encode_int(mp_buf *buf, int64_t n) {
    unsigned char b[9];
    int enclen;

    if (n >= 0) {
        if (n <= 127) {
            //b[0] = n & 0x7f;    /* positive fixnum */
			b[0] = n;
            enclen = mp_encode_fix_len;
        } else if (n <= 0xff) {
            b[0] = mp_uint8;        /* uint 8 */
            mp_encode_8(b, n);
            enclen = mp_encode_8_len;
        } else if (n <= 0xffff) {
            b[0] = mp_uint16;        /* uint 16 */
            mp_encode_16(b, n);
            enclen = mp_encode_16_len;
        } else if (n <= 0xffffffffLL) {
            b[0] = mp_uint32;        /* uint 32 */
            mp_encode_32(b, n);
            enclen = mp_encode_32_len;
        } else {
            b[0] = mp_uint64;        /* uint 64 */
            mp_encode_64(b, n);
            enclen = mp_encode_64_len;
        }
    } else {
        if (n >= -32) {
            b[0] = ((char)n);   /* negative fixnum */
            enclen = mp_encode_fix_len;
        } else if (n >= -128) {
            b[0] = mp_int8;        /* int 8 */
            mp_encode_8(b, n);
            enclen = mp_encode_8_len;
        } else if (n >= -32768) {
            b[0] = mp_int16;        /* int 16 */
            mp_encode_16(b, n);
            enclen = mp_encode_16_len;
        } else if (n >= -2147483648LL) {
            b[0] = mp_int32;        /* int 32 */
            mp_encode_32(b, n);
            enclen = mp_encode_32_len;
        } else {
            b[0] = mp_int64;        /* int 64 */
			mp_encode_64(b, n);
            enclen = mp_encode_64_len;
        }
    }
    mp_buf_append(buf,b,enclen);
}

/* --------------------------------- Decoding --------------------------------- */

void mp_decode_buf(mp_buf *buf, mp_buf *destbuf) {
	const unsigned char type = buf->p[0];
	mp_cur_consume(buf, 1);

	if ( type == mp_tbuf ) {
		uint16_t opcode = mp_decode_int(buf);
		size_t	 size = mp_decode_int(buf);
		//mp_buf_header header;
		//header = *((mp_buf_header*)++p);
		//size   = mp_header_get_size(header);
		//opcode = mp_header_get_opcode(header);
		mp_cur_need(buf,size);
		mp_buf_reset(destbuf, opcode);
		mp_buf_append(destbuf, buf->p, size);
		mp_cur_consume(buf, size);
	} else {
		c_msgpack_error("Bad data format<buf> in input.");
	}
}

uint8_t mp_decode_bool(mp_buf* buf) {
	uint8_t num = 0;
	mp_cur_need_num(buf,1);
	switch(buf->p[0]) {
	case mp_true:  /* true */
		mp_cur_need_num(buf,mp_encode_fix_len);
		num = mp_decode_true(buf->p);
		mp_cur_consume(buf,mp_encode_fix_len);
		break;
	case mp_false:  /* false */
		mp_cur_need_num(buf,mp_encode_fix_len);
		num = mp_decode_false(buf->p);
		mp_cur_consume(buf,mp_encode_fix_len);
		break;
	default:
		c_msgpack_error("Bad data format<bool> in input.");
	}

	return num;
}

int64_t mp_decode_int(mp_buf* buf) {
	int64_t num = 0;
	const unsigned char type = buf->p[0];
	mp_cur_need_num(buf,1);

	if ( type < mp_fixmaplow ) {   /* positive fixnum */
		num = mp_decode_positive_fixnum(buf->p);
		mp_cur_consume(buf,mp_encode_fix_len);
		return num;
	}

	switch(type) {
	case mp_uint8:  /* uint 8 */
		mp_cur_need_num(buf,mp_encode_8_len);
		num = mp_decode_uint8(buf->p);
		mp_cur_consume(buf,mp_encode_8_len);
		break;
	case mp_uint16:  /* uint 16 */
		mp_cur_need_num(buf,mp_encode_16_len);
		num = mp_decode_uint16(buf->p);
		mp_cur_consume(buf,mp_encode_16_len);
		break;
	case mp_uint32:  /* uint 32 */
		mp_cur_need_num(buf,mp_encode_32_len);
		num = mp_decode_uint32(buf->p);
		mp_cur_consume(buf,mp_encode_32_len);
		break;
	case mp_uint64:  /* uint 64 */
		mp_cur_need_num(buf,mp_encode_64_len);
		num = mp_decode_uint64(buf->p);
		mp_cur_consume(buf,mp_encode_64_len);
		break;
	case mp_int8:  /* int 8 */
		mp_cur_need_num(buf,mp_encode_8_len);
		num = mp_decode_int8(buf->p);
		mp_cur_consume(buf,mp_encode_8_len);
		break;
	case mp_int16:  /* int 16 */
		mp_cur_need_num(buf,mp_encode_16_len);
		num = mp_decode_int16(buf->p);
		mp_cur_consume(buf,mp_encode_16_len);
		break;
	case mp_int32:  /* int 32 */
		mp_cur_need_num(buf,mp_encode_32_len);
		num = mp_decode_int32(buf->p);
		mp_cur_consume(buf,mp_encode_32_len);
		break;
	case mp_int64:  /* int 64 */
		mp_cur_need_num(buf,mp_encode_64_len);
		num = mp_decode_int64(buf->p);
		mp_cur_consume(buf,mp_encode_64_len);
		break;
	default:    /* types that can't be idenitified by first byte value. */
		if ( type >= mp_neglow ) {  /* negative fixnum */
			num = mp_decode_negative_fixnum(buf->p);
			mp_cur_consume(buf,mp_encode_fix_len);
		} else {
			c_msgpack_error("Bad data format<int> in input.");
		}
	}
	
	return num;
}

double mp_decode_double(mp_buf* buf) {
	double num = 0.0;
	mp_cur_need_num(buf,1);
	switch(buf->p[0]) {
	case mp_float:  /* float */
		mp_cur_need_num(buf,mp_encode_32_len);
		MSGPACK_ASSERT(sizeof(float) == 4);
		{
			float f;
			mp_decode_f(buf->p, f);
			num = f;
			mp_cur_consume(buf,mp_encode_32_len);
		}
		break;
	case mp_double:  /* double */
		mp_cur_need_num(buf,mp_encode_64_len);
		MSGPACK_ASSERT(sizeof(double) == 8);
		{
			mp_decode_d(buf->p, num);
			mp_cur_consume(buf,mp_encode_64_len);
		}
		break;
	default:
		c_msgpack_error("Bad data format<double> in input.");
	}
	
	if( !(num == num) ) {
		c_msgpack_error("Unpacks number is NaN!");
	}

	return num;
}

void mp_decode_string(mp_buf* buf, char** str, size_t* len) {
	size_t l;
	const unsigned char type = buf->p[0];
	mp_cur_need(buf,1);
	switch(type) {
	case mp_raw16:  /* raw 16 */
		mp_cur_need(buf,mp_encode_16_len);
		{
			l = mp_decode_raw16_len(buf->p);
			mp_cur_need(buf,mp_encode_16_len+l);
			*str = mp_decode_raw16_pointer(buf->p);
			mp_cur_consume(buf,mp_encode_16_len+l);
		}
		break;
	case mp_raw32:  /* raw 32 */
		mp_cur_need(buf,mp_encode_32_len);
		{
			l = mp_decode_raw32_len(buf->p);
			mp_cur_need(buf,mp_encode_32_len+l);
			*str = mp_decode_raw32_pointer(buf->p);
			mp_cur_consume(buf,mp_encode_32_len+l);
		}
		break;
	default:    /* types that can't be idenitified by first byte value. */
		if ( type >= mp_fixrawlow && type < mp_nil ) {  /* fix raw */
			l = mp_decode_fixraw_len(buf->p);
			mp_cur_need(buf,mp_encode_fix_len+l);
			*str = mp_decode_fixraw_pointer(buf->p);
			mp_cur_consume(buf,mp_encode_fix_len+l);
		} else {
			c_msgpack_error("Bad data format<string> in input.");
		}
	}
	*len = l;
}
