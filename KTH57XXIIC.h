#ifndef __KTH57XXIIC_H
#define __KTH57XXIIC_H
#include "py32f072xx_Start_Kit.h"

#define		CMD_DutyCycleMode       0x20
#define		CMD_SINGLE_MEASUREMENT  0x30
#define		CMD_READ_MEASUREMENT    0x40
#define		CMD_READ_REGISTER       0x50
#define		CMD_WRITE_REGISTER      0x60
#define		CMD_EXIT_MODE           0x80
#define		CMD_MEMORY_RECALL       0xd0
#define		CMD_MEMORY_STORE        0xe0
#define		CMD_RESET               0xf0


#define ZERO_ANGLE_FLASH_ADDR     0x0800F000


uint8_t ExitMode(void);
uint8_t reset(void);
uint8_t singleMeasurementMode(uint8_t axis);
uint8_t DutyCycleMode(uint8_t axis);
uint8_t writeRegister(uint8_t address, uint16_t data);
uint8_t readRegister(uint8_t address);
uint8_t readMeasurement(uint8_t axis);
void    RegInitial(void);
void sgdKT5763Init(void);
void sgdReadAngle(void);
int sgdGetAngle(void);

int sgdIsKT5763Online(void);
int sgdSetZeroAngle(int min);
void sgdResetZeroAngle(void);




#endif
