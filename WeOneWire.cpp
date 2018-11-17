#include "WeOneWire.h"
using namespace pxt;

MicroBitPin get_port(uint8_t index)
{
	if (index == 1){
		return uBit.io.P13;
	}
	else if (index == 2){
		return uBit.io.P14;
	}
	else if (index == 3){
		return uBit.io.P15;
	}
	else if (index == 4){
		return uBit.io.P16;
	}
}

uint8_t one_wire_reset(uint8_t pinNum)
{ 
    if(pinNum == 1){
        uBit.io.P13.setDigitalValue(0);
        uBit.io.P13.setDigitalValue(0);
        for (volatile uint16_t i = 0; i < 540; i++);
        int b = uBit.io.P13.getDigitalValue();
        for (volatile uint8_t i = 0; i < 50; i++);
        b = uBit.io.P13.getDigitalValue();
        for (volatile uint8_t i = 0; i < 100; i++);
        return b;
    }
    else if(pinNum == 2){
        uBit.io.P14.setDigitalValue(0);
        uBit.io.P14.setDigitalValue(0);
        for (volatile uint16_t i = 0; i < 540; i++);
        int b = uBit.io.P14.getDigitalValue();
        for (volatile uint8_t i = 0; i < 50; i++);
        b = uBit.io.P14.getDigitalValue();
        for (volatile uint8_t i = 0; i < 100; i++);
        return b;
    }
    else if(pinNum == 3){
        uBit.io.P15.setDigitalValue(0);
        uBit.io.P15.setDigitalValue(0);
        for (volatile uint16_t i = 0; i < 540; i++);
        int b = uBit.io.P15.getDigitalValue();
        for (volatile uint8_t i = 0; i < 50; i++);
        b = uBit.io.P15.getDigitalValue();
        for (volatile uint8_t i = 0; i < 100; i++);
        return b;
    }
    else if(pinNum == 4){
        uBit.io.P16.setDigitalValue(0);
        uBit.io.P16.setDigitalValue(0);
        for (volatile uint16_t i = 0; i < 540; i++);
        int b = uBit.io.P16.getDigitalValue();
        for (volatile uint8_t i = 0; i < 50; i++);
        b = uBit.io.P16.getDigitalValue();
        for (volatile uint8_t i = 0; i < 100; i++);
        return b;
    }
}

uint8_t one_wire_respond(uint8_t pinNum)
{
    if(pinNum == 1){
        uint32_t time = uBit.systemTime();
        while(1== uBit.io.P13.getDigitalValue()){
           if((uBit.systemTime()-time)>100){
               return 0;
           }
        }
        while(0== uBit.io.P13.getDigitalValue());
        for (volatile uint8_t i = 0; i < 30;i++);
        return 1;
    }
    else if(pinNum == 2){
        uint32_t time = uBit.systemTime();
        while(1== uBit.io.P14.getDigitalValue()){
           if((uBit.systemTime()-time)>100){
               return 0;
           }
        }
        while(0== uBit.io.P14.getDigitalValue());
        for (volatile uint8_t i = 0; i < 30;i++);
        return 1;
    }
    else if(pinNum == 3)
    {
        uint32_t time = uBit.systemTime();
        while(1== uBit.io.P15.getDigitalValue()){
           if((uBit.systemTime()-time)>100){
               return 0;
           }
        }
        while(0== uBit.io.P15.getDigitalValue());
        for (volatile uint8_t i = 0; i < 30;i++);
        return 1;
    }
    else if(pinNum == 4){
        uint32_t time = uBit.systemTime();
        while(1== uBit.io.P16.getDigitalValue()){
           if((uBit.systemTime()-time)>10){
               return 0;
           }
        }
        while(0== uBit.io.P16.getDigitalValue());
        for (volatile uint8_t i = 0; i < 30;i++);
        return 1;
    }
}

