# FollowFocusMotorControl
Controlling Zhiyun crane 2 follow focus motor with arduino

The Zhiyun crane 2 follow focus motors are cheap (at time of writing) and provide all the hardware you need to quietly drive a lens. They use a proprietary coms protocol that is similar to XModem.

This work shows a simple hack https://www.youtube.com/watch?v=plnVgDFxTQg, https://www.instructables.com/35-Wireless-Follow-Focus-From-the-Crane-2/  but provides no code so this is my attempt to do somehting similar.

Not in the above it suggests a different pinout to what I found worked.

For "reasons'' they have used a micro-USB connector to send serial data. D+ = Tx, D- =Rx. You also need to supply 5v as well otherwise it will power up but the motor won't turn. The port on the bottom of the motor marked "CRANE2" is one for communicating with it. The second port on the side is to connect a camera to to control other camera functions

When the motor turns on it waits for a valid packet, if none is received it will turn off again after a few seconds. Once it gets valid data it will continue to work so long as the data keeps coming (about one packet every 20ms). If the data stops for too long then the motor will shut down.

The protocol appears to be XModem inspired.

{0xA5, 0x5A, 0x0A, 0xFF, 0x0F, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x49} is the base packet we work with in this. not all fields are understood. The first 2 bytes are a packet number and its checksum i think. Then the next byte I suspect is the packet type. The last 2 bytes are the CRC in Xmodem format (calculated from the rest of the packet). Bytes 9 and 10 (zero indexed) are the ones that appear to be important for this application. I suspect that the rest of the fields are for camera control through the other usb port. Byte 9 is some form of quadrature data and only takes the values 0x00, 0x40, 0x80 or 0xC0 but I can't work out what it adds to anything so as yet I haven't used it.

Byte 10 is the one I have used. It's a bit odd the way it works though. All movements are relative and can be a value of +/127. One tooth on the output is an offset of 17 (17!!!) 
As this thing needs a constant stream of packets then every packet contains a 1 byte value for this field. You change that value by the amount you want the motor to move (roll over at the 255/0 boundary)

The motors use an STM32F411 as the main micro but it has been set to Read protection level 1 so you can't read the firmware out or replace it using the SWD port. Thus a simple firmware rewrite would be non-trivial. Though there is a firmware update system that works similarly to mbed where it enumerates as a disk drive into which you plop the new firmware file, but I suspect it's going to be compressed/encrypted/signed.

Inside the motor the following are the major features for anyone interested in writing their own firmware:
The motor is run from a MP6536 with sense resistors on 2 phases going to a dual opamp to ADC_IN0 and ADC_IN1 so it can work out power usage or torque. This is controlled by TIMER3_CH2, 3 and 4. Position feedback is with the Infineon TLI5012B E1000 magnetic encoder of the back of the drive motor. The USB-OTG port is connected to the side port to connect to host a camera for control and when the switch is in the upgrade position, this becomes a USB device exposing a serial port and a drive. USART1_TX and USART1_RX pins are connected to the other usb port to provide comms. There is a MP2639A for the 2 cell lithium battery.

Even though it has the hardware to detect a stall of the output gear there doesn’t appear to be any indication that does anything to alert the controller or user in the data it sends back. .

