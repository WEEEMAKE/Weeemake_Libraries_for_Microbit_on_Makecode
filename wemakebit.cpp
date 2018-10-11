
#include "pxt.h"
#include <cstdint>
#include <math.h>

using namespace pxt;

// v0 backward compat support
#ifndef PXT_BUFFER_DATA
#define PXT_BUFFER_DATA(buffer) buffer->payload
#endif

namespace wemakebit {
	volatile unsigned int *setdir = (unsigned int *) (0x50000000UL + 0x518);
    volatile unsigned int *clrdir = (unsigned int *) (0x50000000UL + 0x51C);
    volatile unsigned int *inp = (unsigned int *) (0x50000000UL + 0x510);
    uint32_t mask13 = 1 << 23;
    uint32_t mask14 = 1 << 22;
    uint32_t mask15 = 1 << 21;
    uint32_t mask16 = 1 << 16;
    MicroBitPin P13 = uBit.io.P13;
    MicroBitPin P14 = uBit.io.P14;
    MicroBitPin P15 = uBit.io.P15;
    MicroBitPin P16 = uBit.io.P16;

    volatile uint8_t  UltrasonicRGB1_data[3]={0,0,0};
    volatile uint8_t  UltrasonicRGB2_data[3]={0,0,0};
    volatile uint8_t  We7SD_number1=0;
    volatile uint8_t  We7SD_number2=0;
    volatile uint8_t  We7SD_number3=0;
    volatile uint8_t  We7SD_number4=0;
    volatile uint16_t  DC130Last_speed=300;
    volatile uint8_t  JS_Sensor1;
    volatile uint8_t  JS_Sensor2;
    volatile uint8_t  JS_Sensor3;
    volatile uint8_t  Line_Sensor1;
    volatile uint8_t  Line_Sensor2;
    volatile uint8_t  Flame_Sensor1;
    volatile uint8_t  Flame_Sensor2;
    volatile uint8_t  Flame_Sensor3;

    volatile uint16_t UdistanceCm=0;
    volatile uint16_t Redvalue;
    volatile uint16_t Greenvalue;
    volatile uint16_t Bluevalue;
    volatile uint16_t Colorvalue;
    volatile uint8_t Display_Buffer[21]={0};
    volatile uint8_t panel_width=21;
    volatile uint8_t panel_height=7;
    //%
    uint8_t oneWireReset(uint8_t pinNum){
        if(pinNum == 1)
        {
            P13.setDigitalValue(0);
            P13.setDigitalValue(0);
            for (volatile uint16_t i = 0; i < 540; i++);
            int b = P13.getDigitalValue();
            for (volatile uint8_t i = 0; i < 50; i++);
            b = P13.getDigitalValue();
            //int b = P13.getDigitalValue();
            //*clrdir = mask;
            //uint8_t b = (*inp) & mask;
            for (volatile uint8_t i = 0; i < 100; i++);
            return b;
        }
        else if(pinNum == 2)
        {
            P14.setDigitalValue(0);
            P14.setDigitalValue(0);
            for (volatile uint16_t i = 0; i < 540; i++);
            int b = P14.getDigitalValue();
            for (volatile uint8_t i = 0; i < 50; i++);
            b = P14.getDigitalValue();
            //int b = P14.getDigitalValue();
            //*clrdir = mask;
            //uint8_t b = (*inp) & mask;
            for (volatile uint8_t i = 0; i < 100; i++);
            return b;
        }
        else if(pinNum == 3)
        {
            P15.setDigitalValue(0);
            P15.setDigitalValue(0);
            for (volatile uint16_t i = 0; i < 540; i++);
            int b = P15.getDigitalValue();
            for (volatile uint8_t i = 0; i < 50; i++);
            b = P15.getDigitalValue();
            //int b = P15.getDigitalValue();
            //*clrdir = mask;
            //uint8_t b = (*inp) & mask;
            for (volatile uint8_t i = 0; i < 100; i++);
            return b;
        }
        else if(pinNum == 4)
        {
            P16.setDigitalValue(0);
            P16.setDigitalValue(0);
            for (volatile uint16_t i = 0; i < 540; i++);
            int b = P16.getDigitalValue();
            for (volatile uint8_t i = 0; i < 50; i++);
            b = P16.getDigitalValue();
            //int b = P13.getDigitalValue();
            //*clrdir = mask;
            //uint8_t b = (*inp) & mask;
            for (volatile uint8_t i = 0; i < 100; i++);
            return b;
        }
    }

