/*
 * atcmd.hpp
 *
 *  Created on: Nov 6, 2024
 *      Author: user
 */

#ifndef MODEM_ATCMD_HPP_
#define MODEM_ATCMD_HPP_


static constexpr uint8_t atcmd_AT[] = "AT\r\n";
/*Reset GSM*/
static constexpr uint8_t atcmd_ATCRESET[] = "AT+CRESET\r\n";
/*Network registeration*/
static constexpr uint8_t atcmd_ATCREG[] = "AT+CREG=1\r\n";
/*Network registeration PDP context*/
static constexpr uint8_t atcmd_ATCGREG[] = "AT+CGREG=1\r\n";
/*Prefered Message Storage*/
static constexpr uint8_t atcmd_ATCPMS[] = "AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n";
/*Operator Selection*/
static constexpr uint8_t atcmd_ATCOPS[] = "AT+COPS=0\r\n";
/*SMS format*/
static constexpr uint8_t atcmd_ATCMGF[] = "AT+CMGF=1\r\n";
/*Packet domain attach*/
static constexpr uint8_t atcmd_ATCGATT[] = "AT+CGATT=1\r\n";
/*Get CLK time */
static constexpr uint8_t atcmd_ATCCLK[] = "AT+CCLK?\r\n";
/*Sim status*/
static constexpr uint8_t atcmd_ATCPIN[] = "AT+CPIN?\r\n";
/*get Network status*/
static constexpr uint8_t atcmd_GATCREG[] = "AT+CREG?\r\n";
/*get network status PDP context*/
static constexpr uint8_t atcmd_GATCGREG[] = "AT+CGREG?\r\n";
/*get pdp context activate or deactivate*/
static constexpr uint8_t atcmd_GATCGACT[] = "AT+CGACT?\r\n";
/*get uesystem info*/
static constexpr uint8_t atcmd_GATCPSI[] = "AT+CPSI?\r\n";
/*get quality*/
static constexpr uint8_t atcmd_ATCSQ[] = "AT+CSQ\r\n";
/*Current manufacture parameters*/
static constexpr uint8_t atcmd_AT_F0[] = "AT&F0\r\n";
/*Echo control*/
static constexpr uint8_t atcmd_ATE1[] = "ATE1\r\n";
/*MQTT start*/
static constexpr uint8_t atmcd_ATCMQTTSTART[] = "AT+CMQTTSTART\r\n";
/*MQTT stop*/
static constexpr uint8_t atcmd_ATCMQTTSTOP[] = "AT+CMQTTSTOP\r\n";
/*Get if server is connected to something*/
static constexpr uint8_t atcmd_GATCMQTTCONNECT[] = "AT+CMQTTCONNECT?\r\n";
/*Set SSL version*/
static constexpr uint8_t atcmd_ATCSSLCFG_ver[] = "AT+CSSLCFG=\"sslversion\",0,4\r\n";
/*Set auth mode ssl*/
static constexpr uint8_t atcmd_ATCSSLCFG_auth[] = "AT+CSSLCFG=\"authmode\",0,2\r\n";
/*set ca cert*/
static constexpr uint8_t atcmd_ATCSSLCFG_ca[] = "AT+CSSLCFG=\"cacert\",0,\"ca.pem\"\r\n";
/*set device cert*/
static constexpr uint8_t atcmd_ATCSSLCFG_device[] = "AT+CSSLCFG=\"clientcert\",0,\"clientcert.pem\"\r\n";
/*set device key*/
static constexpr uint8_t atcmd_ATCSSLCFG_key[] = "AT+CSSLCFG=\"clientkey\",0,\"clientkey.pem\"\r\n";

#endif /* MODEM_ATCMD_HPP_ */
