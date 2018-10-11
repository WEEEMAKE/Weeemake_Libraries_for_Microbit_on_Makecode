/*
load dependency
"elfshield": "file:../main.ts"
*/

//% color=#ff6680 weight=10 icon="\uf108"
namespace elfshield {

enum NeoPixelColors {
    //% block=red
    Red = 0xFF0000,
    //% block=orange
    Orange = 0xFFA500,
    //% block=yellow
    Yellow = 0xFFFF00,
    //% block=green
    Green = 0x00FF00,
    //% block=blue
    Blue = 0x0000FF,
    //% block=indigo
    Indigo = 0x4b0082,
    //% block=violet
    Violet = 0x8a2be2,
    //% block=purple
    Purple = 0xFF00FF,
    //% block=white
    White = 0xFFFFFF,
    //% block=black
    Black = 0x000000
}
    const PCA9685_ADDRESS = 0x40
    const MODE1 = 0x00
    const MODE2 = 0x01
    const SUBADR1 = 0x02
    const SUBADR2 = 0x03
    const SUBADR3 = 0x04
    const PRESCALE = 0xFE
    const LED0_ON_L = 0x06
    const LED0_ON_H = 0x07
    const LED0_OFF_L = 0x08
    const LED0_OFF_H = 0x09
    const ALL_LED_ON_L = 0xFA
    const ALL_LED_ON_H = 0xFB
    const ALL_LED_OFF_L = 0xFC
    const ALL_LED_OFF_H = 0xFD

    export enum Servos {
        S1 = 2,
        S2 = 1
    }

    export enum Motors {
        M1 = 1,
        M2 = 2
    }
    export enum wPorts {
        PortA = 1,
        PortB = 2,
        PortC = 3,
        PortD = 4
    }
    export enum touchMode {
        Mode0 = 0,
        Mode1 = 1
    }

