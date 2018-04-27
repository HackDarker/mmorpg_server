#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint16_t NetID;


#define INVAILD_SOCKETID 0

#define SERVER_TYPE_GATEWAY  1
#define SERVER_TYPE_DB       2
#define SERVER_TYPE_GLOBAL   3
#define SERVER_TYPE_MAP      4
#define SERVER_TYPE_LOGIN    5

enum
{
	CLIENT_TYPE_NULL = 0,
	CLIENT_TYPE_DATABASE,
	CLIENT_TYPE_GAME,
	CLIENT_TYPE_LOGIN,
};

#endif
