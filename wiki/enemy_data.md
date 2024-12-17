# Enemy Data

Maximum number of enemies = 26 (0x1a). The game stores the state of game objects in several arrays at addresses $500...$7FF. The size of each array is 26.

The first 5 objects are reserved for Zanac (ship + 3 shots + 1 special shot). The `check_enemy_overflow` procedure therefore skips the first 5 slots.

## byte_528 

The most significant bit is used for some purpose (probably for tagging aeral/ground objects).

## byte_542 

## byte_55C

## byte_576

## byte_5AA

## byte_5C4 

## byte_5DE 

## byte_5F8 

## byte_612

## byte_62C

## byte_646

## byte_660

Some flags (each element contains a mask)

## byte_67A

## byte_694

## byte_6AE

## byte_6C8

## byte_6E2

## byte_6FC

## byte_716

## byte_730

## byte_7E6
