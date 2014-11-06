
// Bit function
#define     setBit(x, bit)       (x)|=(1<<(bit))
#define     clearBit(x, bit)     (x)&=~(1<<(bit))
#define     ifBitSet(x, bit)     ((x)&(1<<bit))
#define     WDR()                #asm("wdr")