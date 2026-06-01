#ifndef MXENDIAN_H
#define MXENDIAN_H

#include "mxtypes.h"

#include <SDL3/SDL_endian.h>
#include <string.h>

inline MxU16 EndianReadLE16(const void* p_data)
{
	MxU16 val;
	memcpy(&val, p_data, sizeof(val));
	return SDL_Swap16LE(val);
}

inline MxS16 EndianReadLES16(const void* p_data)
{
	MxU16 val;
	memcpy(&val, p_data, sizeof(val));
	return (MxS16) SDL_Swap16LE(val);
}

inline MxU32 EndianReadLE32(const void* p_data)
{
	MxU32 val;
	memcpy(&val, p_data, sizeof(val));
	return SDL_Swap32LE(val);
}

inline MxS32 EndianReadLES32(const void* p_data)
{
	MxU32 val;
	memcpy(&val, p_data, sizeof(val));
	return (MxS32) SDL_Swap32LE(val);
}

inline MxU64 EndianReadLE64(const void* p_data)
{
	MxU64 val;
	memcpy(&val, p_data, sizeof(val));
	return SDL_Swap64LE(val);
}

inline float EndianReadLEFloat(const void* p_data)
{
	MxU32 val;
	memcpy(&val, p_data, sizeof(val));
	val = SDL_Swap32LE(val);
	float result;
	memcpy(&result, &val, sizeof(result));
	return result;
}

inline double EndianReadLEDouble(const void* p_data)
{
	MxU64 val;
	memcpy(&val, p_data, sizeof(val));
	val = SDL_Swap64LE(val);
	double result;
	memcpy(&result, &val, sizeof(result));
	return result;
}

inline void EndianWriteLE16(void* p_dst, MxU16 p_val)
{
	MxU16 le = SDL_Swap16LE(p_val);
	memcpy(p_dst, &le, sizeof(le));
}

inline void EndianWriteLE32(void* p_dst, MxU32 p_val)
{
	MxU32 le = SDL_Swap32LE(p_val);
	memcpy(p_dst, &le, sizeof(le));
}

#endif // MXENDIAN_H
