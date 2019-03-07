#ifndef _BMP_
#define _BMP_
#include "../I2cPort/I2cPort.h"
#include <math.h>

#define BMP280_ADDRESS			0x77

#define dig_T1				0x88
#define dig_T2				0x8A
#define dig_T3				0x8C
		
#define dig_P1				0x8E
#define dig_P2				0x90
#define dig_P3				0x92
#define dig_P4				0x94
#define dig_P5				0x96
#define dig_P6				0x98
#define dig_P7				0x9A
#define dig_P8				0x9C
#define dig_P9				0x9E

#define BMP280_REG_CHIPID 		0xD0
#define BMP280_REG_CONTROL		0xF4
#define BMP280_REG_CONFIG		0xF5
#define BMP280_REG_PRESSUREDATA		0xF7
#define BMP280_REG_TEMPDATA		0xFA

#define SKIPPED				0
#define ULTRA_LOW_POWER			1
#define LOW_POWER			2
#define STANDARD_RESOLUTION		3
#define HIGH_RESOLUTION			4
#define ULTRA_HIGH_RESOLUTION		5

#define LEVEL0				0
#define LEVEL1				1
#define LEVEL2				2
#define LEVEL3				3
#define LEVEL4				4
#define LEVEL5				5
	
#define SLEEP_MODE			0
#define FORCED_MODE			1
#define NORMAL_MODE			3

class BMP280
{
	public :
	I2cPort *i2c;
	int32_t ref_pressure ;
	BMP280();
	~BMP280();
	bool check(void);

	float getTemperature(void);
	uint32_t getPressure(void);
	float calalt(float pressure);
	void set_ref_pressure();
	uint16_t t1,p1;
	int16_t t2,t3,p2,p3,p4,p5,p6,p7,p8,p9 ;
	
	public :
	int32_t t_fine ;
	
	uint8_t read8(uint8_t reg);
	uint16_t read16(uint8_t reg);
	uint16_t read16le(uint8_t reg);
	int16_t reads16(uint8_t reg);
	int16_t reads16le(uint8_t reg);
	uint32_t read24(uint8_t reg);
	void writeRegister(uint8_t reg,uint8_t val);
	
	void bmp280_set(int8_t ,int8_t , int8_t);
	
};
#endif