void one_wire_write_byte(uint8_t pinNum,uint8_t value)
{
    uint8_t i;
    __disable_irq();
    if(pinNum == 1){
        for(i=0; i<8; i++){
            uBit.io.P13.setDigitalValue(0);
            for (volatile uint8_t j = 0; j < 2;j++);
            if(value & 0x01){
                uBit.io.P13.setDigitalValue(1);
            }
            else{
                uBit.io.P13.setDigitalValue(0);
            }
            value = value>>1;
            for (volatile uint8_t j = 0; j < 32;j++);
            uBit.io.P13.setDigitalValue(1);
            for (volatile uint8_t j = 0; j < 4;j++);
        }
    }
    else if(pinNum == 2){
        for(i=0; i<8; i++){
            uBit.io.P14.setDigitalValue(0);
            for (volatile uint8_t j = 0; j < 2;j++);
            if(value & 0x01){
                uBit.io.P14.setDigitalValue(1);
            }
            else{
                uBit.io.P14.setDigitalValue(0);
            }
            value = value>>1;
            for (volatile uint8_t j = 0; j < 32;j++);
            uBit.io.P14.setDigitalValue(1);
            for (volatile uint8_t j = 0; j < 4;j++);
        }
    }
    else if(pinNum == 3){
        for(i=0; i<8; i++){
            uBit.io.P15.setDigitalValue(0);
            for (volatile uint8_t j = 0; j < 2;j++);
            if(value & 0x01){
                uBit.io.P15.setDigitalValue(1);
            }
            else{
                uBit.io.P15.setDigitalValue(0);
            }
            value = value>>1;
            for (volatile uint8_t j = 0; j < 32;j++);
            uBit.io.P15.setDigitalValue(1);
            for (volatile uint8_t j = 0; j < 4;j++);
        }
    }
    else if(pinNum == 4){
        for(i=0; i<8; i++){
            uBit.io.P16.setDigitalValue(0);
            for (volatile uint8_t j = 0; j < 2;j++);
            if(value & 0x01){
                uBit.io.P16.setDigitalValue(1);
            }
            else{
                uBit.io.P16.setDigitalValue(0);
            }
            value = value>>1;
            for (volatile uint8_t j = 0; j < 32;j++);
            uBit.io.P16.setDigitalValue(1);
            for (volatile uint8_t j = 0; j < 4;j++);
        }
    }
    __enable_irq();
}

uint8_t one_wire_read_byte(uint8_t pinNum)
{
	__disable_irq();
    if(pinNum == 1){   
        uint8_t i,j=0,k=0;
        uBit.io.P13.getDigitalValue();
        for(i=0; i<8; i++){
            while(1== uBit.io.P13.getDigitalValue());
            for (volatile uint8_t l = 0; l < 30;l++);
            j = uBit.io.P13.getDigitalValue();
            for (volatile uint8_t l = 0; l < 50;l++);
            k = (j<<7)|(k>>1);
        }
        return(k);
    }
    else if(pinNum == 2){
        uint8_t i,j=0,k=0;
        uBit.io.P14.getDigitalValue();
        for(i=0; i<8; i++){
            while(1== uBit.io.P14.getDigitalValue());
            for (volatile uint8_t l = 0; l < 30;l++);
            j = uBit.io.P14.getDigitalValue();
            for (volatile uint8_t l = 0; l < 50;l++);
            k = (j<<7)|(k>>1);
        }
        return(k);
    }
    else if(pinNum == 3){
        uint8_t i,j=0,k=0;
        uBit.io.P15.getDigitalValue();
        for(i=0; i<8; i++){
            while(1== uBit.io.P15.getDigitalValue());
            for (volatile uint8_t l = 0; l < 30;l++);
            j = uBit.io.P15.getDigitalValue();
            for (volatile uint8_t l = 0; l < 50;l++);
            k = (j<<7)|(k>>1);
        }
        return(k);
    }
    else if(pinNum == 4){
        uint8_t i,j=0,k=0;
        uBit.io.P16.getDigitalValue();
        for(i=0; i<8; i++){   
            while(1== uBit.io.P16.getDigitalValue());
            for (volatile uint8_t l = 0; l < 30;l++);
            j = uBit.io.P16.getDigitalValue();
            for (volatile uint8_t l = 0; l < 50;l++);
            k = (j<<7)|(k>>1);
        }
        return (k);
    }
    __enable_irq();
}