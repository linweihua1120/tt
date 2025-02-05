#ifndef _SGD_FLASH
#define _SGD_FLASH

#include "py32f072xx_Start_Kit.h"


  
  void APP_FlashErase(uint32_t addr ,uint32_t pages);

  
  void APP_FlashProgram(void);
  
  void APP_FlashBlank(void);

	void APP_FlashVerify(void);


#endif

