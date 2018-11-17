
#include "WeOneWire.h"

// v0 backward compat support
#ifndef PXT_BUFFER_DATA
#define PXT_BUFFER_DATA(buffer) buffer->payload
#endif

namespace elfshield {
	// volatile unsigned int *setdir = (unsigned int *) (0x50000000UL + 0x518);
 //    volatile unsigned int *clrdir = (unsigned int *) (0x50000000UL + 0x51C);
 //    volatile unsigned int *inp = (unsigned int *) (0x50000000UL + 0x510);
 //    uint32_t mask13 = 1 << 23;
 //    uint32_t mask14 = 1 << 22;
 //    uint32_t mask15 = 1 << 21;
 //    uint32_t mask16 = 1 << 16;

    volatile uint8_t  UltrasonicRGB1_data[3]={0,0,0};
    volatile uint8_t  UltrasonicRGB2_data[3]={0,0,0};
    volatile uint8_t  We7SD_number1=0;
    volatile uint8_t  We7SD_number2=0;
    volatile uint8_t  We7SD_number3=0;
    volatile uint8_t  We7SD_number4=0;
    volatile float    last_number=0;
    volatile uint16_t  DC130Last_speed=300;
    // volatile uint8_t  JS_Sensor1;
    // volatile uint8_t  JS_Sensor2;
    // volatile uint8_t  JS_Sensor3;
    volatile uint8_t  Line_Sensor1;
    volatile uint8_t  Line_Sensor2;
    // volatile uint8_t  Flame_Sensor1;
    // volatile uint8_t  Flame_Sensor2;
    // volatile uint8_t  Flame_Sensor3;

    // volatile uint8_t Sensor_data1 = 0;
    // volatile uint8_t Sensor_data2 = 0;
    volatile uint16_t Redvalue;
    volatile uint16_t Greenvalue;
    volatile uint16_t Bluevalue;
    volatile uint16_t Colorvalue;
    volatile uint8_t Display_Buffer[21]={0};
    volatile uint8_t panel_width=21;
    volatile uint8_t panel_height=7;
    