    export enum ultrasonicLED {
        Right = 1,
        Left = 2,
        All = 3
    }
    export enum swStatus {
        On = 1,
        Off = 0
    }
    export enum KeyButtons {
        key1 = 1,
        key2 = 2,
        key3 = 3,
        key4 = 4
    }
    export enum Colors{
        red = 1,
        green = 2,
        blue = 3
    }
    export enum JSAxis{
        X_Axis = 0,
        Y_Axis = 1
    }
    export enum Lines{
        S1 = 1,
        S2 = 2
    }
    export enum Flames{
        S1 = 1,
        S2 = 2,
        S3 = 3
    }
    export enum MP3Device{
        FLASH = 4,
        TFCard = 2
    }
    export enum Humiture{
        temperature = 0,
        humidity = 1
    }    
    let initialized = false
    let neoStrip: neopixel.Strip;

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
     * Init RGB pixels mounted on robotbit
     */
    //% blockId="wemakebit_rgb" block="RGB"
    //% weight=5
    //% blockGap=10
    export function rgb(): neopixel.Strip {
        if (!neoStrip) {
            neoStrip = neopixel.create(DigitalPin.P8, 3, NeoPixelMode.RGB)
        }
        return neoStrip;
    }
    /**
     * get mic analog value.
     */
    //% blockId="wemakebit_mic" block="Volume"
    //% weight=5
    //% blockGap=10
    export function getMicValue(): number {
        let value = 0;
        value = pins.analogReadPin(AnalogPin.P2);
        return value;
    }
    /**
     * Servo Execute
     * @param index Servo Channel; eg: S1, S2
     * @param degree [0-180] degree of servo; eg: 0, 90, 180
    */
    //% blockId=wemakebit_servo block="Servo|%index|degree %degree"
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
    //% blockId=wemakebit_motor_run block="Motor|%index|speed %speed"
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
            setPwm(pp, 0, speed)
            setPwm(pn, 0, 0)
        } else {
            setPwm(pp, 0, 0)
            setPwm(pn, 0, -speed)
        }
    }
    /**
     * Execute single motors with delay
     * @param index Motor Index; eg: M1, M2
     * @param speed [-255-255] speed of motor; eg: 150
     * @param delay seconde delay to stop; eg: 1
    */
    //% blockId=wemakebit_motor_rundelay block="Motor|%index|speed %speed|delay %delay|s"
    //% weight=22
    //% blockGap=10
    //% speed.min=-255 speed.max=255
    //% name.fieldEditor="gridpicker" name.fieldOptions.columns=4
    export function MotorRunDelay(index: Motors, speed: number, delay: number): void {
        MotorRun(index, speed);
        basic.pause(delay * 1000);
        MotorRun(index, 0);
    }

    //% blockId=wemakebit_stop block="Motor Stop|%index|"
    //% weight=22
    //% blockGap=10
    export function MotorStop(index: Motors): void {
        MotorRun(index, 0);
    }

    //% blockId=wemakebit_stop_all block="Motor Stop All"
    //% weight=22
    //% blockGap=10
    export function MotorStopAll(): void {
        for (let idx = 1; idx <= 2; idx++) {
            stopMotor(idx);
        }
    }
    /**
     * Touch Moduel set Mode
     * @param index Mode; eg: Mode1
     * @param Mode [0-1] ; eg: 0, 1
    */
    //% blockId="wemakebit_touchSetMode" block="touch |%pinNum| setMode |%mode"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::touchSetMode
    export function touchSetMode(pinNum: wPorts,mode: touchMode): void {
        return;
    }

    /**
    * get touch module status.
    */
    //% blockId="wemakebit_getTouched" block="touched |%pinNum"
    //% weight=10
    //% blockGap=10
    //% shim=wemakebit::getTouched
    export function getTouched(pinNum: wPorts): number {
        return 0;
    } 

    /**
     * Water Atomizer Moduel set start
    */
    //% blockId="wemakebit_waterAtomizerSet" block="water Atomizer |%pinNum| |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::waterAtomizerSet
    export function waterAtomizerSet(pinNum: wPorts, isOn: swStatus): void {
        return;
    }  

    /**
     * 4 LED Button read Key
    */
    //% blockId="wemakebit_LED4ButtonReadKey" block="LED Button|%pinNum| |%KeyButtons| Pressed"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::LED4ButtonReadKey
    export function LED4ButtonReadKey(pinNum: wPorts, key: KeyButtons): number {
        return 0;
    }
    /**
     * Sliding Potentiometer read value
    */
    //% blockId="wemakebit_slidingPotentiometerReadValue" block="sliding Potentiometer|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::slidingPotentiometerReadValue
    export function slidingPotentiometerReadValue(pinNum: wPorts): number {
        return 0;
    }
    /**
     * Potentiometer read value
    */
    //% blockId="wemakebit_PotentiometerReadValue" block="Potentiometer|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::PotentiometerReadValue
    export function PotentiometerReadValue(pinNum: wPorts): number {
        return 0;
    }
    /**
     * Gas Sensor MQ2 read value
    */
    //% blockId="wemakebit_gasSensorReadValue" block="Gas Sensor|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::gasSensorReadValue
    export function gasSensorReadValue(pinNum: wPorts): number {
        return 0;
    }
    /**
     * get PIR Motion Sensor
    */
    //% blockId="wemakebit_getPIRMotion" block="getPIRMotion|%pinNum|"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::getPIRMotion
    export function getPIRMotion(pinNum: wPorts): number {
        return 0;
    } 
    /**
     * 4-Digital LED module Show Number
    */
    //% blockId="wemakebit_segmentDisplayShowNumber" block="Segment Display|%pinNum| show Number %value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::segmentDisplayShowNumber
    export function segmentDisplayShowNumber(pinNum: wPorts, value: number): void {
        return;
    }
    /**
     * Joystick Module read value
    */
    //% blockId="wemakebit_JoystickReadValue" block="Joystick|%pinNum| get|%JSAxis| Value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::JoystickReadValue
    export function JoystickReadValue(pinNum: wPorts, axis: JSAxis): number {
        return 0;
    }
    /**
     * Line Follower Sensor read value
    */
    //% blockId="wemakebit_lineFollowerRead" block="Line Follower|%pinNum| |%Lines| Value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::lineFollowerRead
    export function lineFollowerRead(pinNum: wPorts, index: Lines): number {
        return 0;
    }
    /**
     * Tilt Switch read value
    */
    //% blockId="wemakebit_tiltSwitchRead" block="Tilt senosr|%pinNum| |%Lines| Value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::tiltSwitchRead
    export function tiltSwitchRead(pinNum: wPorts, index: Lines): number {
        return 0;
    }
    /**
     * Flame Sensor read value
    */
    //% blockId="wemakebit_flameSensorRead" block="Flame senosr|%pinNum| |%Flames| Value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::flameSensorRead
    export function flameSensorRead(pinNum: wPorts, index: Flames): number {
        return 0;
    }
    /**
     * Single LED set Light
    */
    //% blockId="wemakebit_singleLEDSet" block="single LED|%pinNum| set |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::singleLEDSet
    export function singleLEDSet(pinNum: wPorts, isOn: swStatus): void {
        return;
    }
    /**
     * Relay Module set switch
    */
    //% blockId="wemakebit_RelaySet" block="Relay|%pinNum| set |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::RelaySet
    export function RelaySet(pinNum: wPorts, isOn: swStatus): void {
        return;
    }
    /**
     * DC 130 Motor run speed
    */
    //% blockId="elfshield_DC130MotorRunSpeed" block="130 DC Motor|%pinNum| runSpeed |%speed"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::DC130MotorRunSpeed
    export function DC130MotorRunSpeed(pinNum: wPorts, speed: number): void {
        return;
    }
    /**
     * MP3 Player Set music
    */
    //% blockId="wemakebit_MP3AppointMusic" block="MP3|%pinNum| set music|%number"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3AppointMusic
    export function MP3AppointMusic(pinNum: wPorts, num: number): void {
        return;
    }
    /**
     * MP3 Player Set Volume
    */
    //% blockId="wemakebit_MP3AppointVolume" block="MP3|%pinNum| set volmue|%number"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3AppointVolume
    export function MP3AppointVolume(pinNum: wPorts, num: number): void {
        return;
    }
    /**
     * MP3 Player Set Next Music
    */
    //% blockId="wemakebit_MP3NextMusic" block="MP3|%pinNum| next music"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3NextMusic
    export function MP3NextMusic(pinNum: wPorts): void {
        return;
    }
    /**
     * MP3 Player Set Pause
    */
    //% blockId="wemakebit_MP3Pause" block="MP3|%pinNum| pause"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3Pause
    export function MP3Pause(pinNum: wPorts): void {
        return;
    }
    /**
     * MP3 Player Set play
    */
    //% blockId="wemakebit_MP3Play" block="MP3|%pinNum| play"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3Play
    export function MP3Play(pinNum: wPorts): void {
        return;
    }
    /**
     * MP3 Player Set appoint device
    */
    //% blockId="wemakebit_MP3AppointDevice" block="MP3|%pinNum| set device|%MP3Device"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3AppointDevice
    export function MP3AppointDevice(pinNum: wPorts, devtype: MP3Device): void {
        return;
    }

    /**
     * MP3 Player is Over
    */
    //% blockId="wemakebit_MP3isOver" block="MP3|%pinNum| is over"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3isOver
    export function MP3isOver(pinNum: wPorts): number {
        return 0;
    }
    /**
     * MP3 Player Set Prev Music
    */
    //% blockId="wemakebit_MP3PrevMusic" block="MP3|%pinNum| prev music"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::MP3PrevMusic
    export function MP3PrevMusic(pinNum: wPorts): void {
        return;
    }
    /**
     * LEDMatrix Display Set Brightness
    */
    //% blockId="wemakebit_ledPanelSetBrightness" block="LED Matrix|%pinNum|set brightness|%Bright"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ledPanelSetBrightness
    export function ledPanelSetBrightness(pinNum: wPorts, Bright:number): void {
        return;
    }
    /**
     * LEDMatrix Display Clear Screen
    */
    //% blockId="wemakebit_ledPanelClearScreen" block="LED Matrix|%pinNum|clear screen"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ledPanelClearScreen
    export function ledPanelClearScreen(pinNum: wPorts): void {
        return;
    }

    /**
     * LEDMatrix Display Show Clock
    */
    //% blockId="wemakebit_ledPanelShowClock" block="LED Matrix|%pinNum|show Clock|%hour|:|%minute"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ledPanelShowClock
    export function ledPanelShowClock(pinNum: wPorts, hour:number, minute:number): void {
        return;
    }

    /**
     * LEDMatrix Display Show Number
    */
    //% blockId="wemakebit_ledPanelShowNum" block="LED Matrix|%pinNum|show number|%value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ledPanelShowNum
    export function ledPanelShowNum(pinNum: wPorts, value:number): void {
        return;
    }
    /**
     * LEDMatrix Display Show on Dot
    */
    //% blockId="wemakebit_ledPanelTurnOnDot" block="LED Matrix|%pinNum|show pixel x:|%x|y:|%y"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ledPanelTurnOnDot
    export function ledPanelTurnOnDot(pinNum: wPorts, x:number,y:number): void {
        return;
    }
    /**
     * LEDMatrix Display hide Dot
    */
    //% blockId="wemakebit_ledPanelTurnOffDot" block="LED Matrix|%pinNum|hide pixel x:|%x|y:|%y"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ledPanelTurnOffDot
    export function ledPanelTurnOffDot(pinNum: wPorts, x:number,y:number): void {
        return;
    }
    /**
     * ultrasonic get Distance value
    */
    //% blockId="elfshield_ultrasonicDistanceCm" block="ultrasonic Distance (Cm)|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::ultrasonicDistanceCm
    export function ultrasonicDistanceCm(pinNum: wPorts): number {
        return 0;
    }
    /**
     * Humiture Sensor read value
    */
    //% blockId="wemakebit_humitureSensorRead" block="Humiture Sensor|%pinNum| |%Humiture| Value"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::humitureSensorRead
    export function humitureSensorRead(pinNum: wPorts, index: Humiture): number {
        return 0;
    }
    /**
     * Color Sensor set Light
     
    //% blockId="wemakebit_colorSensorSetLight" block="color Sensor|%pinNum| set light |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::colorSensorSetLight
    export function colorSensorSetLight(pinNum: wPorts, isOn: swStatus): void {
        return;
    }
    */
    /**
     * color Sensor White Balance
    
    //% blockId="wemakebit_colorSensorWhiteBalance" block="color Sensor|%pinNum| set White Balance"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::colorSensorWhiteBalance
    export function colorSensorWhiteBalance(pinNum: wPorts): void {
        return;
    }
    */
    /**
     * Color Sensor read value
    
    //% blockId="wemakebit_colorSensorReadValue" block="color Sensor|%pinNum| |%Colors"
    //% weight=22
    //% blockGap=10
    //% shim=wemakebit::colorSensorReadValue
    export function colorSensorReadValue(pinNum: wPorts, type: Colors): number {
        return 0;
    }
    */
}
