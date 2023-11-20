/*
 * fsm.c
 *
 *  Created on: Nov 10, 2023
 *      Author: yshic
 */
#include "fsm.h"

int status = INIT;
int command_flag = 0;
uint8_t temp = 0;
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint32_t ADC_value = 0;
char str[100];

void clear_buffer(){
	memset(buffer,0,sizeof(buffer));
	index_buffer=0;
}

void command_parser_fsm(){
	switch(status){
	case INIT:
		if(temp == '!'){
			status = RECEIVE;
			clear_buffer();
		}
	case RECEIVE:
		if(strcmp(buffer, "RST#") == 0){
			command_flag = 1;
			HAL_ADC_Start(&hadc1);
			ADC_value = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
			status = INIT;
			clear_buffer();
		}
		else if(strcmp(buffer, "OK#") == 0){
			command_flag = 0;
			status = INIT;
			timer_flag[0] = 1;
			clear_buffer();
		}
		break;
	default:
		break;
	}
}

void uart_communication_fsm(){
	switch(command_flag){
	case 1:
		if(timer_flag[0] == 1){
			HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
			HAL_UART_Transmit(&huart2, "\r\n !ADC=",10,1000);
			HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "%d#\r\n", ADC_value), 1000);
			setTimer(3000, 0);
		}
		break;
	default:
		break;
	}
}
