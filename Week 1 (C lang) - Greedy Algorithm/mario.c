#include <stdio.h>
#include <cs50.h>

int pyramidHeight, userInput, i, j, k;
    
int acceptInput(int heightOfPyramid)
{
        for (i=1; i < heightOfPyramid + 1; i++) 
        {
            for (j=heightOfPyramid - i; j > 0; j-- ) 
            { 
                printf(" ");
            }
            
            for (k = 0; k < i ; k++) 
            {   
                if(k==0){
                    printf("##");
                } else {
                    printf("#");
                }
            }
            printf("\n");
        } 
    
    return 0;
}
    

int validateInput(void) 
{
    do
    {
        printf("What is the height of Mario's pyramid? (Hint: 'Height <= 23')  ");
        userInput = GetInt();
//        printf("Height:\e[4m%d\e[0m\n", userInput);
    }
    while (userInput < 0 || userInput > 23);
 
    pyramidHeight = userInput;
    return acceptInput(pyramidHeight);
}


int main(void)
{
   /* printf("\nCREATING MARIO'S half-PYRAMID:\n");
    printf("------------------------------\n"); */
    return validateInput();
}
