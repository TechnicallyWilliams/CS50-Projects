#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, string argv[]) 
{
    //Checks for the correct number of arguments
    if (argc != 2) 
    {
        printf("Please enter a single argument\n");
        return 1;
    }
    
    //Converts string to integer that's not it's ASCII value
    int k = atoi(argv[1]);
    string originText = GetString();
    int i;
    
    //encyrping with a series of carefully placed if statements
    for (i = 0; i < strlen(originText); i++)
    {
        if (isalpha(originText[i])) 
        {
            if (islower(originText[i])) 
            {                                            
                if ((int) originText[i] + k > 122) //Checking if shifting the character exceeds the ASCII of lowercase z
                { 
                    originText[i] = 97 + (((((int) originText[i] + k) - 122) % 26) - 1); //Adding the amount it exceeds ASCII lowercase z to the letter a
                } else 
                {                                                                
                    originText[i] = (int) originText[i] + k;                             
                }                                                                        
            } else
            {
                if ((int) originText[i] + k > 90) //Checking if shifting the character exceeds the ASCII of UPPERcase Z
                {
                    originText[i] = 65 + ((((int) originText[i] + k) - 90) % 26) - 1;  //Adding the amount it exceeds ASCII UPPERcase Z to the letter A
                } else 
                {
                    originText[i] = (int) originText[i] + k;
                }
            }
             printf("%c", originText[i]); //Print encryptic alphabetic characters
        } else {
             printf("%c", originText[i]);  //Print non-alphabetic characters
        }
    } printf("\n");
    
    return 0;    
}
