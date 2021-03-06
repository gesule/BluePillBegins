/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341driver.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define boardLed_Pin GPIO_PIN_13
#define boardLed_GPIO_Port GPIOC
#define lcd_D0_Pin GPIO_PIN_0
#define lcd_D0_GPIO_Port GPIOA
#define lcd_D1_Pin GPIO_PIN_1
#define lcd_D1_GPIO_Port GPIOA
#define lcd_D2_Pin GPIO_PIN_2
#define lcd_D2_GPIO_Port GPIOA
#define lcd_D3_Pin GPIO_PIN_3
#define lcd_D3_GPIO_Port GPIOA
#define lcd_D4_Pin GPIO_PIN_4
#define lcd_D4_GPIO_Port GPIOA
#define lcd_D5_Pin GPIO_PIN_5
#define lcd_D5_GPIO_Port GPIOA
#define lcd_D6_Pin GPIO_PIN_6
#define lcd_D6_GPIO_Port GPIOA
#define lcd_D7_Pin GPIO_PIN_7
#define lcd_D7_GPIO_Port GPIOA
#define lcd_RST_Pin GPIO_PIN_8
#define lcd_RST_GPIO_Port GPIOA
#define lcd_CS_Pin GPIO_PIN_4
#define lcd_CS_GPIO_Port GPIOB
#define lcd_RS_Pin GPIO_PIN_5
#define lcd_RS_GPIO_Port GPIOB
#define lcd_WR_Pin GPIO_PIN_8
#define lcd_WR_GPIO_Port GPIOB
#define lcd_RD_Pin GPIO_PIN_9
#define lcd_RD_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