    /*
    **We Single Touch Sensor V1.0 module.
     */
    //%
    void touchSetMode(uint8_t pinNum, uint8_t mode)
    {
        if(one_wire_reset(pinNum) != 0){
            return;
        }
        one_wire_write_byte(pinNum,0x03);
        if(one_wire_reset(pinNum) != 0){
            return;
        }
        one_wire_write_byte(pinNum,mode);
    }
    //%
    bool getTouched(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0){
            return 0;
        }
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r =one_wire_read_byte(pinNum);
        return r;
    }

    /*
    **We Water Atomizer module V1.0.
     */
    //%
    void waterAtomizerSet(uint8_t pinNum, uint8_t isOn)
    {
        if(one_wire_reset(pinNum) != 0){
            return;
        }
        one_wire_write_byte(pinNum,0x03-isOn);
        uBit.sleep(10);
    }
    /*
    **We 4 LED Button module V1.0.
     */
    //%
    void LED4ButtonSetLed(uint8_t pinNum, uint8_t index, uint8_t isOn)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,isOn ? 3 : 4);
        if(one_wire_reset(pinNum) != 0)return;
        uint8_t value = index ? (1 << (index - 1)) : 0xF;
        one_wire_write_byte(pinNum,value);
    }

    //%
    bool LED4ButtonReadKey(uint8_t pinNum, uint8_t key)
    {
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r=one_wire_read_byte(pinNum);
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
        volatile uint8_t  JS_Sensor1;
        volatile uint8_t  JS_Sensor2;
        volatile uint8_t  JS_Sensor3;
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        JS_Sensor1=one_wire_read_byte(pinNum);
        JS_Sensor2=one_wire_read_byte(pinNum);
        JS_Sensor3=one_wire_read_byte(pinNum);
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
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        Line_Sensor1=one_wire_read_byte(pinNum);
        Line_Sensor2=one_wire_read_byte(pinNum);
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
    bool tiltSwitchRead(uint8_t pinNum, uint8_t index)
    {
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r =one_wire_read_byte(pinNum);
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
    uint8_t flameSensorRead(uint8_t pinNum, uint8_t index)
    {
        volatile uint8_t  Flame_Sensor1;
        volatile uint8_t  Flame_Sensor2;
        volatile uint8_t  Flame_Sensor3;
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        Flame_Sensor1=one_wire_read_byte(pinNum);
        Flame_Sensor2=one_wire_read_byte(pinNum);
        Flame_Sensor3=one_wire_read_byte(pinNum);        
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
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r=one_wire_read_byte(pinNum);
        return r;
    }

    /*
    **We Sliding Potentiometer module V1.0.
    */
    //%
    uint8_t slidingPotentiometerReadValue(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r=one_wire_read_byte(pinNum);
        return r;
    }

    /*
    **We Potentiometer module V1.0.
    */
    //%
    uint8_t PotentiometerReadValue(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r=one_wire_read_byte(pinNum);
        return r;
    }

    /*
    **We PIR Motion Sensor V1.0.
    */
    //%
    bool getPIRMotion(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        uint8_t r=one_wire_read_byte(pinNum);
        return r;
    }

    /*
    **We Color Sensor module V1.0.
     */
    //%
    void colorSensorWhiteBalance(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x04);
        uBit.sleep(10);
    }

    //%
    void colorSensorSetLight(uint8_t pinNum, uint8_t isOn)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x03);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,isOn);        
    }

    //%
    uint16_t colorSensorReadValue(uint8_t pinNum, uint8_t type)
    {
        uint16_t ColorData[8] = {0};
        if(one_wire_reset(pinNum) != 0)return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        for(uint8_t i=0;i<8;i++)
        {
           ColorData[i]=one_wire_read_byte(pinNum);
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
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x02);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,We7SD_number1); 
        one_wire_write_byte(pinNum,We7SD_number2);
        one_wire_write_byte(pinNum,We7SD_number3);
        one_wire_write_byte(pinNum,We7SD_number4);
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
    void segmentDisplayShowNumber(uint8_t pinNum, float value)
    {
        if(last_number==value)
            return;
        else
            last_number=value;

        uint8_t tempBuf[10]={0};
        int buf[4]={0};
        uint8_t isNeg=0;
        uint8_t int_num = 0;
        uint8_t comma_flag=0;
        float number = value;
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
        MicroBitPin port = get_port(pinNum);
        port.setDigitalValue(isOn);
        // if(pinNum == 1)
        // {
        //     port.setDigitalValue(isOn);
        // }
        // else if(pinNum == 2)
        // {
        //     uBit.io.P14.setDigitalValue(isOn);
        // }
        // else if(pinNum == 3)
        // {
        //     uBit.io.P15.setDigitalValue(isOn);
        // }
        // else if(pinNum == 4)
        // {
        //     uBit.io.P16.setDigitalValue(isOn);
        // }
    }

    /*
    **We Relay module V1.0.
     */
    //%
    void RelaySet(uint8_t pinNum, uint8_t isOn)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x02);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,isOn);
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
            if(one_wire_reset(pinNum) != 0)return;
            one_wire_write_byte(pinNum,0x02);
            if(one_wire_reset(pinNum) != 0)return;
            one_wire_write_byte(pinNum,(uint8_t)(speed/2.55));
            uBit.sleep(1);
        }
        else
        {
            if(one_wire_reset(pinNum) != 0)return;
            one_wire_write_byte(pinNum,0x02);
            if(one_wire_reset(pinNum) != 0)return;
            one_wire_write_byte(pinNum,(uint8_t)(100-speed/2.55));
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
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x04);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,_Led_data);
    }
    
    //%
    void ultrasonicRGBShow(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x03);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,UltrasonicRGB1_data[0]);
        one_wire_write_byte(pinNum,UltrasonicRGB1_data[1]);
        one_wire_write_byte(pinNum,UltrasonicRGB1_data[2]);
        one_wire_write_byte(pinNum,UltrasonicRGB2_data[0]);
        one_wire_write_byte(pinNum,UltrasonicRGB2_data[1]);
        one_wire_write_byte(pinNum,UltrasonicRGB2_data[2]);
    }

    //%
    uint32_t ultrasonicRGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        return ((red & 0xFF) << 16) | ((green & 0xFF) << 8) | (blue & 0xFF);
    }

    //%
    void ultrasonicSetColor(uint8_t pinNum, uint8_t index, uint32_t rgb)
    {
        uint8_t red = (rgb >> 16) & 0xFF;
        uint8_t green = (rgb >> 8) & 0xFF;
        uint8_t blue = (rgb) & 0xFF;
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
    float ultrasonicDistanceCm(uint8_t pinNum)
    {
        uint16_t distance = 0;
        uint8_t Sensor_data1 = 0;
        uint8_t Sensor_data2 = 0;
        __disable_irq();
        if(one_wire_reset(pinNum) != 0){   
            __enable_irq();
            return 500;
        }
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        Sensor_data1=one_wire_read_byte(pinNum);
        Sensor_data2=one_wire_read_byte(pinNum);
        // distance=Sensor_data2*256 + Sensor_data1;
        distance=((uint16_t)Sensor_data2<<8) | (uint16_t)Sensor_data1; 
        __enable_irq();
        for (volatile uint16_t i = 0; i < 540; i++); 
        if ((float)distance/17.57>500){
            return 500;
        }
        else{
            return (float)distance/17.57;
        }   
    }
    
    /*
    **We MP3 Player module V1.0.
     */
    //%
    void MP3AppointMusic(uint8_t pinNum, uint16_t num)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x02);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,(uint8_t)(num>>8));
        one_wire_write_byte(pinNum,(uint8_t)(num));
        uBit.sleep(60);
    }

    //%
    void MP3AppointVolume(uint8_t pinNum, uint8_t num)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x03);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,num);
        uBit.sleep(60);
    }

    //%
    void MP3NextMusic(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x04);
        uBit.sleep(60);
    }

    //%
    void MP3Pause(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x05);
        uBit.sleep(60);
    }

    //%
    void MP3Play(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x06);
        uBit.sleep(60);
    }

    //%
    void MP3AppointDevice(uint8_t pinNum, uint8_t devtype)
    {
        if((devtype!=0x02)&&(devtype!=0x04)) return;
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x07);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,devtype);
        uBit.sleep(210);
    }

    //%
    bool MP3isOver(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)
            return 2;
        one_wire_write_byte(pinNum,0x08);
        one_wire_respond(pinNum);
        uint8_t r=one_wire_read_byte(pinNum);
        return r;
    }

    //%
    void MP3PrevMusic(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x09);
        uBit.sleep(60);
    }

    /*
    **We Humiture Sensor Sensor V1.0.
    */
    //%  
    uint16_t humitureSensorRead(uint8_t pinNum, uint8_t index)
    {
        uint8_t _temperature,_humidity;
        if(one_wire_reset(pinNum) != 0)
            return 0;
        one_wire_write_byte(pinNum,0x02);
        one_wire_respond(pinNum);
        _humidity=one_wire_read_byte(pinNum);
        _temperature=one_wire_read_byte(pinNum);
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
        if(Bright > 7){
            Bright = 0x07;
        }
    
        if(Bright >=0){
            Bright = 0x88 | (uint8_t)Bright; 
        }
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x06);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,Bright);    
        uBit.sleep(3);      
    }

    //%
    void ledPanelClearScreen(uint8_t pinNum)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x05);  
        uBit.sleep(5);      
    }

    //%
    void ledPanelShowLine(uint8_t pinNum, uint8_t x, uint8_t buffer)
    {
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x03);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,x);
        one_wire_write_byte(pinNum,buffer);
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
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,0x07);
        if(one_wire_reset(pinNum) != 0)return;
        one_wire_write_byte(pinNum,x);
        one_wire_write_byte(pinNum,y);
        one_wire_write_byte(pinNum,buffer);
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
    void ledPanelShowNum(uint8_t pinNum, float value)
    {
        uint8_t tempBuf[10] = {0};
        int buf[4] = {0};
        uint8_t isNeg = 0;
        uint8_t int_num = 0;
        // uint8_t bit_num = 0;
        uint8_t temp_num = 0;
        uint8_t comma_flag = 0;
        float number = value;
        if(number>=9999){
            ledPanelWriteChar(pinNum,-1,0,9+15);
            ledPanelWriteChar(pinNum,4,0,9+15);
            ledPanelWriteChar(pinNum,9,0,9+15);
            ledPanelWriteChar(pinNum,14,0,9+15);
            ledPanelShowLine(pinNum,20,0x00);
        }
        else if(number<= -999){
            ledPanelWriteChar(pinNum,4,0,9+15);
            ledPanelWriteChar(pinNum,10,0,9+15);
            ledPanelWriteChar(pinNum,15,0,9+15);
            ledPanelShowLine(pinNum,0,0x08);
            ledPanelShowLine(pinNum,1,0x08);
            ledPanelShowLine(pinNum,2,0x08);
            ledPanelShowLine(pinNum,3,0x08);
        }
        else if (number==0){
            ledPanelWriteChar(pinNum,0,0,3);
            ledPanelWriteChar(pinNum,5,0,3);
            ledPanelWriteChar(pinNum,10,0,3);
            ledPanelWriteChar(pinNum,15,0,15);
        }
        else{
            if (number<0){
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
            while(tempBuf[int_num]==0){
                int_num++;
            }
            if(number>=1){  
                buf[0]=tempBuf[int_num];
                buf[1]=tempBuf[int_num+1];
                buf[2]=tempBuf[int_num+2];
                buf[3]=tempBuf[int_num+3];
                comma_flag=4-int_num;
            }
            else{
                buf[0]=tempBuf[3];
                buf[1]=tempBuf[4];
                buf[2]=tempBuf[5];
                buf[3]=tempBuf[6];
                comma_flag=1;
            }

            if(isNeg==1){              
                if (number >= 100){
                    temp_num = 3;
                    ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                }
                else if (number >= 10){
                    if (buf[2]!=0){
                        temp_num = 3;
                        ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                    }
                    else{
                        temp_num =2;
                        ledPanelWriteChar(pinNum,4,0,12);     //'-'
                    }
                }
                else if (number >=1){
                    if (buf[2]!=0){
                        temp_num =3;
                        ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                    }
                    else if (buf[2]==0&&buf[1]!=0){
                        temp_num = 2;
                        ledPanelWriteChar(pinNum,4,0,12);     //'-'
                    }
                    else{
                        temp_num = 1;
                        ledPanelWriteChar(pinNum,9,0,12);     //'-'
                    }
                }
                else{
                    if (buf[2]!=0){
                        temp_num =3;
                        ledPanelWriteChar(pinNum,-1,0,12);     //'-'
                    }
                    else if (buf[2]==0&&buf[1]!=0){
                        temp_num = 2;
                        ledPanelWriteChar(pinNum,4,0,12);     //'-'
                    }
                    else{
                        temp_num = 1;
                        ledPanelWriteChar(pinNum,9,0,12);     //'-'
                    }
                }
               
                for(int i=0;i<temp_num;i++){
                    if (temp_num == 3){
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,6+i*5,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,4+i*5,0,buf[i]+15);
                        }
                        if (number>=100){
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else{
                            ledPanelShowLine(pinNum,5+5*comma_flag,0x40);
                        }
                    }
                    else if (temp_num == 2){
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,6+i*5+5,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,4+i*5+5,0,buf[i]+15);
                        }
                        ledPanelWriteChar(pinNum,-1,0,3);
                        if (number>=10){
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else{
                            ledPanelShowLine(pinNum,5+5*comma_flag+5,0x40);
                        }
                    }
                    else{
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,6+i*5+10,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,4+i*5+10,0,buf[i]+15);
                        }
                        ledPanelWriteChar(pinNum,4,0,3);
                    } 
                }
            }   
  
            else{
                if (number>=1000){
                    temp_num = 4;
                }
                else if(number>=100){
                    if(buf[3]!=0){
                        temp_num = 4;
                    }
                    else{
                        temp_num =3;
                    }
                }
                else if(number>=10){
                    if(buf[3]!=0){
                        temp_num = 4;
                    }
                    else if(buf[3]==0 && buf[2]!=0){
                        temp_num = 3;
                    }
                    else{
                        temp_num = 2;
                    } 
                }
                else if(number>=1){
                    if(buf[3]!=0){
                        temp_num = 4;
                    }
                    else if(buf[3]==0 && buf[2]!=0){
                        temp_num = 3;
                    }
                    else if(buf[3]==0 && buf[2]==0 && buf[1]!=0){
                        temp_num = 2;
                    }
                    else{
                        temp_num = 1;
                    }
                }
                else{
                    if(buf[3]!=0){
                        temp_num = 4;
                    }
                    else if(buf[3]==0 && buf[2]!=0){
                        temp_num = 3;
                    }
                    else if(buf[3]==0 && buf[2]==0 && buf[1]!=0){
                        temp_num = 2;
                    }
                    else{
                        temp_num = 1;
                    }
                }
     
                for(int i=0;i<temp_num;i++){
                    if(temp_num == 4){
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,1+i*5,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,-1+i*5,0,buf[i]+15);
                        }
                        if(number>=1000){
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else{
                            ledPanelShowLine(pinNum,5*comma_flag,0x40);
                        } 
                    }
                    else if(temp_num == 3){
                        ledPanelWriteChar(pinNum,-1,0,3);
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,1+i*5+5,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,-1+i*5+5,0,buf[i]+15);
                        }
                        if (number>=100){
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else{
                            ledPanelShowLine(pinNum,5*comma_flag+5,0x40);
                        }
                    }
                    else if(temp_num == 2){
                        ledPanelWriteChar(pinNum,-1,0,3);
                        ledPanelWriteChar(pinNum,4,0,3);
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,1+i*5+10,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,-1+i*5+10,0,buf[i]+15);
                        }
                        if (number>=10){
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                        else{
                            ledPanelShowLine(pinNum,5*comma_flag+10,0x40);
                        }
                    }
                    else {
                        ledPanelWriteChar(pinNum,-1,0,3);
                        ledPanelWriteChar(pinNum,4,0,3);
                        ledPanelWriteChar(pinNum,9,0,3);
                        if(comma_flag-1<i){
                            ledPanelWriteChar(pinNum,1+i*5+15,0,buf[i]+15);
                        }
                        else{
                            ledPanelWriteChar(pinNum,-1+i*5+15,0,buf[i]+15);
                        }
                        if (number>=1){
                            ledPanelShowLine(pinNum,20,0x00);
                        }
                    }   
                }
            }
        }         
    }
}
