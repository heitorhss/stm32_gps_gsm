/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * @author         : HEITOR SOUSA
  * @e-mail         : heitor_hss@hotmail.com
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
#include <stdint.h> // uint8_t   ...
#include <stdio.h>  // sprintf() ...
#include <string.h> // tratamento de strings ...
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef uint8_t bool;
#define true 1
#define false 0

//TABELA DE COMANDOS AT
#define AT          0
#define AT_CFUN     1
#define AT_CPIN     2
#define AT_CREG     3
#define AT_CSQ      4
#define AT_CSTT     5
#define AT_CIICR    6
#define AT_CIFSR    7
#define AT_CIPSTART 8
#define AT_CIPSEND  9
#define AT_CIPCLOSE 10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
unsigned char data_in_uart1[401] = {0};
unsigned char data_in_uart2[61] = {0};

bool ctrl_uart1 = false;
bool ctrl_uart2 = false;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart -> Instance == USART1)
	{
		MX_USART1_UART_Init();
		HAL_UART_Receive_IT(&huart1, data_in_uart1, 400);
		ctrl_uart1 = true;
	}
	if(huart -> Instance == USART2)
	{
		MX_USART2_UART_Init();
		HAL_UART_Receive_IT(&huart2, data_in_uart2, 60);
		ctrl_uart2 = true;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	unsigned char data_filter_uart1[100] = {0};
	unsigned char data_filter_uart2[100] = {0};

	//unsigned char error_msg_NS[5] = {0}; // VARIAVEL EM DESUSO
	//error_msg_NS[0] = 0x4E;
	//error_msg_NS[1] = 0x53; //mensagem de erro: "NS" no signal

	//unsigned char error_msg_ID[5] = {0}; // VARIAVEL EM DESUSO
	//error_msg_ID[0] = 0x49;
	//error_msg_ID[1] = 0x44; //mensagem de erro: "ID" invalid data

	unsigned char error_msg_NC[5] = {0};
	error_msg_NC[0] = 0x4E;
	error_msg_NC[1] = 0x43; // mensagem de erro: "NC" no connectivity

	unsigned char send_msg[2] = {0};
	send_msg[0] = 0x1A;  //send msg = ctrl+z

	bool ctrl_creg_loop = false;
	uint8_t cont_loop1 = 0;

	// DEFININDO COMANDOS AT
	unsigned char at_command[11][50] = {{0},{0}};

	at_command[AT][0] = 0x41; //A
	at_command[AT][1] = 0x54; //T
	at_command[AT][2] = 0x0A; //enter

	at_command[AT_CFUN][0] = 0x41; //A
	at_command[AT_CFUN][1] = 0x54; //T
	at_command[AT_CFUN][2] = 0x2B; //+
	at_command[AT_CFUN][3] = 0x43; //C
	at_command[AT_CFUN][4] = 0x46; //F
	at_command[AT_CFUN][5] = 0x55; //U
	at_command[AT_CFUN][6] = 0x4E; //N
	at_command[AT_CFUN][7] = 0x3D; //=
	at_command[AT_CFUN][8] = 0x31; //1
	at_command[AT_CFUN][9] = 0x0A; //enter

	at_command[AT_CPIN][0] = 0x41; //A
	at_command[AT_CPIN][1] = 0x54; //T
	at_command[AT_CPIN][2] = 0x2B; //+
	at_command[AT_CPIN][3] = 0x43; //C
	at_command[AT_CPIN][4] = 0x50; //P
	at_command[AT_CPIN][5] = 0x49; //I
	at_command[AT_CPIN][6] = 0x4E; //N
	at_command[AT_CPIN][7] = 0x3F; //?
	at_command[AT_CPIN][8] = 0x0A; //enter

	at_command[AT_CREG][0] = 0x41; //A
	at_command[AT_CREG][1] = 0x54; //T
	at_command[AT_CREG][2] = 0x2B; //+
	at_command[AT_CREG][3] = 0x43; //C
	at_command[AT_CREG][4] = 0x52; //R
	at_command[AT_CREG][5] = 0x45; //E
	at_command[AT_CREG][6] = 0x47; //G
	at_command[AT_CREG][7] = 0x3F; //?
	at_command[AT_CREG][8] = 0x0A; //enter

	at_command[AT_CSQ][0] = 0x41; //A
	at_command[AT_CSQ][1] = 0x54; //T
	at_command[AT_CSQ][2] = 0x2B; //+
	at_command[AT_CSQ][3] = 0x43; //C
	at_command[AT_CSQ][4] = 0x53; //S
	at_command[AT_CSQ][5] = 0x51; //Q
	at_command[AT_CSQ][6] = 0x0A; //?

	at_command[AT_CSTT][0]  = 0x41; //A
	at_command[AT_CSTT][1]  = 0x54; //T
	at_command[AT_CSTT][2]  = 0x2B; //+
	at_command[AT_CSTT][3]  = 0x43; //C
	at_command[AT_CSTT][4]  = 0x53; //S
	at_command[AT_CSTT][5]  = 0x54; //T
	at_command[AT_CSTT][6]  = 0x54; //T
	at_command[AT_CSTT][7]  = 0x3D; //=
	at_command[AT_CSTT][8]  = 0x22; //"
	at_command[AT_CSTT][9]  = 0x74; //t
	at_command[AT_CSTT][10] = 0x69; //i
	at_command[AT_CSTT][11] = 0x6D; //m
	at_command[AT_CSTT][12] = 0x62; //b
	at_command[AT_CSTT][13] = 0x72; //r
	at_command[AT_CSTT][14] = 0x61; //a
	at_command[AT_CSTT][15] = 0x73; //s
	at_command[AT_CSTT][16] = 0x69; //i
	at_command[AT_CSTT][17] = 0x6C; //l
	at_command[AT_CSTT][18] = 0x2E; //.
	at_command[AT_CSTT][19] = 0x62; //b
	at_command[AT_CSTT][20] = 0x72; //r
	at_command[AT_CSTT][21] = 0x22; //"
	at_command[AT_CSTT][22] = 0x2C; //,
	at_command[AT_CSTT][23] = 0x22; //"
	at_command[AT_CSTT][24] = 0x22; //"
	at_command[AT_CSTT][25] = 0x2C; //,
	at_command[AT_CSTT][26] = 0x22; //"
	at_command[AT_CSTT][27] = 0x22; //"
	at_command[AT_CSTT][28] = 0x0A; //enter

	at_command[AT_CIICR][0] = 0x41; //A
	at_command[AT_CIICR][1] = 0x54; //T
	at_command[AT_CIICR][2] = 0x2B; //+
	at_command[AT_CIICR][3] = 0x43; //C
	at_command[AT_CIICR][4] = 0x49; //I
	at_command[AT_CIICR][5] = 0x49; //I
	at_command[AT_CIICR][6] = 0x43; //C
	at_command[AT_CIICR][7] = 0x52; //R
	at_command[AT_CIICR][8] = 0x0A; //enter

	at_command[AT_CIFSR][0] = 0x41; //A
	at_command[AT_CIFSR][1] = 0x54; //T
	at_command[AT_CIFSR][2] = 0x2B; //+
	at_command[AT_CIFSR][3] = 0x43; //C
	at_command[AT_CIFSR][4] = 0x49; //I
	at_command[AT_CIFSR][5] = 0x46; //F
	at_command[AT_CIFSR][6] = 0x53; //S
	at_command[AT_CIFSR][7] = 0x52; //R
	at_command[AT_CIFSR][8] = 0x0A; //enter

	at_command[AT_CIPSTART][0]  = 0x41; //A
	at_command[AT_CIPSTART][1]  = 0x54; //T
	at_command[AT_CIPSTART][2]  = 0x2B; //+
	at_command[AT_CIPSTART][3]  = 0x43; //C
	at_command[AT_CIPSTART][4]  = 0x49; //I
	at_command[AT_CIPSTART][5]  = 0x50; //P
	at_command[AT_CIPSTART][6]  = 0x53; //S
	at_command[AT_CIPSTART][7]  = 0x54; //T
	at_command[AT_CIPSTART][8]  = 0x41; //A
	at_command[AT_CIPSTART][9]  = 0x52; //R
	at_command[AT_CIPSTART][10] = 0x54; //T
	at_command[AT_CIPSTART][11] = 0x3D; //=
	at_command[AT_CIPSTART][12] = 0x22; //"
	at_command[AT_CIPSTART][13] = 0x54; //T
	at_command[AT_CIPSTART][14] = 0x43; //C
	at_command[AT_CIPSTART][15] = 0x50; //P
	at_command[AT_CIPSTART][16] = 0x22; //"
	at_command[AT_CIPSTART][17] = 0x2C; //,
	at_command[AT_CIPSTART][18] = 0x22; //"
	at_command[AT_CIPSTART][19] = 0x6D; //m
	at_command[AT_CIPSTART][20] = 0x71; //q
	at_command[AT_CIPSTART][21] = 0x74; //t
	at_command[AT_CIPSTART][22] = 0x74; //t
	at_command[AT_CIPSTART][23] = 0x2E; //.
	at_command[AT_CIPSTART][24] = 0x65; //e
	at_command[AT_CIPSTART][25] = 0x63; //c
	at_command[AT_CIPSTART][26] = 0x6C; //l
	at_command[AT_CIPSTART][27] = 0x69; //i
	at_command[AT_CIPSTART][28] = 0x70; //p
	at_command[AT_CIPSTART][29] = 0x73; //s
	at_command[AT_CIPSTART][30] = 0x65; //e
	at_command[AT_CIPSTART][31] = 0x70; //p
	at_command[AT_CIPSTART][32] = 0x72; //r
	at_command[AT_CIPSTART][33] = 0x6F; //o
	at_command[AT_CIPSTART][34] = 0x6A; //j
	at_command[AT_CIPSTART][35] = 0x65; //e
	at_command[AT_CIPSTART][36] = 0x63; //c
	at_command[AT_CIPSTART][37] = 0x74; //t
	at_command[AT_CIPSTART][38] = 0x73; //s
	at_command[AT_CIPSTART][39] = 0x2E; //.
	at_command[AT_CIPSTART][40] = 0x69; //i
	at_command[AT_CIPSTART][41] = 0x6F; //o
	at_command[AT_CIPSTART][42] = 0x22; //"
	at_command[AT_CIPSTART][43] = 0x2C; //,
	at_command[AT_CIPSTART][44] = 0x31; //1
	at_command[AT_CIPSTART][45] = 0x38; //8
	at_command[AT_CIPSTART][46] = 0x38; //8
	at_command[AT_CIPSTART][47] = 0x33; //3
	at_command[AT_CIPSTART][48] = 0x0A; //enter

	at_command[AT_CIPSEND][0]  = 0x41; //A
	at_command[AT_CIPSEND][1]  = 0x54; //T
	at_command[AT_CIPSEND][2]  = 0x2B; //+
	at_command[AT_CIPSEND][3]  = 0x43; //C
	at_command[AT_CIPSEND][4]  = 0x49; //I
	at_command[AT_CIPSEND][5]  = 0x50; //P
	at_command[AT_CIPSEND][6]  = 0x53; //S
	at_command[AT_CIPSEND][7]  = 0x45; //E
	at_command[AT_CIPSEND][8]  = 0x4E; //N
	at_command[AT_CIPSEND][9]  = 0x44; //D
	at_command[AT_CIPSEND][10] = 0x0A; //enter

	at_command[AT_CIPCLOSE][0]   = 0x41; //A
	at_command[AT_CIPCLOSE][1]   = 0x54; //T
	at_command[AT_CIPCLOSE][2]   = 0x2B; //+
	at_command[AT_CIPCLOSE][3]   = 0x43; //C
	at_command[AT_CIPCLOSE][4]   = 0x49; //I
	at_command[AT_CIPCLOSE][5]   = 0x50; //P
	at_command[AT_CIPCLOSE][6]   = 0x43; //C
	at_command[AT_CIPCLOSE][7]   = 0x4C; //L
	at_command[AT_CIPCLOSE][8]   = 0x4F; //O
	at_command[AT_CIPCLOSE][9]   = 0x53; //S
	at_command[AT_CIPCLOSE][10]  = 0x45; //E
	at_command[AT_CIPCLOSE][11]  = 0x0A; //enter

	unsigned char mqtt_packt_msg[100] = {0};

	mqtt_packt_msg[0]  = 0x10; // mqtt connect packet
	mqtt_packt_msg[1]  = 0x13; // remaining length
	mqtt_packt_msg[2]  = 0x00; // protocol length
	mqtt_packt_msg[3]  = 0x04; // protocol length
	mqtt_packt_msg[4]  = 0x4D; // protocol M
	mqtt_packt_msg[5]  = 0x51; // protocol Q
	mqtt_packt_msg[6]  = 0x54; // protocol T
	mqtt_packt_msg[7]  = 0x54; // protocol T
	mqtt_packt_msg[8]  = 0x04; // level
	mqtt_packt_msg[9]  = 0x02; // flag
	mqtt_packt_msg[10] = 0x00; // KA
	mqtt_packt_msg[11] = 0x3C; // KA
	mqtt_packt_msg[12] = 0x00; // CID length
	mqtt_packt_msg[13] = 0x07; // CID length
	mqtt_packt_msg[14] = 0x63; // CID c
	mqtt_packt_msg[15] = 0x67; // CID g
	mqtt_packt_msg[16] = 0x70; // CID p
	mqtt_packt_msg[17] = 0x73; // CID s
	mqtt_packt_msg[18] = 0x30; // CID 0
	mqtt_packt_msg[19] = 0x30; // CID 0
	mqtt_packt_msg[20] = 0x31; // CID 1
	mqtt_packt_msg[21] = 0x30; // mqtt publish packet
	mqtt_packt_msg[22] = 0x38; // remaining length
	mqtt_packt_msg[23] = 0x00; // topic length
	mqtt_packt_msg[24] = 0x0B; // topic length
	mqtt_packt_msg[25] = 0x63; // topic c
	mqtt_packt_msg[26] = 0x6F; // topic o
	mqtt_packt_msg[27] = 0x6C; // topic l
	mqtt_packt_msg[28] = 0x65; // topic e
	mqtt_packt_msg[29] = 0x69; // topic i
	mqtt_packt_msg[30] = 0x72; // topic r
	mqtt_packt_msg[31] = 0x61; // topic a
	mqtt_packt_msg[32] = 0x5F; // topic _
	mqtt_packt_msg[33] = 0x67; // topic g
	mqtt_packt_msg[34] = 0x70; // topic p
	mqtt_packt_msg[35] = 0x73; // topic s


	//NAO TRANSMITIR ERROS POR ENQUANTO
	/*unsigned char mqtt_packt_error[50] = {0};

	mqtt_packt_error[0]  = 0x10; // mqtt connect packet
	mqtt_packt_error[1]  = 0x13; // remaining length
	mqtt_packt_error[2]  = 0x00; // protocol length
	mqtt_packt_error[3]  = 0x04; // protocol length
	mqtt_packt_error[4]  = 0x4D; // protocol M
	mqtt_packt_error[5]  = 0x51; // protocol Q
	mqtt_packt_error[6]  = 0x54; // protocol T
	mqtt_packt_error[7]  = 0x54; // protocol T
	mqtt_packt_error[8]  = 0x04; // level
	mqtt_packt_error[9]  = 0x02; // flag
	mqtt_packt_error[10] = 0x00; // KA
	mqtt_packt_error[11] = 0x3C; // KA
	mqtt_packt_error[12] = 0x00; // CID length
	mqtt_packt_error[13] = 0x07; // CID length
	mqtt_packt_error[14] = 0x63; // CID c
	mqtt_packt_error[15] = 0x67; // CID g
	mqtt_packt_error[16] = 0x70; // CID p
	mqtt_packt_error[17] = 0x73; // CID s
	mqtt_packt_error[18] = 0x30; // CID 0
	mqtt_packt_error[19] = 0x30; // CID 0
	mqtt_packt_error[20] = 0x31; // CID 1
	mqtt_packt_error[21] = 0x30; // mqtt publish packet
	mqtt_packt_error[22] = 0x0F; // remaining length
	mqtt_packt_error[23] = 0x00; // topic length
	mqtt_packt_error[24] = 0x0B; // topic length
	mqtt_packt_error[25] = 0x63; // topic c
	mqtt_packt_error[26] = 0x6F; // topic o
	mqtt_packt_error[27] = 0x6C; // topic l
	mqtt_packt_error[28] = 0x65; // topic e
	mqtt_packt_error[29] = 0x69; // topic i
	mqtt_packt_error[30] = 0x72; // topic r
	mqtt_packt_error[31] = 0x61; // topic a
	mqtt_packt_error[32] = 0x5F; // topic _
	mqtt_packt_error[33] = 0x67; // topic g
	mqtt_packt_error[34] = 0x70; // topic p
	mqtt_packt_error[35] = 0x73; // topic s*/

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
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, data_in_uart1, 400);
  HAL_UART_Receive_IT(&huart2, data_in_uart2, 60);

  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_RESET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_SET);
  HAL_Delay(30000);

  //INICIO VERIFICAÇÃO DE CONECTIVIDADE GPRS
  while (ctrl_creg_loop == false)
  {
	  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
	  HAL_Delay(500);
	  if (ctrl_uart2 == true)
	  {
		  for (uint8_t a = 0; a<=60; a++)
		  {
			  if ((data_in_uart2[a] == 0x2B) && (data_in_uart2[a+1] == 0x43) && (data_in_uart2[a+2] == 0x52) && (data_in_uart2[a+3] == 0x45)
					  && (data_in_uart2[a+4] == 0x47) && (data_in_uart2[a+5] == 0x3A))//filtrar dados +CREG:
  			  {
				  for(uint8_t b = 0; b<=10; b++)
				  {
					  data_filter_uart2[b] = data_in_uart2[a+b];
				  }
				  break;
  			  }
		  }
	  }
	  if ((data_filter_uart2[9] == 0x31) || (data_filter_uart2[9] == 0x35))
	  {
		  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
		  ctrl_creg_loop = true;
	  }
	  else
	  {
		  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
		  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
		  HAL_UART_Transmit(&huart1, (unsigned char *)error_msg_NC, 10, 100);
	  }
  }
  ctrl_creg_loop = false;
  //FIM VERIFICAÇÃO DE CONECTIVIDADE GPRS

  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS

  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT], strlen((const char *)at_command[AT]), 100);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CFUN], strlen((const char *)at_command[AT_CFUN]), 100);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CPIN], strlen((const char *)at_command[AT_CPIN]), 100);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSQ], strlen((const char *)at_command[AT_CSQ]), 100);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSTT], strlen((const char *)at_command[AT_CSTT]), 100);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIICR], strlen((const char *)at_command[AT_CIICR]), 100);
  HAL_Delay(5000);
  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIFSR], strlen((const char *)at_command[AT_CIFSR]), 100);
  HAL_Delay(5000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (ctrl_uart1 == true)
	  {
		  for (uint8_t i = 0; i<=400; i++)
		  	{
		  		if ((data_in_uart1[i] == 0x24) && (data_in_uart1[i+1] == 0x47) && (data_in_uart1[i+2] == 0x50) && (data_in_uart1[i+3] == 0x47)
		  				&& (data_in_uart1[i+4] == 0x4C) && (data_in_uart1[i+5] == 0x4C))//filtrar dados GPGLL
		  		{
		  			for(uint8_t j = 0; j<=43; j++)
		  			{
		  				data_filter_uart1[j] = data_in_uart1[i+j];
		  			}
		  			break;
		  		}
		  	}

		  if (data_filter_uart1[7] == 0x2C) //0x2C é o caractere ascii "," , ou seja, não há sinal de GPS
		  {
			  //INICIO VERIFICAÇÃO DE CONECTIVIDADE GPRS
			  while (ctrl_creg_loop == false)
			  {
				  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
			  	  HAL_Delay(500);
			  	  if (ctrl_uart2 == true)
			  	  {
			  		  for (uint8_t a = 0; a<=60; a++)
			  		  {
			  			  if ((data_in_uart2[a] == 0x2B) && (data_in_uart2[a+1] == 0x43) && (data_in_uart2[a+2] == 0x52) && (data_in_uart2[a+3] == 0x45)
			  					  && (data_in_uart2[a+4] == 0x47) && (data_in_uart2[a+5] == 0x3A))//filtrar dados +CREG:
			  			  {
			  				  for(uint8_t b = 0; b<=10; b++)
			  				  {
			  					  data_filter_uart2[b] = data_in_uart2[a+b];
			  				  }
			  				  break;
			  			  }
			  		  }
			  	  }
			  	  if ((data_filter_uart2[9] == 0x31) || (data_filter_uart2[9] == 0x35)) // se a resposta do CREG é '1' ou '5' (módulo registrado na rede)
			  	  {
			  		  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
			  		  ctrl_creg_loop = true;
			  	  }
			  	  else // se o módulo não estiver registrado na rede
			  	  {
			  		  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
			  		  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS

			  		  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
			  		  //reset SIM800L
			  		  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_RESET);
			  		  HAL_Delay(1000);
			  		  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_SET);
			  		  HAL_Delay(30000);

			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT], strlen((const char *)at_command[AT]), 100);
			  		  HAL_Delay(500);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CFUN], strlen((const char *)at_command[AT_CFUN]), 100);
			  		  HAL_Delay(500);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CPIN], strlen((const char *)at_command[AT_CPIN]), 100);
			  		  HAL_Delay(500);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
			  		  HAL_Delay(500);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSQ], strlen((const char *)at_command[AT_CSQ]), 100);
			  		  HAL_Delay(500);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSTT], strlen((const char *)at_command[AT_CSTT]), 100);
			  		  HAL_Delay(500);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIICR], strlen((const char *)at_command[AT_CIICR]), 100);
			  		  HAL_Delay(5000);
			  		  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIFSR], strlen((const char *)at_command[AT_CIFSR]), 100);
			  		  HAL_Delay(5000);
			  	  }
			  	  HAL_GPIO_TogglePin(LED_PC13_GPIO_Port, LED_PC13_Pin);
			  	  HAL_Delay(500);
			  }
			  ctrl_creg_loop = false;
			  //FIM VERIFICAÇÃO DE CONECTIVIDADE GPRS

			  // NÃO ENVIAR MAIS O SÍMBOLO DE 'NS - NO SIGNAL', EM VEZ DISSO, SINALIZAR O LED ERROR_1 PB5
			  /*HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPSTART], strlen((const char *)at_command[AT_CIPSTART]), 100);
			  HAL_Delay(5000);

			  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPSEND], strlen((const char *)at_command[AT_CIPSEND]), 100);
			  HAL_Delay(2000);

			  HAL_UART_Transmit(&huart2, (unsigned char *)mqtt_packt_error, 36, 100);

			  HAL_UART_Transmit(&huart2, (unsigned char *)error_msg_NS, strlen((const char *)error_msg_NS), 100);
			  HAL_UART_Transmit(&huart2, (unsigned char *)send_msg, strlen((const char *)send_msg), 100);
			  HAL_Delay(5000);

			  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPCLOSE], strlen((const char *)at_command[AT_CIPCLOSE]), 100);*/
			  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_SET);   // SIGNIFICA QUE O SIMBOLO DE NS DEVERIA TER SIDO ENVIADO
			  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE O SIMBOLO DE NS DEVERIA TER SIDO ENVIADO
			  HAL_Delay(5000);
		  }
		  else
		  {
			  if ((data_filter_uart1[31] == 0x2C) && ((data_filter_uart1[32] == 0x45) || (data_filter_uart1[32] == 0x57)) && ((data_filter_uart1[33] == 0x2C)))
				{
				  if ((data_filter_uart1[6] == 0x2C) && (data_filter_uart1[17] == 0x2C) && ((data_filter_uart1[18] == 0x4E) || (data_filter_uart1[18] == 0x53))
				  		  					&& (data_filter_uart1[19] == 0x2C))// gambiarra - SE O DADO DE GPS CAPTURADO FOR VÁLIDO, ELE É TRANSMITIDO
				  {
					  //INICIO VERIFICAÇÃO DE CONECTIVIDADE GPRS
					  while (ctrl_creg_loop == false)
					  {
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
						  HAL_Delay(500);
						  if (ctrl_uart2 == true)
						  {
							  for (uint8_t a = 0; a<=60; a++)
							  {
								  if ((data_in_uart2[a] == 0x2B) && (data_in_uart2[a+1] == 0x43) && (data_in_uart2[a+2] == 0x52) && (data_in_uart2[a+3] == 0x45)
										  && (data_in_uart2[a+4] == 0x47) && (data_in_uart2[a+5] == 0x3A))//filtrar dados +CREG:
					  			  {
									  for(uint8_t b = 0; b<=10; b++)
									  {
										  data_filter_uart2[b] = data_in_uart2[a+b];
									  }
									  break;
					  			  }
							  }
						  }
						  if ((data_filter_uart2[9] == 0x31) || (data_filter_uart2[9] == 0x35))
						  {
							  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
							  ctrl_creg_loop = true;
						  }
						  else
						  {
							  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
							  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS

							  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
							  //reset SIM800L
							  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_RESET);
							  HAL_Delay(1000);
							  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_SET);
							  HAL_Delay(30000);

							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT], strlen((const char *)at_command[AT]), 100);
							  HAL_Delay(500);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CFUN], strlen((const char *)at_command[AT_CFUN]), 100);
							  HAL_Delay(500);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CPIN], strlen((const char *)at_command[AT_CPIN]), 100);
							  HAL_Delay(500);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
							  HAL_Delay(500);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSQ], strlen((const char *)at_command[AT_CSQ]), 100);
							  HAL_Delay(500);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSTT], strlen((const char *)at_command[AT_CSTT]), 100);
							  HAL_Delay(500);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIICR], strlen((const char *)at_command[AT_CIICR]), 100);
							  HAL_Delay(5000);
							  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIFSR], strlen((const char *)at_command[AT_CIFSR]), 100);
							  HAL_Delay(5000);
						  }
						  HAL_GPIO_TogglePin(LED_PC13_GPIO_Port, LED_PC13_Pin);
						  HAL_Delay(500);
					  }
					  ctrl_creg_loop = false;
					  //FIM VERIFICAÇÃO DE CONECTIVIDADE GPRS

					  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPSTART], strlen((const char *)at_command[AT_CIPSTART]), 100);
					  HAL_Delay(5000);

					  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPSEND], strlen((const char *)at_command[AT_CIPSEND]), 100);
					  HAL_Delay(2000);

					  HAL_UART_Transmit(&huart2, (unsigned char *)mqtt_packt_msg, 36, 100);
					  HAL_UART_Transmit(&huart2, (unsigned char *)data_filter_uart1, 43, 100);
					  HAL_UART_Transmit(&huart2, (unsigned char *)send_msg, strlen((const char *)send_msg), 100);
					  HAL_Delay(5000);

					  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPCLOSE], strlen((const char *)at_command[AT_CIPCLOSE]), 100);
					  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE NAO HA ERRO DE NS - NO SIGNAL
					  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE NAO HA ERRO DE ID - INVALID DATA
					  HAL_Delay(5000);
				  }
				}
			  else
			  {
				  //INICIO VERIFICAÇÃO DE CONECTIVIDADE GPRS
				  while (ctrl_creg_loop == false)
				  {
					  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
					  HAL_Delay(500);
					  if (ctrl_uart2 == true)
					  {
						  for (uint8_t a = 0; a<=60; a++)
						  {
							  if ((data_in_uart2[a] == 0x2B) && (data_in_uart2[a+1] == 0x43) && (data_in_uart2[a+2] == 0x52) && (data_in_uart2[a+3] == 0x45)
									  && (data_in_uart2[a+4] == 0x47) && (data_in_uart2[a+5] == 0x3A))//filtrar dados +CREG:
				  			  {
								  for(uint8_t b = 0; b<=10; b++)
								  {
									  data_filter_uart2[b] = data_in_uart2[a+b];
								  }
								  break;
				  			  }
						  }
					  }
					  if ((data_filter_uart2[9] == 0x31) || (data_filter_uart2[9] == 0x35))
					  {
						  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
						  ctrl_creg_loop = true;
					  }
					  else
					  {
						  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
						  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS

						  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
						  //reset SIM800L
						  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_RESET);
						  HAL_Delay(1000);
						  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_SET);
						  HAL_Delay(30000);

						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT], strlen((const char *)at_command[AT]), 100);
						  HAL_Delay(500);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CFUN], strlen((const char *)at_command[AT_CFUN]), 100);
						  HAL_Delay(500);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CPIN], strlen((const char *)at_command[AT_CPIN]), 100);
						  HAL_Delay(500);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
						  HAL_Delay(500);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSQ], strlen((const char *)at_command[AT_CSQ]), 100);
						  HAL_Delay(500);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSTT], strlen((const char *)at_command[AT_CSTT]), 100);
						  HAL_Delay(500);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIICR], strlen((const char *)at_command[AT_CIICR]), 100);
						  HAL_Delay(5000);
						  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIFSR], strlen((const char *)at_command[AT_CIFSR]), 100);
						  HAL_Delay(5000);
					  }
	  			  	  HAL_GPIO_TogglePin(LED_PC13_GPIO_Port, LED_PC13_Pin);
	  			  	  HAL_Delay(500);
				  }
				  ctrl_creg_loop = false;
				  //FIM VERIFICAÇÃO DE CONECTIVIDADE GPRS

				  // NÃO ENVIAR MAIS O SÍMBOLO DE 'NS - NO SIGNAL', EM VEZ DISSO, SINALIZAR O LED ERROR_1 PB5
				  /*HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPSTART], strlen((const char *)at_command[AT_CIPSTART]), 100);
				  HAL_Delay(5000);

				  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPSEND], strlen((const char *)at_command[AT_CIPSEND]), 100);
				  HAL_Delay(2000);
	  			  HAL_UART_Transmit(&huart2, (unsigned char *)mqtt_packt_error, 36, 100);

				  HAL_UART_Transmit(&huart2, (unsigned char *)error_msg_ID, strlen((const char *)error_msg_ID), 100);
				  HAL_UART_Transmit(&huart2, (unsigned char *)send_msg, strlen((const char *)send_msg), 100);
				  HAL_Delay(5000);

				  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIPCLOSE], strlen((const char *)at_command[AT_CIPCLOSE]), 100);*/
				  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE O SIMBOLO DE ID DEVERIA TER SIDO ENVIADO
				  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE O SIMBOLO DE ID DEVERIA TER SIDO ENVIADO
				  HAL_Delay(5000);
			  }
		  }
		  ctrl_uart1 = false;

		  //RESETAR O SIM800L A CADA 15 CICLOS PARA EVITAR QUE FIQUE PRESO EM ERROS
		  cont_loop1++;
		  if (cont_loop1 == 15)
		  {
			  for(uint8_t i = 0; i<100; i++)
			  {
				  HAL_GPIO_TogglePin(ERROR_1_GPIO_Port, ERROR_1_Pin);
				  HAL_GPIO_TogglePin(ERROR_2_GPIO_Port, ERROR_2_Pin);
				  HAL_Delay(100);
			  }
			  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_RESET);

			  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_RESET); // SIGNIFICA QUE O SIMBOLO DE ID DEVERIA TER SIDO ENVIADO
			  	HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE O SIMBOLO DE ID DEVERIA TER SIDO ENVIADO

			  	HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_RESET);
			    HAL_Delay(1000);
			    HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_SET);
			    HAL_Delay(30000);

			    //INICIO VERIFICAÇÃO DE CONECTIVIDADE GPRS
			    while (ctrl_creg_loop == false)
			    {
			  	  HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
			  	  HAL_Delay(500);
			  	  if (ctrl_uart2 == true)
			  	  {
			  		  for (uint8_t a = 0; a<=60; a++)
			  		  {
			  			  if ((data_in_uart2[a] == 0x2B) && (data_in_uart2[a+1] == 0x43) && (data_in_uart2[a+2] == 0x52) && (data_in_uart2[a+3] == 0x45)
			  					  && (data_in_uart2[a+4] == 0x47) && (data_in_uart2[a+5] == 0x3A))//filtrar dados +CREG:
			    			  {
			  				  for(uint8_t b = 0; b<=10; b++)
			  				  {
			  					  data_filter_uart2[b] = data_in_uart2[a+b];
			  				  }
			  				  break;
			    			  }
			  		  }
			  	  }
			  	  if ((data_filter_uart2[9] == 0x31) || (data_filter_uart2[9] == 0x35))
			  	  {
			  		  HAL_UART_Transmit(&huart1, (unsigned char *)data_filter_uart2, 10, 100);
			  		  ctrl_creg_loop = true;
			  	  }
			  	  else
			  	  {
			  		  HAL_GPIO_WritePin(ERROR_1_GPIO_Port, ERROR_1_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
			  		  HAL_GPIO_WritePin(ERROR_2_GPIO_Port, ERROR_2_Pin, GPIO_PIN_SET); // SIGNIFICA QUE HA PROBLEMA DE CONECTIVIDADE GPRS
			  		  HAL_UART_Transmit(&huart1, (unsigned char *)error_msg_NC, 10, 100);
			  	  }
			    }
			    ctrl_creg_loop = false;
			    //FIM VERIFICAÇÃO DE CONECTIVIDADE GPRS

			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT], strlen((const char *)at_command[AT]), 100);
			    HAL_Delay(500);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CFUN], strlen((const char *)at_command[AT_CFUN]), 100);
			    HAL_Delay(500);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CPIN], strlen((const char *)at_command[AT_CPIN]), 100);
			    HAL_Delay(500);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CREG], strlen((const char *)at_command[AT_CREG]), 100);
			    HAL_Delay(500);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSQ], strlen((const char *)at_command[AT_CSQ]), 100);
			    HAL_Delay(500);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CSTT], strlen((const char *)at_command[AT_CSTT]), 100);
			    HAL_Delay(500);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIICR], strlen((const char *)at_command[AT_CIICR]), 100);
			    HAL_Delay(5000);
			    HAL_UART_Transmit(&huart2, (unsigned char *)at_command[AT_CIFSR], strlen((const char *)at_command[AT_CIFSR]), 100);
			    HAL_Delay(5000);

			  cont_loop1 = 0;
		  }
	  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 9600;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PC13_GPIO_Port, LED_PC13_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RESET_SIM800l_GPIO_Port, RESET_SIM800l_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ERROR_1_Pin|ERROR_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_PC13_Pin */
  GPIO_InitStruct.Pin = LED_PC13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PC13_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RESET_SIM800l_Pin */
  GPIO_InitStruct.Pin = RESET_SIM800l_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RESET_SIM800l_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ERROR_1_Pin ERROR_2_Pin */
  GPIO_InitStruct.Pin = ERROR_1_Pin|ERROR_2_Pin;
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

