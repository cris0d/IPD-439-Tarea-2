/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>  // memcpy
#include <stdio.h>   // sprintf
#include <math.h>    // sqrtf (desviación estándar)

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* Definitions for Task1 */
osThreadId_t Task1Handle;
const osThreadAttr_t Task1_attributes = {
  .name = "Task1",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for AdqTask */
osThreadId_t AdqTaskHandle;
const osThreadAttr_t AdqTask_attributes = {
  .name = "AdqTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for ProcTask */
osThreadId_t ProcTaskHandle;
const osThreadAttr_t ProcTask_attributes = {
  .name = "ProcTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CtrlTask */
osThreadId_t CtrlTaskHandle;
const osThreadAttr_t CtrlTask_attributes = {
  .name = "CtrlTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Queue01 */
osMessageQueueId_t Queue01Handle;
const osMessageQueueAttr_t Queue01_attributes = {
  .name = "Queue01"
};
/* Definitions for QueueADC */
osMessageQueueId_t QueueADCHandle;
const osMessageQueueAttr_t QueueADC_attributes = {
  .name = "QueueADC"
};
/* Definitions for QueueLED */
osMessageQueueId_t QueueLEDHandle;
const osMessageQueueAttr_t QueueLED_attributes = {
  .name = "QueueLED"
};
/* USER CODE BEGIN PV */

float valor_rx;           // Valor float reconstruido
uint8_t buffer_uart[4];   // Buffer temporal para los 4 bytes del float de Rx

//uint16_t raw_val;
//uint32_t adc_raw;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
void StartTask1(void *argument);
void StartAdqTask(void *argument);
void StartProcTask(void *argument);
void StartCtrlTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart2, buffer_uart, 4);	// Se almacena en el buffer las 4 tramas (float)

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Queue01 */
  Queue01Handle = osMessageQueueNew (20, sizeof(float), &Queue01_attributes);

  /* creation of QueueADC */
  QueueADCHandle = osMessageQueueNew (5, sizeof(uint16_t), &QueueADC_attributes);

  /* creation of QueueLED */
  QueueLEDHandle = osMessageQueueNew (5, sizeof(uint8_t), &QueueLED_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task1 */
  Task1Handle = osThreadNew(StartTask1, NULL, &Task1_attributes);

  /* creation of AdqTask */
  AdqTaskHandle = osThreadNew(StartAdqTask, NULL, &AdqTask_attributes);

  /* creation of ProcTask */
  ProcTaskHandle = osThreadNew(StartProcTask, NULL, &ProcTask_attributes);

  /* creation of CtrlTask */
  CtrlTaskHandle = osThreadNew(StartCtrlTask, NULL, &CtrlTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : LD2_Pin PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// RX UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
        // Copia segura de los 4 bytes del buffer al float
        memcpy(&valor_rx, buffer_uart, sizeof(float));

        // Enviar a la cola de FreeRTOS
        osMessageQueuePut(Queue01Handle, &valor_rx, 0, 0);

        // Reactivar interrupción para los próximos 4 bytes
        HAL_UART_Receive_IT(&huart2, buffer_uart, 4);
    }
}





/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartTask1 */
/**
  * @brief  Function implementing the Task1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask1 */
void StartTask1(void *argument)
{
  /* USER CODE BEGIN 5 */
  float datos[100];
  int contador = 0;
  float suma, promedio, suma_cuadrados, desv_est;
  float valor_recibido;
  float resultados[2]; // Arreglo a enviar [Promedio, Desviación]

  for(;;)
  {
    if (osMessageQueueGet(Queue01Handle, &valor_recibido, NULL, osWaitForever) == osOK)	// Consumir datos
    {

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

      datos[contador++] = valor_recibido;	// Contador de 100 valores

      if (contador == 100)	// Cuando llegue a 100 datos consumidos se empieza a calcular
      {
        // PROMEDIO
        suma = 0;
        for(int i = 0; i < 100; i++) suma += datos[i];
        promedio = suma / 100.0f;

        // DESVIACION ESTANDAR
        suma_cuadrados = 0;
        for(int i = 0; i < 100; i++) {
          suma_cuadrados += powf(datos[i] - promedio, 2);
        }
        desv_est = sqrtf(suma_cuadrados / 100.0f);

        // PREPARAR ENVÍO
        resultados[0] = promedio;
        resultados[1] = desv_est;

        // UART TX
        HAL_UART_Transmit(&huart2, (uint8_t*)resultados, sizeof(resultados), HAL_MAX_DELAY);

        contador = 0;
      }

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
    }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartAdqTask */
/**
* @brief Function implementing the AdqTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAdqTask */
void StartAdqTask(void *argument)
{
  /* USER CODE BEGIN StartAdqTask */

  uint16_t adc_raw;

  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_ADC_Start(&hadc1);										// Empieza a medir el ADC
	if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {		// Si el valor es estable se guarda
		adc_raw = (uint16_t)HAL_ADC_GetValue(&hadc1);			// Lee el dato
	    osMessageQueuePut(QueueADCHandle, &adc_raw, 0, 0);		// Escribe en la cola
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    osDelay(100);												// Cada 100 ms se ejecuta
  }
  /* USER CODE END StartAdqTask */
}

/* USER CODE BEGIN Header_StartProcTask */
/**
* @brief Function implementing the ProcTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartProcTask */
void StartProcTask(void *argument)
{
  /* USER CODE BEGIN StartProcTask */

  uint16_t raw_val;

  uint8_t led_id;

  for(;;)
  {
    // Se bloquea hasta recibir un dato de Adquisición
    if (osMessageQueueGet(QueueADCHandle, &raw_val, NULL, osWaitForever) == osOK)
    {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
      //Umbrales: 1.1V -> 1365  |  2.2V -> 2730
      if (raw_val <= 1365) {
        led_id = 1; // Rango Bajo
      }
      else if (raw_val <= 2730) {
        led_id = 2; // Rango Medio
      }
      else {
        led_id = 3; // Rango Alto
      }
      // Se escribe sobre la queue de control
      osMessageQueuePut(QueueLEDHandle, &led_id, 0, 0);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    }
  }
  /* USER CODE END StartProcTask */
}

/* USER CODE BEGIN Header_StartCtrlTask */
/**
* @brief Function implementing the CtrlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCtrlTask */
void StartCtrlTask(void *argument)
{
  /* USER CODE BEGIN StartCtrlTask */
  uint8_t state;

  for(;;)
  {
    // Se bloquea hasta recibir la decisión de Procesamiento
    if (osMessageQueueGet(QueueLEDHandle, &state, NULL, osWaitForever) == osOK)
    {
      HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
      //  Apagar todos los LEDs primero
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

      // 2. Encender solo el que corresponde
      if (state == 1) {
    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
      }
      else if (state == 2) {
    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
      }
      else if (state == 3) {
    	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
      }
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
    }
  }
  /* USER CODE END StartCtrlTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
