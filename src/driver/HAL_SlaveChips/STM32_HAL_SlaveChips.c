#include "HAL_Spi.h"
#include <stdint.h>
#include <string.h>
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include <stdlib.h>

//Initialization Parameters
#define ADC_MODE (uint8_t) 0x10 //7KHz, default/recommended
#define DCP_VALUE (uint8_t) 0x1 //Not sure
#define CH_VALUE (uint8_t) 0x0 //All cells
#define CHG_VALUE (uint8_t) 0x0 //Just read all GPIOs

//Command Codes (CC)
#define READ_VOLTAGE_GROUP_A 0x4
#define READ_VOLTAGE_GROUP_B 0x6
#define READ_VOLTAGE_GROUP_C 0x8
#define READ_VOLTAGE_GROUP_D 0xA

#define READ_CONFIGURATUION_REGISTER_GROUP 0x2



//WRITE_CONFIGURATION_REGISTER_GROUP (DCC for setting is_draining state)
#define WRITE_CONFIGURATION_REGISTER_GROUP 0x1
#define WRITE_CONFIGURATION_REGISTER_PEC0 0x3D
#define WRITE_CONFIGURATION_REGISTER_PEC1 0x6E


//GPIO
#define READ_AUX_REG_A 0xC
#define GPIO_START_CONVERSION_MASK 0x470 //CC
#define CHG_GPIO1 0x1
#define CHG_GPIO2 0x2
#define CHG_GPIO3 0x3



//#define CMD_MASK (uint8_t) 0xFF

#define CONFIGURATION_REGISTER_DRAINING_MASK 0xF

#define SYS_CLK 48 //Clk in MHz

/*!
  6804 conversion command variables.  
*/
uint8_t ADCV[2]; //!< Cell Voltage conversion command.
uint8_t ADAX[2]; //!< GPIO conversion command.

const char ReadVoltagesCMD0[4] = {(char)(0xFF && READ_VOLTAGE_GROUP_A), (char)(0xFF && READ_VOLTAGE_GROUP_B), 
    (char)(0xFF && READ_VOLTAGE_GROUP_C), (char)(0xFF && READ_VOLTAGE_GROUP_D)};
const char ReadVoltagesCMD1[4] = {(char)(0xFF && (READ_VOLTAGE_GROUP_A>>8)), (char)(0xFF && (READ_VOLTAGE_GROUP_B>>8)), 
    (char)(0xFF && (READ_VOLTAGE_GROUP_C>>8)), (char)(0xFF && (READ_VOLTAGE_GROUP_D>>8))};

static const unsigned int crc15Table[256] = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  //!<precomputed CRC15 Table
0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1, 
0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd, 
0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c, 
0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d, 
0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b, 
0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921, 
0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 
0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528, 
0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59, 
0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01, 
0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9, 
0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a, 
0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25, 
0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453, 
0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b, 
0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3, 
0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095}; 

/*!*******************************************************************************************************************
 \brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands
 
@param[in] uint8_t MD The adc conversion mode
@param[in] uint8_t DCP Controls if Discharge is permitted during cell conversions
@param[in] uint8_t CH Determines which cells are measured during an ADC conversion command
@param[in] uint8_t CHG Determines which GPIO channels are measured during Auxiliary conversion command
 
Command Code:
-------------

|command	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] | 
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
 ******************************************************************************************************************/
