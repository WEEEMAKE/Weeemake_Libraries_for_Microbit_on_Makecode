 
enum WePorts {
    //% block="PortA"
    PortA = 1,
    //% block="PortB"
    PortB = 2,
    //% block="PortC"
    PortC = 3,
    //% block="PortD"
    PortD = 4
}

enum WeRGBPorts {
    //% block="OnBoard"
    OnBoard = 0,
    //% block="PortA"
    PortA = 1,
    //% block="PortB"
    PortB = 2,
    //% block="PortC"
    PortC = 3,
    //% block="PortD"
    PortD = 4
}

enum swStatus {
    //% block="On"
    On = 1,
    //% block="Off"
    Off = 0
}

enum TouchMode {
    //% block="Mode0"
    Mode0 = 0,
    //% block="Mode1"
    Mode1 = 1
}

enum Servos {
    Servo1 = 2,
    Servo2 = 1
}

enum Motors {
    M1 = 1,
    M2 = 2
}

enum ultrasonicLED {
    //% block="All"
    All = 3,
    //% block="Left"
    Left = 2,
    //% block="Right"
    Right = 1  
}

enum JSAxis{
    //% block="X_Axis"
    X_Axis = 0,
    //% block="Y_Axis"
    Y_Axis = 1
}

enum KeyButtons {
    K1 = 1,
    K2 = 2,
    K3 = 3,
    K4 = 4
}

enum Lines{
    S1 = 1,
    S2 = 2
}

enum Flames{
    S1 = 1,
    S2 = 2,
    S3 = 3
}

enum MP3Device{
    FLASH = 4,
    //% block="TFCard"
    TFCard = 2
}

enum Humiture{
    //% block="temperature"
    temperature = 0,
    //% block="humidity"
    humidity = 1
}  

enum PM25_Sensor{
    //%block="PM1.0"
    PM1_0 = 0,
    //%block="PM2.5"
    PM2_5 = 1,
    //%block="PM10"
    PM10 = 2
}

enum PM25_Sensor_Num{
    //%block="0.3um"
    Num0_3 = 0,
    //%block="0.5um"
    Num0_5 = 1,
    //%block="1.0um"
    Num1_0 = 2,
    //%block="2.5um"
    Num2_5 = 3,
    //%block="5.0um"
    Num5_0 = 4,
    //%block="10um"
    Num10 = 5
}

enum Color_Sensor{
    //%block="Light"
    LightValue=0,
    //%block="Red"
    RedValue=1,
    //%block="Green"
    GreenValue=2,
    //%block="Blue"
    BlueValue=3
}

//% icon="\uf021" color=#E94C4A
namespace ELF_Executive{

    const PCA9685_ADDRESS = 0x40
    const MODE1 = 0x00
    const MODE2 = 0x01
    const SUBADR1 = 0x02
    const SUBADR2 = 0x03
    const SUBADR3 = 0x04
    const PRESCALE = 0xFE
    const LED0_ON_L = 0x06
    // const LED0_ON_H = 0x07
    // const LED0_OFF_L = 0x08
    // const LED0_OFF_H = 0x09
    // const ALL_LED_ON_L = 0xFA
    // const ALL_LED_ON_H = 0xFB
    // const ALL_LED_OFF_L = 0xFC
    // const ALL_LED_OFF_H = 0xFD

    let initialized = false

    function i2cwrite(addr: number, reg: number, value: number) {
        let buf = pins.createBuffer(2)
        buf[0] = reg
        buf[1] = value
        pins.i2cWriteBuffer(addr, buf)
    }

    function i2ccmd(addr: number, value: number) {
        let buf = pins.createBuffer(1)
        buf[0] = value
        pins.i2cWriteBuffer(addr, buf)
    }

    function i2cread(addr: number, reg: number) {
        pins.i2cWriteNumber(addr, reg, NumberFormat.UInt8BE);
        let val = pins.i2cReadNumber(addr, NumberFormat.UInt8BE);
        return val;
    }

