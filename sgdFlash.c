


#include "KTH57XXIIC.h"
#include "MyI2C.h"

#include "main.h"
#include "string.h"
#include "sgd.h"
#include "SEGGER_RTT.h"
//#include "py32f0xx_hal.h"
//#include "py32f0xx_hal_adc.h"
#include "py32f072xx_Start_Kit.h"

#define FLASH_USER_START_ADDR     0x0800F000
uint32_t DATA[64];
uint32_t length = 0;


/**
  * @brief  FLASH erase function
  * @param  None
  * @retval None
  */
 void APP_FlashErase(uint32_t addr ,uint32_t pages)
{
	 SEGGER_RTT_printf(0, "APP_FlashErase %x  %d\r\n",addr,pages);
  uint32_t PAGEError = 0;
  FLASH_EraseInitTypeDef EraseInitStruct = {0};

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;        /* Erase type (FLASH_TYPEERASE_PAGES for page erase, FLASH_TYPEERASE_SECTORS for sector erase) */
  EraseInitStruct.PageAddress = addr;            /* Erase start address */
  EraseInitStruct.NbPages  = pages;      /* Number of pages to be erased */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  /* Execute page erase, PAGEError returns the page with erase error, 0xFFFFFFFF indicates successful erase */
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  FLASH programming function
  * @param  None
  * @retval None
  */