static void set_adc(uint8_t MD, //ADC Mode
			 uint8_t DCP, //Discharge Permit
			 uint8_t CH, //Cell Channels to be measured
			 uint8_t CHG //GPIO Channels to be measured
			 )
{
  uint8_t md_bits;
  
  md_bits = (MD & 0x02) >> 1;
  ADCV[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;
 
  md_bits = (MD & 0x02) >> 1;
  ADAX[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  ADAX[1] = md_bits + 0x60 + CHG ;
  
}

/*!**********************************************************
 \brief calaculates  and returns the CRC15
  
  @param[in] uint8_t len: the length of the data array being passed to the function
               
  @param[in] uint8_t data[] : the array of data that the PEC will be generated from
  

  @returns The calculated pec15 as an unsigned int
***********************************************************/
static uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
					uint8_t *data //Array of data that will be used to calculate  a PEC
					)
{
	uint16_t remainder,addr;
	
	remainder = 16;//initialize the PEC
	for(uint8_t i = 0; i<len;i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address 
		remainder = (remainder<<8)^crc15Table[addr];
	}
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

/*!*********************************************************************************************
  \brief Starts cell voltage conversion
  
  Starts ADC conversions of the LTC6804 Cpin inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CH     | Determines which cell channels are converted |
 | DCP    | Determines if Discharge is Permitted	     |
  
Command Code:
-------------
  
|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] |  
***********************************************************************************************/
static void LTC6804_adcv()
{

  char cmd[4];
  uint16_t cmd_pec;
  
  //1
  cmd[0] = ADCV[0];
  cmd[1] = ADCV[1];
  
  //2
  cmd_pec = pec15_calc(2, ADCV);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  
  //3
  //wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  //4
  //output_low(LTC6804_CS);
  //spi_write_array(4,cmd);
  //output_high(LTC6804_CS);
  HAL_Spi_transmit_and_receive(&cmd[0], 4, NULL, 0);

}

/*!******************************************************************************************************
 \brief Start an GPIO Conversion
 
  Starts an ADC conversions of the LTC6804 GPIO inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CHG    | Determines which GPIO channels are converted |
 
 
Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
*********************************************************************************************************/
static void LTC6804_adax()
{
  char cmd[4];
  uint16_t cmd_pec;
 
  cmd[0] = ADAX[0];
  cmd[1] = ADAX[1];
  cmd_pec = pec15_calc(2, ADAX);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
 
//   wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
//   output_low(LTC6804_CS);
//   spi_write_array(4,cmd);
//   output_high(LTC6804_CS);
    HAL_Spi_transmit_and_receive(&cmd[0], 4, NULL, 0);

}

/***********************************************//**
 \brief Read the raw data from the LTC6804 cell voltage register
 
 The function reads a single cell voltage register and stores the read data
 in the *data point as a byte array. This function is rarely used outside of 
 the LTC6804_rdcv() command. 
 
 @param[in] uint8_t reg; This controls which cell voltage register is read back. 
         
          1: Read back cell group A 
		  
          2: Read back cell group B 
		  
          3: Read back cell group C 
		  
          4: Read back cell group D 
		  		  		  
 @param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only)

 @param[out] uint8_t *data; An array of the unparsed cell codes 
 
Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDCVA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |
|RDCVB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   | 
|RDCVC:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |   0   | 
|RDCVD:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1   |   0   |  

 *************************************************/
void LTC6804_rdcv_reg(uint8_t reg, //Determines which cell voltage register is read back
					  uint8_t total_ic, //the number of ICs in the
					  uint8_t *data //An array of the unparsed cell codes
					  )
{
  const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC
  char cmd[4];
  uint16_t cmd_pec;
  
  //1
  if (reg == 1)     //1: RDCVA
  {
    cmd[1] = 0x04;
    cmd[0] = 0x00;
  }
  else if(reg == 2) //2: RDCVB
  {
    cmd[1] = 0x06;
    cmd[0] = 0x00;
  } 
  else if(reg == 3) //3: RDCVC
  {
    cmd[1] = 0x08;
    cmd[0] = 0x00;
  } 
  else if(reg == 4) //4: RDCVD
  {
    cmd[1] = 0x0A;
    cmd[0] = 0x00;
  } 

  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec); 
  
  //3
  //wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  //4
//   output_low(LTC6804_CS);
//   spi_write_read(cmd,4,data,(REG_LEN*total_ic));
//   output_high(LTC6804_CS);
    HAL_Spi_transmit_and_receive(&cmd[0], 4, data, (REG_LEN*total_ic));

}

/***********************************************//**
 \brief Reads and parses the LTC6804 cell voltage registers.
 
 The function is used to read the cell codes of the LTC6804.
 This function will send the requested read commands parse the data
 and store the cell voltages in cell_codes variable. 

 @param[in] uint8_t reg; This controls which cell voltage register is read back. 
 
          0: Read back all Cell registers 
		  
          1: Read back cell group A 
		  
          2: Read back cell group B 
		  
          3: Read back cell group C 
		  
          4: Read back cell group D 
 
 @param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only) 
 
 @param[out] uint16_t cell_codes[]; An array of the parsed cell codes from lowest to highest. The cell codes will
  be stored in the cell_codes[] array in the following format:
  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
 
  @return int8_t, PEC Status.
 
		0: No PEC error detected
  
		-1: PEC error detected, retry read
 
 
 *************************************************/
uint8_t LTC6804_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
					 uint8_t total_ic, // the number of ICs in the system
					 uint16_t cell_codes[][12] // Array of the parsed cell codes
					 )
{
  
  const uint8_t NUM_RX_BYT = 8;
  const uint8_t BYT_IN_REG = 6;
  const uint8_t CELL_IN_REG = 3;
  
  uint8_t *cell_data;
  uint8_t pec_error = 0;
  uint16_t parsed_cell;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t data_counter=0; //data counter
  cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
  //1.a
  if (reg == 0)
  {
    //a.i
    for(uint8_t cell_reg = 1; cell_reg<5; cell_reg++)         			 			//executes once for each of the LTC6804 cell voltage registers
    {
      data_counter = 0;
      LTC6804_rdcv_reg(cell_reg, total_ic,cell_data );								//Reads a single Cell voltage register
	  
      for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) 			// executes for every LTC6804 in the daisy chain
      {																 	  			// current_ic is used as the IC counter
	  
        //a.ii
		for(uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++)	 	// This loop parses the read back data into cell voltages, it 
        {														   		  			// loops once for each of the 3 cell voltage codes in the register 
		
          parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each cell code is received as two bytes and is combined to
																					 // create the parsed cell voltage code
																					 
          cell_codes[current_ic][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
          data_counter = data_counter + 2;											 //Because cell voltage codes are two bytes the data counter
																					//must increment by two for each parsed cell code
        }
		//a.iii
        received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																				   //after the 6 cell voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;															//The pec_error variable is simply set negative if any PEC errors 
																					//are detected in the serial data
        }
        data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs cell voltage data
      }
    }
  }
 //1.b
  else
  {
	//b.i
    LTC6804_rdcv_reg(reg, total_ic,cell_data);
    for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) 				// executes for every LTC6804 in the daisy chain
    {																 	  			// current_ic is used as the IC counter
		//b.ii
		for(uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++)   // This loop parses the read back data into cell voltages, it 
        {														   		  			// loops once for each of the 3 cell voltage codes in the register 
		
			parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8); //Each cell code is received as two bytes and is combined to
																					// create the parsed cell voltage code
																					
			cell_codes[current_ic][current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
			data_counter= data_counter + 2;     									//Because cell voltage codes are two bytes the data counter
																					//must increment by two for each parsed cell code
		}
		//b.iii
	    received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 cell voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
		if(received_pec != data_pec)
		{
			pec_error = -1;															//The pec_error variable is simply set negative if any PEC errors 
																					//are detected in the serial data
		}
		data_counter= data_counter + 2; 											//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs cell voltage data
	}
  }

 //2
 free(cell_data);