    function initPCA9685(): void {
        i2cwrite(PCA9685_ADDRESS, MODE1, 0x00)
        setFreq(50);
        for (let idx = 0; idx < 16; idx++) {
            setPwm(idx, 0 ,0);
        }
        initialized = true
    }

    function setFreq(freq: number): void {
        // Constrain the frequency
        let prescaleval = 25000000;
        prescaleval /= 4096;
        prescaleval /= freq;
        prescaleval -= 1;
        let prescale = prescaleval; //Math.Floor(prescaleval + 0.5);
        let oldmode = i2cread(PCA9685_ADDRESS, MODE1);
        let newmode = (oldmode & 0x7F) | 0x10; // sleep
        i2cwrite(PCA9685_ADDRESS, MODE1, newmode); // go to sleep
        i2cwrite(PCA9685_ADDRESS, PRESCALE, prescale); // set the prescaler
        i2cwrite(PCA9685_ADDRESS, MODE1, oldmode);
        control.waitMicros(5000);
        i2cwrite(PCA9685_ADDRESS, MODE1, oldmode | 0xa1);
    }

    function setPwm(channel: number, on: number, off: number): void {
        if (channel < 0 || channel > 15)
            return;

        let buf = pins.createBuffer(5);
        buf[0] = LED0_ON_L + 4 * channel;
        buf[1] = on & 0xff;
        buf[2] = (on >> 8) & 0xff;
        buf[3] = off & 0xff;
        buf[4] = (off >> 8) & 0xff;
        pins.i2cWriteBuffer(PCA9685_ADDRESS, buf);
    }

    function stopMotor(index: number) {
        setPwm((index - 1)*2, 0, 0);
        setPwm((index - 1)*2 + 1, 0, 0);
    }

    /**
     * Servo Execute
     * @param index Servo Channel; eg: S1, S2
     * @param degree [0-180] degree of servo; eg: 0, 90, 180
    */
    //% blockId=ELF_Executive_servo block="Servo|%index|degree %degree"
    //% weight=22
    //% blockGap=10
    //% degree.min=0 degree.max=180
    //% name.fieldEditor="gridpicker" name.fieldOptions.columns=4
    export function Servo(index: Servos, degree: number): void {
        if (!initialized) {
            initPCA9685()
        }
        // 50hz: 20,000 us
        let v_us = (degree * 1800 / 180 + 600) // 0.6 ~ 2.4
        let value = v_us * 4096 / 20000
        setPwm(index + 12, 0, value)
    }

    //% blockId=ELF_Executive_motor_run block="Motor|%index|speed %speed"
    //% weight=22
    //% blockGap=10
    //% speed.min=-255 speed.max=255
    //% name.fieldEditor="gridpicker" name.fieldOptions.columns=4
    export function MotorRun(index: Motors, speed: number): void {
        if (!initialized) {
            initPCA9685()
        }
        speed = speed * 16; // map 255 to 4096
        if (speed >= 4096) {
            speed = 4095
        }
        if (speed <= -4096) {
            speed = -4095
        }
        if (index > 2 || index <= 0)
            return
        let pp = (index - 1)*2;
        let pn = (index - 1)*2 + 1;
        if (speed >= 0) {
            setPwm(pp, 0, 0)
            setPwm(pn, 0, speed)
        } else {
            setPwm(pp, 0, -speed)
            setPwm(pn, 0, 0)
        }
    }

    /**
     * Execute single motors with delay
     * @param index Motor Index; eg: M1, M2
     * @param speed [-255-255] speed of motor; eg: 150
     * @param delay seconde delay to stop; eg: 1
    */
    //% blockId=ELF_Executive_motor_rundelay block="Motor|%index|speed %speed|delay %delay|s"
    //% weight=22
    //% blockGap=10
    //% speed.min=-255 speed.max=255
    //% name.fieldEditor="gridpicker" name.fieldOptions.columns=4
    export function MotorRunDelay(index: Motors, speed: number, delay: number): void {
        MotorRun(index, speed);
        basic.pause(delay * 1000);
        MotorRun(index, 0);
    }

