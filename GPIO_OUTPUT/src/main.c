#include "stm32f4xx_hal.h"

void GPIO_Init();

int main()
{

	GPIO_Init();

	while (1)
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12 | GPIO_PIN_13,GPIO_PIN_SET);		// D Portu GPIO_12-13 SET edildi
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);    // D Portu GPIO_14-15 RESET edildi
		HAL_Delay(100); 	//100 ms bekleme
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);	// D Portu GPIO_14-15 SET edildi
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_RESET);	// D Portu GPIO_14-15 RESET edildi
		HAL_Delay(100);
	}

}

void GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;  // GPIO için struct tanımlandı

	HAL_Init();   // HAL kütüphanesi init edildi

	__HAL_RCC_GPIOD_CLK_ENABLE();	// GPIOD clok hattı aktif edildi

	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;	// Structa tanımlı olan Pin nesnesine GPIO_12-15 tanımlanır
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		// GPIO_12-15 output olarak tanımlanır
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	// GPIO_12-15 PULLDOWN olarak tanımlanır
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;		// GPIO_12-15'nin çalışma frekansı tanımlanır

	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);	// Nesneleri tanımlanan struct Init edildi
}
