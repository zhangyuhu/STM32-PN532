/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
/*
 * 文件参考原pn532_mifare_ultralight 文件改写
 * NTAG216 大部分命令可以兼容
 * 原文件夹中有pn532_mifare_ultralight 文件可以供参考
 * 需要增加部分快速读写逻辑等以满足大量数据读取的需求
 */
#include <string.h>
#include "pn532.h"
#include "pn532_bus.h"
#include "pn532_ntag216.h"
#include "delay.h"
#include "log.h"
#include <stdio.h>

/**************************************************************************/
/*
 *检测卡被动等卡模式
 *获取卡的信息
*/
/**************************************************************************/
pn532_error_t pn532_mifareultralight_WaitForPassiveTarget (uint8_t * pbtCUID, size_t * szCUIDLen)
{
    uint8_t abtResponse[PN532_RESPONSELEN_INLISTPASSIVETARGET];
    pn532_error_t error;
    size_t szLen;

#ifdef PN532_DEBUGMODE
    PN532_DEBUG("Waiting for an ISO14443A Card%s", CFG_PRINTF_NEWLINE);
#endif

    /* Try to initialise a single ISO14443A tag at 106KBPS                  */
    /* Note:  To wait for a card with a known UID, append the four byte     */
    /*        UID to the end of the command.                                */
    uint8_t abtCommand[] = { PN532_COMMAND_INLISTPASSIVETARGET, 0x01, PN532_MODULATION_ISO14443A_106KBPS};
    error = pn532Write(abtCommand, sizeof(abtCommand));
    if (error)
        return error;

    /* Wait until we get a valid response or a timeout                      */
    do
    {
        delayMs(25);
        error = pn532Read(abtResponse, &szLen);
    }
    while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
    if (error)
        return error;

    /* Check SENS_RES to make sure this is a Mifare Ultralight card         */
    /*          Mifare Ultralight = 00 44                                   */
    if (abtResponse[10] == 0x44)
    {
        /* Card appears to be Mifare Ultralight */
        *szCUIDLen = abtResponse[12];
        uint8_t i;
        for (i=0; i < *szCUIDLen; i++)
        {
            pbtCUID[i] = abtResponse[13+i];
        }
#ifdef PN532_DEBUGMODE
            PN532_DEBUG("Card Found: %s", CFG_PRINTF_NEWLINE);
            PN532_DEBUG("      ATQA: ");
            pn532PrintHex(abtResponse+9, 2);
            PN532_DEBUG("      SAK: %02x%s", abtResponse[11], CFG_PRINTF_NEWLINE);
            PN532_DEBUG("      UID: ");
            pn532PrintHex(pbtCUID, *szCUIDLen);
#endif
    }
    else
    {
        /* Card is ISO14443A but doesn't appear to be Mifare Ultralight       */
        /*    Mifare Classic       = 0x0002, 0x0004, 0x0008                   */
        /*    Mifare DESFire       = 0x0344                                   */
        /*    Innovision Jewel     = 0x0C00                                   */
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Wrong Card Type (Expected ATQA 00 44) %s%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
        PN532_DEBUG("  ATQA       : ");
        pn532PrintHex(abtResponse+9, 2);
        PN532_DEBUG("  SAK        : %02x%s", abtResponse[11], CFG_PRINTF_NEWLINE);
        PN532_DEBUG("  UID Length : %d%s", abtResponse[12], CFG_PRINTF_NEWLINE);
        PN532_DEBUG("  UID        : ");
        size_t pos;
        for (pos=0; pos < abtResponse[12]; pos++)
        {
            printf("%02x ", abtResponse[13 + pos]);
        }
        printf("%s%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
#endif
        return PN532_ERROR_WRONGCARDTYPE;
    }

    return PN532_ERROR_NONE;
}

/**************************************************************************/
/*
*读取卡的一个page
*/
/**************************************************************************/
pn532_error_t pn532_mifareultralight_ReadPage (uint8_t page, uint8_t * pbtBuffer)
{
    pn532_error_t error;
    uint8_t abtCommand[4];
    uint8_t abtResponse[PN532_RESPONSELEN_INDATAEXCHANGE];
    size_t szLen;

    if (page >= MAX_PAGE)
    {
        return PN532_ERROR_ADDRESSOUTOFRANGE;
    }

#ifdef PN532_DEBUGMODE
    PN532_DEBUG("Reading page %03d%s", page, CFG_PRINTF_NEWLINE);
#endif

    /* Prepare the command */
    abtCommand[0] = PN532_COMMAND_INDATAEXCHANGE;
    abtCommand[1] = 1;                            /* Card number */
    abtCommand[2] = PN532_MIFARE_CMD_READ;        /* Mifare Read command = 0x30 */
    abtCommand[3] = page;                         /* Page Number (0..63 in most cases) */

    /* Send the commands */
    error = pn532Write(abtCommand, sizeof(abtCommand));
    if (error)
    {
    /* Bus error, etc. */
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
#endif
        return error;
    }

    /* Read the response */
    memset(abtResponse, 0, PN532_RESPONSELEN_INDATAEXCHANGE);
    do
    {
        delayMs(50);
        error = pn532Read(abtResponse, &szLen);
    }
    while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
    if (error)
    {
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
#endif
        return error;
    }

    /* Make sure we have a valid response (should be 26 bytes long) */
    if (szLen == 26)
    {
        /* Copy the 4 data bytes to the output buffer         */
        /* Block content starts at byte 9 of a valid response */
        /* Note that the command actually reads 16 byte or 4  */
        /* pages at a time ... we simply discard the last 12  */
        /* bytes                                              */
        memcpy (pbtBuffer, abtResponse+8, 4);
    }
    else
    {
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Unexpected response reading block %d.  Bad key?%s", page, CFG_PRINTF_NEWLINE);
#endif
        return PN532_ERROR_BLOCKREADFAILED;
    }

    /* Display data for debug if requested */
#ifdef PN532_DEBUGMODE
    PN532_DEBUG("Page %02d: %s", page, CFG_PRINTF_NEWLINE);
    //    pn532PrintHexVerbose(pbtBuffer, 4);
#endif

    // Return OK signal
    return PN532_ERROR_NONE;
}

/**************************************************************************/
/*!
*写一个page
*/
/**************************************************************************/
pn532_error_t PN532_mifareultralight_WritePage (uint8_t page, uint8_t *buffer)
{
    pn532_error_t error;
    uint8_t abtCommand[8];
    uint8_t abtResponse[PN532_RESPONSELEN_INDATAEXCHANGE];
    size_t szLen;

    /* Prepare the first command */
    abtCommand[0] = PN532_COMMAND_INDATAEXCHANGE;
    abtCommand[1] = 1;                           /* Card number */
    abtCommand[2] = MIFARE_CMD_WRITE_ULTRALIGHT; /* Mifare UL Write cmd = 0xA2 */
    abtCommand[3] = page;                        /* page Number (0..63) */
    memcpy (abtCommand + 4, buffer, 4);          /* Data Payload */

    /* Send the command */
    error = pn532Write(abtCommand, sizeof(abtCommand));

    if (error)
    {
        /* Bus error, etc. */
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
#endif
        return error;
    }

    /* Read the response */
    memset(abtResponse, 0, PN532_RESPONSELEN_INDATAEXCHANGE);
    do
    {
        delayMs(50);
        error = pn532Read(abtResponse, &szLen);
    }
    while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
    if (error)
    {
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
#endif
        return error;
    }
}

/**************************************************************************/
/*
 *快速的读取page
*/
/**************************************************************************/
pn532_error_t pn532_ntag216_fast_ReadPage (uint8_t * pbtBuffer)
{
    pn532_error_t error;
    uint8_t abtCommand[5];
    uint8_t abtResponse[PN532_RESPONSELEN_INDATAEXCHANGE];
    size_t szLen;


    /* Prepare the command */
    abtCommand[0] = PN532_COMMAND_INDATAEXCHANGE;
    abtCommand[1] = 1;                           /* Card number */
    abtCommand[2] = 0x60; /* Mifare UL Write cmd = 0xA2 */

    /* Send the commands */
    error = pn532Write(abtCommand, sizeof(abtCommand));
    if (error)
    {
    /* Bus error, etc. */
#ifdef PN532_DEBUGMODE
        PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
#endif
        return error;
    }

    /* Read the response */
    memset(abtResponse, 0, PN532_RESPONSELEN_INDATAEXCHANGE);
    do
    {
        delayMs(50);
        error = pn532Read(abtResponse, &szLen);
    }
    while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
//    if (error)
//    {
//#ifdef PN532_DEBUGMODE
//        PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
//#endif
//        //return error;
//    }

    /* Make sure we have a valid response (should be 26 bytes long) */
    //if (szLen == 26)
    //{
        /* Copy the 4 data bytes to the output buffer         */
        /* Block content starts at byte 9 of a valid response */
        /* Note that the command actually reads 16 byte or 4  */
        /* pages at a time ... we simply discard the last 12  */
        /* bytes                                              */
        memcpy (pbtBuffer, abtResponse+8, 24);
    //}
//    else
//    {
//#ifdef PN532_DEBUGMODE
//        PN532_DEBUG("Unexpected response reading block .  Bad key?%s", CFG_PRINTF_NEWLINE);
//#endif
//        return PN532_ERROR_BLOCKREADFAILED;
//    }

    /* Display data for debug if requested */
//#ifdef PN532_DEBUGMODE
//    PN532_DEBUG("Page : %s", CFG_PRINTF_NEWLINE);
//    //    pn532PrintHexVerbose(pbtBuffer, 4);
//#endif

    // Return OK signal
    return PN532_ERROR_NONE;
}
