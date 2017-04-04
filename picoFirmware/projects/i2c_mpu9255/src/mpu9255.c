/*
 * mpu9255.c
 *
 *  Created on: Sep 5, 2016
 *      Author: sfmaudet
 */
#include "i2c.h"
#include "mpu9255.h"


#define BUFFER_SIZE		7

	static uint8_t gyro_conf = MPU9255_GYRO_FS_250;
	static uint8_t accel_conf = MPU9255_ACCEL_FS_2;

	static void initAcell(void){
		uint8_t rx[BUFFER_SIZE],tx[BUFFER_SIZE];
		uint8_t txSize,rxSize;

		tx[0] = MPU9255_ACCEL_CONFIG; //Set Accel Config Reg Address.
		tx[1] = MPU9255_ACCEL_Cten | MPU9255_ACCEL_FS_2;  //Enable self-test and set 2g as first configuration.

		rxSize = 0;
		txSize = 2;

		SetupXferRecAndExecute(I2C_ADDR_7BIT,tx,txSize,rx,rxSize);
	}



	static void initGyro(void){
		uint8_t rx[BUFFER_SIZE],tx[BUFFER_SIZE];
		uint8_t txSize,rxSize;

		tx[0] = MPU9255_GYRO_CONFIG; //Set Accel Config Reg Address.
		tx[1] = MPU9255_GYRO_Cten | MPU9255_GYRO_FS_250;
		rxSize = 0;
		txSize = 2;
		SetupXferRecAndExecute(I2C_ADDR_7BIT,tx,txSize,rx,rxSize);
	}


	static void initMagnet(void){
		uint8_t tx[BUFFER_SIZE],rx[BUFFER_SIZE];
		uint8_t txSize = 2,rxSize = 0;

		tx[0] = MPU9255_MAG_CNTL1;
		tx[1] = MAG_CNTL1_MODE_1_BIT | MAG_CNTL1_MODE_2_BIT | MAG_CNTL1_16_BIT ;

		SetupXferRecAndExecute(I2C_ADDR_MAG,tx,txSize,rx,rxSize);

	}



	static void initGeneral(){
		uint8_t rx[BUFFER_SIZE],tx[BUFFER_SIZE];
		uint8_t txSize = 2,rxSize = 0;


		tx[0] = MPU9255_USER_CTRL;
		tx[1] = 0x00;

		SetupXferRecAndExecute(I2C_ADDR_7BIT,tx,txSize,rx,rxSize);

		tx[0] = MPU9255_INT_PIN_CFG;
		tx[1] = 0x02;

		SetupXferRecAndExecute(I2C_ADDR_7BIT,tx,txSize,rx,rxSize);
	}



	void initMPU9255 (void){
		initGeneral();
		initAcell();
		initGyro();
		initMagnet();
	}


	void readGyroData(float giro_data[]){
		uint8_t rx[BUFFER_SIZE],tx[BUFFER_SIZE];
		uint8_t txSize = 1,rxSize = 6;

		tx[0] = MPU9255_GYRO_XOUT_H;

		SetupXferRecAndExecute(I2C_ADDR_7BIT,tx,txSize,rx,rxSize);

		switch(gyro_conf){

		case MPU9255_GYRO_FS_250:
			giro_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1])/GYRO_250_LSB) - 131.2;
			giro_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3])/GYRO_250_LSB) - 148.5;
			giro_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5])/GYRO_250_LSB) - 191.0;
			break;
		case MPU9255_GYRO_FS_500:
			giro_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1])/GYRO_500_LSB) - 131.2;
			giro_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3])/GYRO_500_LSB) - 148.5;
			giro_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5])/GYRO_500_LSB) - 191.0;
			break;
		case MPU9255_GYRO_FS_1000:
			giro_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1])/GYRO_1000_LSB) - 131.2;
			giro_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3])/GYRO_1000_LSB) - 148.5;
			giro_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5])/GYRO_1000_LSB) - 191.0;
			break;
		case MPU9255_GYRO_FS_2000:
			giro_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1])/GYRO_2000_LSB) - 131.2;
			giro_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3])/GYRO_2000_LSB) - 148.5;
			giro_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5])/GYRO_2000_LSB) - 191.0;
			break;

		}
	}


	void readAccelData(float accel_data[]){
		uint8_t rx[BUFFER_SIZE],tx[BUFFER_SIZE];
		uint8_t txSize = 1,rxSize = 6;

		tx[0] = MPU9255_ACCEL_XOUT_H ;

		SetupXferRecAndExecute(I2C_ADDR_7BIT,tx,txSize,rx,rxSize);

		switch(accel_conf){
		case MPU9255_ACCEL_FS_2:
			accel_data[0] = (((float)((int16_t)(rx[0] << 8) | rx[1]))/ACCEL_2g_LSB) - 0.454346;
			accel_data[1] = (((float)((int16_t)(rx[2] << 8) | rx[3]))/ACCEL_2g_LSB) - 0.454346;
			accel_data[2] = (((float)((int16_t)(rx[4] << 8) | rx[5]))/ACCEL_2g_LSB) - 0.484863;
			break;
		case MPU9255_ACCEL_FS_4:
			accel_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1]))/ACCEL_4g_LSB;
			accel_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3]))/ACCEL_4g_LSB;
			accel_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5]))/ACCEL_4g_LSB;
			break;
		case MPU9255_ACCEL_FS_8:
			accel_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1]))/ACCEL_8g_LSB;
			accel_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3]))/ACCEL_8g_LSB;
			accel_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5]))/ACCEL_8g_LSB;
			break;
		case MPU9255_ACCEL_FS_16:
			accel_data[0] = ((float)((int16_t)(rx[0] << 8) | rx[1]))/ACCEL_16g_LSB;
			accel_data[1] = ((float)((int16_t)(rx[2] << 8) | rx[3]))/ACCEL_16g_LSB;
			accel_data[2] = ((float)((int16_t)(rx[4] << 8) | rx[5]))/ACCEL_16g_LSB;
			break;
		}
	}

	void readMagnetData(float mag_data[]){
		uint8_t rx[BUFFER_SIZE],tx[BUFFER_SIZE];
		uint8_t txSize = 1,rxSize = 7;

		tx[0] = MPU9255_MAG_HXL ;

		SetupXferRecAndExecute(I2C_ADDR_MAG,tx,txSize,rx,rxSize);


		mag_data[0] = (((float)((int16_t)(rx[1] << 8) | rx[0]))*MAG_LSB);
		mag_data[1] = (((float)((int16_t)(rx[3] << 8) | rx[2]))*MAG_LSB);
		mag_data[2] = (((float)((int16_t)(rx[5] << 8) | rx[4]))*MAG_LSB);

}

	void readMagnetCode(uint8_t * mag_data){
		uint8_t tx[BUFFER_SIZE];
		uint8_t txSize = 1,rxSize = 1;

		tx[0] = 0x00 ;

		SetupXferRecAndExecute(I2C_ADDR_MAG,tx,txSize,mag_data,rxSize);


}