    //%
    uint8_t OneWireRespond(uint8_t pinNum)
    {
        if(pinNum == 1)
        {
            uint32_t time = uBit.systemTime();
            while(1== P13.getDigitalValue()){
               if((uBit.systemTime()-time)>100){
                   return 0;
               }
            }
            while(0== P13.getDigitalValue());
            for (volatile uint8_t i = 0; i < 30;i++);
            //P13.getDigitalValue();
            return 1;
        }
        else if(pinNum == 2)
        {
            uint32_t time = uBit.systemTime();
            while(1== P14.getDigitalValue()){
               if((uBit.systemTime()-time)>100){
                   return 0;
               }
            }
            while(0== P14.getDigitalValue());
            //P14.setDigitalValue(0);
            for (volatile uint8_t i = 0; i < 30;i++);
            return 1;
        }
        else if(pinNum == 3)
        {
            uint32_t time = uBit.systemTime();
            while(1== P15.getDigitalValue()){
               if((uBit.systemTime()-time)>100){
                   return 0;
               }
            }
            while(0== P15.getDigitalValue());
            //P15.setDigitalValue(0);
            for (volatile uint8_t i = 0; i < 30;i++);
            return 1;
        }
        else if(pinNum == 4)
        {
            uint32_t time = uBit.systemTime();
            while(1== P16.getDigitalValue()){
               if((uBit.systemTime()-time)>10){
                   return 0;
               }
            }
            while(0== P16.getDigitalValue());
            //P16.setDigitalValue(0);
            for (volatile uint8_t i = 0; i < 30;i++);
            return 1;
        }
    }

    //%
    void OneWireWriteByte(uint8_t pinNum,uint8_t value)
    {
        uint8_t i;
        if(pinNum == 1)
        {
            for(i=0; i<8; i++){
                P13.setDigitalValue(0);
                for (volatile uint8_t j = 0; j < 2;j++);
                if(value & 0x01)
                {
                    P13.setDigitalValue(1);
                }
                else
                {
                    P13.setDigitalValue(0);
                }
                value = value>>1;
                for (volatile uint8_t j = 0; j < 32;j++);
                P13.setDigitalValue(1);
                for (volatile uint8_t j = 0; j < 4;j++);
            }
        }
        else if(pinNum == 2)
        {
            for(i=0; i<8; i++){
                P14.setDigitalValue(0);
                for (volatile uint8_t j = 0; j < 2;j++);
                if(value & 0x01)
                {
                    P14.setDigitalValue(1);
                }
                else
                {
                    P14.setDigitalValue(0);
                }
                value = value>>1;
                for (volatile uint8_t j = 0; j < 32;j++);
                P14.setDigitalValue(1);
                for (volatile uint8_t j = 0; j < 4;j++);
            }
        }
        else if(pinNum == 3)
        {
            for(i=0; i<8; i++){
                P15.setDigitalValue(0);
                for (volatile uint8_t j = 0; j < 2;j++);
                if(value & 0x01)
                {
                    P15.setDigitalValue(1);
                }
                else
                {
                    P15.setDigitalValue(0);
                }
                value = value>>1;
                for (volatile uint8_t j = 0; j < 32;j++);
                P15.setDigitalValue(1);
                for (volatile uint8_t j = 0; j < 4;j++);
            }
        }
        else if(pinNum == 4)
        {
            for(i=0; i<8; i++){
                P16.setDigitalValue(0);
                for (volatile uint8_t j = 0; j < 2;j++);
                if(value & 0x01)
                {
                    P16.setDigitalValue(1);
                }
                else
                {
                    P16.setDigitalValue(0);
                }
                value = value>>1;
                for (volatile uint8_t j = 0; j < 32;j++);
                P16.setDigitalValue(1);
                for (volatile uint8_t j = 0; j < 4;j++);
            }
            //P16.getDigitalValue();
        }
    }

