/*****************************************************
This program was produced by the
CodeWizardAVR V2.04.4a Advanced
Automatic Program Generator
© Copyright 1998-2009 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 07.02.2014
Author  : NeVaDa
Company : 
Comments: 


Chip type               : ATtiny2313
AVR Core Clock frequency: 8,000000 MHz
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 32
*****************************************************/

#include <mega8.h>
#include <delay.h>
// Declare your global variables here
#include <global.h>
#include "bitfunc.h"

// I2C Bus functions
#asm
   .equ __i2c_port=0x15 ;PORTC
   .equ __sda_bit=4
   .equ __scl_bit=3
#endasm
#include <i2c.h>
                            //  0    1    2    3    4    5    6    7    8    9
const unsigned char codes[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
const unsigned char symbGrad = 0x63; //
const unsigned char symbC = 0x39; //
const unsigned char codeErr[3] = {0x79, 0x50, 0x50};//Err 
volatile unsigned char segment = 0; //отображаемый сешмент
  
volatile int start_temp = 0;
volatile int curent_temp = 0;
volatile int set_temp = 55; // 55 градусов
     
volatile unsigned int  start_second = 0;
volatile unsigned int  work_second = 0;
volatile unsigned char show_time = 0;
volatile unsigned int  WORK_STATUS = 0;


void config_chip();
void disp_segment_code(unsigned char code, int segment);
void display_temp(int temp);
void display_work_time(unsigned int second);
unsigned char get_temp_f(float* temp_val, unsigned int addr);
unsigned char get_temp(int* temp_val, unsigned int addr);


void config_chip()
{
    // Port B initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTB=0x00;
    DDRB=0x00;
    
    // Port D initialization
    // Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTD=0x00;
    DDRD=0x00;
    
    // Timer/Counter 0 initialization
    // Clock source: 125,000 kHz
    // Clock value: Timer 0 Stopped
    // Mode: Normal top=FFh
    // OC0A output: Disconnected
    // OC0B output: Disconnected
    TCCR0=0x03;
    TCNT0=0x00;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 7,813 kHz
    // Mode: CTC top=OCR1A
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: On
    // Compare B Match Interrupt: Off
    TCCR1A=0x00;
    TCCR1B=0x0D;
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x1E;
    OCR1AL=0x84;
    OCR1BH=0x00;
    OCR1BL=0x00; 
        
    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: Timer2 Stopped
    // Mode: Normal top=FFh
    // OC2 output: Disconnected
    ASSR=0x00;
    TCCR2=0x00;
    TCNT2=0x00;
    OCR2=0x00;

    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // Interrupt on any change on pins PCINT0-7: Off
    MCUCR=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x11;

    // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR=0x80;
    SFIOR=0x00;   
    
    
    // ADC initialization
    // ADC disabled
    ADCSRA=0x00;

    // SPI initialization
    // SPI disabled
    SPCR=0x00;

    // TWI initialization
    // TWI disabled
    TWCR=0x00;
        
    // Global enable interrupts
    #asm("sei")
}


// Отображение температуры, дисплей с Общим Катодом
// для инверсии кода цифры нужно отнять 255
void display_temp(int temp)
{    
    unsigned char Dig[2]={0};
      
    while (temp >= 100) {
        temp -= 100;  
//        Dig[2]++; 
    }
    
    while (temp >= 10) {
        temp -= 10;  
        Dig[1]++; 
    } 
    
    Dig[0] = temp;     
                
    segment++; 
    if (segment==MAX_SEGMENT) 
        segment = 0; 
        
    switch (segment) {           
        case 0:
            //TODO: знак температуры 
            disp_segment_code(0xFF, segment);
        break;
            
        case 1: 
        case 2:
            disp_segment_code( codes[ Dig[sizeof(Dig)-segment] ], segment);                     
        break;   
        
        case 3: 
            disp_segment_code(symbC, segment);   //цельсия
        break;    
        
        default: 
            disp_segment_code(0xFF, segment);
        break;
    }              
}


void display_work_time(unsigned int second)
{
    unsigned int minute = second/60;
    unsigned char Dig[3]={0};

    while (minute >= 100) {
        minute -= 100;  
        Dig[2]++; 
    }
    
    while (minute >= 10) {
        minute -= 10;  
        Dig[1]++; 
    } 
    
    Dig[0] = minute;                         
    
    segment++; 
    if (segment==MAX_SEGMENT) 
        segment = 0; 
        
    switch (segment) {
        case 1:   
        case 2:   
        case 3:
            disp_segment_code(codes[Dig[sizeof(Dig)-segment]], segment);   //цельсия
        break;
            
        default:  
            disp_segment_code(0x00, segment);
        break;
    }   
}


void display_err(unsigned int code)
{                        
    segment++; 
    if (segment==MAX_SEGMENT) 
        segment = 0; 
        
    switch (segment) {
        case 0:              
        case 1:
        case 2:           
            disp_segment_code(codeErr[segment], segment);
        break;
        
        case 3:
            disp_segment_code(code, segment);   
        break;
            
        default:  
            disp_segment_code(0x00, segment);
        break;
    }   
}


// конфигурация портов индикатора
void disp_config(unsigned char reverse)
{
    if (reverse) {
        DISP_SEG_DDR |= ((1<<DISP_SEG_1)|(1<<DISP_SEG_2)|(1<<DISP_SEG_3)|(1<<DISP_SEG_4));
        DISP_SEG &= ~((1<<DISP_SEG_1)|(1<<DISP_SEG_2)|(1<<DISP_SEG_3)|(1<<DISP_SEG_4));
    
        DISP_DATA_DDR = 0xFF;
        DISP_DATA = 0x00;
    }else {               
        DISP_SEG_DDR &= ~((1<<DISP_SEG_1)|(1<<DISP_SEG_2)|(1<<DISP_SEG_3)|(1<<DISP_SEG_4)); 
        DISP_SEG |= (1<<DISP_SEG_1)|(1<<DISP_SEG_2)|(1<<DISP_SEG_3)|(1<<DISP_SEG_4);
    
        DISP_DATA_DDR = 0x00;
        DISP_DATA = 0x00;
    }
}

// зажечь сегмент
void disp_segment_code(unsigned char code, int segment)
{        
    DISP_SEG &= ~((1<<DISP_SEG_1)|(1<<DISP_SEG_2)|(1<<DISP_SEG_3)|(1<<DISP_SEG_4));       
        
    DISP_DATA = 0xFF - code;
                   
    if (segment == 0)  
        setBit(DISP_SEG, DISP_SEG_1);
    else if (segment == 1)
        setBit(DISP_SEG, DISP_SEG_2);
    else if (segment == 2)           
        setBit(DISP_SEG, DISP_SEG_3);
    else if (segment == 3)           
        setBit(DISP_SEG, DISP_SEG_4);
}


unsigned char get_temp_f(float* temp_val, unsigned int addr)
{
    char p1,p2,p3;
    float temp_thermo;
    unsigned char ch;
 
    i2c_stop();                                 //stop i2c if not ack
    delay_ms(10);
    
    i2c_start();                                //start I2C
                       
    ch = i2c_write(addr);                 //device address with write condition    
    if (!ch) return 0;                      //No Ack, return False
    
    ch = i2c_write(0x07);                       //device ram address where Tobj value is present 
    if(!ch) return 0;                       //No Ack, return False
 
 
    i2c_start();                                //repeat start
    ch = i2c_write(addr|0x01);           //device address with read condition 
    if (!ch) return 0;                      //No Ack, return False
 
    p1 = i2c_read(1);     //Tobj low byte
    p2 = i2c_read(1);     //Tobj heigh byte
    p3 = i2c_read(0);     //PEC
    
    i2c_stop();                            //stop condition
         
    temp_thermo = ((((p2&0x007f)<<8)+p1)*0.02)-0.01;      //degree centigrate conversion
    *temp_val = temp_thermo-273;                          //Convert kelvin to degree Celsius
    
    return 1;                            //load data successfully, return true 
}


unsigned char get_temp(int* temp_val, unsigned int addr)
{
    float temp = 0;
    unsigned char ret;   
    
    *temp_val = 0;
    ret = get_temp_f(&temp, addr);
    if (!ret) 
        return 0; 
        
    *temp_val = (int)temp;
    
    return 1;
}


interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{   
    if (show_time >= 5) {
        if ( ifBitSet(WORK_STATUS, FLAG_START) )
            display_work_time(start_second);
        else           
            display_work_time(work_second);          
    }else {      
        if (ifBitSet(WORK_STATUS, FLAG_ERR))
            display_err(0);
        else 
            display_temp(curent_temp);
    }  
}

// resolution 1sec
// Timer 0 overflow interrupt service routine
interrupt [TIM1_COMPA] void timer1_ovf_isr(void)
{   
    if (ifBitSet(DDRC, 5)) clearBit(DDRC, 5);   
    else setBit(DDRC, 5);
      
    show_time++;
    if (show_time > 7)
        show_time = 0;
        
    if (!ifBitSet(WORK_STATUS, FLAG_END)) {
        if ( ifBitSet(WORK_STATUS, FLAG_START) )      
            start_second++;                                         
        else
            work_second++;
    }          
}


void main(void)
{
    unsigned char req;
    int temp;
    config_chip();
    disp_config(1);
         
    // I2C Bus initialization    
    i2c_init();                  
    delay_ms(2000);
     
    // MLX90614 чтение температуры         
    req = get_temp(&start_temp, MLX90614_ADDR); 
    setBit(WORK_STATUS, FLAG_START);
                  
    
    while (1)
    {
        // MLX90614 чтение температуры         
        req = get_temp(&curent_temp, MLX90614_ADDR);
        if (!req) {   
            setBit(WORK_STATUS, FLAG_ERR);        
            delay_ms(1000); 
            continue;
        }else {
            clearBit(WORK_STATUS, FLAG_ERR);
        }                        
        
        if ( !ifBitSet(WORK_STATUS, FLAG_END) ) {
            // режим плавного нагрева, по 1С за 2мин
            if ( ifBitSet(WORK_STATUS, FLAG_START) ) {    // плавный разогрев до MAX_TEMP        
                temp = start_temp + (start_second/180);
                // если достиги необходимой температуры, вклбчим режим поддержки
                if (curent_temp >= set_temp)  
                    clearBit(WORK_STATUS, FLAG_START); 
            
                if (curent_temp > temp) {
                    if ((curent_temp-temp) >= 2) {
                        clearBit(TEN_CTRL, TEN_PIN1);
                        clearBit(TEN_CTRL, TEN_PIN2); 
                        //TEN_CTRL &=~((1<<PIN_TEN_1)|(1<<PIN_TEN_2)); 
                    }else {  
                        clearBit(TEN_CTRL, TEN_PIN2);         
                    }
                }else {
                    if ((temp-curent_temp) >= 2) {
                        setBit(TEN_CTRL, TEN_PIN1);
                        setBit(TEN_CTRL, TEN_PIN2); 
                        //TEN_CTRL |= (1<<PIN_TEN_1)|(1<<PIN_TEN_2);
                    }else {                   
                        setBit(TEN_CTRL, TEN_PIN1);
                    }
                }                      
            }else {     // разогрев произошел, поддерживаем температуру
                if (work_second >= MAX_SECOND) {
                    clearBit(TEN_CTRL, TEN_PIN1);
                    clearBit(TEN_CTRL, TEN_PIN2);             
                    // программа нагрева завершена
                    setBit(WORK_STATUS, FLAG_END);   
                }          
                
                if (curent_temp > temp) {
                    if ((curent_temp-temp) >= 2) {
                        clearBit(TEN_CTRL, TEN_PIN1);
                        clearBit(TEN_CTRL, TEN_PIN2); 
                        //TEN_CTRL &=~((1<<PIN_TEN_1)|(1<<PIN_TEN_2)); 
                    }else {  
                        clearBit(TEN_CTRL, TEN_PIN2);         
                    }
                }else {
                    if ((temp-curent_temp) >= 2) {
                        setBit(TEN_CTRL, TEN_PIN1);
                        setBit(TEN_CTRL, TEN_PIN2); 
                        //TEN_CTRL |= (1<<PIN_TEN_1)|(1<<PIN_TEN_2);
                    }else {                   
                        setBit(TEN_CTRL, TEN_PIN1);
                    }
                }     
            } 
        }     
        
        delay_ms(3000);      
    };         
}