return(pec_error);
}
/***********************************************//**
 \brief Read the raw data from the LTC6804 auxiliary register
 
 The function reads a single GPIO voltage register and stores thre read data
 in the *data point as a byte array. This function is rarely used outside of 
 the LTC6804_rdaux() command. 
 
 @param[in] uint8_t reg; This controls which GPIO voltage register is read back. 
		  
          1: Read back auxiliary group A
		  
          2: Read back auxiliary group B 

         
@param[in] uint8_t total_ic; This is the number of ICs in the daisy chain
 
@param[out] uint8_t *data; An array of the unparsed aux codes 
 
 
 
Command Code:
-------------

|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDAUXA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   |   0   |
|RDAUXB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   | 

 *************************************************/
static void LTC6804_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
					   uint8_t total_ic, //The number of ICs in the system
					   uint8_t *data //Array of the unparsed auxiliary codes 
					   )
{
  const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;
  
  //1
  if (reg == 1)			//Read back auxiliary group A
  {
    cmd[1] = 0x0C;
    cmd[0] = 0x00;
  }
  else if(reg == 2)		//Read back auxiliary group B 
  {
    cmd[1] = 0x0e;
    cmd[0] = 0x00;
  } 
  else					//Read back auxiliary group A
  {
     cmd[1] = 0x0C;		
     cmd[0] = 0x00;
  }
  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  
  //3
  //wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake, this command can be removed.
  //4
//   output_low(LTC6804_CS);
//   spi_write_read(cmd,4,data,(REG_LEN*total_ic));
//   output_high(LTC6804_CS);
HAL_Spi_transmit_and_receive(&cmd[0], 4, data, (REG_LEN*total_ic));

}