    //%
    uint8_t OneWireReadByte(uint8_t pinNum)
    {
        
        if(pinNum == 1)
        {   
            uint8_t i,j=0,k=0;
            P13.getDigitalValue();
            for(i=0; i<8; i++)
            {
                while(1== P13.getDigitalValue());
                for (volatile uint8_t l = 0; l < 30;l++);
                j = P13.getDigitalValue();
                for (volatile uint8_t l = 0; l < 50;l++);
                k = (j<<7)|(k>>1);
            }
            return(k);
        }
        else if(pinNum == 2)
        {
            uint8_t i,j=0,k=0;
            P14.getDigitalValue();
            for(i=0; i<8; i++)
            {
                while(1== P14.getDigitalValue());
                for (volatile uint8_t l = 0; l < 30;l++);
                j = P14.getDigitalValue();
                for (volatile uint8_t l = 0; l < 50;l++);
                k = (j<<7)|(k>>1);
            }
            return(k);
        }
        else if(pinNum == 3)
        {
            uint8_t i,j=0,k=0;
            P15.getDigitalValue();
            for(i=0; i<8; i++)
            {
                while(1== P15.getDigitalValue());
                for (volatile uint8_t l = 0; l < 30;l++);
                j = P15.getDigitalValue();
                for (volatile uint8_t l = 0; l < 50;l++);
                k = (j<<7)|(k>>1);
            }
            return(k);
        }
        else if(pinNum == 4)
        {
            uint8_t i,j=0,k=0;
            P16.getDigitalValue();
            for(i=0; i<8; i++)
            {   
                while(1== P16.getDigitalValue());
                for (volatile uint8_t l = 0; l < 30;l++);
                j = P16.getDigitalValue();
                for (volatile uint8_t l = 0; l < 50;l++);
                k = (j<<7)|(k>>1);
            }
            return (k);
        }
    }

