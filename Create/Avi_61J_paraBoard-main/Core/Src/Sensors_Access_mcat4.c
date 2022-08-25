/*
 * Sensors_Access.c
 *ICM20602,LPS22HB���g�p���邽�߂̊֐��Q
 *  Created on: 2020/05/07
 *      Author: tomoki
 */
#include <stdio.h>
#include <Sensors_Access_mcat4.h>
#include "main.h"
#include "stm32f4xx_hal.h"

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;
//ICM20602 Define
#define ICM_SPI hspi3 //ICM SPI
#define ICM_Data_Adress 0x3B //�Z���f�[�^��ۊǂ��郌�W�X�^�̃A�h���X
#define ICM_WakeUp_Adress 0x6B //ICM20602�X���[�v�����A�h���X
#define ICM_Accelerometer_Setting_Adress 0x1B //ICM20602�����x�����W�ݒ�A�h���X
#define ICM_Gyro_Setting_Adress 0x1C //ICM20602�p���x�����W�ݒ�A�h���X
#define ICM_WhoAmI_Adress 0x75//WhoAmI�̃A�h���X
#define ICM_16G 0x18
#define ICM_2500deg 0x18
//LPS22HB�̏��萔
#define LPS_SPI hspi3 //LPS SPI
#define LPS_Data_Adress 0x28 //�C���f�[�^��ۊǂ��郌�W�X�^�̃A�h���X
#define LPS_WakeUp_Adress 0x10 //LPS22HB�X���[�v�����A�h���X
#define LPS_WakeUp_Value 0x40 //LPS22HB�X���[�v������������
#define LPS_WhoAmI_Adress 0x0F//WhoAmI�̃A�h���X
//MPU9250 Define
#define MPU_SPI hspi3//MPU SPI
#define MPU_Data_Adress 0xBB
#define MPU_WakeUp_Adress 0x6B
#define MPU_Accelerometer_Setting_Adress 0x1C
#define MPU_Gyro_Setting_Adress 0x1B
#define MPU_FullScale 0x18//16g
#define MPU_WhoAmI_Adress 0x75//return 0x71
//CS�s���͂����Őݒ�

void ICM_CS_Low(){
#ifdef ICMCS_GPIO_Port
	HAL_GPIO_WritePin(ICMCS_GPIO_Port,ICMCS_Pin,RESET);
#endif
}
void ICM_CS_High(){
#ifdef ICMCS_GPIO_Port
	HAL_GPIO_WritePin(ICMCS_GPIO_Port,ICMCS_Pin,SET);
#endif
}


void LPS_CS_Low(){
#ifdef LPSCS_GPIO_Port
    HAL_GPIO_WritePin(LPSCS_GPIO_Port,LPSCS_Pin,RESET);
#endif
}
void LPS_CS_High(){
#ifdef LPSCS_GPIO_Port
	HAL_GPIO_WritePin(LPSCS_GPIO_Port,LPSCS_Pin,SET);
#endif
}


void MPU_CS_Low(){
#ifdef MPUCS_GPIO_Port
	HAL_GPIO_WritePin(MPUCS_GPIO_Port,MPUCS_Pin,RESET);
#endif
}
void MPU_CS_High(){
#ifdef MPUCS_GPIO_Port
	HAL_GPIO_WritePin(MPUCS_GPIO_Port,MPUCS_Pin,SET);
#endif
}

//�ȉ��}�C�R���̐ݒ�Ɉˑ����Ȃ��֐�
void SPI_Transmit(uint8_t value, SPI_HandleTypeDef spi){
	HAL_SPI_Transmit(&spi,(uint8_t*)&value,1,100);
}

void SPI_TransmitRecieve(uint8_t RecieveData[],int Size, SPI_HandleTypeDef spi){
	uint8_t DummyData = 0x00;
	HAL_SPI_TransmitReceive(&spi, &DummyData, RecieveData,Size,200);
}

void ICM_Write_Byte(uint8_t Adress,uint8_t Value){
	ICM_CS_Low();
	SPI_Transmit(Adress,ICM_SPI);
	SPI_Transmit(Value,ICM_SPI);
	ICM_CS_High();
}

void ICM_Read_Bytes(uint8_t Adress,uint8_t RecieveData[], int Size){
	Adress = Adress | 0x80;

	ICM_CS_Low();
	SPI_Transmit(Adress,ICM_SPI);
	SPI_TransmitRecieve(RecieveData,Size,ICM_SPI);
	ICM_CS_High();
}

void LPS_Write_Byte(uint8_t Adress,uint8_t Value){
	LPS_CS_Low();
	SPI_Transmit(Adress,LPS_SPI);
	SPI_Transmit(Value,LPS_SPI);
	LPS_CS_High();
}

void LPS_Read_Bytes(uint8_t Adress,uint8_t RecieveData[], int Size){
	Adress = Adress | 0x80;

	LPS_CS_Low();
	SPI_Transmit(Adress,LPS_SPI);
	SPI_TransmitRecieve(RecieveData,Size,LPS_SPI);
	LPS_CS_High();
}

void MPU_Write_Byte(uint8_t Adress,uint8_t Value){
	MPU_CS_Low();
	SPI_Transmit(Adress,MPU_SPI);
	SPI_Transmit(Value,MPU_SPI);
	MPU_CS_High();
}

void MPU_Read_Bytes(uint8_t Adress,uint8_t RecieveData[],int Size){
	Adress=Adress|0x80;
	MPU_CS_Low();
	SPI_Transmit(Adress,MPU_SPI);
	SPI_TransmitRecieve(RecieveData,Size,MPU_SPI);
	MPU_CS_High();
}

