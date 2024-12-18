# Enemy Data

Maximum number of enemies = 26 (0x1a). The game stores the state of game objects in several arrays at addresses $500...$7FF. The size of each array is 26.

The first 5 objects are reserved for Zanac (ship + 3 shots + 1 special shot). The `check_enemy_overflow` procedure therefore skips the first 5 slots.

The term "enemy" should be understood in a broad sense - it is any game object (shots, bonus items, enemies, the player himself, etc.). It was just initially assumed that it was only for enemies, but it turned out that the game universally handles all game objects, but the term "enemy" was retained.

## byte_528 - Type

The most significant bit is used to show that the object is spawned. Each game object handler procedure first checks - if the object is not spawned, it prepares its other parameters and sets msb.

## byte_542 - Y Position

## byte_55C - X Position

## byte_576

## byte_590

## byte_5AA

## byte_5C4 

## byte_5DE 

## byte_5F8 

## byte_612 - Y Speed

## byte_62C

## byte_646 - X Speed

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

## byte_764 - Enemy Health

For example, Giza has 16 HP. Each shot at such a tough enemy is accompanied by a ringing sound.

## byte_7E6