/***********************************************************************************//**
 \brief Reads and parses the LTC6804 auxiliary registers.
 
 The function is used
 to read the  parsed GPIO codes of the LTC6804. This function will send the requested 
 read commands parse the data and store the gpio voltages in aux_codes variable 

@param[in] uint8_t reg; This controls which GPIO voltage register is read back. 
		  
          0: Read back all auxiliary registers 
		  
          1: Read back auxiliary group A 
		  
          2: Read back auxiliary group B 
		  
 
@param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only) 
 

 @param[out] uint16_t aux_codes[][6]; A two dimensional array of the gpio voltage codes. The GPIO codes will
 be stored in the aux_codes[][6] array in the following format:
 |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
 |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
 |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
 
@return  int8_t, PEC Status
 
  0: No PEC error detected
  
 -1: PEC error detected, retry read
 *************************************************/
static int8_t LTC6804_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back. 
					 uint8_t total_ic,//the number of ICs in the system
					 uint16_t aux_codes[][6]//A two dimensional array of the gpio voltage codes.
					 )
{


  const uint8_t NUM_RX_BYT = 8;
  const uint8_t BYT_IN_REG = 6;
  const uint8_t GPIO_IN_REG = 3;
  
  uint8_t *data;
  uint8_t data_counter = 0; 
  int8_t pec_error = 0;
  uint16_t parsed_aux;
  uint16_t received_pec;
  uint16_t data_pec;
  data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
  //1.a
  if (reg == 0)
  {
	//a.i
    for(uint8_t gpio_reg = 1; gpio_reg<3; gpio_reg++)		 	   		 			//executes once for each of the LTC6804 aux voltage registers
    {
      data_counter = 0;
      LTC6804_rdaux_reg(gpio_reg, total_ic,data);									//Reads the raw auxiliary register data into the data[] array
	  
      for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) 			// executes for every LTC6804 in the daisy chain
      {																 	  			// current_ic is used as the IC counter
	  
        //a.ii
		for(uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	// This loop parses the read back data into GPIO voltages, it 
        {														   		  			// loops once for each of the 3 gpio voltage codes in the register 
          
		  parsed_aux = data[data_counter] + (data[data_counter+1]<<8);              //Each gpio codes is received as two bytes and is combined to
																				    // create the parsed gpio voltage code
																					
          aux_codes[current_ic][current_gpio +((gpio_reg-1)*GPIO_IN_REG)] = parsed_aux;
          data_counter=data_counter+2;												//Because gpio voltage codes are two bytes the data counter
																					//must increment by two for each parsed gpio voltage code
		  
        }
		//a.iii
        received_pec = (data[data_counter]<<8)+ data[data_counter+1]; 				 //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;															//The pec_error variable is simply set negative if any PEC errors 
																					//are detected in the received serial data
        }
       
        data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
      }
   

    }
  
  }
  else
  {
	//b.i
    LTC6804_rdaux_reg(reg, total_ic, data);
    for (int current_ic = 0 ; current_ic < total_ic; current_ic++) 			  		// executes for every LTC6804 in the daisy chain
    {							   									          		// current_ic is used as an IC counter
	
		//b.ii
		for(int current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)  	 	// This loop parses the read back data. Loops 
		{						 											  		// once for each aux voltage in the register 
		
			parsed_aux = (data[data_counter] + (data[data_counter+1]<<8));    		//Each gpio codes is received as two bytes and is combined to
																					// create the parsed gpio voltage code
			aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = parsed_aux;
			data_counter=data_counter+2;									 		//Because gpio voltage codes are two bytes the data counter
																					//must increment by two for each parsed gpio voltage code
		}
		//b.iii
		received_pec = (data[data_counter]<<8) + data[data_counter+1]; 				 //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;													   		//The pec_error variable is simply set negative if any PEC errors 
																					//are detected in the received serial data
        }
        
		data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
	}
  }
  free(data);
  return (pec_error);
}

