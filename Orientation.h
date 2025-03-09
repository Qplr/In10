#pragma once
#include "Tile.h"

class Orientation
{
	static const unsigned char north_mask = 0x01;
	static const unsigned char south_mask = 0x02;
	static const unsigned char west_mask = 0x04;
	static const unsigned char east_mask = 0x08;

	unsigned char mask = 0x0;
	static unsigned char sideToMask(Tile::Side side);
public:
	inline bool hasInputFrom(Tile::Side side) const { return mask & sideToMask(side); }
	inline bool hasOutputTo(Tile::Side side) const { return mask & (sideToMask(side) << 4); }

	inline bool hasConnection(Tile::Side side) const { return hasInputFrom(side) || hasOutputTo(side); }
	inline void setConnection(Tile::Side side) { setInputFrom(side); setOutputTo(side); }
	inline void unSetConnection(Tile::Side side) { unSetInputFrom(side); unSetOutputTo(side); }

	inline void setInputFrom(Tile::Side side) { mask |= sideToMask(side); }
	inline void setOutputTo(Tile::Side side) { mask |= (sideToMask(side) << 4); }

	inline void unSetInputFrom(Tile::Side side) { mask &= ~sideToMask(side); }
	inline void unSetOutputTo(Tile::Side side) { mask &= ~(sideToMask(side) << 4); }
};

