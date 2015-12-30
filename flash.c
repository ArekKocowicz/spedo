#define MODULE_FLASH
#include "flash.h"

void my_flash_unlock(void)
{
    PMCON2 = 0x55;
	PMCON2 = 0xaa;
	WR=1;
	NOP();
	NOP();
}

unsigned int my_flash_read(uint16_t addr)
{
    PMADR=addr;
	CFGS=0;
    RD=1;
	NOP();
	NOP();
    return PMDAT; 
}

void my_flash_erase(uint16_t addr)
{
    PMADR=addr;
	CFGS=0;
    FREE=1;
    WREN=1;
    my_flash_unlock();
	WREN=0;
}

void my_flash_write(uint16_t addr, uint16_t * data, uint8_t len)
{
    uint8_t writeIndex=0;
    
    WREN=1;
    CFGS=0;
    LWLO=1;
    for(writeIndex=0;writeIndex<len;writeIndex++){
        PMADRL=(addr)&0xff;
        PMADRH=(addr)>>8;
        PMDATL=(*data)&0xff;
        PMDATH=(*data)>>8;
        my_flash_unlock();
        addr++;
        data++;
    }    
    LWLO=0;
    my_flash_unlock();
    WREN=0;
}