//Function that reads the cell voltages for all of the cells
//voltages: pointer to where cell voltages will be put
//num_Groups_of_12: the number of LTC6904s chained together or total number of cells/12
static void read_All_Cell_Voltages(float* voltages, int num_Groups_of_12){
    //May need to run ADCV command to read cell voltages first, and wait for them to be read
    char transmitCommand[4];
    for(int i = 0; i++; i < 4) //Cycle through each register group (A,B,C,D)
    {
        transmitCommand[0] = ReadVoltagesCMD0[i];
        transmitCommand[1] = ReadVoltagesCMD1[i];
        //NEED TO IMPLEMENT PEC
        //transmitCommand[2] and [3], for PEC0 and PEC1
        char *cellVoltagesRecieved[6*num_Groups_of_12];
        HAL_Spi_transmit_and_receive(&transmitCommand[0], 4, cellVoltagesRecieved[0], 6*num_Groups_of_12); //not sure about thisPin
        for(int x = 0; x++; x < num_Groups_of_12) //Cycle through each of the LTC boards in daisychain
        {
            for(int y = 0; y++; y < 3)
            {
                //Cell Voltage for Cell x = CxV • 100µV 
                float thisVoltage = ((float) (cellVoltagesRecieved[(x*6)+(y*2)] && (*(cellVoltagesRecieved[(x*6)+(y*2)+1])<<8)))/((float)10000);
                //memcpy(voltages[(i*6)+(x*12*2)+(y*2)], &thisVoltage, 1);
                voltages[(i*6)+(x*12*2)+(y*2)] = thisVoltage;
            }
            //strncpy(&voltages[(i*6) + (x*(12*2))], &cellVoltagesRecieved[x*6], 6); //Copy the group values to the voltage values
        }

    }
}

