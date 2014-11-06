#define MLX90614_ADDR   0xB4

#define DISP_DATA       PORTB
#define DISP_DATA_DDR   DDRB

#define MAX_SEGMENT     4     
#define DISP_SEG        PORTD
#define DISP_SEG_DDR    DDRD
#define DISP_SEG_1      4
#define DISP_SEG_2      5
#define DISP_SEG_3      6
#define DISP_SEG_4      7


#define TEN_CTRL        PORTC
#define TEN_PIN1        0
#define TEN_PIN2        1


#define  MAX_TEMP       55      //70 градусов
#define  MAX_SECOND     32400   //9 часов


#define FLAG_START      0
#define FLAG_WORK       1
#define FLAG_END        2
#define FLAG_ERR        3