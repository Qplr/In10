#include "Orientation.h"

unsigned char Orientation::sideToMask(Tile::Side side)
{
    switch (side)
    {
    case Tile::N: return north_mask;
    case Tile::S: return south_mask;
    case Tile::W: return west_mask;
    case Tile::E: return east_mask;
    default: return 0x0;
    }
}
