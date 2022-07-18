/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_sensors.c
  * @author  MCD Application Team
  * @brief   Manages the sensors on the application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "stdint.h"
#include "platform.h"
#include "sys_conf.h"
#include "sys_sensors.h"
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 0)
#include "adc_if.h"
#endif /* SENSOR_ENABLED */

/* USER CODE BEGIN Includes */
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
#if defined (X_NUCLEO_IKS01A2)
#warning "IKS drivers are today available for several families but not stm32WL"
#warning "up to the user adapt IKS low layer to map it on WL board driver"
#warning "this code would work only if user provide necessary IKS and BSP layers"
#include "iks01a2_env_sensors.h"
#elif defined (X_NUCLEO_IKS01A3)

/*
## How to add IKS01A3 to STM32CubeWL
   Note that LoRaWAN_End_Node Example is used as an example for steps below.
 1. Open the LoRaWAN_End_Node CubeMX project by double-clicking on the LoRaWAN_End_Node.ioc under "STM32Cube_FW_WL_V1.x.x\Projects\NUCLEO-WL55JC\Applications\LoRaWAN\LoRaWAN_End_Node"
 2. From the CubeMX project, click on "Software Packs"->"Manage Software Packs" to open the Embedded Software Packages Manager. Then, click on the "STMicroelectronics" tab, expand the X-CUBE-MEMS1, check the latest version of this pack (i.e. 9.0.0), and install. Then, close the Embedded Software Packages Manager.
 3. From the CubeMX project, click on "Software Packs"->"Select Components" to open the Software Packs Component Selector, expand the X-CUBE-MEMS1 pack and select the "Board Extension IKS01A3" component by checking the respective box, and click OK.
 4. From the CubeMX project, expand the "Connectivity" category and enable I2C2 on pins PA11 (I2C2_SDA) and PA12 (I2C2_SCK).
 5. From the CubeMX project, expand the "Software Packs" category and enable the "Board Extension IKS01A3" by checking the box, and choose I2C2 under the "Found Solutions" menu.
 6. From the CubeMX project, click the "Project Manager" section
    - From the "Project Settings" section, select your Toolchain/IDE of choice (if CubeIDE, uncheck the "Generator Under Root" option).
    - From the "Code Generator" section, select "Copy only the necessary library files".
 7. Click "GENERATE CODE" to generate the code project with the MEMS drivers integrated.
 8. From the code project, find and open the sys_conf.h and make the following edits
    - Set the #define SENSOR_ENABLED to 1
    - Set the #define LOW_POWER_DISABLE to 1 to prevent the device from entering low power mode. This is needed, since the I2C2 requires handling when exiting low power modes, so to prevent issues, best is to disable low power mode, however, if low power mode is desired, you'll have to re-initialize the I2C2 from PWR_ExitStopMode() in stm32_lpm_if.c, so you can just call HAL_I2C_Init() from there.
 9. From the code project, find and open lora_app.h, and uncomment the following line
    #define CAYENNE_LPP
 10. From the code project properties, add X_NUCLEO_IKS01A3 Pre-processor Defined symbol.
 11. Save all changes and build project
 12. Connect the X-NUCLEO-IKS01A3 expansion board on the NUCLEO-WL55JC1
 13. Load and run the code
*/
#warning "IKS drivers are today available for several families but not stm32WL, follow steps defined in sys_sensors.c"
#include "iks01a3_env_sensors.h"
#else  /* not X_IKS01xx */
#error "user to include its sensor drivers"
#endif  /* X_NUCLEO_IKS01xx */
#elif !defined (SENSOR_ENABLED)
#error SENSOR_ENABLED not defined
#endif  /* SENSOR_ENABLED */

