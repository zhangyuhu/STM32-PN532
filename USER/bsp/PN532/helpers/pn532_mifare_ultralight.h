/**************************************************************************/
/*! 
    @file     pn532_mifare_ultralight.h
*/
/**************************************************************************/

#ifndef __PN532_MIFARE_ULTRALIGHT_H__
#define __PN532_MIFARE_ULTRALIGHT_H__

#include "bsp_config.h"
#include "pn532_mifare.h"

pn532_error_t pn532_mifareultralight_WaitForPassiveTarget (uint8_t * pbtCUID, size_t * szCUIDLen);
pn532_error_t pn532_mifareultralight_ReadPage (uint8_t page, uint8_t * pbtBuffer);

#endif
