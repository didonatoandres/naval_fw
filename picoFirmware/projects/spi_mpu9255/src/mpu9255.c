/*
 * mpu9255.c
 *
 *  Created on: Sep 5, 2016
 *      Author: sfmaudet
 */
#include "i2c.h"
#include "mpu9255.h"
#include "spi.h"

#define FRAME_SIZE		7
#define FIFO_SIZE 		512

static uint8_t gyro_conf = MPU9255_GYRO_FS_250;
static uint8_t accel_conf = MPU9255_ACCEL_FS_2;

uint8_t fifoData[FIFO_SIZE + 1];

uint8_t flag2=0;



static void intPinConfig(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;
	tx[0] = MPU9255_INT_PIN_CFG | SPI_WRITE;
	tx[1] = 0x30; // 7 Active High - 6 Int Pin PUSH PULL - 5 INT High until read - 4 Cleared at any read operation - 3/0 All 0.
	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}

static void sampleRateDivConfig(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_SMPLRT_DIV | SPI_WRITE;
	tx[1] = 0;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}


static void configRegister(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_CONFIG | SPI_WRITE;
	tx[1] = 0x41; // 7 Reserverd - 6 FIFO_MODE 0 - 5/3 EXT_SYNC_SET Disabled - 2:0 DLPF 1dec. Fs 1KHz

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}

static void intEnable(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_INT_ENABLE | SPI_WRITE;
	tx[1] = 0x01;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}

static void fifoEnable(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_USER_CTRL | SPI_WRITE;	//FIFO ENABLE MEM DIR Register
	tx[1] = MPU9255_USERCTRL_FIFO_EN_BIT;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

}



void fifoDisable(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_USER_CTRL | SPI_WRITE;	//FIFO ENABLE MEM DIR Register
	tx[1] = 0x00;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

}

static void fifoGyroAccel_enable(void){

	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_FIFO_EN | SPI_WRITE;	//FIFO ENABLE MEM DIR Register
	tx[1] = MPU9255_XG_FIFO_EN_BIT | MPU9255_YG_FIFO_EN_BIT | MPU9255_ZG_FIFO_EN_BIT | MPU9255_ACCEL_FIFO_EN_BIT; //Gyro & Accel XYZ Enabled in FIFO

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}


void fifoGyroAccel_disable(void){

	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_FIFO_EN | SPI_WRITE;	//FIFO ENABLE MEM DIR Register
	tx[1] = 0x00; //Gyro & Accel XYZ Enabled in FIFO

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

}



void fifoGetCount(uint16_t * fifoCount){
	uint8_t rx[3],tx[3];
	uint8_t len = 3;

	tx[0] = MPU9255_FIFO_COUNTH | SPI_READ;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

	*fifoCount = (uint16_t)((rx[1] << 8) | rx[2]);
}

void fifoReadBurst(void){
	uint8_t tx[513];
	uint16_t len = 513;

	tx[0] = MPU9255_FIFO_R_W | SPI_READ;

	cs_low();
	SPI_tranfer(fifoData,tx,len);
	cs_high();
}



uint8_t fifoReadByte(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_FIFO_R_W | SPI_READ;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

	return rx[1];
}

void fifoReset(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_USER_CTRL | SPI_READ;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

	tx[0] = rx[1] | MPU9255_USERCTRL_FIFO_RESET_BIT | SPI_WRITE;

	len = 1;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();


}



static void accLPF_1130(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_ACCEL_CONFIG_2 | SPI_WRITE;
	tx[1] = 0x01;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();


}


static void accLPF_460(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_ACCEL_CONFIG_2 | SPI_WRITE;
	tx[1] = MPU_9255_ACC_DLPF_BW_460;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();


}

static void accLPF_184(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_ACCEL_CONFIG_2 | SPI_WRITE;
	tx[1] = MPU_9255_ACC_DLPF_BW_184;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

}

static void accLPF_5(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_ACCEL_CONFIG_2 | SPI_WRITE;
	tx[1] = MPU_9255_ACC_DLPF_BW_184;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

}


static void initGeneral(){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	tx[0] = MPU9255_USER_CTRL | SPI_WRITE;
	tx[1] = 0x00; // 7 Reserver - 6Disable FIFO -5 I2CMaster Disable - 4 I2C_IF_DIS Disable - 3 Reserved 0 - 2 FIFO_RESET 0  - 1 I2C_MST_RST Disable - 0 SIG_COND_RST  Disabled.

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

}



