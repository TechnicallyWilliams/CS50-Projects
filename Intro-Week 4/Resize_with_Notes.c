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
    /*// remember factor of n
    int n = atoi(argv[1]);
    
    // ensure proper usage
    if (argc != 4 || n < 0 || n > 99 )
    {
        if (n < 0 || n > 99) 
        {
            printf("Usage: integer must be between 1 and 100\n");
            return 1;
        }
        
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];*/
    
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
    //printf("This is the RGB size: %d \n", newPadding);
    
    // remember original size of the image (including it's padding)
    DWORD biSizeImage = bi.biSizeImage;
    DWORD bfSize = bf.bfSize;
    LONG origHeight =  bi.biHeight;
    LONG origWidth = bi.biWidth;
    
    //printf("This is the original bi.biWidth: %d \n", bi.biWidth);

    // Resize the images length and width by a factor of n
    bi.biHeight = bi.biHeight * n;
    bi.biWidth = bi.biWidth * n;

    //printf("This is newPadding: %d \n", newPadding);
    //printf("This is original Padding: %d \n", padding);
    bi.biSizeImage = (abs(bi.biHeight) * (bi.biWidth) * 3) + (newPadding * abs(bi.biHeight)); //create enough space for the new size. The maximum  is 4 bytes, thats 32 bits!
    //printf("This is bi.Height typecased: %d \n", (BYTE) bi.biHeight);
    //multiply bi.biHeight by negative 1
    //knowing that padding adds nothing to biSizeImage or bfSize would of helped
    
    // bi.biWidth += newPadding;
    // A combination of the correct data types seems like it will do the trick
    
    // removing 3 here helped
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
    for (int h = 0, biHeight = abs(bi.biHeight); h < biHeight; h++) //this abs should of been a clue that the bi.Height was negative
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
                // skip over padding, if any
               //fseek(inptr, padding, SEEK_CUR);
            
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
                    fseek(inptr,  padding, SEEK_CUR); //This line changed the game and is what completed this project.
                }
            }
        }

    }
    
//even though it resize correctly, bfSize and biSizeImage are different than large.bmp even though it's the same size

 
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