uint8_t ICM_WhoAmI(){
	uint8_t RecieveData[1];
	ICM_Read_Bytes(ICM_WhoAmI_Adress,RecieveData,1);
	return RecieveData[0];
}

uint8_t LPS_WhoAmI(){
	uint8_t RecieveData[1];
	LPS_Read_Bytes(LPS_WhoAmI_Adress,RecieveData,1);
	return RecieveData[0];
}

uint8_t MPU_WhoAmI(){
	uint8_t RecieveData[1];
	MPU_Read_Bytes(MPU_WhoAmI_Adress,RecieveData,1);
	return RecieveData[0];
}

//ICM20602�����ݒ�
void ICM_Initialization() {
	ICM_Write_Byte(ICM_WakeUp_Adress, 0x00); //�X���[�v����
	ICM_Write_Byte(ICM_Accelerometer_Setting_Adress, ICM_16G); //�����x�����W�ő�ɐݒ�
	ICM_Write_Byte(ICM_Gyro_Setting_Adress, ICM_2500deg); //�p���x�����W�ő�ɐݒ�
}


void ICM_6axis_Raw(int16_t ICM_Raw_Data[6]) {
	uint8_t RecieveData[14];
	ICM_Read_Bytes(ICM_Data_Adress,RecieveData,14);

	ICM_Raw_Data[0] = (RecieveData[0] << 8 | RecieveData[1] );
	ICM_Raw_Data[1] = (RecieveData[2] << 8 | RecieveData[3] );
	ICM_Raw_Data[2] = (RecieveData[4] << 8 | RecieveData[5] );
	ICM_Raw_Data[3] = (RecieveData[8] << 8 | RecieveData[9] );
	ICM_Raw_Data[4] = (RecieveData[10] << 8 | RecieveData[11] );
	ICM_Raw_Data[5] = (RecieveData[12] << 8 | RecieveData[13] );
}
void ICM_14Byte(uint8_t RecieveData[14]){
	ICM_Read_Bytes(ICM_Data_Adress,RecieveData,14);
	return;
}

void ICM_Calibration(int16_t Gyro_Calibrate_Data[3]) {
	int16_t ICM_Raw_Data[6];
	int CalibrateCount = 0;
	Gyro_Calibrate_Data[0] = 0;
	Gyro_Calibrate_Data[1] = 0;
	Gyro_Calibrate_Data[2] = 0;

	for(int i = 0;i<8000; i++) {
		ICM_6axis_Raw(ICM_Raw_Data);
		if(i>10){
			Gyro_Calibrate_Data[0] += ICM_Raw_Data[3];
			Gyro_Calibrate_Data[1] += ICM_Raw_Data[4];
			Gyro_Calibrate_Data[2] += ICM_Raw_Data[5];
			CalibrateCount++;
		}
		HAL_Delay(1);
	}
	Gyro_Calibrate_Data[0] = Gyro_Calibrate_Data[0]/CalibrateCount;
	Gyro_Calibrate_Data[1] = Gyro_Calibrate_Data[1]/CalibrateCount;
	Gyro_Calibrate_Data[2] = Gyro_Calibrate_Data[2]/CalibrateCount;
}

//LPS22HB�����ݒ�
void LPS_WakeUp() {
	LPS_Write_Byte(LPS_WakeUp_Adress,LPS_WakeUp_Value);//�X���[�v�����A�o��50Hz
}

uint32_t LPS_RawandPressure(uint8_t RecieveData[3]) {
	uint32_t PressureRaw;
	LPS_Read_Bytes(LPS_Data_Adress,RecieveData,3);
	PressureRaw = (uint32_t)RecieveData[2] << 16 | (uint16_t)RecieveData[1] << 8 | RecieveData[0];
	return PressureRaw;
}

uint32_t LPS_Pressure_Raw() {
	uint32_t PressureRaw;
	uint8_t RecieveData[3];
	LPS_Read_Bytes(LPS_Data_Adress,RecieveData,3);
	PressureRaw = (uint32_t)RecieveData[2] << 16 | (uint16_t)RecieveData[1] << 8 | RecieveData[0];
	return PressureRaw;
}

//MPU9250 Functions
void MPU_Initialization(){
	MPU_Write_Byte(MPU_WakeUp_Adress,0x00);
	MPU_Write_Byte(MPU_Accelerometer_Setting_Adress,MPU_FullScale);
	MPU_Write_Byte(MPU_Gyro_Setting_Adress,MPU_FullScale);
}

void MPU_6axis_Raw(uint8_t Recieve_Data[14],int16_t MPU_Raw_Data[6]){
	MPU_Read_Bytes(0x80|MPU_Data_Adress,Recieve_Data,14);

	MPU_Raw_Data[0]=(Recieve_Data[0]<<8|Recieve_Data[1]);
	MPU_Raw_Data[1]=(Recieve_Data[2]<<8|Recieve_Data[3]);
	MPU_Raw_Data[2]=(Recieve_Data[4]<<8|Recieve_Data[5]);
	MPU_Raw_Data[3]=(Recieve_Data[8]<<8|Recieve_Data[9]);
	MPU_Raw_Data[4]=(Recieve_Data[10]<<8|Recieve_Data[11]);
	MPU_Raw_Data[5]=(Recieve_Data[12]<<8|Recieve_Data[13]);
}

void MPU_6axis(uint16_t MPU_Data[6]){
	uint8_t Recieve_Data[14];
	uint16_t MPU_Raw_Data[6];
	MPU_6axis_Raw(Recieve_Data,MPU_Raw_Data);
	for(int8_t i=0;i<6;i++){
		MPU_Data[i]=MPU_Raw_Data[i] *16.0*10/32764.0;
	}
}
