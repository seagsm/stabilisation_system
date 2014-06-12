#ifndef API_PACKET_STRUCTURE_H
#define API_PACKET_STRUCTURE_H 1

#include "stm32f10x.h"

#define HEAD_OF_PACKET  0x73U

#define READ            0x00U
#define READ8           0x10U
#define READ16          0x20U
#define READ32          0x30U
#define READ64          0x40U
#define READQUAT        0x50U
#define READVECTOR3D    0x60U

#define WRITE    0x01U
#define WRITE8   0x11U
#define WRITE16  0x21U
#define WRITE32  0x31U
#define WRITE64  0x41U

#define ANSWER   0x02U
#define WRITE_OK 0x03U
#define ERROR    0x04U

#endif