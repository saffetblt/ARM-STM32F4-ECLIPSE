#include "stm32f4xx_hal.h"


void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
}


int main()
{
	GPIO_Init();

	while(1)
	{

	}
}

void GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;		// GPIO için nesne oluşuturuldu
	HAL_Init();		// HAL lib init edildi

	__HAL_RCC_GPIOA_CLK_ENABLE();		// A portunun clock hattı enable edildi
	GPIO_InitStruct.Pin = GPIO_PIN_0;		// GPIO_0 Seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;		// Seçilen GPIO'nun modu interrupt ve yükselen kenar seçildi
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// GPIO pulldown seçildi --> normalde 0
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 		// Frekans hızı high seçildi
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		// Oluşturulan nesne A portuna init edildi
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);		// EXTI0 interruptı kullanılacağı belirlendi

	__HAL_RCC_GPIOD_CLK_ENABLE();		// D portunun clock hattı enable edildi
	GPIO_InitStruct.Pin = GPIO_PIN_12;		// GPIO_12 seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		// SEçilen GPIO'nun modu output yapıldı
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// Pulldown seçildi --> normalde 0
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;		// Düşük frekans seçildi
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		// Oluşuturulan nesne D portuna init edildi


}