static void initAcell(void){
	uint8_t rx[2],tx[2];
	uint8_t len = 2;

	accLPF_1130();

	tx[0] = MPU9255_ACCEL_CONFIG | SPI_WRITE; //Set Accel Config Reg Address.
	tx[1] = 0xE0;  //Enable self-test and set 2g as first configuration.

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}



static void initGyro(void){

	uint8_t rx[2],tx[2];
	uint8_t len = 2;
	tx[0] = MPU9255_GYRO_CONFIG | SPI_WRITE;
	tx[1] = 0xE0;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();
}



void fifoDataSplit(float * gyro_data,float * accel_data){

		accel_data[0] = (((float)((int16_t)(fifoData[1] << 8) | fifoData[2]))/ACCEL_2g_LSB) - 0.454346;
		accel_data[1] = (((float)((int16_t)(fifoData[3] << 8) | fifoData[4]))/ACCEL_2g_LSB) - 0.454346;
		accel_data[2] = (((float)((int16_t)(fifoData[5] << 8) | fifoData[6]))/ACCEL_2g_LSB) - 0.484863;

		gyro_data[0] = ((float)((int16_t)(fifoData[7] << 8) | fifoData[8])/GYRO_250_LSB) - 131.2;
		gyro_data[1] = ((float)((int16_t)(fifoData[9] << 8) | fifoData[10])/GYRO_250_LSB) - 148.5;
		gyro_data[2] = ((float)((int16_t)(fifoData[11]<< 8) | fifoData[12])/GYRO_250_LSB) - 191.0;

}



void initMPU9255 (void){
	intPinConfig();
	initGeneral();
	sampleRateDivConfig();
	intEnable();
	configRegister();
	initAcell();
	initGyro();

	//fifoGyroAccel_enable();
	//fifoEnable();

}



void readGyroData(float giro_data[]){
	uint8_t rx[FRAME_SIZE],tx[FRAME_SIZE];

	uint8_t len = FRAME_SIZE;

	tx[0] = MPU9255_GYRO_XOUT_H | SPI_READ;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

	switch(gyro_conf){

	case MPU9255_GYRO_FS_250:
		giro_data[0] = ((float)((int16_t)(rx[1] << 8) | rx[2])/GYRO_250_LSB) - 131.2;
		giro_data[1] = ((float)((int16_t)(rx[3] << 8) | rx[4])/GYRO_250_LSB) - 148.5;
		giro_data[2] = ((float)((int16_t)(rx[5] << 8) | rx[6])/GYRO_250_LSB) - 191.0;
		break;
	case MPU9255_GYRO_FS_500:
		giro_data[0] = ((float)((int16_t)(rx[1] << 8) | rx[2])/GYRO_500_LSB) - 131.2;
		giro_data[1] = ((float)((int16_t)(rx[3] << 8) | rx[4])/GYRO_500_LSB) - 148.5;
		giro_data[2] = ((float)((int16_t)(rx[5] << 8) | rx[6])/GYRO_500_LSB) - 191.0;
		break;
	case MPU9255_GYRO_FS_1000:
		giro_data[0] = ((float)((int16_t)(rx[1] << 8) | rx[2])/GYRO_1000_LSB) - 131.2;
		giro_data[1] = ((float)((int16_t)(rx[3] << 8) | rx[4])/GYRO_1000_LSB) - 148.5;
		giro_data[2] = ((float)((int16_t)(rx[5] << 8) | rx[6])/GYRO_1000_LSB) - 191.0;
		break;
	case MPU9255_GYRO_FS_2000:
		giro_data[0] = ((float)((int16_t)(rx[1] << 8) | rx[2])/GYRO_2000_LSB) - 131.2;
		giro_data[1] = ((float)((int16_t)(rx[3] << 8) | rx[4])/GYRO_2000_LSB) - 148.5;
		giro_data[2] = ((float)((int16_t)(rx[5] << 8) | rx[6])/GYRO_2000_LSB) - 191.0;
		break;
	}

}


void readAccelData(uint8_t  * accel_data){

	uint8_t rx[FRAME_SIZE],tx[FRAME_SIZE];

	uint8_t len = FRAME_SIZE;

	tx[0] = MPU9255_ACCEL_XOUT_H | SPI_READ;

	cs_low();
	SPI_tranfer(rx,tx,len);
	cs_high();

	accel_data = rx;

	return;
}



void readMagnetData(float mag_data[]){



}



void readMagnetCode(uint8_t * mag_data){



}





/*
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
*/













