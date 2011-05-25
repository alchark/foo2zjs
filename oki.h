/*
 * Manifest constants for the ZjStream protocol (okidata flavor)
 */

#include <inttypes.h>
typedef uint32_t	DWORD;
typedef uint16_t	WORD;
typedef uint8_t		BYTE;

typedef enum {
    OKI_START_PAGE	= 0,
    OKI_DATA		= 1,
    OKI_END_PAGE	= 255,
} OKI_RECTYPE;

typedef union _SWAP_32{
    char		byte[sizeof(uint32_t)];
    uint32_t		dword;
} SWAP_32;

typedef union _SWAP_16{
    char		byte[sizeof(uint16_t)];
    uint16_t		word;
} SWAP_16;

static inline uint32_t
be32(uint32_t dword)
{
    SWAP_32	swap;
    uint32_t	probe = 1;

    if (((char *)&probe)[0] == 1)
    {
	swap.byte[3] = (( SWAP_32 )dword).byte[0]; 
	swap.byte[2] = (( SWAP_32 )dword).byte[1];
	swap.byte[1] = (( SWAP_32 )dword).byte[2]; 
	swap.byte[0] = (( SWAP_32 )dword).byte[3];
	return swap.dword;
    }
    else
	return dword;
}

static inline uint16_t
be16(uint16_t word)
{
    SWAP_16	swap;
    uint16_t	probe = 1;

    if (((char *)&probe)[0] == 1)
    {
	swap.byte[1] = (( SWAP_16 )word).byte[0]; 
	swap.byte[0] = (( SWAP_16 )word).byte[1];
	return swap.word;
    }
    else
	return word;
}