    //% blockId=ELF_Executive_stop block="Motor Stop|%index|"
    //% weight=22
    //% blockGap=10
    export function MotorStop(index: Motors): void {
        MotorRun(index, 0);
    }

    //% blockId=ELF_Executive_stop_all block="Motor Stop All"
    //% weight=22
    //% blockGap=10
    export function MotorStopAll(): void {
        for (let idx = 1; idx <= 2; idx++) {
            stopMotor(idx);
        }
    }

    /**
     * DC 130 Motor run speed
    */

    //% blockId=ELF_Executive_DC130MotorRunSpeed block="130 DC Motor|%pinNum| runSpeed |%speed"
    //% weight=22
    //% blockGap=10
    //% speed.min=-255 speed.max=255
    //% shim=elfshield::DC130MotorRunSpeed
    export function DC130MotorRunSpeed(pinNum: WePorts, speed: number): void {
        return;
    }

    /**
     * Water Atomizer Moduel set start
    */
    //% blockId="ELF_Executive_waterAtomizerSet" block="Water Atomizer |%pinNum| |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::waterAtomizerSet
    export function waterAtomizerSet(pinNum: WePorts, isOn: swStatus): void {
        return;
    }  

    /**
     * Relay Module set switch
    */
    //% blockId="ELF_Executive_RelaySet" block="Relay|%pinNum| set |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::RelaySet
    export function RelaySet(pinNum: WePorts, isOn: swStatus): void {
        return;
    }
    
}

//% color=#EB5B18 icon="\uf26c"
namespace ELF_Display{
    
    let neoStrip:  neopixel.Strip = null;
    let neoStripA: neopixel.Strip = null;
    let neoStripB: neopixel.Strip = null;
    let neoStripC: neopixel.Strip = null;
    let neoStripD: neopixel.Strip = null;

    /**
     * Init RGB pixels mounted on elfshield
     * @param num_pixels , eg: 3
     */
    //% blockId="ELF_Display_rgb" block="|%pinNum|with|%num_pixels|leds"
    //% weight=22
    //% num_pixels.min=1 num_pixels.max=60
    //% blockGap=10
    export function rgb(pinNum: WeRGBPorts, num_pixels: number): neopixel.Strip {
        if (pinNum == 0) {
            if (!neoStrip) {
                neoStrip = neopixel.create(DigitalPin.P8, 3, NeoPixelMode.RGB)
            }
            return neoStrip;
        }
        else if (pinNum == 1) {
            if (!neoStripA) {
                neoStripA = neopixel.create(DigitalPin.P13, num_pixels, NeoPixelMode.RGB)
            }
            return neoStripA;
        }
        else if (pinNum == 2) {
            if (!neoStripB) {
                neoStripB = neopixel.create(DigitalPin.P14, num_pixels, NeoPixelMode.RGB)
            }
            return neoStripB;
        }
        else if (pinNum == 3) {
            if (!neoStripC) {
                neoStripC = neopixel.create(DigitalPin.P15, num_pixels, NeoPixelMode.RGB)
            }
            return neoStripC;
        }
        else if (pinNum == 4) {
            if (!neoStripD) {
                neoStripD = neopixel.create(DigitalPin.P16, num_pixels, NeoPixelMode.RGB)
            }
            return neoStripD;
        }
        else
            return null;
    }

    /**
     * Init RGB pixels mounted on elfshield
     * @param index , eg:0
     */
    //% blockId="ELF_Display_showRGB" block="RGB at%strip=ELF_Display_rgb|at|%index|show color|%rgb=neopixel_colors"
    //% weight=22
    //% blockGap=10
    export function showRGB(strip:neopixel.Strip,index:number,rgb:number):void{
        if (index == 0) {
            strip.showColor(rgb);
        }
        else{
            strip.setPixelColor(index-1, rgb);
            strip.show();
        } 
    }

