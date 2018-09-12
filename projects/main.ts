basic.forever(() => {
    serial.writeNumber(elfshield.ultrasonicDistanceCm(elfshield.wPorts.PortA))
    basic.pause(100)
})
