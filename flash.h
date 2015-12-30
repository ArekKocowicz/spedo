#include <xc.h>
#include <stdint.h>
#ifdef MODULE_FLASH

void my_flash_unlock(void);

unsigned int my_flash_read(uint16_t addr);
void my_flash_erase(uint16_t addr);
void my_flash_write(uint16_t addr, uint16_t * data, uint8_t len);

#endif


#ifndef MODULE_FLASH

extern unsigned int my_flash_read(uint16_t addr);
extern void my_flash_erase(uint16_t addr);
extern void my_flash_write(uint16_t addr, uint16_t * data, uint8_t len);

#endif