    /**
     * Clear RGB
     * 
     */
    //% blockId="ELF_Display_clearAllRGB" block="All RGB at%strip=ELF_Display_rgb|clear"
    //% weight=22
    //% blockGap=10
    export function clearAllRGB(strip:neopixel.Strip):void{
        strip.clear();
        strip.show();
    }

    /**
     * 4-Digital LED module Show Number
    */
    //% blockId="ELF_Display_segmentDisplayShowNumber" block="Segment Display|%pinNum| show Number %value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::segmentDisplayShowNumber
    export function segmentDisplayShowNumber(pinNum: WePorts, value: number): void {
        return;
    }

    /**
     * ultrasonic led set
    */

    //% weight=22
    //% blockGap=10
    //% blockId=ELF_Display_ultrasonicSetLed 
    //% block="Ultrasonic|%pinNum| yellow led |%index| set %isOn"
    //% shim=elfshield::ultrasonicSetLed
    export function ultrasonicSetLed(pinNum: WePorts, index: ultrasonicLED, isOn:swStatus): void {
        return;
    }

    /**
     * ultrasonic rgb value
    */

    //% weight=42
    //% blockGap=10
    //% blockId=ELF_Display_rgb_value block="red|%red green |%green| blue|%blue"
    //% red.min=0 red.max=255
    //% green.min=0 green.max=255
    //% blue.min=0 blue.max=255
    //% shim=elfshield::ultrasonicRGB
    export function rgb_value(red: number, green: number, blue: number): number {
        return 0;
    }

    /**
     * ultrasonic rgb led show
    */

    //% weight=42
    //% blockGap=10
    //% blockId=ELF_Display_ultrasonicRGBShow block="Ultrasonic %pinNum RGB led |%index| rgb|%rgb=neopixel_colors"
    //% shim=elfshield::ultrasonicSetColor
    export function ultrasonicRGBShow(pinNum: WePorts, index: ultrasonicLED, rgb: number): void {
        return;
    }

    /**
     * Single LED set Light
    */
    //% blockId="ELF_Display_singleLEDSet" block="Single LED|%pinNum| set |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::singleLEDSet
    export function singleLEDSet(pinNum: WePorts, isOn: swStatus): void {
        return;
    }

    /**
     * LEDMatrix Display Set Brightness
    */
    //% blockId="ELF_Display_ledPanelSetBrightness" block="LED Matrix|%pinNum|set brightness|%Bright"
    //% weight=22
    //% blockGap=10
    //% Bright.min=1 Bright.max=7
    //% shim=elfshield::ledPanelSetBrightness
    export function ledPanelSetBrightness(pinNum: WePorts, Bright:number): void {
        return;
    }
    /**
     * LEDMatrix Display Clear Screen
    */
    //% blockId="ELF_Display_ledPanelClearScreen" block="LED Matrix|%pinNum|clear screen"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::ledPanelClearScreen
    export function ledPanelClearScreen(pinNum: WePorts): void {
        return;
    }

    /**
     * LEDMatrix Display Show Clock
    */
    //% blockId="ELF_Display_ledPanelShowClock" block="LED Matrix|%pinNum|show Clock|%hour|:|%minute"
    //% weight=22
    //% blockGap=10
    //% hour.min=0 hour.max=23
    //% minute.min=0 minute.max=59
    //% shim=elfshield::ledPanelShowClock
    export function ledPanelShowClock(pinNum: WePorts, hour:number, minute:number): void {
        return;
    }

