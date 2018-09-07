/*
load dependency
"elfshield": "file:../elfshield.ts"
*/

//% color=#f44242 weight=10 icon="\uf1d0"
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
        S1 = 0x01,
        S2 = 0x02,
    }

    export enum Motors {
        M1 = 0x1,
        M2 = 0x2,
    }
    export enum wPorts {
        PortA = 1,
        PortB = 2,
        PortC = 3,
        PortD = 4
    }
    export enum touchMode {
        Mode0 = 1,
        Mode1 = 2
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
        //light = 0,
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
        setPwm((index - 1), 0, 0);
        setPwm((index - 1) + 1, 0, 0);
    }
    /**
     * Init RGB pixels mounted on elfshield
     */
    //% blockId="elfshield_rgb" block="RGB"
    //% weight=5
    //% blockGap=10
    export function rgb(): neopixel.Strip {
        if (!neoStrip) {
            neoStrip = neopixel.create(DigitalPin.P8, 5, NeoPixelMode.RGB)
        }
        return neoStrip;
    }
    /**
     * get mic analog value.
     */
    //% blockId="elfshield_mic" block="Volume"
    //% weight=5
    //% blockGap=10
    export function getMicValue(): number {
        let value = 0;
        value = pins.analogReadPin(AnalogPin.P2);
        return value;
    }
    /**
     * Servo Execute
     * @param index Servo Channel; eg: S1
     * @param degree [0-180] degree of servo; eg: 0, 90, 180
    */
    //% blockId=robotbit_servo block="Servo|%index|degree %degree"
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
    //% blockId=robotbit_motor_run block="Motor|%index|speed %speed"
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
        let pp = (index - 1) 
        let pn = (index - 1) + 1
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
     * @param index Motor Index; eg: M1A, M1B, M2A, M2B
     * @param speed [-255-255] speed of motor; eg: 150, -150
     * @param delay seconde delay to stop; eg: 1
    */
    //% blockId=robotbit_motor_rundelay block="Motor|%index|speed %speed|delay %delay|s"
    //% weight=22
    //% blockGap=10
    //% speed.min=-255 speed.max=255
    //% name.fieldEditor="gridpicker" name.fieldOptions.columns=4
    export function MotorRunDelay(index: Motors, speed: number, delay: number): void {
        MotorRun(index, speed);
        basic.pause(delay * 1000);
        MotorRun(index, 0);
    }

    //% blockId=robotbit_stop block="Motor Stop|%index|"
    //% weight=22
    //% blockGap=10
    export function MotorStop(index: Motors): void {
        MotorRun(index, 0);
    }

    //% blockId=robotbit_stop_all block="Motor Stop All"
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
    //% blockId="elfshield_touchSetMode" block="touch |%pinNum| setMode |%mode"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::touchSetMode
    export function touchSetMode(pinNum: wPorts,mode: touchMode): void {
        return;
    }

    /**
    * get touch module status.
    */
    //% blockId="elfshield_getTouched" block="touched |%pinNum"
    //% weight=10
    //% blockGap=10
    //% shim=elfshield::getTouched
    export function getTouched(pinNum: wPorts): number {
        return 0;
    }

    /**
     * Water Atomizer Moduel set start
    */
    //% blockId="elfshield_waterAtomizerSet" block="water Atomizer |%pinNum| |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::waterAtomizerSet
    export function waterAtomizerSet(pinNum: wPorts, isOn: swStatus): void {
        return;
    }

    /**
     * 4 LED Button set LED
    */
    //% blockId="elfshield_LED4ButtonSetLed" block="LED Button|%pinNum| set led |%KeyButtons| |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::LED4ButtonSetLed
    export function LED4ButtonSetLed(pinNum: wPorts,index: KeyButtons, isOn: swStatus): void {
        return;
    }

    /**
     * 4 LED Button read Key
    */
    //% blockId="elfshield_LED4ButtonReadKey" block="LED Button|%pinNum| |%KeyButtons| Pressed"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::LED4ButtonReadKey
    export function LED4ButtonReadKey(pinNum: wPorts, key: KeyButtons): number {
        return 0;
    }

    /**
     * Sliding Potentiometer read value
    */
    //% blockId="elfshield_slidingPotentiometerReadValue" block="sliding Potentiometer|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::slidingPotentiometerReadValue
    export function slidingPotentiometerReadValue(pinNum: wPorts): number {
        return 0;
    }
    /**
     * Potentiometer read value
    */
    //% blockId="elfshield_PotentiometerReadValue" block="Potentiometer|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::PotentiometerReadValue
    export function PotentiometerReadValue(pinNum: wPorts): number {
        return 0;
    }
    /**
     * Gas Sensor MQ2 read value
    */
    //% blockId="elfshield_gasSensorReadValue" block="Gas Sensor|%pinNum| value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::gasSensorReadValue
    export function gasSensorReadValue(pinNum: wPorts): number {
        return 0;
    }
    /**
     * get PIR Motion Sensor
    */
    //% blockId="elfshield_getPIRMotion" block="getPIRMotion|%pinNum|"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::getPIRMotion
    export function getPIRMotion(pinNum: wPorts): number {
        return 0;
    }
    /**
     * Joystick Module read value
    */
    //% blockId="elfshield_JoystickReadValue" block="Joystick|%pinNum| get|%JSAxis| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::JoystickReadValue
    export function JoystickReadValue(pinNum: wPorts, axis: JSAxis): number {
        return 0;
    }
    /**
     * Line Follower Sensor read value
    */
    //% blockId="elfshield_lineFollowerRead" block="Line Follower|%pinNum| |%Lines| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::lineFollowerRead
    export function lineFollowerRead(pinNum: wPorts, index: Lines): number {
        return 0;
    }
    /**
     * Tilt Switch read value
    */
    //% blockId="elfshield_tiltSwitchRead" block="Tilt senosr|%pinNum| |%Lines| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::tiltSwitchRead
    export function tiltSwitchRead(pinNum: wPorts, index: Lines): number {
        return 0;
    }
    /**
     * Flame Sensor read value
    */
    //% blockId="elfshield_flameSensorRead" block="Flame senosr|%pinNum| |%Flames| Value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::flameSensorRead
    export function flameSensorRead(pinNum: wPorts, index: Flames): number {
        return 0;
    }
    /**
     * 4-Digital LED module Show Number
    */
    //% blockId="elfshield_segmentDisplayShowNumber" block="Segment Display|%pinNum| show Number %value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::segmentDisplayShowNumber
    export function segmentDisplayShowNumber(pinNum: wPorts, value: number): void {
        return;
    }
    /**
     * 4-Digital LED module Show Char
    
    //% blockId="elfshield_segmentDisplayShowChar" block="Segment Display|%pinNum| |%Addr| show Char %value"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::segmentDisplayShowChar
    export function segmentDisplayShowChar(pinNum: wPorts, Addr: number, Data: number): void {
        return;
    }
    */
    /**
     * Single LED set Light
    */
    //% blockId="elfshield_singleLEDSet" block="single LED|%pinNum| set |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::singleLEDSet
    export function singleLEDSet(pinNum: wPorts, isOn: swStatus): void {
        return;
    }
    /**
     * Relay Module set switch
    */
    //% blockId="elfshield_RelaySet" block="Relay|%pinNum| set |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::RelaySet
    export function RelaySet(pinNum: wPorts, isOn: swStatus): void {
        return;
    }
    /**
     * DC 130 Motor run speed
    */
    //% blockId="elfshield_DC130MotorRunSpeed" block="130 DC Motor|%pinNum| runSpeed |%speed"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::DC130MotorRunSpeed
    export function DC130MotorRunSpeed(pinNum: wPorts, speed: number): void {
        return;
    }
    /**
     * Color Sensor set Light
    */
    //% blockId="elfshield_colorSensorSetLight" block="color Sensor|%pinNum| set light |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::colorSensorSetLight
    export function colorSensorSetLight(pinNum: wPorts, isOn: swStatus): void {
        return;
    }
    /**
     * color Sensor White Balance
    */
    //% blockId="elfshield_colorSensorWhiteBalance" block="color Sensor|%pinNum| set White Balance"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::colorSensorWhiteBalance
    export function colorSensorWhiteBalance(pinNum: wPorts): void {
        return;
    }
    /**
     * Color Sensor read value
    */
    //% blockId="elfshield_colorSensorReadValue" block="color Sensor|%pinNum| |%Colors"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::colorSensorReadValue
    export function colorSensorReadValue(pinNum: wPorts, type: Colors): number {
        return 0;
    }
    /**
     * MP3 Player Set music
    */
    //% blockId="elfshield_MP3AppointMusic" block="MP3|%pinNum| set music|%number"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3AppointMusic
    export function MP3AppointMusic(pinNum: wPorts, num: number): void {
        return;
    }
    /**
     * MP3 Player Set Volume
    */
    //% blockId="elfshield_MP3AppointVolume" block="MP3|%pinNum| set volmue|%number"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3AppointVolume
    export function MP3AppointVolume(pinNum: wPorts, num: number): void {
        return;
    }
    /**
     * MP3 Player Set Next Music
    */
    //% blockId="elfshield_MP3NextMusic" block="MP3|%pinNum| next music"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3NextMusic
    export function MP3NextMusic(pinNum: wPorts): void {
        return;
    }
    /**
     * MP3 Player Set Pause
    */
    //% blockId="elfshield_MP3Pause" block="MP3|%pinNum| pause"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3Pause
    export function MP3Pause(pinNum: wPorts): void {
        return;
    }
    /**
     * MP3 Player Set play
    */
    //% blockId="elfshield_MP3Play" block="MP3|%pinNum| play"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3Play
    export function MP3Play(pinNum: wPorts): void {
        return;
    }
    /**
     * MP3 Player Set appoint device
    */
    //% blockId="elfshield_MP3AppointDevice" block="MP3|%pinNum| set device|%MP3Device"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3AppointDevice
    export function MP3AppointDevice(pinNum: wPorts, devtype: MP3Device): void {
        return;
    }

    /**
     * MP3 Player is Over
    */
    //% blockId="elfshield_MP3isOver" block="MP3|%pinNum| is over"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3isOver
    export function MP3isOver(pinNum: wPorts): number {
        return 0;
    }
    /**
     * MP3 Player Set Prev Music
    */
    //% blockId="elfshield_MP3PrevMusic" block="MP3|%pinNum| prev music"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::MP3PrevMusic
    export function MP3PrevMusic(pinNum: wPorts): void {
        return;
    }
    /**
     * LEDMatrix Display Set Brightness
    */
    //% blockId="elfshield_ledPanelSetBrightness" block="LED Matrix|%pinNum|set brightness|%Bright"
    //% weight=16
    //% blockGap=10
    //% shim=elfshield::ledPanelSetBrightness
    export function ledPanelSetBrightness(pinNum: wPorts, Bright:number): void {
        return;
    }
    /**
     * LEDMatrix Display Clear Screen
    */
    //% blockId="elfshield_ledPanelClearScreen" block="LED Matrix|%pinNum|clear screen"
    //% weight=16
    //% blockGap=10
    //% shim=elfshield::ledPanelClearScreen
    export function ledPanelClearScreen(pinNum: wPorts): void {
        return;
    }

    /**
     * LEDMatrix Display Show Clock
    */
    //% blockId="elfshield_ledPanelShowClock" block="LED Matrix|%pinNum|show time|%hour|:|%minute"
    //% weight=16
    //% blockGap=10
    //% shim=elfshield::ledPanelShowClock
    export function ledPanelShowClock(pinNum: wPorts, hour:number, minute:number): void {
        return;
    }

    /**
     * LEDMatrix Display Show Number
    */
    //% blockId="elfshield_ledPanelShowNum" block="LED Matrix|%pinNum|show number|%value"
    //% weight=16
    //% blockGap=10
    //% shim=elfshield::ledPanelShowNum
    export function ledPanelShowNum(pinNum: wPorts, value:number): void {
        return;
    }
    /**
     * LEDMatrix Display Show on Dot
    */
    //% blockId="elfshield_ledPanelTurnOnDot" block="LED Matrix|%pinNum|show pixel x:|%x|y:|%y"
    //% weight=16
    //% blockGap=10
    //% shim=elfshield::ledPanelTurnOnDot
    export function ledPanelTurnOnDot(pinNum: wPorts, x:number,y:number): void {
        return;
    }
    /**
     * LEDMatrix Display hide Dot
    */
    //% blockId="elfshield_ledPanelTurnOffDot" block="LED Matrix|%pinNum|hide pixel x:|%x|y:|%y"
    //% weight=16
    //% blockGap=10
    //% shim=elfshield::ledPanelTurnOffDot
    export function ledPanelTurnOffDot(pinNum: wPorts, x:number,y:number): void {
        return;
    }
    /**
     * Ultrasonic Sensor set LED
    
    //% blockId="elfshield_ultrasonicSetLed" block="ultrasonic Sensor|%pinNum| set Yellow led |%ultrasonicLED| |%swStatus"
    //% weight=22
    //% blockGap=10
    //% shim=elfshield::ultrasonicSetLed
    export function ultrasonicSetLed(pinNum: wPorts,index: ultrasonicLED, isOn: swStatus): void {
        return;
    }
    */
}
