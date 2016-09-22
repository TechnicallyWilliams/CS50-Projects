#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, string argv[]) 
{
    //This validates that the user truly only entered two arguments.
    if (argc != 2)
    {
        printf("Please enter a single argument\n");
        return 1;
    } 
    
    int i;
    string cipherKey = argv[1];
    int keyLength = strlen(argv[1]);
    
    //This validates that the user did not entry any nonalphabetical character ($ can be exploited)
    for (i = 0; i < keyLength; i++)
    {
        if (isalpha(cipherKey[i]))
        {
            continue;
            
        } else 
        {
            printf("Please enter alphabetical (aA-zZ) characters only\n");
            return 1;
        }
        
    }
    
    string originText = GetString();
    int originTextLength = strlen(originText);
    int j = 0;
    
    //This encrypts the original text using a series of carefully placed if statements.
    for (i = 0; i < originTextLength; i++) 
    {                                           
        if (isalpha(originText[i])) 
        { 
            if (j >= strlen(cipherKey)) //This resets the index for the encryption key
            j = 0;
            
            if (islower(originText[i]) && islower(cipherKey[j])) //One of the 4 total combinations the cipher key and original text can be paired
            {   
                int k = ((int) cipherKey[j] + 7) % 26;
                
                if ((int) originText[i] + k > 122)
                { 
                    originText[i] = 97 + (((((int) originText[i] + k) - 122) % 26) - 1); 
                } else 
                {                                                                 
                    originText[i] = (int) originText[i] + k;    
                }                                                                        
            } else if (isupper(originText[i]) && islower(cipherKey[j]))
            { 
                int k = ((int) cipherKey[j] + 7) % 26;
                
                if ((int) originText[i] + k > 90) 
                {
                    originText[i] = 65 + ((((int) originText[i] + k) - 90) % 26) - 1;
                } else 
                {
                    originText[i] = (int) originText[i] + k;
                }
            } else if (isupper(originText[i]) && isupper(cipherKey[j]))
            {
                int k = ((int) cipherKey[j] + 13) % 26;
                
                if ((int) originText[i] + k > 90) 
                {
                    originText[i] = 65 + ((((int) originText[i] + k) - 90) % 26) - 1;
                } else 
                {
                    originText[i] = (int) originText[i] + k;
                }
            } else
            {
                int k = ((int) cipherKey[j] + 13) % 26;
                
                if ((int) originText[i] + k > 122)
                { 
                    originText[i] = 97 + (((((int) originText[i] + k) - 122) % 26) - 1); 
                } else 
                {                                                                 
                    originText[i] = (int) originText[i] + k;
                } 
            }  printf("%c", originText[i]); j++;
        } else
        {
                printf("%c", originText[i]);   
        }
    }  printf("\n");
    
    
    
    return 0;
}