    /**
     * LEDMatrix Display Show Number
    */
    //% blockId="ELF_Display_ledPanelShowNum" block="LED Matrix|%pinNum|show number|%value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::ledPanelShowNum
    export function ledPanelShowNum(pinNum: WePorts, value:number): void {
        return;
    }
    /**
     * LEDMatrix Display Show on Dot
    */
    //% blockId="ELF_Display_ledPanelTurnOnDot" block="LED Matrix|%pinNum|show pixel x:|%x|y:|%y"
    //% weight=22
    //% blockGap=10
    //% x.min=0 x.max=20 y.min=0 y.max=6
    //% shim=elfshield::ledPanelTurnOnDot
    export function ledPanelTurnOnDot(pinNum: WePorts, x:number,y:number): void {
        return;
    }
    /**
     * LEDMatrix Display hide Dot
    */
    //% blockId="ELF_Display_ledPanelTurnOffDot" block="LED Matrix|%pinNum|hide pixel x:|%x|y:|%y"
    //% weight=22
    //% blockGap=10
    //% x.min=0 x.max=20 y.min=0 y.max=6
    //% shim=elfshield::ledPanelTurnOffDot
    export function ledPanelTurnOffDot(pinNum: WePorts, x:number,y:number): void {
        return;
    }
}

//% color=#ff6680 weight=10 icon="\uf001"
namespace ELF_Music{

    /**
     * MP3 Player Set music
    */
    //% blockId="ELF_Music_MP3AppointMusic" block="MP3|%pinNum| set music|%number"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3AppointMusic
    export function MP3AppointMusic(pinNum: WePorts, num: number): void {
        return;
    }
    /**
     * MP3 Player Set Volume
    */
    //% blockId="ELF_Music_MP3AppointVolume" block="MP3|%pinNum| set volmue|%number"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3AppointVolume
    export function MP3AppointVolume(pinNum: WePorts, num: number): void {
        return;
    }
    /**
     * MP3 Player Set Next Music
    */
    //% blockId="ELF_Music_MP3NextMusic" block="MP3|%pinNum| next music"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3NextMusic
    export function MP3NextMusic(pinNum: WePorts): void {
        return;
    }
    /**
     * MP3 Player Set Pause
    */
    //% blockId="ELF_Music_MP3Pause" block="MP3|%pinNum| pause"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3Pause
    export function MP3Pause(pinNum: WePorts): void {
        return;
    }
    /**
     * MP3 Player Set play
    */
    //% blockId="ELF_Music_MP3Play" block="MP3|%pinNum| play"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3Play
    export function MP3Play(pinNum: WePorts): void {
        return;
    }
    /**
     * MP3 Player Set appoint device
    */
    //% blockId="ELF_Music_MP3AppointDevice" block="MP3|%pinNum| set device|%MP3Device"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3AppointDevice
    export function MP3AppointDevice(pinNum: WePorts, devtype: MP3Device): void {
        return;
    }

    /**
     * MP3 Player is Over
    */
    //% blockId="ELF_Music_MP3isOver" block="MP3|%pinNum| is over"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3isOver
    export function MP3isOver(pinNum: WePorts): boolean {
        return false;
    }
    /**
     * MP3 Player Set Prev Music
    */
    //% blockId="ELF_Music_MP3PrevMusic" block="MP3|%pinNum| prev music"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3PrevMusic
    export function MP3PrevMusic(pinNum: WePorts): void {
        return;
    }
}

//% color=#25B7DB weight=10 icon="\uf2a2"
namespace ELF_Sensors{

    /**
     * get mic analog value.
     */
    //% blockId="ELF_Sensors_mic" block="Volume"
    //% weight=22
    //% blockGap=10
    export function getMicValue(): number {
        let value = 0;
        value = pins.analogReadPin(AnalogPin.P2);
        return value;
    }

