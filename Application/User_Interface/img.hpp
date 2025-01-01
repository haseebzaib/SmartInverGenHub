/*
 * img.hpp
 *
 *  Created on: Dec 31, 2024
 *      Author: user
 */

#ifndef USER_INTERFACE_IMG_HPP_
#define USER_INTERFACE_IMG_HPP_


#include "main.h"

struct imgContainer {
	const uint8_t *img;
	uint8_t w;
	uint8_t h;
};


namespace imgcont {
extern const struct imgContainer smartgrid;
extern const struct imgContainer setting;
extern const struct imgContainer Selectedsetting;
extern const struct imgContainer leftrightArrow;
extern const struct imgContainer Phase;
extern const struct imgContainer selectedPhase;
extern const struct imgContainer Network;
extern const struct imgContainer Source;
extern const struct imgContainer Battery;
extern const struct imgContainer Alarms;
extern const struct imgContainer SensorInfo;
}

#endif /* USER_INTERFACE_IMG_HPP_ */
