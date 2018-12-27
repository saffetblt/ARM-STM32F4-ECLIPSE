#include "stm32f4xx_hal.h"

void GPIO_Init();

int main()
{
	GPIO_Init();

	while(1)
	{

		while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
		{
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);		// GPIO pin set
			HAL_Delay(2);		// 2 ms bekle
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);		// // GPIO pin reset
			HAL_Delay(2);		// 2 ms bekle

		}

		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);	// // GPIO pin reset
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);	// // GPIO pin reset
		HAL_Delay(6);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);	// // GPIO pin reset
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);	// // GPIO pin reset
		HAL_Delay(6);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);	// // GPIO pin reset
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);	// // GPIO pin reset
		HAL_Delay(6);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET);	// // GPIO pin reset
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);	// // GPIO pin reset
		HAL_Delay(6);





	}

}

void GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;		// GPIO_InitStruct adında nesne oluşturuldu
	HAL_Init();		// HAL komutseti init edildi

	__HAL_RCC_GPIOD_CLK_ENABLE();		// D portunun clock registeri aktif edildi
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;		// GPIO_12-13-14-15 seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		// OUTPUT olarak çalışacağı belirlendi
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// PULLDOWN olarak çalışacak (normalde 0)
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;		// Çalışma frekansı belirlendi
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		// Oluşturulan nesne GPIOD registerine init edildi

	__HAL_RCC_GPIOA_CLK_ENABLE();		// A portunun clock registeri aktif edildi
	GPIO_InitStruct.Pin = GPIO_PIN_0;		// GPIO_0 seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;		// INPUT olarak çalışacağı belirlendi
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// PULDOWN olarak çalışacak (normalde 0)
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;		// Çalışma frekansı belirlendi
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		// Oluşturulan nesne GPIOA registerine init edildi

}