    /**
     * Gas Sensor MQ2 read value
    */
    //% blockId="ELF_Sensors_gasSensorReadValue" block="Gas Sensor|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::gasSensorReadValue
    export function gasSensorReadValue(pinNum: WePorts): number {
        return 0;
    }
    /**
     * get PIR Motion Sensor
    */
    //% blockId="ELF_Sensors_getPIRMotion" block="PIRMotion|%pinNum|"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::getPIRMotion
    export function getPIRMotion(pinNum: WePorts): boolean {
        return false;
    } 

    /**
     * Line Follower Sensor read value
    */
    //% blockId="ELF_Sensors_lineFollowerRead" block="Line Follower|%pinNum| |%Lines| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::lineFollowerRead
    export function lineFollowerRead(pinNum: WePorts, index: Lines): number {
        return 0;
    }
    /**
     * Tilt Switch read value
    */
    //% blockId="ELF_Sensors_tiltSwitchRead" block="Tilt senosr|%pinNum| |%Lines| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::tiltSwitchRead
    export function tiltSwitchRead(pinNum: WePorts, index: Lines): boolean {
        return false;
    }
    /**
     * Flame Sensor read value
    */
    //% blockId="ELF_Sensors_flameSensorRead" block="Flame senosr|%pinNum| |%Flames| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::flameSensorRead
    export function flameSensorRead(pinNum: WePorts, index: Flames): number {
        return 0;
    }

     /**
     * ultrasonic get Distance value
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId=ELF_Sensors_ultrasonicDistanceCm block="Ultrasonic|%pinNum| Distance(cm)"
    //% shim=elfshield::ultrasonicDistanceCm
    export function ultrasonicDistanceCm(pinNum: WePorts): number {
        return 0;
    }

    /**
     * Humiture Sensor read value
    */
    //% blockId="ELF_Sensors_humitureSensorRead" block="Humiture Sensor|%pinNum| |%Humiture| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::humitureSensorRead
    export function humitureSensorRead(pinNum: WePorts, index: Humiture): number {
        return 0;
    }

    /**
     * PM2.5 Sensor Set Fan
    */
    //% blockId="ELF_Sensors_PM2.5_Sensor_Set_Fan" block="PM2.5 Sensor |%pinNum| set fan insert |%isOn|"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::PM25_setFanLaser
    export function PM25_setFanLaser(pinNum: WePorts, isOn: swStatus): void {
        return;
    }

