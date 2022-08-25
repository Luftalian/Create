/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Sensors_Access_mcat4.h"
#include "uart_mcat2.h"
#include "SPIflash_mcat.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ROCKETSTATUS_FLIGHTMODE_ON 0
#define ROCKETSTATUS_LIFTOFFED 1
#define ROCKETSTATUS_PARAOPEND 2
#define ROCKETSTATUS_NOFLIGHTMODE 3

#define MPUDATAFLASH 0
#define LPSDATAFLASH 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;
USART_HandleTypeDef husart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART6_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t flag_1kHz = 0;
uint32_t flag_50Hz = 0;
uint32_t clock_1kHz = 0;

//******************LPS Lunch countAccOverTH and Detect Top Variable******************//
double sum_Pressure = 0., ave_Pressure = 0., old_Pressure = 0.;
uint8_t sum_P_counter = 0, over_P_count = 0;
//******************End LPS Lunch countAccOverTH and Detect Top Variable******************//

//******************Timer Setting******************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == htim2.Instance)
  {
    flag_1kHz++;
    clock_1kHz++;
  }
  if (htim->Instance == htim1.Instance)
  {
    flag_50Hz++;
  }
}
//******************End Timer Setting******************//

const uint32_t IntervalLaunchAndParaOpen = 11000;
uint8_t rocketStatus = ROCKETSTATUS_NOFLIGHTMODE; // 0~3
uint8_t MPUFlashBuff[256];
uint8_t CountMPUDataSetExistInBuff = 0;
uint8_t LPSFlashBuff[256];
uint8_t CountLPSDataSetExistInBuff = 0;
uint32_t MPUFlashLatestAddress = 0x00, LPSFlashLatestAddress = 0x00;
uint8_t isLaunchDetectedMessageSended = 0;
uint32_t LaunchDetectedTime = 0;
uint8_t LaunchDetectedType = 0;
uint32_t TopDetectedTime = 0;
uint8_t TopDetectedType = 0;


void logingParametersInit()
{
  CountMPUDataSetExistInBuff = 0;
  CountLPSDataSetExistInBuff = 0;
  MPUFlashLatestAddress = 0;
  LPSFlashLatestAddress = 0;
  isLaunchDetectedMessageSended = 0;
  LaunchDetectedTime = 0;
  LaunchDetectedType = 0; // 1...pressure 2...acc
  TopDetectedTime = 0;
  TopDetectedType = 0; // 1...pressure 2...time
}

void paraOpen()
{
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 90);
}

void paraClose()
{
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 45);
}

void paraServoCutoff()
{
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
}

void detectLaunchByPressure(double Pressure_data)
{
  sum_Pressure += Pressure_data;
  sum_P_counter++;
  if (sum_P_counter >= 5)
  {
    old_Pressure = ave_Pressure;
    ave_Pressure = sum_Pressure / sum_P_counter;
    sum_Pressure = 0.;
    sum_P_counter = 0;
    if ((old_Pressure - ave_Pressure) > 0.5)
    { // change true = 10, experiment = 3
      over_P_count++;
    }
    else
    {
      over_P_count = 0;
    }
  }
  if (over_P_count >= 10)
  {
    rocketStatus = ROCKETSTATUS_LIFTOFFED;
    LaunchDetectedTime = clock_1kHz;
    LaunchDetectedType = 1;
    sum_Pressure = 0.;
    old_Pressure = 0.;
    sum_P_counter = 0;
    over_P_count = 0;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
  }
}