#include "i2c.h"
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

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN PD */
#define STSOP_LATTITUDE           ((float) 43.618622 )  /*!< default latitude position */
#define STSOP_LONGITUDE           ((float) 7.051415  )  /*!< default longitude position */
#define MAX_GPS_POS               ((int32_t) 8388607 )  /*!< 2^23 - 1 */
#define HUMIDITY_DEFAULT_VAL      50.0f                 /*!< default humidity */
#define TEMPERATURE_DEFAULT_VAL   18.0f                 /*!< default temperature */
#define PRESSURE_DEFAULT_VAL      1000.0f               /*!< default pressure */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
#if defined (X_NUCLEO_IKS01A2)
#warning "IKS drivers are today available for several families but not stm32WL"
#warning "up to the user adapt IKS low layer to map it on WL board driver"
#warning "this code would work only if user provide necessary IKS and BSP layers"
IKS01A2_ENV_SENSOR_Capabilities_t EnvCapabilities;
#elif defined (X_NUCLEO_IKS01A3)
IKS01A3_ENV_SENSOR_Capabilities_t EnvCapabilities;
#else  /* not X_IKS01Ax */
#error "user to include its sensor drivers"
#endif  /* X_NUCLEO_IKS01 */
#elif !defined (SENSOR_ENABLED)
#error SENSOR_ENABLED not defined
#endif  /* SENSOR_ENABLED */

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

// structure to pass data between functions
bme bmes;
// Sampling results variable
struct bme680_field_data data;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

