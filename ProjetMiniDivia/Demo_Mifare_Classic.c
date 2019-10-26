/*


  Copyright (c) 2011 ODALID SARL - www.odalid.com

  Mifare_Classic.c
  ------------

  This is the reference application that shows how to read/write data
  in a Philips MIFARE classic tag.

  VT 02/01/2012 : initial release
  VT 03/01/2012 : Mifare_Classic_CDC
  VT 11/01/2012 : Mifare_Classic CDC + TCP

*/


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ODALID.h"
#include "MfErrNo.h"


#define Auth_KeyA				TRUE
#define Auth_KeyB				FALSE

unsigned char key_ff[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


int card_read(BYTE sect_count);

BOOL bench = TRUE;

char pszHost[] = "192.168.1.4";

ReaderName MonLecteur;

int main(int argc, char **argv)
{
    int status = MI_OK;
    int i;
    char s_buffer[64];
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    BYTE uid_len = 12;
    BYTE sect_count = 0;
    BOOL KeyA;
    BYTE key_index;
    BOOL internal_Key;
    BYTE sector;
    char device = 0;
    unsigned long int value = 0;
    unsigned long int valuebackup = 0;

    unsigned char BufSend[256];
    unsigned char BufRecv[256];
    unsigned char atr[256];
    int len_atr;
    char LenSend;
    char LenRecv;
    char *line0 = "  Bonjour";
    char *line1 = "  ODALID ";

    uint8_t version[30];
    uint8_t serial[4];
    uint8_t stackReader[20];
    uint8_t KeyBNr[2] = {0x00, 0x00};

    unsigned char data[48];
    for(i=0 ; i<0xF0 ; i++)
        data[i] = 0x00;

    //MonLecteur.Type = ReaderTCP;
    //MonLecteur.IPReader = NULL;
    //strcpy(MonLecteur.IPReader, pszHost);

    MonLecteur.Type = ReaderCDC;
    MonLecteur.device = 0;

    printf("ODALID SDK\n");
    printf("\n");
    printf("NXP MIFARE 'CLASSIC' (1k and 4k) reference demo\n");
    printf("-----------------------------------------------\n");
    printf("www.odalid.com\n\n");

    status = GetLibrary(s_buffer, sizeof(s_buffer));
    if (status != MI_OK){
        printf("Failed to get API version\n");
        goto done;
    }
    printf("API version : %s\n", s_buffer);
    printf("\n");


  // Open reader
    status = OpenCOM1(&MonLecteur);
    if (status != MI_OK){
        printf("Reader not found\n");
        goto done;
    }
    else{
        switch(MonLecteur.Type)
        {
            case ReaderTCP:
                sprintf(s_buffer, "IP : %s", MonLecteur.IPReader);
            break;
            case ReaderCDC:
                sprintf(s_buffer, "COM%d", MonLecteur.device);
            break;

        }
        printf("Reader found on %s\n", s_buffer);
    }


    status = Version(&MonLecteur, version, serial, stackReader);
    if (status == MI_OK){
        printf("Reader firwmare is %s\n", version);
        printf("Reader serial is %02X%02X%02X%02X\n", serial[0], serial[1], serial[2], serial[3]);
        printf("Reader stack is %s\n", stackReader);
    }


    status = LCD(&MonLecteur, 0, line0);
    if (status != MI_OK){
        printf("LCD [FAILED]\n");
        goto close;
    }

    status = LCD(&MonLecteur, 1, line1);
    if (status != MI_OK){
        printf("LCD [FAILED]\n");
        goto close;
    }


    status = LEDBuzzer(&MonLecteur, LED_YELLOW_ON);
    if (status != MI_OK){
        printf("LED [FAILED]\n");
        goto close;
    }


    // Load Key
    key_index = 0;
    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyA, key_ff, key_index);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
        goto close;
    }



    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyB, key_ff, key_index);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
        goto close;
    }

    // RF field ON
    RF_Power_Control(&MonLecteur, TRUE, 0);

    status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
    if (status != MI_OK){
        printf("No available tag in RF field\n");
        goto close;
    }

    printf("Tag found: UID=");
    for (i = 0; i < uid_len; i++)
        printf("%02X", uid[i]);
    printf(" ATQ=%02X%02X SAK=%02X\n", atq[1], atq[0], sak[0]);


    if ((atq[1] != 0x00) || ((atq[0] != 0x02) && (atq[0] != 0x04) && (atq[0] != 0x18))){
        printf("This is not a Mifare classic tag\n");
        goto tag_halt;
    }

    if ((sak[0] & 0x1F) == 0x08){
        // Mifare classic 1k : 16 sectors, 3+1 blocks in each sector
        printf("Tag appears to be a Mifare classic 1k\n");
        sect_count = 16;
    } else if ((sak[0] & 0x1F) == 0x18){
        // Mifare classic 4k : 40 sectors, 3+1 blocks in 32-first sectors, 15+1 blocks in the 8-last sectors
        printf("Tag appears to be a Mifare classic 4k\n");
        sect_count = 40;
    }



    //while(1);

    /*status = Mf_Classic_AuthenticateCDC(Auth_KeyB, FALSE,0, key1, 0);
    if (status == MI_OK){
        printf("ok\n");
    }
    else{
        printf("Nok\n");
        goto tag_halt;
    }

    status = Mf_Classic_UpdadeAccessBlockCDC(FALSE, 0, key1, key_ff, key_ff, ACC_BLOCK_TRANSPORT, ACC_BLOCK_TRANSPORT, ACC_BLOCK_TRANSPORT, ACC_AUTH_TRANSPORT, Auth_KeyA);
    //status = Mf_Classic_UpdadeAccessBlockCDC(BOOL auth, uint8_t sector, uint8_t old_key_index, uint8_t *new_key_A, uint8_t *new_key_B, uint8_t bc0, uint8_t bc1, uint8_t bc2, uint8_t bc3, BOOL Auth_Key);

    if (status == MI_OK){
        printf("ok\n");
    }
    else{
        printf("Nok\n");
        goto tag_halt;
    }
    while(1);*/
