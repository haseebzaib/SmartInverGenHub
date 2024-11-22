/*
 * cmd.hpp
 *
 *  Created on: Nov 14, 2024
 *      Author: user
 */

#ifndef INVERTER_INVERTERCMD_HPP_
#define INVERTER_INVERTERCMD_HPP_


static constexpr uint8_t DevProtoID[] = "QPI\r";
static constexpr uint8_t DevID[] = "QID\r";
static constexpr uint8_t CPUFW[] = "QVFW\r";
static constexpr uint8_t DevModel[] = "QMD\r";
static constexpr uint8_t DevGenStatus[] = "QPIGS\r";
static constexpr uint8_t DevGenStatus2[] = "QPIGS2\r";
static constexpr uint8_t DevGenStatusN[] = "QPGSn\r";
static constexpr uint8_t DevModeInquiry[] = "QMOD\r";
static constexpr uint8_t DevWarnInquiry[] = "QPIWS\r";
static constexpr uint8_t DevPVGenEnergy[] = "QET\r";
static constexpr uint8_t DevPVGenEnergyYear[] = "QEYyyyy\r";
static constexpr uint8_t DevPVGenEnergyMonth[] = "QEMyyyymm\r";
static constexpr uint8_t DevPVGenEnergyDay[] = "QEDyyyymmdd\r";
static constexpr uint8_t DevOutLoadEnergy[] = "QLT\r";
static constexpr uint8_t DevOutLoadEnergyYear[] = "QLYyyyy\r";
static constexpr uint8_t DevOutLoadEnergyMonth[] = "QLMyyyymm\r";
static constexpr uint8_t DevOutLoadEnergyDay[] = "QLDyyyymmdd\r";


enum class invertermode : char{
	PowerOnMode = 'P',
	StandByMode = 'S',
	LineMode    = 'L',
	BatteryMode = 'B',
	FaultMode   = 'F',
	ShutdownMode = 'D',

};




#endif /* INVERTER_INVERTERCMD_HPP_ */
