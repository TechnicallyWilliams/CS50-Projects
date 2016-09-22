/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }
    else if (atoi(argv[1]) < 1 || atoi(argv[1]) > 100)
    {
        printf("Your multiplier should be a number between 1 and 100\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];
    int n = atoi(argv[1]); 


    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // determine padding for scanlines
    int padding =  (4 - (( bi.biWidth) * sizeof(RGBTRIPLE)) % 4) % 4;
    int newPadding = (4 - (( bi.biWidth * n ) * sizeof(RGBTRIPLE)) % 4) % 4;    
    
    // remember original sizes of the image (including it's padding)
    DWORD biSizeImage = bi.biSizeImage;
    DWORD bfSize = bf.bfSize;
    LONG origHeight =  bi.biHeight;
    LONG origWidth = bi.biWidth;


    // Resize the images length and width by a factor of n
    bi.biHeight = bi.biHeight * n;
    bi.biWidth = bi.biWidth * n;

    bi.biSizeImage = (abs(bi.biHeight) * (bi.biWidth) * 3) + (newPadding * abs(bi.biHeight)); //create enough space for the new size. The maximum  is 4 bytes, thats 32 bits!

    
    // 54 is the bytes required to hold the header values
    bf.bfSize = 54 + bi.biSizeImage;

    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
   
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    bi.biHeight = origHeight;
    bi.biWidth = origWidth;
    bi.biSizeImage = biSizeImage;
    bf.bfSize = bfSize;

    
    // iterate over infile's scanlines
    for (int h = 0, biHeight = abs(bi.biHeight); h < biHeight; h++) //this abs should be a clue that the bi.Height was negative to begin with
    {
        // repeat iteration over infile's scanlines
       for (int i = 0; i < n; i++ ) 
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
        
                // write RGB triple to outfile n times
                for (int k = 0; k < n; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            
            //Add padding if needed
            for (int l = 0; l < newPadding; l++)
            {
                fputc(0x00, outptr);
            }
                            
            if ( i != n - 1)
            {
                fseek(inptr,  - 1 * ( bi.biWidth * 3) , SEEK_CUR);
            } else 
            {
                if (h != h - 1)
                {
                    fseek(inptr,  padding, SEEK_CUR); //This line was a game changer
                }
            }
        }

    }
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    return 0;
}
