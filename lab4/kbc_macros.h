// KBC commands: 
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60

// KBC ports:
#define KBC_OUT_BUF 0x60
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64

// KBC bit masks:
#define KBC_PAR_ERR BIT(7) // Parity error
#define KBC_ST_IBF BIT(1)  // IBF bit set (IBF full)
#define KBC_OUT_FULL BIT(0) // OBF bit set (OBF full => data available for reading)
#define KBC_TO_ERR BIT(6)   // Time out error
#define KBC_AUX BIT(5)      // Aux bit (set if there is input from mouse)

#define MOUSE_IRQ 12