void detectTopByPressure(double Pressure_data)
{
  sum_Pressure += Pressure_data;
  sum_P_counter++;
  if (sum_P_counter >= 5)
  {
    old_Pressure = ave_Pressure;
    ave_Pressure = sum_Pressure / sum_P_counter;
    sum_Pressure = 0;
    sum_P_counter = 0;
    if (old_Pressure < ave_Pressure)
    {
      over_P_count++;
    }
    else
    {
      over_P_count = 0;
    }
  }
  if (over_P_count >= 10)
  {
    rocketStatus = ROCKETSTATUS_PARAOPEND;
    TopDetectedTime = clock_1kHz;
    TopDetectedType = 1;
    paraOpen();
    sum_Pressure = 0;
    sum_P_counter = 0;
    over_P_count = 0;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
  }
}
//******************End LPS Pressure Detect Top******************//

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART6_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */

  // time setup
  uint32_t milisecondAfterLiftof = 0;

  // average20
  double average[20];
  int avrnum = 0;
  double Avr = 0.0; // average of (average[20])

  // take off countAccOverTH
  double value5[5] = {0.0, 0.0, 0.0, 0.0, 0.0}; // Avr * 5

  int countAccOverTH = 0;

  // MPU setup
  int16_t MPUMoldingData[6];
  uint8_t MPURecievedData[14];
  double ax, ay, az, gx, gy, gz, axyz;

  CS_Set(MPUDATAFLASH);
  CS_Set(LPSDATAFLASH);
  MPU_CS_High();
  LPS_CS_High();

  HAL_Delay(10);

  MPU_Initialization();

  // LPS setup
  double LPS_Pressure = 0;
  uint8_t LPS_data[3];
  LPS_WakeUp();
  uint16_t LPSLogPreScaler = 0;

  // timer setup

  // uart_putln("timer start",2);

  // PWM setup (timer 1)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 45);
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (USART2->SR & USART_SR_RXNE)
    {
      char rec = uart_receive(2);
      if (rec == 'i')
      {
        if (MPU_WhoAmI() == 113 && LPS_WhoAmI() == 177)
        {
          uart_putln("i", 2);
        }
        else
        {
          uart_putln("o", 2);
        }
      }
      else if (rec == 'l')
      {
        logingParametersInit();
        uart_putln("l", 2);
        milisecondAfterLiftof = 0;
        sum_Pressure = 0;
        sum_P_counter = 0;
        over_P_count = 0;
        rocketStatus = ROCKETSTATUS_FLIGHTMODE_ON;
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
        HAL_TIM_Base_Start_IT(&htim1); // tim2:1kHz
        HAL_TIM_Base_Start_IT(&htim2);
      }
      else if (rec == 's')
      {
        uart_putln("s", 2);
        HAL_TIM_Base_Stop_IT(&htim1); // tim2:1kHz
        HAL_TIM_Base_Stop_IT(&htim2);
        rocketStatus = ROCKETSTATUS_NOFLIGHTMODE;
      }
      else if (rec == 'd')
      {
        flash_wren(MPUDATAFLASH); // allow writing
        flash_be(MPUDATAFLASH);
        while (flash_is_wip(MPUDATAFLASH))
        {
          // uart_puts(".",2);
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
          HAL_Delay(100);
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
          HAL_Delay(100);
        }
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
        HAL_Delay(1000);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
        HAL_Delay(100);

        // erase flash 1
        flash_wren(LPSDATAFLASH); // allow writing
        flash_be(LPSDATAFLASH);
        while (flash_is_wip(LPSDATAFLASH))
        {
          // uart_puts(".",2);
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
          HAL_Delay(100);
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
          HAL_Delay(100);
        }
        uart_putln("f", 2);
      }
      else if (rec == 'r')
      {
        uart_putln("mpu data read", 2);
        for (int index = 0; index < 65536; index++)
        {
          uint8_t readbf[256];
          flash_read(index * 256, readbf, 256, LPSDATAFLASH);
          data_debug_print(readbf, 256);
        }
        uart_putln("lps data read", 2);
        for (int index = 0; index < 65536; index++)
        {
          uint8_t readbf[256];
          flash_read(index * 256, readbf, 256, MPUDATAFLASH);
          data_debug_print(readbf, 256);
        }
      }
      else if (rec == 'e')
      {
        uart_putln("e", 2);
        paraOpen();
      }
      else if (rec == 'c')
      {
        uart_putln("c", 2);
        paraClose();
        HAL_Delay(2000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, RESET);
      }
    }

    if (flag_1kHz > 0)
    { // 1kHz,MPU & flash 0

      LPSLogPreScaler++;
      if (LPSLogPreScaler >= 20)
      {
        // read LPSdata
        LPS_WakeUp();
        LPS_Pressure = (double)LPS_RawandPressure(LPS_data);
        LPS_Pressure = LPS_Pressure * 100. / 4096.;

        // lps
        for (int index = 0; index < 3; index++)
        {
          LPSFlashBuff[16 * CountLPSDataSetExistInBuff + index] = LPS_data[index];
        }
        LPSFlashBuff[16 * CountLPSDataSetExistInBuff + 3] = (LaunchDetectedType << 4) + TopDetectedType;
        // launch detect data
        for (int index = 0; index < 4; index++)
        {
          LPSFlashBuff[16 * CountLPSDataSetExistInBuff + index + 4] = 0xFF & (LaunchDetectedTime >> (8 * index));
        }
        // top detected data
        for (int index = 0; index < 4; index++)
        {
          LPSFlashBuff[16 * CountLPSDataSetExistInBuff + index + 8] = 0xFF & (TopDetectedTime >> (8 * index));
        }
        // time
        for (int index = 0; index < 4; index++)
        {
          LPSFlashBuff[16 * CountLPSDataSetExistInBuff + index + 12] = 0xFF & (clock_1kHz >> (8 * index));
        }
        CountLPSDataSetExistInBuff++;
        if (CountLPSDataSetExistInBuff == 16)
        {
          flash_wren(LPSDATAFLASH);
          flash_pp(LPSFlashLatestAddress, LPSFlashBuff, 0x100, LPSDATAFLASH);
          LPSFlashLatestAddress += 0x100;
          CountLPSDataSetExistInBuff = 0;
        }

        // write LPSdata & time

        if (rocketStatus == ROCKETSTATUS_FLIGHTMODE_ON)
        {
          detectLaunchByPressure(LPS_Pressure);
        }
        else if (rocketStatus == ROCKETSTATUS_LIFTOFFED)
        {
          detectTopByPressure(LPS_Pressure);
        }
        LPSLogPreScaler = 0;
      }
      // MPU countAccOverTH
      MPU_6axis_Raw(MPURecievedData, MPUMoldingData);
      //			uart_puts("\tax:",2);
      //			uart_putint((int)MPUMoldingData[0],2);
      //			uart_puts("\tay:",2);
      //			uart_putint((int)MPUMoldingData[1],2);
      //			uart_puts("\taz:",2);
      //			uart_putintln((int)MPUMoldingData[2],2);

      // make mpu dataset
      for (int index = 0; index < 6; index++)
      {
        MPUFlashBuff[16 * CountMPUDataSetExistInBuff + index] = MPURecievedData[index];
      }
      for (int index = 8; index < 14; index++)
      {
        MPUFlashBuff[16 * CountMPUDataSetExistInBuff + index - 2] = MPURecievedData[index];
      }
      for (int index = 0; index < 4; index++)
      {
        MPUFlashBuff[16 * CountMPUDataSetExistInBuff + index + 12] = 0xFF & (clock_1kHz >> (8 * index));
      }
      CountMPUDataSetExistInBuff++;
      if (CountMPUDataSetExistInBuff == 16)
      {
        flash_wren(MPUDATAFLASH);
        flash_pp(MPUFlashLatestAddress, MPUFlashBuff, 0x100, 0);
        MPUFlashLatestAddress += 0x100;
        CountMPUDataSetExistInBuff = 0;
      }

      ax = (double)MPUMoldingData[0];
      ay = (double)MPUMoldingData[1];
      az = (double)MPUMoldingData[2];
      gx = (double)MPUMoldingData[3];
      gy = (double)MPUMoldingData[4];
      gz = (double)MPUMoldingData[5];

      ax = ax * 16. / 32767.;
      ay = ay * 16. / 32767.;
      az = az * 16. / 32767.;
      gx = gx * 16. / 32767.;
      gy = gy * 16. / 32767.;
      gz = gz * 16. / 32767.;

      axyz = ax * ax + ay * ay + az * az;

      // make average[20] and take off countAccOverTH

      //***********************************MPU Accel lunch**************************************//
      if (avrnum != 20)
      {
        average[avrnum] = axyz;
      }
      else
      {
        for (int a = 0; a < 20; a++)
        {
          Avr += average[a];
        }
        Avr = Avr / 20.0;

        // update value5
        for (int n = 3; n >= 0; n--)
        {
          value5[n + 1] = value5[n];
        }
        value5[0] = Avr;
        if (rocketStatus == ROCKETSTATUS_FLIGHTMODE_ON)
        {
          countAccOverTH = 0;
          for (int m = 0; m < 5; m++)
          {
            if (value5[m] > 4.0)
            {
              countAccOverTH++;
            }
          }
          if (countAccOverTH >= 5)
          { // countAccOverTH = 5 -> take off
            rocketStatus = ROCKETSTATUS_LIFTOFFED;
            LaunchDetectedTime = clock_1kHz;
            LaunchDetectedType = 2; // 1...pressure 2...acc
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
          }
        }
        avrnum = 0;
        average[avrnum] = axyz;
      }
      avrnum++;
      //********************************End MPU Accel lunch*****************************************//

      if ((rocketStatus == ROCKETSTATUS_LIFTOFFED) && (milisecondAfterLiftof > IntervalLaunchAndParaOpen))
      {
        if (!isLaunchDetectedMessageSended)
        {
          uart_putc('t', 2);
          isLaunchDetectedMessageSended = 1;
        }
        TopDetectedTime = clock_1kHz;
        TopDetectedType = 2;
        paraOpen();
        rocketStatus = ROCKETSTATUS_PARAOPEND;
      }
      else if (rocketStatus == ROCKETSTATUS_LIFTOFFED)
      {
        milisecondAfterLiftof++;
      }

      if (MPUFlashLatestAddress == 65535)
      {
        HAL_TIM_Base_Stop_IT(&htim1); // tim2:1kHz
        HAL_TIM_Base_Stop_IT(&htim2);
        rocketStatus = ROCKETSTATUS_NOFLIGHTMODE;
      }

      flag_1kHz = 0;
    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief SPI3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1799;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 89;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 3599;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 699;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);
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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
 * @brief USART6 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART6_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  husart6.Instance = USART6;
  husart6.Init.BaudRate = 115200;
  husart6.Init.WordLength = USART_WORDLENGTH_8B;
  husart6.Init.StopBits = USART_STOPBITS_1;
  husart6.Init.Parity = USART_PARITY_NONE;
  husart6.Init.Mode = USART_MODE_TX_RX;
  husart6.Init.CLKPolarity = USART_POLARITY_LOW;
  husart6.Init.CLKPhase = USART_PHASE_1EDGE;
  husart6.Init.CLKLastBit = USART_LASTBIT_DISABLE;
  if (HAL_USART_Init(&husart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS1_GPIO_Port, FLASH_CS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, FLASH_CS_Pin | LED_Pin | MPUCS_Pin | LPSCS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FLASH_CS1_Pin */
  GPIO_InitStruct.Pin = FLASH_CS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLASH_CS1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLASH_CS_Pin LED_Pin MPUCS_Pin LPSCS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin | LED_Pin | MPUCS_Pin | LPSCS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
