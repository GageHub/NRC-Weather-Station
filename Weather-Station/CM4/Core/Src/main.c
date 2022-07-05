/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// C Includes
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BME680 Include
#include <bme680.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define DELAY_PERIOD_MS (5*1000) // 5 seconds

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// Sample index count initialization
static int sampleNumber = 1;

// structure for returning data between functions
struct bmeReturn {
    int8_t result;
    uint16_t min_period;
    struct bme680_dev gs;
};
typedef struct bmeReturn bme;

// Data used during BME680 sampling
struct bme680_dev gas_sensor;
char i2c_reading_buf[100];
int8_t rslt = BME680_OK;

// rain vars
static float rainFallInMM = 0;
static int rainTips = 0;

// wind vars
static int windCounts = 0;
static int windTips = 0;
static float windValues[60] = {0};

// timestamping via rtc
char time[30];
char date[30];
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM16_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

// prints messages to UART
size_t DebugSerialOutput(const char *message);

// BME680 Forward Declarations
int8_t bme680I2cRead(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t bme680I2cWrite(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
bme bme680Init(struct bme680_dev gas_sensor, int8_t rslt, bme bmes);
void bme680TakeSample(char i2c_reading_buf[100], int8_t rslt, struct bme680_field_data data, uint16_t min_sampling_period, struct bme680_dev gas_sensor);

void getWindDir(void);
int getRainfall(int rainTips);
void getWindSpeed();
void printWindSpeed();

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

  //-------------- GPIO INITS -----------------------
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  //--------------- GPIO INITS DONE -----------------------

  // structure to pass data between functions
  bme bmes;
  // Sampling results variable
  struct bme680_field_data data;

  // debug vars
  char initMsg[100];
  char countMsg[80];

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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_ADC_Init();
  MX_TIM16_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  // Initialize BME680 sensor
  bmes = bme680Init(gas_sensor, rslt, bmes);

  // Print initialization messages to serial
  sprintf(initMsg, "Time between samples: %u seconds \r\n", DELAY_PERIOD_MS/1000);
  DebugSerialOutput("Program Start!\r\n");
  DebugSerialOutput(initMsg);

  // start timer
  HAL_TIM_Base_Start(&htim16);

  //------------------Config done-------------------------

  /* USER CODE END 2 */

  /* Boot CPU2 */
  HAL_PWREx_ReleaseCore(PWR_CORE_CPU2);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	//--------------== Sample init and time stamp ==-------------------------

	sprintf(countMsg, "\r\n Taking sample %u: \r\n", sampleNumber);
	DebugSerialOutput(countMsg);
	sampleNumber++;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	sprintf(date,"Date: %02d.%02d.%02d \t" ,sDate.Date,sDate.Month,sDate.Year);
	sprintf(time,"Time: %02d.%02d.%02d\r\n",sTime.Hours,sTime.Minutes,sTime.Seconds);
	DebugSerialOutput(date);
	DebugSerialOutput(time);

	//-------WIND SPEED START---------
	printWindSpeed();
	//-------WIND SPEED END-----------

	//----- BME680 START ------------
	bme680TakeSample(i2c_reading_buf, bmes.result, data, bmes.min_period, bmes.gs);
	//------ BME680 END -------------

	//----- ADC WIND DIR START --------
	getWindDir();
	//----- ADC WIND DIR END ----------

	//------GPIO RAIN START-----------
	// done via interrupts
	sprintf(countMsg, "Rainfall since program start: %f mm \r\n", rainFallInMM);
	DebugSerialOutput(countMsg);
	//------GPIO RAIN END-------------

	//------ DELAY START ------
	// reset wind tips per second
	windTips = 0;

	// Wait between samples
	HAL_Delay(DELAY_PERIOD_MS);
	//------- DELAY END--------

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 6;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20303E5D;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0xBEBE) {

	  // Write Back Up Register 1 Data
	  HAL_PWR_EnableBkUpAccess();
	  // Writes a data in a RTC Backup data Register 1
	  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0xBEBE);
	  HAL_PWR_DisableBkUpAccess();
  }
  else {
	  return;
  }


  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x10;
  sTime.Minutes = 0x53;
  sTime.Seconds = 0x30;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_JUNE;
  sDate.Date = 0x21;
  sDate.Year = 0x22;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the TimeStamp
  */
  if (HAL_RTCEx_SetTimeStamp(&hrtc, RTC_TIMESTAMPEDGE_RISING, RTC_TIMESTAMPPIN_DEFAULT) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the reference Clock input
  */
  if (HAL_RTCEx_SetRefClock(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 4800-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 10000;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

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
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : Wind_speed_Pin Rain_Tip_Pin */
  GPIO_InitStruct.Pin = Wind_speed_Pin|Rain_Tip_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

size_t DebugSerialOutput(const char *message) {

	// print to UART easily

	static const size_t USART1_TIMEOUT = 250;
	size_t result = 0;

	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), USART1_TIMEOUT);
	if (HAL_OK == status) {
		result = sizeof(message);
	} else {
		result = 0;
	}

	return result;
}

int8_t bme680I2cRead(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

	// read I2C line

	int8_t result;
	static const size_t I2C_READ_TIMEOUT = 250;

	if (HAL_I2C_Master_Transmit(&hi2c1, (dev_id << 1), &reg_addr, 1, I2C_READ_TIMEOUT) != HAL_OK) {
		result = -1;
	} else if (HAL_I2C_Master_Receive (&hi2c1, (dev_id << 1) | 0x01, reg_data, len, I2C_READ_TIMEOUT) != HAL_OK) {
		result = -1;
	} else {
		result = 0;
	}

	return result;

}

int8_t bme680I2cWrite(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

  // write to I2C line
  int8_t result;
  int8_t *buf;

  // Allocate and load I2C transmit buffer
  buf = malloc(len + 1);
  buf[0] = reg_addr;
  memcpy(buf + 1, reg_data, len);

  if (HAL_I2C_Master_Transmit(&hi2c1, (dev_id << 1), (uint8_t *) buf, len + 1, HAL_MAX_DELAY) != HAL_OK) {
    result = -1;
  } else {
    result = 0;
  }

  free(buf);
  return result;
}

bme bme680Init(struct bme680_dev gas_sensor, int8_t rslt, bme bmes) {

  // ----------------------Configure the BME680 driver-----------------------------

  gas_sensor.dev_id = BME680_I2C_ADDR_PRIMARY;
  gas_sensor.intf = BME680_I2C_INTF;
  gas_sensor.read = bme680I2cRead;
  gas_sensor.write = bme680I2cWrite;
  gas_sensor.delay_ms = HAL_Delay;
  gas_sensor.amb_temp = 25;

  // Initialize the driver
  if (bme680_init(&gas_sensor) != BME680_OK) {
	char bme_msg[] = "BME680 Initialization Error\r\n";
	DebugSerialOutput(bme_msg);
  } else {
	char bme_msg[] = "BME680 Initialized and Ready\r\n";
	DebugSerialOutput(bme_msg);
  }

  // Select desired over sampling rates
  gas_sensor.tph_sett.os_hum = BME680_OS_2X;
  gas_sensor.tph_sett.os_pres = BME680_OS_4X;
  gas_sensor.tph_sett.os_temp = BME680_OS_8X;
  gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

  /* Set the remaining gas sensor settings and link the heating profile */
  gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
  /* Create a ramp heat waveform in 3 steps */
  gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
  gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

  // Set sensor to "always on"
  gas_sensor.power_mode = BME680_FORCED_MODE;

  // Set over sampling settings
  uint8_t required_settings = (BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL);
  rslt = bme680_set_sensor_settings(required_settings, &gas_sensor);

  // Set sensor mode
  rslt = bme680_set_sensor_mode(&gas_sensor);

  // Query minimum sampling period
  uint16_t min_sampling_period;
  bme680_get_profile_dur(&min_sampling_period, &gas_sensor);

  bmes.result = rslt;
  bmes.min_period = min_sampling_period;
  bmes.gs = gas_sensor;

  return bmes;

}//bme680 init

void bme680TakeSample(char i2c_reading_buf[100], int8_t rslt, struct bme680_field_data data, uint16_t min_sampling_period, struct bme680_dev gas_sensor) {

	//------------------------Take sample using BME-----------------------------------

	// Allow BME680 to sample environment
	HAL_Delay(min_sampling_period);

	// Query the sample data
	rslt = bme680_get_sensor_data(&data, &gas_sensor);

	// Format results into a readable string
	sprintf(i2c_reading_buf,
	  "Temp: %u.%u degC, Pres: %u.%u hPa, Humi: %u.%u %%rH\r\n",
	  (unsigned int)data.temperature / 100,
	  (unsigned int)data.temperature % 100,
	  (unsigned int)data.pressure / 100,
	  (unsigned int)data.pressure % 100,
	  (unsigned int)data.humidity / 1000,
	  (unsigned int)data.humidity % 1000);

	// Publish result to connected PC
	DebugSerialOutput(i2c_reading_buf);

	// Request the next sample
	if (gas_sensor.power_mode == BME680_FORCED_MODE) {
	  rslt = bme680_set_sensor_mode(&gas_sensor);
	}

	//------------------------Finish BME sample--------------------------------

} // bme680TakeSample function

void getWindDir(void) {

	//------------------------Take Sample Using weather vane ADC ---------------------------
	// init variables
    uint16_t raw;
    char msg[80];
    int numDirs = 7;
    int inIf = 0;
    // digital voltage values and the directions they correspond to
    uint16_t vMin[] = {1660, 625, 120, 220, 330, 1045, 2860, 2300};  	// 5V = 4095, 0V = (50) 0
    uint16_t vMax[] = {1700, 670, 160, 260, 380, 1080, 2890, 2355};
    char* windDir[] = {"North","North East","East","South East","South","South West","West","North West"};

    // poll the ADC for its value, that value corresponds to a direction
    HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
	raw = HAL_ADC_GetValue(&hadc);

	// check that direction, print it to serial
	for(int i=0; i<=numDirs; i++) {
		if(raw >= vMin[i] && raw <= vMax[i]) {
			sprintf(msg, "The wind is blowing %s \r\n", windDir[i]);
			DebugSerialOutput(msg);
			inIf = 1;
			break;
		}//if
	} //for

	// during rapid wind direction change voltage can fall out of range of values. failsafe.
	if(!inIf) {
		sprintf(msg, "The wind direction is changing rapidly \r\n");
		DebugSerialOutput(msg);
	}

	//------------------------Finish weather vane sample --------------------------------

} // getWindDir function

int getRainfall(int rainTips) {

	// interrupt function. Increments every reed closure. twice per tip (debounce)
	rainTips++;

	char rainfallMsg[80];
	rainFallInMM = rainTips*0.1397; //2791 per tip, 2x due to debounce

	// only print once every tip, again accounting for debounce
	if(rainTips % 2 == 0) {
		sprintf(rainfallMsg, "Rainfall since program start: %f mm \r\n", rainFallInMM);
		DebugSerialOutput(rainfallMsg);
	}

	return rainTips;

} // getRainfall function

void getWindSpeed() {

	// interrupt function. Increments every reed closure. ~3 times in a full rotation
	windTips++;

} // getWindSpeed function

void printWindSpeed() {

	// hold uart msg
	char windMsg[80];

	// calculate a store windspeed taken over 5 sec period
	windValues[windCounts++] = windTips*2.4/5/3;

	// reset array index if filled. for purposes of averaging
	if(windCounts > 60) {
		windCounts = 0;
	}

	//sprintf(windMsg, "Full spins in 5 seconds: %u \r\n", windTips/3);
	//DebugSerialOutput(windMsg);

	sprintf(windMsg, "Wind speed over 5 seconds: %f \r\n", windValues[windCounts-1]);
	DebugSerialOutput(windMsg);

	// find and report average windspeed
	float avgWindSpeed = 0.0;
	for(int i = 0; i<60; i++) {
		avgWindSpeed += windValues[i];
	}

	if(sampleNumber < 60) {
		avgWindSpeed = avgWindSpeed / windCounts;
	}
	else {
		avgWindSpeed = avgWindSpeed / 60;
	}

	sprintf(windMsg, "Avg Wind speed over 15 min: %f km/h \r\n", avgWindSpeed);
	DebugSerialOutput(windMsg);
}

// EXTI Line15 10 External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == Rain_Tip_Pin) {
    	rainTips = getRainfall(rainTips);
    }
    if(GPIO_Pin == Wind_speed_Pin) {
    	getWindSpeed();
    }
}

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
