from enum import IntEnum

class Dir(IntEnum):
    EAST = 0
    NORTHEAST = 1
    NORTH = 2
    NORTHWEST = 3
    WEST = 4
    SOUTHWEST = 5
    SOUTH = 6
    SOUTHEAST = 7

    def dir_to_str(self):
        if self == Dir.EAST:
            return "EAST"
        elif self == Dir.NORTHEAST:
            return "NORTHEAST"
        elif self == Dir.NORTH:
            return "NORTH"
        elif self == Dir.NORTHWEST:
            return "NORTHWEST"
        elif self == Dir.WEST:
            return "WEST"
        elif self == Dir.SOUTHWEST:
            return "SOUTHWEST"
        elif self == Dir.SOUTH:
            return "SOUTH"
        elif self == Dir.SOUTHEAST:
            return "SOUTHEAST"


