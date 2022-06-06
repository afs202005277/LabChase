// KBC commands: 
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60

// KBC ports:
#define KBC_OUT_BUF 0x60
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_IN_BUF 0x60

// KBC bit masks:
#define KBC_PAR_ERR BIT(7) // Parity error
#define KBC_ST_IBF BIT(1)  // IBF bit set (IBF full)
#define KBC_OUT_FULL BIT(0) // OBF bit set (OBF full => data available for reading)
#define KBC_TO_ERR BIT(6)   // Time out error
#define KBC_AUX BIT(5)      // Aux bit (set if there is input from mouse)

#define MOUSE_IRQ 12

#define MOUSE_DELAY 20000
#define KEYBOARD_DELAY 5000

// Mouse commands:
#define DISABLE_DATA_REPORT 0xF5
#define ENABLE_DATA_REPORT 0xF4
#define SET_STREAM_MODE 0xEA

// Mouse answers:
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

#define WRITE_TO_MOUSE 0xD4