int8_t bme680I2cRead(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t bme680I2cWrite(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
bme bme680Init(struct bme680_dev gas_sensor, int8_t rslt, bme bmes);
void bme680TakeSample(sensor_t *sensor_data, char i2c_reading_buf[100], int8_t rslt, struct bme680_field_data data, uint16_t min_sampling_period, struct bme680_dev gas_sensor);
void getWindDir(void);
int getRainfall(int rainTips);
void getWindSpeed();
void printWindSpeed();

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t EnvSensors_Read(sensor_t *sensor_data)
{
  /* USER CODE BEGIN EnvSensors_Read */
  float HUMIDITY_Value = HUMIDITY_DEFAULT_VAL;
  float TEMPERATURE_Value = TEMPERATURE_DEFAULT_VAL;
  float PRESSURE_Value = PRESSURE_DEFAULT_VAL;

#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  IKS01A2_ENV_SENSOR_GetValue(HTS221_0, ENV_HUMIDITY, &HUMIDITY_Value);
  IKS01A2_ENV_SENSOR_GetValue(HTS221_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  IKS01A2_ENV_SENSOR_GetValue(LPS22HB_0, ENV_PRESSURE, &PRESSURE_Value);
  IKS01A2_ENV_SENSOR_GetValue(LPS22HB_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_HUMIDITY, &HUMIDITY_Value);
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_LPS22HH_0, ENV_PRESSURE, &PRESSURE_Value);
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_LPS22HH_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */
#else
  TEMPERATURE_Value = (SYS_GetTemperatureLevel() >> 8);
#endif  /* SENSOR_ENABLED */

  sensor_data->humidity    = HUMIDITY_Value;
  sensor_data->temperature = TEMPERATURE_Value;
  sensor_data->pressure    = PRESSURE_Value;

  sensor_data->latitude  = (int32_t)((STSOP_LATTITUDE  * MAX_GPS_POS) / 90);
  sensor_data->longitude = (int32_t)((STSOP_LONGITUDE  * MAX_GPS_POS) / 180);

  bme680TakeSample(sensor_data, i2c_reading_buf, bmes.result, data, bmes.min_period, bmes.gs);

  return 0;
  /* USER CODE END EnvSensors_Read */
}

int32_t EnvSensors_Init(void)
{
#if defined( USE_IKS01A2_ENV_SENSOR_HTS221_0 ) || defined( USE_IKS01A2_ENV_SENSOR_LPS22HB_0 ) || \
    defined( USE_IKS01A3_ENV_SENSOR_HTS221_0 ) || defined( USE_IKS01A3_ENV_SENSOR_LPS22HH_0 ) || \
    defined( USE_BSP_DRIVER )
  int32_t ret = BSP_ERROR_NONE;
#else
  int32_t ret = 0;
#endif /* USE_BSP_DRIVER */
  /* USER CODE BEGIN EnvSensors_Init */
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
  /* Init */
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Init(HTS221_0, ENV_TEMPERATURE | ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Init(LPS22HB_0, ENV_TEMPERATURE | ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Init(IKS01A3_HTS221_0, ENV_TEMPERATURE | ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Init(IKS01A3_LPS22HH_0, ENV_TEMPERATURE | ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */

  /* Enable */
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Enable(HTS221_0, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A2_ENV_SENSOR_Enable(HTS221_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Enable(LPS22HB_0, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A2_ENV_SENSOR_Enable(LPS22HB_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_HTS221_0, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_HTS221_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_LPS22HH_0, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_LPS22HH_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */

  /* Get capabilities */
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A2_ENV_SENSOR_GetCapabilities(HTS221_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  ret = IKS01A2_ENV_SENSOR_GetCapabilities(LPS22HB_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A3_ENV_SENSOR_GetCapabilities(IKS01A3_HTS221_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  ret = IKS01A3_ENV_SENSOR_GetCapabilities(IKS01A3_LPS22HH_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */

#elif !defined (SENSOR_ENABLED)
#error SENSOR_ENABLED not defined
#endif /* SENSOR_ENABLED  */

  // Initialize BME680 sensor
  bmes = bme680Init(gas_sensor, rslt, bmes);

  /* USER CODE END EnvSensors_Init */
  return ret;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

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
	//char bme_msg[] = "BME680 Initialization Error\r\n";
	//DebugSerialOutput(bme_msg);
  } else {
	//char bme_msg[] = "BME680 Initialized and Ready\r\n";
	//DebugSerialOutput(bme_msg);
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

void bme680TakeSample(sensor_t *sensor_data, char i2c_reading_buf[100], int8_t rslt, struct bme680_field_data data, uint16_t min_sampling_period, struct bme680_dev gas_sensor) {

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

	sensor_data->humidity    = data.humidity / 1000;
	sensor_data->temperature = data.temperature / 100;
	sensor_data->pressure    = data.pressure / 100;

	// Publish result to connected PC
	//DebugSerialOutput(i2c_reading_buf);

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
			//sprintf(msg, "The wind is blowing %s \r\n", windDir[i]);
			//DebugSerialOutput(msg);
			inIf = 1;
			break;
		}//if
	} //for

	// during rapid wind direction change voltage can fall out of range of values. failsafe.
	if(!inIf) {
		//sprintf(msg, "The wind direction is changing rapidly \r\n");
		//DebugSerialOutput(msg);
	}

	//------------------------Finish weather vane sample --------------------------------

} // getWindDir function

int getRainfall(int rainTips) {

	// interrupt function. Increments every reed closure. twice per tip (debounce)
	rainTips++;

	//char rainfallMsg[80];
	//rainFallInMM = rainTips*0.1397; //2791 per tip, 2x due to debounce

	// only print once every tip, again accounting for debounce
	if(rainTips % 2 == 0) {
		//sprintf(rainfallMsg, "Rainfall since program start: %f mm \r\n", rainFallInMM);
		//DebugSerialOutput(rainfallMsg);
	}

	return rainTips;

} // getRainfall function

void getWindSpeed() {

	// interrupt function. Increments every reed closure. ~3 times in a full rotation
	//windTips++;

} // getWindSpeed function

void printWindSpeed() {

	// hold uart msg
	//char windMsg[80];

	// calculate a store windspeed taken over 5 sec period
	//windValues[windCounts++] = windTips*2.4/5/3;

	// reset array index if filled. for purposes of averaging
	//if(windCounts > 60) {
	//	windCounts = 0;
	//}

	//sprintf(windMsg, "Full spins in 5 seconds: %u \r\n", windTips/3);
	//DebugSerialOutput(windMsg);

	//sprintf(windMsg, "Wind speed over 5 seconds: %f \r\n", windValues[windCounts-1]);
	//DebugSerialOutput(windMsg);

	// find and report average windspeed
	//float avgWindSpeed = 0.0;
	//for(int i = 0; i<60; i++) {
	//	avgWindSpeed += windValues[i];
	//}

	//if(sampleNumber < 60) {
	//	avgWindSpeed = avgWindSpeed / windCounts;
	//}
	//else {
	//	avgWindSpeed = avgWindSpeed / 60;
	//}

	//sprintf(windMsg, "Avg Wind speed over 15 min: %f km/h \r\n", avgWindSpeed);
	//DebugSerialOutput(windMsg);
}

/* USER CODE END PrFD */
