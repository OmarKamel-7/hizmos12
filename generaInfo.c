/*

Chapter 1; 

  Line 55 : Hizmos journey summary until (13th on june 2026)
  Line () : Hizmos User Manual
  Line () : Hizmos Bill of material

Chapter 2;

  Line (314) : Introduction into Embedded systems
  Line (329) : GPIOS & Analog vs digital
  Line (345) : Processors Types (Cisc Vs Risc , Arm Vs x86 , Risc-v vs xtensa lx)
  Line (365) : Communication protocols
  Line (383) : Introduction Into Registers & Memory


chapter 3;

  Line (): Rf fundemntals
  Line (): 2.4ghz protocols and channels
  Line (): RF modulation
  Line (): Sub-ghz (315/433/868/938) Uses 


chapter 4;

  Line (): RFID freqs
  Line (): Nfc Types
  Line (): Nfc Emulation

chapter 5;
  Line (): Wifi fundmentals
  Line (): Wifi Frames and packets
  Line (): Wifi Attacks


chapter 6;
  Line (): Bluetooth Fundmentals
  Line (): Bluetooth classic vs Ble
  Line (): Bluetooth advertising













HIZMOS started as a simple idea: build an open-source hardware hacking and STEM platform inspired by devices like Flipper Zero, but more affordable, more powerful, and fully customizable for education, research, and embedded development.

Over time, it evolved into something much larger than expected—a full ecosystem combining RF analysis, wireless experimentation, hardware tools, and embedded software development in one device.

The motivation was simple: in many regions, including mine, hardware tools are expensive, limited, or difficult to access. HIZMOS was built to remove that barrier and give students, makers, and hardware enthusiasts a practical platform to learn, experiment, and create.

Challenges Along the Way

Hardware Availability (Egypt)
One of the biggest challenges has been sourcing components. Many RF modules, SDR tools, and advanced chips are not locally available. Importing them increases cost, adds long delays, and often involves strict customs processes, slowing down prototyping.

Complex Hardware Design (SPI & Integration)
The hardware design became increasingly complex, combining ESP32-S3, dual CC1101 sub-GHz modules, NRF24 2.4GHz modules, SD card storage, and expansion interfaces—all sharing SPI buses. This required multiple PCB revisions and careful optimization.

Firmware Complexity
As features grew, firmware became harder to manage. Every new module required careful integration to avoid conflicts between subsystems, along with continuous debugging, driver development, and optimization.

Low-Level Drivers
Working with hardware like ST25R3916, CC1101, and BLE stacks required deep low-level driver development. Many components needed custom implementations and careful register-level control.

Motivation Community Impact

Seeing people use HIZMOS, fork the project, or share feedback has been one of the strongest motivations. The open-source and maker community turned this from a personal project into something meaningful.

Industry Support

Support from companies made a real difference:

NextPCB sponsored 10 professional PCB prototypes, accelerating hardware iteration. ELECHOUSE provided NFC modules like PN5180 and ST25R3916, enabling advanced NFC experimentation.

These contributions helped transform HIZMOS from concept into real working hardware.

Current State

HIZMOS is now a working platform with both software and hardware features:

Infrared (read, save, replay; universal remote in progress) BadUSB (HID emulation, payloads, script execution) NRF / 2.4GHz tools (analysis, jamming, channel testing) File manager (browse, rename, delete) Mini apps and system tools Sub-GHz RF tools (scan, analyze, capture, replay) Settings system (SD card tools, device info, version, restart)

Some systems are still in development, including NFC drivers, GPIO expansion tools, BLE features, and full Wi-Fi attack modules.

Planned Expansion

HIZMOS is moving toward a multi-core hardware ecosystem:

Advanced NFC (ST25R3916) Read / write / emulate NFC Advanced NFC research tools 5GHz Wi-Fi (ESP32-C5) 5GHz scanning and analysis Dual-band wireless research (2.4 + 5GHz) Wardriving capabilities SBC Integration (Raspberry Pi Zero 2 W)

A companion compute unit for:

Packet capture Network analysis Advanced processing Professional-grade tooling FPGA Coprocessor (Sipeed Tang Nano 20K)

Used for real-time hardware acceleration:

Spectrum processing Signal classification Pulse decoding Fast protocol analysis (OOK / FSK) SDR Integration

For:

Sub-GHz debugging RF signal analysis Wideband research and experimentation Funding Needs

To continue development, HIZMOS requires funding for:

ST25R3916 NFC integration ESP32-C5 5GHz Wi-Fi development CC1101 and Ebyte RF modules (rare locally) Raspberry Pi Zero 2 W for SBC layer Sipeed Tang Nano FPGA board RTL-SDR for RF debugging and analysis Future PCB 2.0 after full prototyping phase

PCB production will only begin after full hardware validation.

Constraints

Hardware development in Egypt is significantly limited due to availability issues and strict import/customs procedures. Many essential components—especially SDRs, FPGA boards, and advanced RF modules—are difficult to obtain quickly, which slows down prototyping and iteration.

Funding would directly help overcome these barriers and accelerate development.

Vision

HIZMOS aims to become an affordable, powerful, open-source platform for RF analysis, embedded systems development, and cybersecurity education—built not just as a tool, but as a complete learning ecosystem for future hardware engineers.






this file has a summary for hizmos journey







# HIZMOS Journey

It's been a long time since I started working on HIZMOS. 
The original goal was simple: create a hardware hacking and STEM device inspired by devices like Flipper Zero,
but at a much lower price and with more powerful hardware and having every single feature i have dreamed for

## Why I Started HIZMOS

I wanted to create a device that students, makers, and hardware enthusiasts could actually get.
many existing devices are expensive or difficult to obtain in my region,
so I started designing a platform that could combine wireless experimentation
, hardware development, electronics tools, and educational features into a single device.
hizmos started as a very simple project but now
it is a very unique and cool device combining the powerful software, hardware affordabilty and opensource 


## Four Major Challenges

### 1. Sourcing Components in Egypt

One of the biggest challenges was finding the required components and modules. 
Some parts were difficult to obtain locally,
getting the ultimate components and tools to make the best tool ever was a real problem
while importing parts increased costs and delivery times and also the customs clearance was a huge problem



### 2. SPI busses config

Combining an ESP32-S3,2x CC1101 (sub-ghz), 2x nrf24 2.4ghz , Sd card, and expansion interfaces 
all of those works on spi bus required many design revisions and consumed alot of effort 
to get the pcb design right from the first time

### 3. features implemntation

Building firmware for multiple features at the same time was difficult.
Every new capability required testing and a certain config so it wouldn't conflict with any other feature or hardware.
debugging, drivers and libs editing, and optimization to make everything work together reliably.

### 4. drivers 
every time i need to add a new feature or config something in 
the hardware you have a problem with a very low level hardware driver , datasheets and registers 
dealing with stuff like the st25r3916, cc1101 , ble tools
all of those needs a custom drivers to do the function for hizmos and now we are working on them



## What Motivated Me

### 1. The Maker and Open-Source Community

Every time i see somebody trying hizmos or making a video about it it was wonderful for me 
everytime somebody forks hizmos or comment i think a idid something great and appreciatable 

### 2. Industry Support and Sponsorship

NextPCB sponsored the manufacturing of 10 double-layer PCB prototypes,
allowing me to test, iterate, and improve the hardware design much faster than would have been possible otherwise.
Having access to professionally manufactured PCBs was a significant step toward turning HIZMOS from a concept 
into real hardware.

ELECHOUSE sponsored both the PN5180 and ST25R3916 NFC modules.
These are among the most advanced NFC MODULES available for makers and embedded developers
Access to these modules enabled me to explore advanced NFC functionality and integrate professional-grade NFC features into HIZMOS.


## Where HIZMOS Is Today:



Today, HIZMOS has evolved from an idea into a working platform. Multiple hardware and software components have been designed, tested, and improved over time.

## Current Features
📡 Infrared (IR)

❌ Universal Remotes (Work in progress)

✅ Read Remotes

✅ Save Remotes

💻 BadUSB

✅ Demo Payload

✅ Keyboard Emulation

✅ HID Script Execution

📶 NRF Tools

✅ Signal Analyzer

❌ Hijack Nerf Devices (Work in progress)

✅ 2.4GHz Jammer

✅ Jammer (Specific Channel)

🗂️ File Manager

✅ Browse Files

✅ Rename Files

✅ Delete Files

🎮 Mini Apps

✅ Basic Flipper-like Apps

⚙️ Settings

✅ Show Usage

✅know the software version 

✅ Format SD Card

✅ Restart Device

❌ Battery Info (Work in progress)

✅ SD Card Info

✅ About

✅ Check Connected Devices

📶 Sub-GHz

✅read raw

✅spectrum graph

✅freq analyzer
 
✅jammer

✅modulation config


🧩 Not Yet Implemented

🔲 NFC (waiting for Drivers Dev)

🔲 GPIO Tools

🔲 Wi-Fi Tools (only the attacks left)

🔲 BLE Tools



## Looking Forward

The project is still evolving. Future work includes improving hardware reliability,
expanding software capabilities, refining the user interface,
and growing the open-source ecosystem around HIZMOS.

HIZMOS started as an idea for a cheaper alternative,
but it has grown into a much larger learning experience involving hardware design, 
embedded programming, problem solving, and product development.









# Introduction into Embedded Systems

An embedded system is a computer designed to perform a specific task.
Unlike desktop computers, embedded systems are usually dedicated to one function.
They are found in everyday devices such as microwaves, routers, cars, and smart watches.
Most embedded systems contain a microcontroller or microprocessor.
The software is usually stored in Flash memory.
Many embedded systems operate in real time.
This means they must respond to events within a predictable period.
Embedded systems are often optimized for low power consumption.
They are designed to be reliable and operate for long periods.
Common examples include ESP32, STM32, AVR, and Raspberry Pi Pico boards.
Embedded systems are the foundation of modern electronics.
Understanding them is the first step toward hardware development.

# GPIOs & Analog vs Digital

GPIO stands for General Purpose Input Output.
GPIO pins allow a microcontroller to interact with external hardware.
Pins can be configured as inputs or outputs.
Input pins read signals from sensors, switches, and buttons.
Output pins control LEDs, displays, relays, and other devices.
Digital signals have only two states: HIGH and LOW.
A HIGH signal typically represents logic 1.
A LOW signal typically represents logic 0.
Analog signals can have many voltage levels.
Examples of analog signals include microphones and temperature sensors.
Microcontrollers use ADCs to convert analog voltages into digital values.
DACs perform the opposite operation and generate analog signals.
Understanding GPIOs is essential for connecting hardware to software.

# Processor Types (CISC vs RISC, ARM vs x86, RISC-V vs Xtensa LX)

Processors execute instructions stored in memory.
Different processor architectures use different instruction sets.
CISC stands for Complex Instruction Set Computer.
CISC processors use many powerful instructions.
x86 processors are the most common CISC architecture.
They are widely used in desktop and laptop computers.
RISC stands for Reduced Instruction Set Computer.
RISC processors use fewer and simpler instructions.
This often improves efficiency and power consumption.
ARM is the most popular RISC architecture today.
Most smartphones use ARM-based processors.
RISC-V is a modern open-source instruction set architecture.
Anyone can design a RISC-V processor without licensing fees.
Xtensa LX is a configurable architecture developed by Cadence.
ESP32 chips use Xtensa LX processor cores.
Each architecture has strengths depending on the application.
Low-power devices often favor ARM, RISC-V, or Xtensa designs.

# Communication Protocols

Electronic devices communicate using protocols.
A protocol defines how data is transmitted and received.
UART is one of the simplest communication protocols.
It uses TX and RX lines for serial communication.
UART is commonly used for debugging and programming.
SPI is a fast communication protocol.
It uses MOSI, MISO, SCK, and CS signals.
Displays, RF modules, and memory chips often use SPI.
I2C is another popular protocol.
It requires only two wires: SDA and SCL.
Multiple devices can share the same I2C bus.
CAN is designed for reliable communication in vehicles.
USB supports high-speed communication and power delivery.
Bluetooth and Wi-Fi are wireless communication protocols.
Choosing the correct protocol depends on speed, distance, and complexity requirements.

# Introduction Into Registers & Memory

Registers are very small storage locations inside the processor.
They are the fastest memory available in a system.
The CPU constantly uses registers during program execution.
Memory stores both data and program instructions.
RAM is temporary memory used while the system is running.
Data in RAM is lost when power is removed.
Flash memory stores firmware permanently.
The program code of a microcontroller is usually stored in Flash.
EEPROM is used for storing small amounts of persistent data.
Each memory location has a unique address.
The processor reads and writes data using these addresses.
Understanding registers and memory is essential for low-level programming.
They form the bridge between software and hardware operation.







# RF Fundamentals

RF stands for Radio Frequency.
RF communication uses electromagnetic waves to transfer information wirelessly.
These waves travel through the air at the speed of light.
RF technology is used in Wi-Fi, Bluetooth, cellular networks, RFID, and satellites.
Every RF signal operates at a specific frequency.
Frequency is measured in Hertz (Hz).
One kilohertz equals 1,000 Hz.
One megahertz equals 1,000,000 Hz.
One gigahertz equals 1,000,000,000 Hz.
Higher frequencies can usually carry more data.
Lower frequencies often travel farther and penetrate obstacles better.
The strength of a radio signal is measured in decibels (dB).
Antennas are used to transmit and receive RF signals.
Different antenna designs provide different coverage patterns.
RF systems are affected by interference and noise.
Metal objects can reflect radio waves.
Walls and obstacles can weaken signals.
The study of radio communication is known as RF engineering.
Understanding RF fundamentals is essential for wireless technology.

# 2.4GHz Protocols and Channels

The 2.4GHz band is one of the most widely used radio bands.
It belongs to the ISM (Industrial, Scientific, and Medical) spectrum.
Most countries allow its use without a license.
Wi-Fi commonly operates in the 2.4GHz band.
Bluetooth also uses the same frequency range.
Many IoT devices communicate using 2.4GHz radios.
Examples include ESP32, NRF24L01, and Zigbee devices.
The band is divided into multiple channels.
Wi-Fi channels are spaced closely together.
Some channels overlap with neighboring channels.
Channels 1, 6, and 11 are commonly used because they do not overlap significantly.
Bluetooth uses frequency hopping across many channels.
Frequency hopping helps reduce interference.
Crowded environments may contain hundreds of active devices.
Interference can reduce speed and reliability.
Spectrum analyzers help visualize channel activity.
Channel selection is important for optimal performance.
Proper channel management improves wireless communication quality.

# RF Modulation

Modulation is the process of placing information onto a radio wave.
Without modulation, data cannot be transmitted wirelessly.
A carrier wave acts as the signal that carries information.
The information modifies a property of the carrier wave.
Amplitude Modulation (AM) changes signal amplitude.
Frequency Modulation (FM) changes signal frequency.
Phase Modulation (PM) changes signal phase.
Modern digital systems use advanced modulation methods.
ASK stands for Amplitude Shift Keying.
FSK stands for Frequency Shift Keying.
PSK stands for Phase Shift Keying.
QPSK is a common form of PSK.
QAM combines amplitude and phase modulation.
Higher-order QAM can transmit more bits per symbol.
More complex modulation increases data rates.
Complex modulation often requires stronger signals.
Receivers demodulate the signal to recover data.
Modulation is one of the most important concepts in RF communication.

# Sub-GHz (315MHz, 433MHz, 868MHz, 915MHz) Uses

Sub-GHz refers to frequencies below 1GHz.
These frequencies are widely used for long-range communication.
Sub-GHz signals generally travel farther than 2.4GHz signals.
They also penetrate walls and obstacles more effectively.
315MHz is commonly used in remote control systems.
433MHz is popular for home automation devices.
Many wireless doorbells use 433MHz communication.
Weather stations often operate on 433MHz.
868MHz is widely used across Europe.
915MHz is commonly used in North America.
LoRa networks frequently use 868MHz and 915MHz bands.
Industrial monitoring systems often use Sub-GHz frequencies.
Agricultural sensors benefit from their long range.
Smart utility meters commonly use these bands.
Many security sensors communicate using Sub-GHz radios.
Data rates are usually lower than 2.4GHz systems.
The tradeoff is improved range and reliability.
Sub-GHz communication is a key technology for IoT applications.
Understanding these frequencies helps engineers choose the right wireless solution.



# RFID Frequencies

RFID stands for Radio Frequency Identification.
It is a technology used to identify objects wirelessly.
An RFID system consists of a reader and a tag.
Tags store data that can be read by compatible devices.
RFID operates on several frequency ranges.
Low Frequency (LF) RFID typically uses 125kHz.
LF tags are commonly found in access control systems.
They offer short range but good reliability.
High Frequency (HF) RFID operates at 13.56MHz.
Many smart cards use the HF band.
NFC technology is based on HF RFID.
Ultra High Frequency (UHF) RFID usually operates between 860MHz and 960MHz.
UHF systems provide much longer reading distances.
Warehouses and logistics companies often use UHF RFID.
Different frequencies offer different speed, range, and capabilities.
Choosing the correct frequency depends on the application.

# NFC Types

NFC stands for Near Field Communication.
It is a short-range wireless communication technology.
NFC operates at 13.56MHz.
Communication usually occurs within a few centimeters.
NFC devices can operate in different modes.
Reader/Writer mode allows a device to read NFC tags.
Most smartphones support Reader/Writer mode.
Card Emulation mode allows a device to act like a smart card.
Mobile payment systems commonly use this mode.
Peer-to-Peer mode allows two NFC devices to exchange data.
Android Beam was an example of Peer-to-Peer communication.
Several NFC standards exist.
NFC-A is based on ISO 14443 Type A.
NFC-B is based on ISO 14443 Type B.
NFC-F is based on the FeliCa system.
These standards differ in modulation and communication methods.
NFC is widely used for payments, transportation, and access control.

# NFC Emulation

NFC emulation allows a device to behave like an NFC tag or card.
The reader sees the device as if it were a real card.
This technology is known as Card Emulation.
Modern smartphones use NFC emulation for contactless payments.
Access control systems can also use card emulation.
The emulated device follows standard NFC communication rules.
Data is exchanged using the same protocols as physical cards.
Host Card Emulation (HCE) is a common implementation.
HCE allows software to emulate cards without secure hardware.
Emulation increases flexibility and convenience.
Developers can test NFC applications without physical tags.
Many transportation and payment systems rely on card emulation.
Security depends on the protocols and authentication mechanisms used.
NFC emulation is one of the most powerful features of modern NFC devices.
Understanding emulation helps explain how digital wallets function.





# Wi-Fi Fundamentals

Wi-Fi is a wireless networking technology used to connect devices.
It is based on the IEEE 802.11 family of standards.
Wi-Fi allows data transmission without physical cables.
Most Wi-Fi networks operate on the 2.4GHz and 5GHz bands.
Modern devices may also support the 6GHz band.
A Wi-Fi network usually consists of clients and an access point.
Clients include phones, laptops, and IoT devices.
The access point manages communication between devices.
Each Wi-Fi network is identified by an SSID.
Devices use radio waves to send and receive data.
Signal strength affects speed and reliability.
Walls and obstacles can weaken signals.
Interference from nearby devices may reduce performance.
Higher frequencies generally provide faster speeds.
Lower frequencies often provide better range.
Wi-Fi has become one of the most important communication technologies.
It powers homes, schools, businesses, and public networks.
Understanding Wi-Fi is essential for networking and cybersecurity.

# Wi-Fi Frames and Packets

Wi-Fi communication is divided into packets of data.
These packets are transmitted inside structures called frames.
Every frame contains information used by the network.
Frames help devices communicate efficiently.
There are three main frame categories.
Management frames control network discovery and connections.
Beacon frames announce the presence of a Wi-Fi network.
Probe frames help devices search for nearby networks.
Authentication frames establish trust between devices.
Control frames coordinate packet delivery.
Acknowledgment frames confirm successful transmission.
Request-To-Send and Clear-To-Send frames help avoid collisions.
Data frames carry actual user information.
Web pages, messages, and files travel in data frames.
Packet analyzers can capture and inspect Wi-Fi frames.
Understanding frame types is important for troubleshooting and analysis.
Frames form the foundation of Wi-Fi communication.

# Wi-Fi Attacks

Wireless networks can be targeted by various attacks.
Many attacks focus on weak security configurations.
Password guessing attempts may target weak Wi-Fi passwords.
Fake access points can imitate legitimate networks.
These are often called rogue access points.
Users may unknowingly connect to malicious networks.
Packet capture can reveal information about network activity.
Attackers sometimes attempt to force devices to reconnect.
This may increase opportunities for analysis or credential theft.
Misconfigured networks are often easier to attack.
Open networks provide little or no encryption.
Strong encryption significantly improves security.
WPA2 and WPA3 are commonly used security standards.
Regular updates help fix security vulnerabilities.
Strong passwords reduce unauthorized access risks.
User awareness is an important part of network security.
Understanding common attack methods helps improve defensive practices.
Secure configuration is the best protection against most Wi-Fi threats.








# Bluetooth Fundamentals

Bluetooth is a short-range wireless communication technology.
It was designed to replace cables between electronic devices.
Bluetooth operates in the 2.4GHz ISM frequency band.
The technology is used in billions of devices worldwide.
Common examples include headphones, keyboards, mice, and smart watches.
Bluetooth uses radio waves to exchange information.
Devices communicate through a process called pairing.
Once paired, devices can exchange data securely.
Bluetooth uses frequency hopping to reduce interference.
Frequency hopping means rapidly changing channels during communication.
This improves reliability in crowded radio environments.
Bluetooth is designed for low power consumption.
Different Bluetooth versions provide improvements in speed and range.
Modern smartphones include Bluetooth as a standard feature.
Many IoT devices rely on Bluetooth communication.
Understanding Bluetooth is important for wireless electronics development.
It is one of the most widely used wireless technologies today.

# Bluetooth Classic vs BLE

Bluetooth Classic was the original Bluetooth technology.
It is optimized for continuous data transmission.
Classic Bluetooth is commonly used for audio streaming.
Wireless headphones and speakers often use Bluetooth Classic.
It provides higher data throughput than BLE.
BLE stands for Bluetooth Low Energy.
BLE was introduced for battery-powered devices.
Its primary goal is reducing power consumption.
BLE devices can operate for months or years on small batteries.
Fitness trackers commonly use BLE communication.
Smart sensors and IoT devices often rely on BLE.
BLE sends small amounts of data efficiently.
Classic Bluetooth usually requires more power.
Both technologies operate in the 2.4GHz band.
Many modern devices support both Classic and BLE.
Choosing between them depends on power and bandwidth requirements.
BLE has become the dominant choice for IoT applications.

# Bluetooth Advertising

Advertising is a core feature of Bluetooth Low Energy.
BLE devices announce their presence using advertising packets.
These packets are broadcast without establishing a connection.
Nearby devices can detect advertisements automatically.
Advertising packets contain basic device information.
Examples include device names and service identifiers.
Advertising helps devices discover each other quickly.
BLE beacons use advertising to broadcast information.
Many indoor positioning systems rely on BLE advertisements.
Advertising can operate continuously or at intervals.
Shorter intervals improve discovery speed.
Longer intervals reduce power consumption.
A scanner listens for advertising packets in the area.
Devices can choose to connect after discovery.
Advertising is the first step in most BLE communications.
Understanding advertising is essential for BLE development.
It enables efficient, low-power wireless device discovery.


*/