//Function that reads the is_draining state for all of the cells
//is_draining: pointer to where is_draining states will be put
//return: true if PEC error, false otherwise
static bool read_All_Is_Draining(bool* is_draining, unsigned int num_Groups_of_12)
{
    bool pec_error = false;
    char transmitCommand[4];
    transmitCommand[0] = 0xFF && READ_CONFIGURATUION_REGISTER_GROUP;
    transmitCommand[1] = 0xFF && (READ_CONFIGURATUION_REGISTER_GROUP >> 8);
    //NEED TO IMPLEMENT PEC
    //transmitCommand[2] and [3] for PEC0 and PEC1
    char isDrainingRecieved[8*num_Groups_of_12];
    //Read the Configuration Register Group to get DCC
    HAL_Spi_transmit_and_receive(&transmitCommand[0], 4, &isDrainingRecieved[0], 8*num_Groups_of_12);
    for(int i = 0; i++; i < num_Groups_of_12) //Cycle through data for ech daisy-chained LTC
    {
        //Set bits in bit array
        is_draining[(i*12)] = (isDrainingRecieved[(i*8)+4] && 0x1) != 0;
        is_draining[(i*12)+1] = (isDrainingRecieved[(i*8)+4] && 0x2) != 0;
        is_draining[(i*12)+2] = (isDrainingRecieved[(i*8)+4] && 0x4) != 0;
        is_draining[(i*12)+3] = (isDrainingRecieved[(i*8)+4] && 0x8) != 0;
        is_draining[(i*12)+4] = (isDrainingRecieved[(i*8)+4] && 0x10) != 0;
        is_draining[(i*12)+5] = (isDrainingRecieved[(i*8)+4] && 0x20) != 0;
        is_draining[(i*12)+6] = (isDrainingRecieved[(i*8)+4] && 0x40) != 0;
        is_draining[(i*12)+7] = (isDrainingRecieved[(i*8)+4] && 0x80) != 0;
        is_draining[(i*12)+8] = (isDrainingRecieved[(i*8)+5] && 0x1) != 0;
        is_draining[(i*12)+9] = (isDrainingRecieved[(i*8)+5] && 0x2) != 0;
        is_draining[(i*12)+10] = (isDrainingRecieved[(i*8)+5] && 0x4) != 0;
        is_draining[(i*12)+11] = (isDrainingRecieved[(i*8)+5] && 0x8) != 0;
        uint16_t pec_expected = pec15_calc(6, &isDrainingRecieved[i*8]);
        uint16_t pec_recieved = (isDrainingRecieved[(i*8)+6] || (isDrainingRecieved[(i*8)+7] << 8));
        if(pec_expected != pec_recieved){
            pec_error = true;
        }
    }
    return pec_error;

}


void HAL_SlaveChip_Init(){
    HAL_Spi_init();
    set_adc(ADC_MODE, DCP_VALUE, CH_VALUE, CHG_VALUE);
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = SYS_CLK/1; //Prescale for 1 MHz clk
    TIM6->CR1 |= (TIM_CR1_ARPE | TIM_CR1_URS | TIM_CR1_DIR);
//     Using it:
//     TIM6->ARR = val;
//     TIM6->EGR |= TIM_EGR_UG;
//     TIM6->SR = 0;
//     TIM6->CR1 |= TIM_CR1_CEN;
}

//Masks for broadcast command format
//CMD0 = CC & 0x700
//CMD1 = CC & 0xFF

//DCC[x]: Discharge cell X, x = 1 - 12, 1->turn on short switching for cell x, 0->turn off short switching for cell x
//DCC in configuration register group
Error_t HAL_SlaveChips_get_all_cell_data(float* voltages, bool* is_draining, unsigned int num)
{
    LTC6804_adcv();
    Error_t  error;
    error.active = false;

    //WAIT 2335 us
    TIM6->ARR = 2500; //Need to wait 2335 seconds, rounded up to 2500 for now
    TIM6->EGR |= TIM_EGR_UG;
    TIM6->SR = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
    while(TIM6->SR == 0);


    uint8_t Sets_of_12 = num / 12;
    if((num % 12) != 0)
    {
        Sets_of_12++;
    }
    uint16_t AllCellVoltages[Sets_of_12][12]; //Make sure if num isn't a multiple of 12, array is big enough
    bool AllIsDraining[Sets_of_12*12];
    LTC6804_rdcv(0, Sets_of_12, AllCellVoltages);
    if(read_All_Is_Draining(AllIsDraining, Sets_of_12)){
        error.active = true;
    }
    float floatCellVoltages[Sets_of_12][12];
    for(int i = 0; i < Sets_of_12; i++){
        for(int x = 0; x < 12; x++){
            floatCellVoltages[i][x] = (float) AllCellVoltages[i][x];
        }
    }
    memcpy(voltages, floatCellVoltages, num);
    memcpy(is_draining, AllIsDraining, num);
    return error;
}

