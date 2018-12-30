#include"stm32f4xx_hal.h"

void GPIO_Init();		// Fonksiyon prototipi
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);		// Fonksiyon prototipi
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); 		// Fonksiyon prototipi

int delay_wait_time = 100 ; 		// int türünde bir zaman değişkeni tanımlandı

int main()
{
	GPIO_Init();		// GPIO ayarlarının olduğu fonksiyon çağırıldı

	while(1)
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);		// PIN_12 SET
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);		// PIN_15 RESET
		HAL_Delay(delay_wait_time);								// tanımlanan değişken kadar bekle
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);		// PIN_13 SET
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);		// PIN_12 RESET
		HAL_Delay(delay_wait_time);								// tanımlanan değişken kadar bekle
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);		// PIN_14 SET
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);		// PIN_13 RESET
		HAL_Delay(delay_wait_time);								// tanımlanan değişken kadar bekle
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);		// PIN_15 SET
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);		// PIN_14 RESET
		HAL_Delay(delay_wait_time);

			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))		// PIN_1'in durumu okunur 1 olduğu sürece içeride kalır
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);	// GPIO_12-13-14-15 SET
				HAL_Delay(delay_wait_time);																		// tanımlanan değişken kadar bekle
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);	// GPIO_12-13-14-15 RESET
				HAL_Delay(delay_wait_time);																		// tanımlanan değişken kadar bekle
			}
	}
}

void GPIO_Init()
{
	HAL_Init();		// Hal kütüphanesi init edildi
	GPIO_InitTypeDef GPIO_InitStruct;		// Nesne oluşturuldu

	__HAL_RCC_GPIOD_CLK_ENABLE();		// GPIOD portunun RCC clock hattı aktif edildi
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;	// PIN_12_13_14_15 Seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		// OUTPUT olarak set edildi
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// PULLDOWN olacak normalde 1
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;		// Çalışma frekansı seçildi
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		// Oluşturulan nesne GPIOD portuna init edildi
	HAL_GPIO_LockPin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);		// Pin ayarlarının değiştirilememesi için LOCK fonksiyonu ile kilitlendi


	__HAL_RCC_GPIOA_CLK_ENABLE();		// GPIOA portunun RCC clock hattı aktif edildi
	GPIO_InitStruct.Pin = GPIO_PIN_0;	// PIN_0 seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;		// İnterrupt olarak çalıştırılacak
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// PULLDOWN olacak normalde 1
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;		// Çalışma frekansı seçildi
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		// Oluşturulan nesne GPIOA portuna init edildi
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); 		// İnterrupt kullanılması için enable edildi ve kaçıncı interrupt kullanılacağı belirlendi EXTI0_IRQn

	__HAL_RCC_GPIOA_CLK_ENABLE();		// GPIOA portunun RCC clock hattı aktif edildi
	GPIO_InitStruct.Pin = GPIO_PIN_1;	// PIN_1 seçildi
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;	// Normal input olarak kullanılacak - interruptsız
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;		// PULLDOWN olacak normalde 1
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;		// Çalışma frekansı seçildi
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		// Oluşturulan nesne GPIOA portuna init edildi
}

void EXTI0_IRQHandler(void)		// Oluşturulan interrupt fonksiyonu
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);		// İnterrupt Handlerını getiren komut
}

 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 		// Oluşan handlerın çağırdı fonksiyon, Interruptın ne yapacağını belirlediğimiz fonksiyon
{
	 delay_wait_time = delay_wait_time + 100;		// zaman değişkeni arttırıldı
	 if(delay_wait_time == 1000)			// zaman değişkeni 1000 olunca
		 delay_wait_time = 100;				// 100 olarak set edilecek
}

