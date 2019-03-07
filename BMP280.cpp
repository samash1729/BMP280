#include "BMP280.h"
#include "../I2cPort/I2cPort.h"
#include <math.h>
#include <unistd.h>
#include <iostream>

using namespace std;
BMP280::BMP280()
{
	this->i2c = new I2cPort(BMP280_ADDRESS,2);
}

void BMP280::set_ref_pressure()
{
	long long int sum = 0 ;	
	for (int j=0;j<100;++j)
	{
		getTemperature();
		int32_t val=getPressure();
		sum+=val;
		usleep(7000);
	}
	ref_pressure = (int32_t)(sum/100);
}

BMP280::~BMP280()
{
	delete i2c;
}

bool BMP280::check()
{
	if(read8(BMP280_REG_CHIPID)==0x58)
	{
		t1 = read16le(dig_T1);
		t2 = reads16le(dig_T2);
		t3 = reads16le(dig_T3);
		
		p1 = read16le(dig_P1);
		p2 = reads16le(dig_P2);
		p3 = reads16le(dig_P3);
		p4 = reads16le(dig_P4);
		p5 = reads16le(dig_P5);
		p6 = reads16le(dig_P6);
		p7 = reads16le(dig_P7);
		p8 = reads16le(dig_P8);
		p9 = reads16le(dig_P9);
		
		return true;
	}
	else
	{
		return false;
	}
}

float BMP280::calalt(float pressure)
{
	float altitude ;
	altitude = (float)44330*((float)1 -(float) (pow(((float)pressure/(float)ref_pressure),((float)1/5.255))));
	return altitude ;
}

float BMP280::getTemperature(void)
{
	int32_t var1 , var2 ;
	int32_t adc_T = read24(BMP280_REG_TEMPDATA);
	adc_T>>=4;
	var1 = (((adc_T>>3)-((int32_t)(t1<<1)))*
		((int32_t)t2))>>11;
	var2 = (((((adc_T>>4)-((int32_t)t1))*
		((adc_T>>4)-((int32_t)t1)))>>12)*
		((int32_t)t3))>>14;
	t_fine = var1 + var2 ;
	float T = (t_fine * 5 +128 )>>8;
	return T/100;
}

uint32_t BMP280::getPressure(void)
{
	int64_t var1,var2,p ;
	int32_t adc_P = read24(BMP280_REG_PRESSUREDATA);
	adc_P>>=4;
	
	var1 = ((int64_t)t_fine)-128000;
	var2 = var1 * var1 * (int64_t)p6 ;
	var2 = var2 + ((var1*(int64_t)p5)<<17);
	var2 = var2 + (((int64_t)p4)<<35);
	var1 = ((var1*var1*(int64_t)p3)>>8) + ((var1*(int64_t)p2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)p1)>>33;
	if(var1==0)
	return 0;
	
	p = 1048576 - adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)p9)*(p>>13)*(p>>13))>>25;
	var2 = (((int64_t)p8)*p)>>19;
	p = ((p+var1+var2)>>8)+(((int64_t)p7)<<4);
	return (uint32_t)p/256;
}


uint8_t BMP280::read8(uint8_t reg)
{
	i2c->openConnection();
	uint8_t val = i2c->readByte(reg);
	i2c->closeConnection();
	return val;
}


uint16_t BMP280::read16(uint8_t reg)
{
	uint8_t msb,lsb ;
	msb = read8(reg);
	++reg;
	lsb = read8(reg);
	return ((msb<<8)|lsb);
}

uint16_t BMP280::read16le(uint8_t reg)
{
	uint16_t data = read16(reg);
	return (data>>8) | (data<<8);
}

int16_t BMP280::reads16(uint8_t reg)
{
	return (int16_t)read16(reg);
}

int16_t BMP280::reads16le(uint8_t reg)
{
	return (int16_t)read16le(reg);
}

uint32_t BMP280::read24(uint8_t reg)
{
	uint32_t data ;
	data = read8(reg);
	++reg;
	data = data<<8 ;
	data = data | (read8(reg)) ;
	data = data<<8 ;
	++reg ;
	data = data | (read8(reg)) ;
	return data;
}

void BMP280::writeRegister(uint8_t reg , uint8_t val)
{
	i2c->openConnection();
	i2c->writeByte(reg,val);
	i2c->closeConnection();
}

//BMP280 SETTINGS REGISTER
//OVERSAMPLING SETTING		PRESSURE OVERSAMPLING		PRESSURE RESOLUTION	RECOMMENDED TEMPERATURE		VALUE
//												SETTING 	
//SKIPPED			-				-			-				000
//ULTRA_LOW_POWER		*1				16 bit/2.62 Pa		*1				001
//LOW_POWER			*2				17 bit/1.31 Pa		*1				010
//STANDARD_RESOLUTION		*4				18 bit/0.66 Pa		*1				011
//HIGH_RESOLUTION		*8				19 bit/0.33 Pa		*1				100
//ULTRA_HIGH_RESOLUTION		*16				20 bit/0.16 Pa		*1				101

//VALUE TO BE SENT		TEMPERATURE OVERSAMPLING	TEMPERATURE RESOLUTION	VALUE
//LEVEL0			- 				-			000
//LEVEL1			*1				16 bit/0.0050  C	001
//LEVEL2			*2				17 bit/0.0025  C	010
//LEVEL3			*4				18 bit/0.0012  C	011
//LEVEL4			*8				19 bit/0.0006  C	100
//LEVEL5			*16				20 bit/0.0003  C	101

//POWER MODE SETTINGS
//MODES			VALUE
//SLEEP_MODE		00
//FORCED_MODE		01
//NORMAL_MODE		11

void BMP280::bmp280_set(int8_t pressure_oversampling , int8_t temperature_oversampling , int8_t power_mode )
{
	int8_t val = pressure_oversampling ;
	val = (val<<3) | temperature_oversampling ;
	val = (val<<2) | power_mode ;
	writeRegister(BMP280_REG_CONTROL,val);
}