//Every other LTC has 3 thermistors, 1st,3rd, etc have thermistors
Error_t HAL_SlaveChips_get_all_tm_readings(float* temperatures, unsigned int num){
    Error_t error;
    error.active = false;
    //GPIO voltage in Auxilary Register Group A
    LTC6804_adax();

    //WAIT
    TIM6->ARR = 2500; //Need to wait 2335 seconds, rounded up to 2500 for now
    TIM6->EGR |= TIM_EGR_UG;
    TIM6->SR = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
    while(TIM6->SR == 0);

    //NEED TO IMPLEMENT PEC
    uint8_t setsOfTwelve = ((num / 3) * 2);
    //NEED TO IMPLEMENT PEC
    //transmitCommand[2] and [3] for PEC0 and PEC1
    uint16_t tempRecieved[setsOfTwelve][6];
    //Read the Configuration Register Group to get DCC
    if(LTC6804_rdaux(1, setsOfTwelve, tempRecieved) == -1){
        error.active = true;
    }
    
    for(int i = 0; i < setsOfTwelve; i = i + 2){
        temperatures[(i/2) * 3] = (float) tempRecieved[i][0];
        temperatures[((i/2) * 3) + 1] = (float) tempRecieved[i][1];
        temperatures[((i/2) * 3) + 2] = (float) tempRecieved[i][2];
    }
}

Error_t HAL_SlaveChips_request_cell_drain_state(bool* cells, unsigned int num){
    Error_t error;
    error.active = false;
    int Sets_of_12 = num / 12;
    if((num % 12) != 0)
    {
        Sets_of_12++;
    }
    char tx[4 + (Sets_of_12 * 8)]; //CMD0, CMD1, PEC0, PEC1, 6 bytes of data and 2 bytes PEC for each set of 12
    tx[0] = WRITE_CONFIGURATION_REGISTER_GROUP && 0xFF;
    tx[1] = (WRITE_CONFIGURATION_REGISTER_GROUP >> 8) && 0xFF;
    tx[2] = WRITE_CONFIGURATION_REGISTER_PEC0;
    tx[3] = WRITE_CONFIGURATION_REGISTER_PEC1;
    //Set other parameters of Configuration Register
    //Alternative way, could be to read the register, and only change needed bits, but would take longer
    char ByteOne = 0xF8; //GPIO pins, and some others, all set to default
    char ByteTwo = 0x00; //Under voltage comparison , set to default
    char ByteThree = 0x00; //Under Voltage comparison and over voltage comparison
    char ByteFour = 0x00; //Over Voltage Comparison, set to default
    for(int i = 0; i = i + 8; i < (8 * Sets_of_12)) //Cycle through each set of 12, adding the 6 bytes for configuration register + 2 for PEC
    {
        tx[4 + i] = ByteOne;
        tx[5 + i] = ByteTwo;
        tx[6 + i] = ByteThree;
        tx[7 + i] = ByteFour;
        char temp = cells[i*2] && (cells[(i*2)+1] << 1) && (cells[(i*2)+2] << 2) && (cells[(i*2)+3] << 3) && 
            (cells[(i*2)+4] << 4) &&  (cells[(i*2)+5] << 5) && (cells[(i*2)+6] << 6) && (cells[(i*2)+7] << 7);
        tx[8 + i] = temp;
        temp = cells[(i*2)+8] && (cells[(i*2)+9] << 1) && (cells[(i*2)+10] << 2) && (cells[(i*2)+11] << 3) && 0x0F;
        tx[9 + i] = temp; //First four Discharge Timeout
        uint16_t pec = pec15_calc(6, &tx[4 + i]);
        tx[10 + i] = (char) (pec && 0xFF);
        tx[11 + i] = (char) ((pec >> 8) && 0xFF);
    }
    char *rx;
    HAL_Spi_transmit_and_receive(tx, 4 + (Sets_of_12 * 6), rx, 0);
    //TO DO: add error checking
    return error;
}