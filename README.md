# 7-seg-quick-tester
Simple to use, quick and reliable tester of 4-element 7-segment (+dot) displays. 

![7-seg-tester concept](https://github.com/mijanik/7-seg-tester-Project/blob/main/7-seg-tester-photo.jpg)

## Purpose of the project
We are creating this project to allow quick testing of 7-seg displays used (and probably broken) by other students.

## Parts being used
For this project, we are using: 

- [x] Freescale Freedom Development Platform - Kinetis FRDM-KL05Z
- [x] 3D-printed Pyramid, well, base and pry tool
- [x] Adafruit ALS-PT19 Phototransistor
- [x] 3 Logic voltage coverters (3.3V to 5V)
- [x] LCD display 2x16
- [x] Red and green LED diodes
- [x] PS10BRD button
- [x] USB type-B breakout and plug
- [x] Screws and PCB standoffs

## Microcontroller functionality used
- Finite State Machine and SysTick for switching the test routine for each segment
- Analog-Digital Converter to read voltage value from phototransistor
- UART to send test information to PC
- I2C to handle LCD display
- Interrupts

## Description

At the end of the semester, it is necessary to test a large number of 7-segment displays returned by students to laboratories. However, manually checking each of their segments, e.g. using a multimeter, would require a lot of work and would be prone to a validation error. Hence the idea to construct an automatic testing device that would speed up this process. The main assumptions were ease of use and speed of operation.
The tester has been enclosed in a custom-made housing made with the use of 3D printing technology. The characteristic shape of the truncated pyramid ensures a strong structure and convenient location of the most important part - the recess for the tested display, located on the top of the device. It is a small darkroom with a light intensity sensor module with a phototransistor at the bottom. By placing a display in it, we provide all electrical connections at the same time. Its walls are additionally covered with a reflective foil to facilitate detection.
On the front of the housing there is a blue LCD display informing about the currently performed activity, one button for operating the entire device, red and green LED diode indicating the test result and a USB socket for connecting to a computer or the power supply itself. The push-in base is easy to detach for easy access to the inside of the housing. There, in addition to a large number of connecting cables, there is the heart of the tester - the FRDM-KL05Z development board and 3 bi-directional logic level converters, which made it possible to control the 7-segment display using a higher voltage of 5V. Thus, its brightness was increased during the test.
To learn more about the tested display, we can connect the tester to the computer and use the connection through the UART serial port. By default, the device sends a text representation of the tested display to the computer after each test, which makes it possible to check which segments are faulty.


## Project & Authors information

###### Authors:

Miłosz Janik & Antoni Kijania

###### Subjects:

- Technika Mikroprocesorowa 2
- Design Laboratory

###### Project supervisor:

mgr inż. Sebastian Koryciak
