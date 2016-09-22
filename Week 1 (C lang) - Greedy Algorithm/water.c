#include <stdio.h>
#include <cs50.h>

int main(void)
{
    /* a program to express number of water bottles used via total minutes in the shower. */
    
    // Not used: float avgGallonsPerMinute_2016 = 1.5;
    float showerMinutes, totalOz, bottlesPerShower;
    
    //Based on Scientific Survey
    int avgOzPerMinute_2016 = 192;
    int ozSingleWaterBottle = 16;
    
    printf("How many minutes do you spend in the shower?  ");
    
    showerMinutes = GetInt();
    
    totalOz = (float) avgOzPerMinute_2016 * showerMinutes;
    
    bottlesPerShower = totalOz / (float) ozSingleWaterBottle;
    
    printf("\nSHOWER LENGTH       # OF BOTTLES USED \n");
    printf("-------------       ----------------- \n");
    printf("%.2f mins          %.2f bottles\n\n", showerMinutes, bottlesPerShower);
    
    return 0;
    
}