sect_count = 16;
    status = card_read(sect_count);

    goto tag_halt;


tag_halt:

    // Halt the tag
    status = ISO14443_3_A_Halt(&MonLecteur);
    if (status != MI_OK){
        printf("Failed to halt the tag\n");
    goto close;

}

close:
  // Close the reader

    RF_Power_Control(&MonLecteur, FALSE, 0);


    CloseCOM1(&MonLecteur);

done:
  // Display last error
  if (status == MI_OK)
  {
    printf("Done\n");
  } else
  {
    printf("%s (%d)\n", GetErrorMessage(status), status);
  }
  return 0;
}


int card_read(BYTE sect_count)
{
    unsigned char data[240];
    clock_t t0, t1;
    BYTE bloc_count, bloc, sect;
    BYTE offset;
    int status = 0;
    char LenRecv;
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    BYTE uid_len = 12;

    if (bench){
        printf("Reading %d sectors...\n", sect_count);
        t0 = clock();
    }
    bloc = 0;
    for (sect = 0; sect < sect_count; sect++){
        if (!bench)
        printf("Reading sector %02d : ", sect);

        status = Mf_Classic_Authenticate(&MonLecteur, Auth_KeyB, FALSE, sect, key_ff, 0);

        status = Mf_Classic_Read_Sector(&MonLecteur, FALSE, sect, data, Auth_KeyA, 0);


        //status = Mf_Classic_Read_Sector(&MonLecteur, TRUE, sect, data, Auth_KeyA, 0);

        if (status != MI_OK){
            if (bench)
                printf("Read sector %02d ", sect);
            printf("[Failed]\n");
            printf("  %s (%d)\n", GetErrorMessage(status), status);
            status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
            if (status != MI_OK){
                printf("No available tag in RF field\n");
                //goto close;
            }
        }
        else{
            if (!bench){
                printf("[OK]\n");
                // Display sector's data
                if (sect < 32)
                    bloc_count = 3;
                else
                    bloc_count = 15;
                for (bloc = 0; bloc < bloc_count; bloc++){
                    printf("%02d : ", bloc);
                    // Each blocks is 16-bytes wide
                    for (offset = 0; offset < 16; offset++){
                        printf("%02X ", data[16 * bloc + offset]);
                    }
                    for (offset = 0; offset < 16; offset++){
                        if (data[16 * bloc + offset] >= ' '){
                            printf("%c", data[16 * bloc + offset]);
                        } else
                            printf(".");

                    }
                    printf("\n");
                }
            }
        }
    }

    if (bench){
        t1 = clock();
        printf("Time elapsed: %ldms\n", (t1 - t0) / (CLOCKS_PER_SEC/1000));
    }
    return MI_OK;
}


