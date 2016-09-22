/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

/*
typedef struct
{
    BYTE  firstByte;
    BYTE  secondByte;
    BYTE  thirdByte;
    BYTE  fourthByte;
} __attribute__((__packed__))
JPEGSig; */

int main(int argc, char* argv[])
{

    // open input file (memory card file) 
    FILE* inptr = fopen("card.raw", "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", "card.raw");
        return 2;
    }
        
        
    BYTE jpegBuff[512];
   // JPEGSig jpegBuff[128]; //this was a struct buffer in the bitmap code, now it's an jpeg buffer
   // JPEGSig jpegClear;//this is to replace all elements of a buffer once it's been cleared
    int writeFlag = 0;
    
    
   char title[8]; //this is a character array for creating new files
   int jpegCounter = 0;


    while ( fread(&jpegBuff, sizeof(jpegBuff), 1, inptr) == 1) //Reads the memory card file 512 bytes at a time
    { 
       
       if (!writeFlag) 
       {
           if ( jpegBuff[0] == 255 && jpegBuff[1] == 216 && jpegBuff[2] == 255 && jpegBuff[3] == 224 ) //((jpegBuff[3] > 223) && (jpegBuff[3] < 240)
           {
                sprintf(title, "%03d.jpg", jpegCounter);
                FILE* img = fopen(title, "w");
                if (img == NULL)
                {
                    fclose(inptr);
                    fprintf(stderr, "Could not create %s.%d\n", title, jpegCounter);
                    return 3;
                }
                
                fwrite(&jpegBuff, sizeof(jpegBuff), 1, img);
        
                fclose(img);
                jpegCounter++;
                writeFlag = 1;
                
            }
        
        } else if ( jpegBuff[0] == 255 && jpegBuff[1] == 216 && jpegBuff[2] == 255 && ( (jpegBuff[3] > 223) && (jpegBuff[3] < 240) ) )
        {
            sprintf(title, "%03d.jpg", jpegCounter);
            FILE* img = fopen(title, "w");
            if (img == NULL)
            {
                fclose(inptr);
                fprintf(stderr, "Could not create %s.%d\n", title, jpegCounter);
                return 3;
            }
            
            fwrite(&jpegBuff, sizeof(jpegBuff), 1, img);
            fclose(img);
            jpegCounter++;
            
        } else 
        {
            FILE* img = fopen(title, "a");
            if (img == NULL)
            {
                fclose(inptr);
                fprintf(stderr, "Could not create %s.%d\n", title, jpegCounter - 1);
                return 3;
            }
            
             fwrite(&jpegBuff, sizeof(jpegBuff), 1, img);
             fclose(img);
        }
       
    }

    fclose(inptr);

        

    return 0;
    
}