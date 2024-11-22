/*
 * Display_HMI.cpp
 *
 *  Created on: Nov 1, 2024
 *      Author: user
 */



#include "Display_HMI.hpp"
#include "cstring"
//#include "regex"
#include "string"

namespace HMI {



DWIN::DWIN(UART_HandleTypeDef *huart) : serial_(huart){

}


void DWIN::Startup()
{

	clearBox(Time);
	clearBox(Date);
	clearBox(Network);
	clearBox(SigQ);
	clearBox(Net);

	clearBox(source_bat);
	clearBox(source_gen);
	clearBox(source_sol);
	clearBox(alarm_freq);
	clearBox(alarm_overl);
	clearBox(alarm_phasel);
	clearBox(chrgT);
	clearBox(dchgT);

	SendNumericData(Temperature,0);
	SendNumericData(Humidity,0);
	SendNumericData(TankLevel,0);
	SendNumericData(Battery,0);


	SendFloatData(phase1,0.00);
	SendFloatData(phase2,0.00);
	SendFloatData(phase3,0.00);
	SendFloatData(curr1,0.00);
	SendFloatData(curr2,0.00);
	SendFloatData(curr3,0.00);
	SendFloatData(energy,0.00);
	SendFloatData(pwr1,0.00);
	SendFloatData(pwr2,0.00);
	SendFloatData(pwr3,0.00);



}

enum DWIN::status DWIN::SendNumericData(uint16_t id,uint16_t val)
   {
       status stat = DWIN_OK;
       uint8_t buf[100] = {0};
       buf[0] = frame1;
       buf[1] = frame2;
       buf[2] = 0x05;
       buf[3] = Send;
       convert_16bit_to_8bit.W = id;
       buf[4] = convert_16bit_to_8bit.B.Hb;
       buf[5] = convert_16bit_to_8bit.B.Lb;
       convert_16bit_to_8bit.W = val;
       buf[6] = convert_16bit_to_8bit.B.Hb;
       buf[7] = convert_16bit_to_8bit.B.Lb;

       //clearTextBox(id);
       serial_.TransmitData(buf, 8, 1000);

       return stat;
   }

enum DWIN::status DWIN::SendFloatData(uint16_t id,float val)
{
    status stat = DWIN_OK;
    uint8_t buf[100] = {0};
    buf[0] = frame1;
    buf[1] = frame2;
    buf[2] = 0x07;
    buf[3] = Send;
    convert_16bit_to_8bit.W = id;
    buf[4] = convert_16bit_to_8bit.B.Hb;
    buf[5] = convert_16bit_to_8bit.B.Lb;
    convert_float_to_bytes.f = val;
    buf[6] = convert_float_to_bytes.B[3];
    buf[7] = convert_float_to_bytes.B[2];
    buf[8] = convert_float_to_bytes.B[1];
    buf[9] = convert_float_to_bytes.B[0];

    //clearTextBox(id);
    serial_.TransmitData(buf, 10, 1000);

    return stat;
}



enum DWIN::status DWIN::SendTextData(uint16_t id,char *data)
{
	 status stat = DWIN_OK;
	 uint8_t len = 0;
	 uint8_t buf[100] = {0};

    buf[0] = frame1;
    buf[1] = frame2;
    buf[3] = Send;
    convert_16bit_to_8bit.W = id;
    buf[4] = convert_16bit_to_8bit.B.Hb;
    buf[5] = convert_16bit_to_8bit.B.Lb;
    len = 3;
    len = stringTohex(data,buf,6,len);

    len = len + 3;
    buf[len] = ' ';
    len = len + 1;
    buf[len] = ' ';

    buf[2] = len; //additional to erase extra text in system

    //clearTextBox(id);
    serial_.TransmitData(buf, len + 3, 1000);

	 return stat;
}

enum DWIN::status DWIN::clearBox(uint16_t id)
{
	 status stat = DWIN_OK;
	 uint8_t buf[100] = {0};

    buf[0] = frame1;
    buf[1] = frame2;
    buf[2] = 0x05;
    buf[3] = Send;
    convert_16bit_to_8bit.W = id;
    buf[4] = convert_16bit_to_8bit.B.Hb;
    buf[5] = convert_16bit_to_8bit.B.Lb;
    buf[6] = 0x00;
    buf[7] = 0x00;

    serial_.TransmitData(buf, 8, 1000);
    return stat;
}


uint8_t DWIN::stringTohex(char *input,uint8_t *output,uint8_t index,uint8_t len)
{

	    size_t length = std::strlen(input);


	     for (size_t i = 0; i < length; ++i) {
	    	if(input[i] != '\0')
	    	 {
	    	   output[index] = static_cast<uint8_t>(input[i]);
	    	   len++;
	    	   index++;
	    	 }
	    	else
	    	{
	    		 output[index] = 0x00;
	    		break;
	    	}
	     }

	    return len;

}

}
