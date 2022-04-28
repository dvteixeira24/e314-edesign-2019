/*
 * File: i2c.c
 * Author: 20976313
 *
 * Description: Handles functions relating the i2c communication bus
 * which the BME and Aaccelerometer use to communicate with the Nucleo.
 *
 */

#include "prog_def.h"

int8_t bme_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t bme_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
void bme_delay_ms(uint32_t period);
void bme_sensor_data();

int32_t lis2dh12_i2c_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
int32_t lis2dh12_i2c_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
void clip_acc();

static axis3bit16_t data_raw_acceleration;
static uint8_t device_id;
lis2dh12_reg_t reg;

/*
 *  BME I2C Functions
 */

void bme_init(){
	uint8_t settings_sel;
	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = bme_i2c_read;
	dev.write = bme_i2c_write;
	dev.delay_ms = bme_delay_ms;
	bme280_init(&dev);
	dev.settings.osr_h = BME280_OVERSAMPLING_16X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_16X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	dev.settings.standby_time = BME280_STANDBY_TIME_125_MS;
	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	bme280_set_sensor_settings(settings_sel, &dev);
	bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);
}


void bme_poll()
{
		bme_delay_ms(70);
		bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
		bme_sensor_data();
}

void bme_sensor_data(){
        tmp = comp_data.temperature;
        prs = comp_data.pressure/1000;
        hum = comp_data.humidity;
}

int8_t bme_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
    if(!HAL_I2C_Mem_Read(&hi2c1, (dev_id<<1) & 0xFE, reg_addr, 1, reg_data, len, 100) == HAL_OK) rslt = 1;
    return rslt;
}

int8_t bme_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
    if(!HAL_I2C_Mem_Write(&hi2c1, (dev_id<<1) & 0xFE, reg_addr, 1, reg_data, len, 100) == HAL_OK) rslt = 1;
    return rslt;
}

void bme_delay_ms(uint32_t period)
{
    dwt_us_delay(period*1000);
}

/*
 *  lis2dh12 I2C Functions
 */

void lis2dh21_init()
{
	dev_ctx.read_reg = lis2dh12_i2c_read;
	dev_ctx.write_reg = lis2dh12_i2c_write;
	dev_ctx.handle = &hi2c1;

	lis2dh12_device_id_get(&dev_ctx, &device_id); //make sure the device is connected
	if (device_id != LIS2DH12_ID)
	{
		LCD_write_string((uint8_t*) "LIS2DH12notfound"); //warn if not
		dwt_us_delay(2000000);
	}

	// Enable Block Data Update
	lis2dh12_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
	// Set Output Data Rate to 1Hz
	lis2dh12_data_rate_set(&dev_ctx, LIS2DH12_ODR_1Hz);
	// Set full scale to 2g
	lis2dh12_full_scale_set(&dev_ctx, LIS2DH12_2g);
	// Set device in continuous mode with 12 bit resolution
	lis2dh12_operating_mode_set(&dev_ctx, LIS2DH12_HR_12bit);
}

void lis2dh12_poll()
{
	lis2dh12_xl_data_ready_get(&dev_ctx, &reg.byte);

	if (reg.byte) // Read output only if new value available
	{
		// Read accelerometer data
		memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
		lis2dh12_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
		accx = lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[2]) * -1;
		accy = lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[1]);
		accz = lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[0]) * -1;
		clip_acc();
	}

}

int32_t lis2dh12_i2c_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len){
	reg |= 0x80;
	HAL_I2C_Mem_Read(handle, LIS2DH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	return 0;
}

int32_t lis2dh12_i2c_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len){
	reg |= 0x80;
	HAL_I2C_Mem_Write(handle, LIS2DH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	return 0;
}

void clip_acc(){

	if (accx > ACC_MAX)
		accx = ACC_MAX;
	if (accy > ACC_MAX)
		accy = ACC_MAX;
	if (accz > ACC_MAX)
		accz = ACC_MAX;

	if (accx < -ACC_MAX)
		accx = -ACC_MAX;
	if (accy < -ACC_MAX)
		accy = -ACC_MAX;
	if (accz < -ACC_MAX)
		accz = -ACC_MAX;
}

