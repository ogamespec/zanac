# Enemy Data

Maximum number of enemies = 26 (0x1a). The game stores the state of game objects in several arrays at addresses $500...$7FF. The size of each array is 26.

The first 5 objects are reserved for Zanac (ship + 3 shots + 1 special shot). The `check_enemy_overflow` procedure therefore skips the first 5 slots.

The term "enemy" should be understood in a broad sense - it is any game object (shots, bonus items, enemies, the player, etc.). It was just initially assumed that it was only for enemies, but it turned out that the game universally handles all game objects, but the term "enemy" was retained.

## byte_528 - Type

The most significant bit is used to show that the object is spawned. Each game object handler procedure first checks - if the object is not spawned, it prepares its other parameters and sets msb.

## byte_542 - Y Position

## byte_55C - X Position

## byte_576 - Sprite ID

The identifier of the meta sprite, for displaying the object.

## byte_590

## byte_5AA

## byte_5C4 

## byte_5DE 

## byte_5F8 

## byte_612 - Y Speed

## byte_62C

## byte_646 - X Speed

## byte_660 - Flags

Flags for applying speed to object coordinates (kinematics), each element contains a mask:
- 8:
- 0x10:
- 1: Y Speed active
- 2: 

(It also looks like there are more flags, not sure what they are yet)

## byte_67A

## byte_694

## byte_6AE

## byte_6C8

## byte_6E2 - Chase Timeout

Timer (down counter) used to update the pursuit coordinates. After reaching 0, it is refreshed from Chase Initial Timeout.

Pursuit is done by the following enemies: Carla(T-Cell), Valkyrie, Fairy, as well as Capital Ships: Girevu (shoot all around), Hume (X-wing), Soreido (with Super hard bolts) and Somasu (huge carrier).

## byte_6FC - Chase Counter

Number of Zanac pursuit attempts (down counter).

## byte_716 - Chase Initial Timeout

Copied to Chase Timeout after the countdown ends and sub_18024 is called.

## byte_730

## byte_74A

## byte_764 - Enemy Health

For example, Giza has 16 HP. Each shot at such a tough enemy is accompanied by a ringing sound.

## byte_77E

## byte_798

## byte_7B2

## byte_7CC

## byte_7E6