    /**
     * PM2.5 Sensor get value (ug/m^3)
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_PM25_read_PM_value" block="PM25 Sensor |%pinNum| get |%index| value(ug/m^3)"
    //% shim=elfshield::PM25_readPMConcentration
    export function PM25_readPMConcentration(pinNum: WePorts, index: PM25_Sensor): number {
        return 0;
    }

    /**
     * PM2.5 Sensor get the number of different particles in 0.1L air
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_PM25_read_the_number_of_particles" block="PM25 Sensor |%pinNum| get number of |%index|particles in 0.1L air"
    //% shim=elfshield::PM25_readNumIn100ml
    export function PM25_readNumIn100ml(pinNum: WePorts, index: PM25_Sensor_Num): number {
        return 0;
    }

    /**
     * Barometer Sensor set Origin Position
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_Barometer_set_Origin_Position" block="Barometer Sensor |%pinNum| set Origin Position"
    //% shim=elfshield::Barometer_setOrigin
    export function Barometer_setOrigin(pinNum: WePorts): void {
        return;
    }

    /**
     * Barometer Sensor get RelativeHeight
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_Barometer_read_the_Relative_Height" block="Barometer Sensor |%pinNum| get the Relative Height"
    //% shim=elfshield::Barometer_readRelativeHeight
    export function Barometer_readRelativeHeight(pinNum: WePorts): number {
        return 0;
    }

    /**
     * Barometer Sensor get Pressure
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_Barometer_read_the_Pressure" block="Barometer Sensor |%pinNum| get the Pressure"
    //% shim=elfshield::Barometer_readPressure
    export function Barometer_readPressure(pinNum: WePorts): number {
        return 0;
    }

    /**
     * Water Sensor get Analog Value
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_Water_read_the_Value" block="Water Sensor |%pinNum| get the Analog Value"
    //% shim=elfshield::Water_Sensor_readAnalog
    export function Water_Sensor_readAnalog(pinNum: WePorts): number {
        return 0;
    }

    /**
     * UV Sensor get Analog Value
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_UV_read_the_Value" block="UV Sensor |%pinNum| get the Analog Value"
    //% shim=elfshield::UV_Sensor_readAnalog
    export function UV_Sensor_readAnalog(pinNum: WePorts): number {
        return 0;
    }

    /**
     * UV Sensor get the Index
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_UV_read_the_Index" block="UV Sensor |%pinNum| get the Index"
    //% shim=elfshield::UV_Sensor_readIndex
    export function UV_Sensor_readIndex(pinNum: WePorts): number {
        return 0;
    }

    /**
     * Color Sensor set light on board
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_Color_set_light" block="Color Sensor |%pinNum| set LED |%index|"
    //% shim=elfshield::Color_Sensor_setLight
    export function Color_Sensor_setLight(pinNum: WePorts, isOn:swStatus): void {
        return ;
    }

    /**
     * Color Sensor get the value
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Sensors_Color_read_value" block="Color Sensor |%pinNum| get |%index|value"
    //% shim=elfshield::Color_Sensor_readValue
    export function Color_Sensor_readValue(pinNum: WePorts, index:Color_Sensor): number {
        return 0;
    }


}
//% icon="\uf11b" color=#B9D01E
namespace ELF_Control{
    /**
     * Touch Moduel set Mode
     * @param index Mode; eg: Mode1
     * @param Mode [0-1] ; eg: 0, 1
    */
    //% blockId="ELF_Control_touchSetMode" block="Touch Sensor |%pinNum|setMode|%mode"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::touchSetMode
    export function touchSetMode(pinNum: WePorts,mode: TouchMode): void {
        return;
    }

    /**
    * get touch module status.
    */
    //% blockId="ELF_Control_getTouched" block="Touched |%pinNum"
    //% weight=10
    //% blockGap=10
    //% shim=elfshield::getTouched
    export function getTouched(pinNum: WePorts): boolean {
        return false;
    } 

    /**
     * 4 LED Button read Key
    */
    //% blockId="ELF_Control_LED4ButtonReadKey" block="LED Button|%pinNum| |%KeyButtons| Pressed"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::LED4ButtonReadKey
    export function LED4ButtonReadKey(pinNum: WePorts, key: KeyButtons): boolean {
        return false;
    }
    /**
     * Sliding Potentiometer read value
    */
    //% blockId="ELF_Control_slidingPotentiometerReadValue" block="Sliding Potentiometer|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::slidingPotentiometerReadValue
    export function slidingPotentiometerReadValue(pinNum: WePorts): number {
        return 0;
    }
    /**
     * Potentiometer read value
    */
    //% blockId="ELF_Control_PotentiometerReadValue" block="Potentiometer|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::PotentiometerReadValue
    export function PotentiometerReadValue(pinNum: WePorts): number {
        return 0;
    }

    /**
     * Joystick Module read value
    */
    //% blockId="ELF_Control_JoystickReadValue" block="Joystick|%pinNum| get|%JSAxis| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::JoystickReadValue
    export function JoystickReadValue(pinNum: WePorts, axis: JSAxis): number {
        return 0;
    }

    /**
     * Funny Touch Sensor get the value
    */
    
    //% weight=22
    //% blockGap=10
    //% blockId="ELF_Control_Funny Touch_read_the_value" block="Funny Touch Sensor |%pinNum| get the value"
    //% shim=elfshield::Funny_Touch_Sensor_readValue
    export function Funny_Touch_Sensor_readValue(pinNum: WePorts): number {
        return 0;
    }
}
