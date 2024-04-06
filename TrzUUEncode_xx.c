#include <stdio.h>
#include <string.h>

// NOT COMPLETE YET

unsigned short CalcCRC16(char *, unsigned long, unsigned short);


int main(int argc, char *argv[])
{
   FILE *hFileI, *hFileO;
   unsigned short sCrc;
   char cBufO[63];
   char cBufI[3];
   unsigned char cR, cI, cO;

   if(argc == 3)
   {
      hFileI = fopen(argv[1], "rb");
      if(hFileI)
      {
         hFileO = fopen(argv[2], "wb");
         if(hFileO)
         {
            for(cR = strlen(argv[1]); cR; cR--)
            {
               if((argv[1][cR - 1] == '\\') || (argv[1][cR - 1] == ':')) break;
            }
            fprintf(hFileO, "begin 644 %s\r\n", &argv[1][cR]);
            sCrc = 0;
            cI = 0;
            cO = 1;
            while((cR = fread(cBufI, 1, 3, hFileI)) != 0)
            {
               sCrc = CalcCRC16(cBufI, cR, sCrc);
               cI += cR;
               if(cR < 2) cBufI[1] = 0;
               if(cR < 3) cBufI[2] = 0;
               cBufO[cO] = (cBufI[0] >> 2) & 0x3F;
               cBufO[cO++] += cBufO[cO] ? 0x20 : 0x60;
               cBufO[cO] = ((cBufI[0] << 4) & 0x30) | ((cBufI[1] >> 4) & 0x0F);
               cBufO[cO++] += cBufO[cO] ? 0x20 : 0x60;
               cBufO[cO] = ((cBufI[1] << 2) & 0x3C) | ((cBufI[2] >> 6) & 0x03);
               cBufO[cO++] += cBufO[cO] ? 0x20 : 0x60;
               cBufO[cO] = cBufI[2] & 0x3F;
               cBufO[cO++] += cBufO[cO] ? 0x20 : 0x60;
               if(cO == 61)
               {
                  cBufO[0] = cI + 0x20;
                  cBufO[cO++] = 0x0D;
                  cBufO[cO++] = 0x0A;
                  fwrite(cBufO, 1, cO, hFileO);
                  cI = 0;
                  cO = 1;
               }
            }
            if(cO > 1)
            {
               cBufO[0] = cI + 0x20;
               cBufO[cO++] = 0x0D;
               cBufO[cO++] = 0x0A;
               fwrite(cBufO, 1, cO, hFileO);
            }
            fprintf(hFileO, "`\r\nend\r\n%04x\r\n", sCrc);
            fclose(hFileO);
         }
         else
         {
            printf("Cannot open \"%s\"\n", argv[2]);
         }
         fclose(hFileI);
      }
      else
      {
         printf("Cannot open \"%s\"\n", argv[1]);
      }
   }
   else
   {
      printf("Syntax: UUEncode InFile OutFile\n");
      printf("Copyright (C) 2014 Mehrdad Taraz\n");
   }
   
   return 0;
}


// on entry, pDat => start of data, lLen = length of data, sCrc = incoming CRC
unsigned short CalcCRC16(char *pDat, unsigned long lLen, unsigned short sCrc)
{
   #define poly 0x1021

   unsigned char c;

   while(lLen--)                 // step through bytes in buffer
   {
      sCrc ^= (*pDat++ << 8);    // xor data with crc's high byte
      c = 8;
      while(c--)                 // shift 8 timess
      {
         if(sCrc & 0x8000)       // if bit 15 is set
         {
            sCrc <<= 1;          // shift
            sCrc ^= poly;        // xor with crc polynomial
         }
         else
         {
            sCrc <<= 1;          // shift
         }
      }
   }

   return sCrc;                  // return updated crc
}