    /*
    **We Single Touch Sensor V1.0 module.
     */
    //%
    void touchSetMode(uint8_t pinNum, uint8_t mode)
    {
        if(oneWireReset(pinNum) != 0){
            return;
        }
        OneWireWriteByte(pinNum,0x03);
        if(oneWireReset(pinNum) != 0){
            return;
        }
        OneWireWriteByte(pinNum,mode);
    }
    //%
    uint8_t getTouched(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0){
            return 0;
        }
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r =OneWireReadByte(pinNum);
        return r;
    }
    /*
    **We Water Atomizer module V1.0.
     */
    //%
    void waterAtomizerSet(uint8_t pinNum, uint8_t isOn)
    {
        if(oneWireReset(pinNum) != 0){
            return;
        }
        OneWireWriteByte(pinNum,0x03-isOn);
        uBit.sleep(10);
    }
    /*
    **We 4 LED Button module V1.0.
     */
    //%
    void LED4ButtonSetLed(uint8_t pinNum, uint8_t index, uint8_t isOn)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,isOn ? 3 : 4);
        if(oneWireReset(pinNum) != 0)return;
        uint8_t value = index ? (1 << (index - 1)) : 0xF;
        OneWireWriteByte(pinNum,value);
    }
    //%
    uint8_t LED4ButtonReadKey(uint8_t pinNum, uint8_t key)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r=OneWireReadByte(pinNum);
        switch(r)
        {
            case 1: r=1;break;
            case 2: r=2;break;
            case 4: r=3;break;
            case 8: r=4;break;
            default: break;
        }
        if(r == key){
            return 1;
        }
        return 0;
    }
    /*
    **We Joystick module V1.0.
    */
    //%
    uint8_t JoystickReadValue(uint8_t pinNum, uint8_t axis)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        JS_Sensor1=OneWireReadByte(pinNum);
        JS_Sensor2=OneWireReadByte(pinNum);
        JS_Sensor3=OneWireReadByte(pinNum);
        switch(axis)
        {
            case 0: return JS_Sensor1;
            case 1: return JS_Sensor2;
        }
        return JS_Sensor3;
    }
    /*
    **We Line Follower Sensor V1.0.
    */
    //%  
    uint16_t lineFollowerRead(uint8_t pinNum, uint8_t index)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        Line_Sensor1=OneWireReadByte(pinNum);
        Line_Sensor2=OneWireReadByte(pinNum);
        switch(index)
        {
            case 1: return (uint16_t)(1023 * (1 - Line_Sensor1 / 255.0));
            case 2: return (uint16_t)(1023 * (1 - Line_Sensor2 / 255.0));
        }
    }
    /*
    **We Tilt Switch Sensor V1.0.
    */
    //%  
    uint16_t tiltSwitchRead(uint8_t pinNum, uint8_t index)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r =OneWireReadByte(pinNum);
        switch(index)
        {
            case 1: return (r==2);
            case 2: return (r==1);
        }
        return r != 0;
    }
    /*
    **We Flame Sensor V1.0.
    */
    //%  
    uint16_t flameSensorRead(uint8_t pinNum, uint8_t index)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        Flame_Sensor1=OneWireReadByte(pinNum);
        Flame_Sensor2=OneWireReadByte(pinNum);
        Flame_Sensor3=OneWireReadByte(pinNum);        
        switch(index)
        {
            case 1: return Flame_Sensor1;
            case 3: return Flame_Sensor3;
        }
        return Flame_Sensor2;
    }
    /*
    **We Gas Sensor MQ2 module V1.0.
    */
    //%
    uint8_t gasSensorReadValue(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r=OneWireReadByte(pinNum);
        return r;
    }
    /*
    **We Sliding Potentiometer module V1.0.
    */
    //%
    uint8_t slidingPotentiometerReadValue(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r=OneWireReadByte(pinNum);
        return r;
    }
    /*
    **We Potentiometer module V1.0.
    */
    //%
    uint8_t PotentiometerReadValue(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r=OneWireReadByte(pinNum);
        return r;
    }

    /*
    **We PIR Motion Sensor V1.0.
    */
    //%
    uint8_t getPIRMotion(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        uint8_t r=OneWireReadByte(pinNum);
        return r;
    }
    /*
    **We Color Sensor module V1.0.
     */
    //%
    void colorSensorWhiteBalance(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x04);
        uBit.sleep(10);
    }

    //%
    void colorSensorSetLight(uint8_t pinNum, uint8_t isOn)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x03);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,isOn);        
    }

    //%
    uint16_t colorSensorReadValue(uint8_t pinNum, uint8_t type)
    {
        uint16_t ColorData[8] = {0};
        if(oneWireReset(pinNum) != 0)return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        for(uint8_t i=0;i<8;i++)
        {
           ColorData[i]=OneWireReadByte(pinNum);
        }
        Redvalue   = (uint16_t)(ColorData[1] << 8 | ColorData[0]);
        Greenvalue = (uint16_t)(ColorData[3] << 8 | ColorData[2]);
        Bluevalue  = (uint16_t)(ColorData[5] << 8 | ColorData[4]);
        Colorvalue = (uint16_t)(ColorData[7] << 8 | ColorData[6]);
        uBit.sleep(10);
        switch(type){
            case 1: return Redvalue;
            case 2: return Greenvalue;
            case 3: return Bluevalue;
        }
        //return Redvalue;
        return 0;
    }
    /*
    **We 4-Digital LED module V1.0.
     */
    //%
    void segmentDisplaySendNumber(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x02);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,We7SD_number1); 
        OneWireWriteByte(pinNum,We7SD_number2);
        OneWireWriteByte(pinNum,We7SD_number3);
        OneWireWriteByte(pinNum,We7SD_number4);
    }
    //%
    void segmentDisplayShowChar(uint8_t pinNum, uint8_t Addr, uint8_t Data)
    {
        switch(Addr)
        {
           case 1: We7SD_number1 = Data;break;
           case 2: We7SD_number2 = Data;break;
           case 3: We7SD_number3 = Data;break;
           case 4: We7SD_number4 = Data;break;
        }
        segmentDisplaySendNumber(pinNum);
    }    
    //%
    void segmentDisplayShowNumber(uint8_t pinNum, int16_t value)
    {
        uint8_t tempBuf[10]={0};
        int buf[4]={0};
        uint8_t isNeg=0;
        uint8_t int_num = 0;
        uint8_t comma_flag=0;
        double number = value;
        if (number>=9999)
        {
            We7SD_number1=0x09;
            We7SD_number2=0x09;
            We7SD_number3=0x09;
            We7SD_number4=0x09;
        }
        else if(number<=-999)
        {
            We7SD_number1=0x10;    //"-"
            We7SD_number2=0x09;
            We7SD_number3=0x09;
            We7SD_number4=0x09;
        }
        else if(number==0)
        {
            We7SD_number1=0x40;   //不显示
            We7SD_number2=0x40;
            We7SD_number3=0x40;
            We7SD_number4=0x00;
        }
        else
        {
            if(number<0)
            {
              number = -number;
              isNeg = 1;
            }
            tempBuf[0]=number/1000;
            tempBuf[1]=(int)number%1000/100;
            tempBuf[2]=(int)number%100/10;
            tempBuf[3]=(int)number%10;
            tempBuf[4]=(int)(number*10)%10;
            tempBuf[5]=(int)(number*100)%10;
            tempBuf[6]=(int)(number*1000)%10;
            while(tempBuf[int_num]==0)
            {
                int_num++;
            }
            if(number>=1)
            {       
                buf[0]=tempBuf[int_num];
                buf[1]=tempBuf[int_num+1];
                buf[2]=tempBuf[int_num+2];
                buf[3]=tempBuf[int_num+3];
                comma_flag=4-int_num;
            }
            else
            {
                buf[0]=tempBuf[3];
                buf[1]=tempBuf[4];
                buf[2]=tempBuf[5];
                buf[3]=tempBuf[6];
                comma_flag=1;
            }
            if(isNeg==1)
            {
                We7SD_number1=0x10;    //"-"
                We7SD_number2=buf[0];
                We7SD_number3=buf[1];
                We7SD_number4=buf[2];
                if (comma_flag==2)
                We7SD_number3 =We7SD_number3|0x20; //"."
                else if (comma_flag==1)
                We7SD_number2 =We7SD_number2|0x20; //"."
            }
            else
            {
               We7SD_number1=buf[0];    
               We7SD_number2=buf[1];
               We7SD_number3=buf[2];
               We7SD_number4=buf[3];
               if (comma_flag==3)
                  We7SD_number3=We7SD_number3|0x20; //"."
               else if (comma_flag==2)
                  We7SD_number2=We7SD_number2|0x20; //"."
               else if (comma_flag==1)
                  We7SD_number1=We7SD_number1|0x20; //"."
            }
           
        }

     
        if((We7SD_number1&0x20)==0x20)     //去除小数点后多余0位
        {
           if ((We7SD_number2==0)&&(We7SD_number3==0)&&(We7SD_number4==0))
           { 
              
              We7SD_number4=We7SD_number1&0x0f;
              We7SD_number1=0x40;
              We7SD_number2=0x40;
              We7SD_number3=0x40;
           }
           else if((We7SD_number3==0)&&(We7SD_number4==0))
           {
              We7SD_number4=We7SD_number2;
              We7SD_number3=We7SD_number1;
              We7SD_number1=0x40;
              We7SD_number2=0x40;
           //   _number3=_number3|0x20; //"."
           }
           else if (We7SD_number4==0)
           {
              We7SD_number4=We7SD_number3;
              We7SD_number3=We7SD_number2;
              We7SD_number2=We7SD_number1;
              We7SD_number1=0x40;
          //  _number2=_number2|0x20; //"."
           }
        }
        else if((We7SD_number2&0x20)==0x20)
        {
           if ((We7SD_number3==0)&&(We7SD_number4==0))
           {
              We7SD_number4=We7SD_number2&0x0f;
              We7SD_number3=We7SD_number1;
              We7SD_number1=0x40;
              We7SD_number2=0x40;
           }
           else if (We7SD_number4==0)
           {
              We7SD_number4=We7SD_number3;
              We7SD_number3=We7SD_number2;
              We7SD_number2=We7SD_number1;
              We7SD_number1=0x40;
          
           }
        }
        else if((We7SD_number3&0x20)==0x20)
        {
           if (We7SD_number4==0)
           {
              We7SD_number4=We7SD_number3&0x0f;
              We7SD_number3=We7SD_number2;
              We7SD_number2=We7SD_number1;
              We7SD_number1=0x40;
           }
        }
        segmentDisplaySendNumber(pinNum);
    }
    /*
    **We Single LED module V1.0.
     */
    //%
    void singleLEDSet(uint8_t pinNum, uint8_t isOn)
    {
        if(pinNum == 1)
        {
            P13.setDigitalValue(isOn);
        }
        else if(pinNum == 2)
        {
            P14.setDigitalValue(isOn);
        }
        else if(pinNum == 3)
        {
            P15.setDigitalValue(isOn);
        }
        else if(pinNum == 4)
        {
            P16.setDigitalValue(isOn);
        }
    }
    /*
    **We Relay module V1.0.
     */
    //%
    void RelaySet(uint8_t pinNum, uint8_t isOn)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x02);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,isOn);
    }
    /*
    **We 130 DC Motor module V1.0.
     */
    //%
    void DC130MotorRunSpeed(uint8_t pinNum, int16_t speed)
    {
        speed = speed > 255 ? 255 : speed;
        speed = speed < -255 ? -255 : speed;
        
        if(speed >= 0)
        {   
            if(oneWireReset(pinNum) != 0)return;
            OneWireWriteByte(pinNum,0x02);
            if(oneWireReset(pinNum) != 0)return;
            OneWireWriteByte(pinNum,(uint8_t)(speed/2.55));
            uBit.sleep(1);
        }
        else
        {
            if(oneWireReset(pinNum) != 0)return;
            OneWireWriteByte(pinNum,0x02);
            if(oneWireReset(pinNum) != 0)return;
            OneWireWriteByte(pinNum,(uint8_t)(100-speed/2.55));
            uBit.sleep(1);
        }
    }
    /*
    **We Ultrasonic Sensor module V1.0.
     */
    //%
    void ultrasonicSetLed(uint8_t pinNum, uint8_t index, uint8_t isOn)
    {
        uint8_t _Led_data=0;
        index = index&3;
        if(0 == index)
            return;
        if(isOn){
            _Led_data |= index;
        }else{
            _Led_data &= ~index;
        }
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x04);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,_Led_data);
    }
    
    //%
    void ultrasonicRGBShow(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x03);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,UltrasonicRGB1_data[0]);
        OneWireWriteByte(pinNum,UltrasonicRGB1_data[1]);
        OneWireWriteByte(pinNum,UltrasonicRGB1_data[2]);
        OneWireWriteByte(pinNum,UltrasonicRGB2_data[0]);
        OneWireWriteByte(pinNum,UltrasonicRGB2_data[1]);
        OneWireWriteByte(pinNum,UltrasonicRGB2_data[2]);
    }

    //%
    void ultrasonicSetColor(uint8_t pinNum, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
    {
        if(index & 1){
            UltrasonicRGB1_data[0]=red;
            UltrasonicRGB1_data[1]=green;
            UltrasonicRGB1_data[2]=blue;
        }
        if(index & 2){
            UltrasonicRGB2_data[0]=red;
            UltrasonicRGB2_data[1]=green;
            UltrasonicRGB2_data[2]=blue;
        }
        if(index & 3){
            ultrasonicRGBShow(pinNum);
        }
    }
    
    //%
    uint16_t ultrasonicDistanceCm(uint8_t pinNum)
    {
        uint16_t Sensor_data1 = 0,Sensor_data2 = 0;
        if(oneWireReset(pinNum) != 0)
            return 500;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        Sensor_data1=OneWireReadByte(pinNum);
        Sensor_data2=OneWireReadByte(pinNum);
        UdistanceCm=Sensor_data2*256 + Sensor_data1;
        //UdistanceCm=(uint16_t)(Sensor_data2<<8 | Sensor_data1); 
        UdistanceCm=(uint16_t)UdistanceCm/17.57;
        
        if (UdistanceCm>500)
        {
            return 500;
        }
        else
        {
            return UdistanceCm;
        }
        
    }
    
    /*
    **We MP3 Player module V1.0.
     */
    //%
    void MP3AppointMusic(uint8_t pinNum, uint16_t num)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x02);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,(uint8_t)(num>>8));
        OneWireWriteByte(pinNum,(uint8_t)(num));
        uBit.sleep(60);
    }

    //%
    void MP3AppointVolume(uint8_t pinNum, uint8_t num)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x03);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,num);
        uBit.sleep(60);
    }

    //%
    void MP3NextMusic(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x04);
        uBit.sleep(60);
    }

    //%
    void MP3Pause(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x05);
        uBit.sleep(60);
    }

    //%
    void MP3Play(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x06);
        uBit.sleep(60);
    }

    //%
    void MP3AppointDevice(uint8_t pinNum, uint8_t devtype)
    {
        if((devtype!=0x02)&&(devtype!=0x04)) return;
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x07);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,devtype);
        uBit.sleep(210);
    }

    //%
    uint8_t MP3isOver(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)
            return 2;
        OneWireWriteByte(pinNum,0x08);
        OneWireRespond(pinNum);
        uint8_t r=OneWireReadByte(pinNum);
        return r;
    }

    //%
    void MP3PrevMusic(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x09);
        uBit.sleep(60);
    }
    /*
    **We Humiture Sensor Sensor V1.0.
    */
    //%  
    uint16_t humitureSensorRead(uint8_t pinNum, uint8_t index)
    {
        uint8_t _temperature,_humidity;
        if(oneWireReset(pinNum) != 0)
            return 0;
        OneWireWriteByte(pinNum,0x02);
        OneWireRespond(pinNum);
        _humidity=OneWireReadByte(pinNum);
        _temperature=OneWireReadByte(pinNum);
        uBit.sleep(25);
        switch(index)
        {
            case 0: return _temperature;
            case 1: return _humidity;
        }
    }
    /*
    **We LED Matrix7*21 module V1.0.
    */
    //%
    void ledPanelSetBrightness(uint8_t pinNum, uint8_t Bright)
    {
        if(Bright > 7)
        {
            Bright = 0x07;
        }
    
        if(Bright >=0)
        {
            Bright = 0x88 | (uint8_t)Bright; 
        }
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x06);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,Bright);    
        uBit.sleep(3);      
    }

    //%
    void ledPanelClearScreen(uint8_t pinNum)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x05);  
        uBit.sleep(5);      
    }

    //%
    void ledPanelShowLine(uint8_t pinNum, uint8_t x, uint8_t buffer)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x03);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,x);
        OneWireWriteByte(pinNum,buffer);
    }

    //%
    void ledPanelTurnOnDot(uint8_t pinNum,uint8_t x,uint8_t y)
    {
        if(x>(panel_width-1)||y>(panel_height-1)) return;
        Display_Buffer[x]=Display_Buffer[x]|(0x01<<(y));   
        ledPanelShowLine(pinNum,x,Display_Buffer[x]);
    }

    //%
    void ledPanelTurnOffDot(uint8_t pinNum,uint8_t x,uint8_t y)
    {
        if(x>(panel_width-1)||y>(panel_height-1)) return;
        Display_Buffer[x]=Display_Buffer[x]&(~(0x01<<(y)));
        ledPanelShowLine(pinNum,x,Display_Buffer[x]);
    }

    //%
    void ledPanelWriteChar(uint8_t pinNum,int8_t x,int8_t y,uint8_t buffer)
    {
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,0x07);
        if(oneWireReset(pinNum) != 0)return;
        OneWireWriteByte(pinNum,x);
        OneWireWriteByte(pinNum,y);
        OneWireWriteByte(pinNum,buffer);
        uBit.sleep(3);
    }

    //%
    void ledPanelShowClock(uint8_t pinNum, uint8_t hour, uint8_t minute)
    {
        ledPanelWriteChar(pinNum, -1, 0, hour / 10 + 15);
        ledPanelWriteChar(pinNum,  4, 0, hour % 10 + 15); 
        ledPanelWriteChar(pinNum, 11, 0, minute / 10 + 15);   
        ledPanelWriteChar(pinNum, 16, 0, minute % 10 + 15);
        ledPanelShowLine(pinNum, 10, 0x14);
    }

    //%
    void ledPanelShowNum(uint8_t pinNum, int32_t value)
    {
        uint8_t tempBuf[10] = {0};
        int buf[4] = {0};
        uint8_t isNeg = 0;
        uint8_t int_num = 0;
        uint8_t bit_num = 0;
        uint8_t temp_num = 0;
        uint8_t comma_flag = 0;
        double number = value;
        if(number>=9999)
        {
            ledPanelWriteChar(pinNum,-1,0,9+15);
            ledPanelWriteChar(pinNum,4,0,9+15);
            ledPanelWriteChar(pinNum,9,0,9+15);
            ledPanelWriteChar(pinNum,14,0,9+15);
            ledPanelShowLine(pinNum,20,0x00);
        }
        else if(number<= -999)
        {
            ledPanelWriteChar(pinNum,4,0,9+15);
            ledPanelWriteChar(pinNum,10,0,9+15);
            ledPanelWriteChar(pinNum,15,0,9+15);
            ledPanelShowLine(pinNum,0,0x08);
            ledPanelShowLine(pinNum,1,0x08);
            ledPanelShowLine(pinNum,2,0x08);
            ledPanelShowLine(pinNum,3,0x08);
        }
        else if (number==0)
        {
            ledPanelWriteChar(pinNum,0,0,3);
            ledPanelWriteChar(pinNum,5,0,3);
            ledPanelWriteChar(pinNum,10,0,3);
            ledPanelWriteChar(pinNum,15,0,15);
        }
        else
        {
            if (number<0)
            {
                number = -number;
                isNeg = 1;
            }
            tempBuf[0]=number/1000;
            tempBuf[1]=(int)number%1000/100;
            tempBuf[2]=(int)number%100/10;
            tempBuf[3]=(int)number%10;
            tempBuf[4]=(int)(number*10)%10;
            tempBuf[5]=(int)(number*100)%10;
            tempBuf[6]=(int)(number*1000)%10;
            while(tempBuf[int_num]==0)
            {
                int_num++;
            }
            if(number>=1)
            {  
                buf[0]=tempBuf[int_num];
                buf[1]=tempBuf[int_num+1];
                buf[2]=tempBuf[int_num+2];
                buf[3]=tempBuf[int_num+3];
                comma_flag=4-int_num;
            }
            else
            {
                buf[0]=tempBuf[3];
                buf[1]=tempBuf[4];
                buf[2]=tempBuf[5];
                buf[3]=tempBuf[6];
                comma_flag=1;
            }

            if(isNeg==1)         //
            {
                if (number >= 100)
                {
                    temp_num = 3;
                    ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                }
                else if (number >= 10)
                {
                    if (buf[2]!=0)
                    {
                        temp_num = 3;
                        ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                    }
                    else
                    {
                        temp_num =2;
                        ledPanelWriteChar(pinNum,4,0,12);     //'-'
                    }
                }
                else if (number >=1)
                {
                    if (buf[2]!=0)
                    {
                        temp_num =3;
                        ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                    }
                    else if (buf[2]==0&&buf[1]!=0)
                    {
                        temp_num = 2;
                        ledPanelWriteChar(pinNum,4,0,12);     //'-'
                    }
                    else
                    {
                        temp_num = 1;
                        ledPanelWriteChar(pinNum,9,0,12);     //'-'
                    }
                }
                else
                {
                    if (buf[2]!=0)
                    {
                        temp_num =3;
                        ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                    }
                    else if (buf[2]==0&&buf[1]!=0)
                    {
                        temp_num = 2;
                        ledPanelWriteChar(pinNum,4,0,12);     //'-'
                    }
                    else
                    {
                        temp_num = 1;
                        ledPanelWriteChar(pinNum,9,0,12);     //'-'
                    }
                }
               
                for(int i=0;i<temp_num;i++)
                {
                    if (temp_num == 3)
                    {
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,6+i*5,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,4+i*5,0,buf[i]+15);
                        }
                        if (number>=100)
                        {
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else
                        {
                            ledPanelShowLine(pinNum,5+5*comma_flag,0x40);
                        }
                    }
                    else if (temp_num == 2)
                    {
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,6+i*5+5,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,4+i*5+5,0,buf[i]+15);
                        }
                        ledPanelWriteChar(pinNum,-1,0,3);
                        if (number>=10)
                        {
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else
                        {
                            ledPanelShowLine(pinNum,5+5*comma_flag+5,0x40);
                        }
                    }
                    else
                    {
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,6+i*5+10,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,4+i*5+10,0,buf[i]+15);
                        }
                        ledPanelWriteChar(pinNum,4,0,3);
                    } 
                }
            }   
  
            else
            {
                if (number>=1000)
                {
                    temp_num = 4;
                }
                else if(number>=100)
                {
                    if(buf[3]!=0)
                    {
                        temp_num = 4;
                    }
                    else
                    {
                        temp_num =3;
                    }
                }
                else if(number>=10)
                {
                    if(buf[3]!=0)
                    {
                        temp_num = 4;
                    }
                    else if(buf[3]==0 && buf[2]!=0)
                    {
                        temp_num = 3;
                    }
                    else
                    {
                        temp_num = 2;
                    } 
                }
                else if(number>=1)
                {
                    if(buf[3]!=0)
                    {
                        temp_num = 4;
                    }
                    else if(buf[3]==0 && buf[2]!=0)
                    {
                        temp_num = 3;
                    }
                    else if(buf[3]==0 && buf[2]==0 && buf[1]!=0)
                    {
                        temp_num = 2;
                    }
                    else
                    {
                        temp_num = 1;
                    }
                }
                else
                {
                    if(buf[3]!=0)
                    {
                        temp_num = 4;
                    }
                    else if(buf[3]==0 && buf[2]!=0)
                    {
                        temp_num = 3;
                    }
                    else if(buf[3]==0 && buf[2]==0 && buf[1]!=0)
                    {
                        temp_num = 2;
                    }
                    else
                    {
                        temp_num = 1;
                    }
                }
     
                for(int i=0;i<temp_num;i++)
                {
                    if(temp_num == 4)
                    {
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,1+i*5,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,-1+i*5,0,buf[i]+15);
                        }
                        if(number>=1000)
                        {
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else
                        {
                            ledPanelShowLine(pinNum,5*comma_flag,0x40);
                        } 
                    }
                    else if(temp_num == 3)
                    {
                        ledPanelWriteChar(pinNum,-1,0,3);
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,1+i*5+5,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,-1+i*5+5,0,buf[i]+15);
                        }
                        if (number>=100)
                        {
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else
                        {
                            ledPanelShowLine(pinNum,5*comma_flag+5,0x40);
                        }
                    }
                    else if(temp_num == 2)
                    {
                        ledPanelWriteChar(pinNum,-1,0,3);
                        ledPanelWriteChar(pinNum,4,0,3);
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,1+i*5+10,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,-1+i*5+10,0,buf[i]+15);
                        }
                        if (number>=10)
                        {
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else
                        {
                            ledPanelShowLine(pinNum,5*comma_flag+10,0x40);
                        }
                    }
                    else 
                    {
                        ledPanelWriteChar(pinNum,-1,0,3);
                        ledPanelWriteChar(pinNum,4,0,3);
                        ledPanelWriteChar(pinNum,9,0,3);
                        if(comma_flag-1<i)
                        {
                            ledPanelWriteChar(pinNum,1+i*5+15,0,buf[i]+15);
                        }
                        else
                        {
                            ledPanelWriteChar(pinNum,-1+i*5+15,0,buf[i]+15);
                        }
                        if (number>=1)
                        {
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                    }   
                }
            }
        }  
    }
}
