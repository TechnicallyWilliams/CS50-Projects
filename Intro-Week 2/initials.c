#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[]) 
{
    int i, letterCount;
    string userName;
    
    userName = GetString();
    letterCount = strlen(userName);
    
    for (i=0; i <= letterCount; i++)
    {
           if (((int) userName[i] != 32 && i == 0) || ((int) userName[i] != 32 && ((int) userName[i-1] == 32))) //Accounts for spacing initially and non-itially
           {
               if (((int) userName[i] >= (int) 'a') && ((int) userName[i] <= (int) 'z'))
                {
                    userName[i] = userName[i] - ('a' - 'A');
                    printf("%c", userName[i]);  
                } else 
                {
                    printf("%c", userName[i]);
                }  
           }
           else if (i == letterCount)
           {
               printf("\n");
           }
     }
    
    return 0;
}