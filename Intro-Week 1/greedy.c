#include <stdio.h>
#include <cs50.h>
#include <math.h>

float moneyRequested;
float thousandthsPlace;
int moneyOwed;
int coins, quarters, dimes, nickels, pennies;
int i;

//Prototypes
void DecideWhereToBegin();
void GrabQuarters();
void GrabDimes();
void GrabNickels();

        
int main(void)
{

//printf("\nGREEDY ALGORITHM:\n");
//printf("----------------\n");

do
{
    printf("How much money are you owed?\n");
    moneyRequested = .000006 + GetFloat(); 
}
while (moneyRequested < 0);

    
if ( 0 <= moneyRequested && (10000 > moneyRequested) ) 
{
    thousandthsPlace = moneyRequested * 100.000000; 
    moneyOwed = (int) (thousandthsPlace); //this is done to get rid of any numbers past the thousandths place, 
    pennies = moneyOwed;
} else 
{
    return 0;
}

    DecideWhereToBegin();
    
    coins = quarters + dimes + nickels + pennies;
    
    printf("%i\n", coins);

return 0;

}

void GrabNickels() {
        
    if (pennies % 5 == 0)
    {
        nickels = pennies / 5;
        if ((pennies - (nickels * 5)) == 0)
        {
            pennies = pennies - (nickels * 5);
        };
        
    } else 
    {
        nickels = (pennies / 5);  //extracts the amount of nickels used
        pennies = pennies - (nickels * 5);
    }
} 

void GrabDimes() {
    
    if (pennies % 10 == 0)
    {
        dimes = pennies / 10;
        if ((pennies - (dimes * 10)) == 0)
        {
            pennies = pennies - (dimes * 10);
        };
    } else 
    {
        dimes = (pennies / 10);  //extracts the amount of dimes used
        pennies = pennies - (dimes * 10);
        DecideWhereToBegin();        
    }
    
} 

void GrabQuarters() {
    
    if (pennies % 25 == 0)
    {
        quarters = pennies / 25;
        if ((pennies - (quarters * 25)) == 0) 
        {
            pennies = pennies - (quarters * 25);
        };
        
    } else 
    {
        quarters = (pennies / 25);  //extracts the amount of quarters used
        pennies = pennies - (quarters * 25);
        DecideWhereToBegin();
    }

}

void DecideWhereToBegin() {
        
            if (pennies >= 25)
            {   
                GrabQuarters();
            } else if (pennies < 25 && pennies >= 10)
            {
                GrabDimes();
            } else if (pennies < 10 && pennies >= 5)
            {
                GrabNickels();
            } 
            
}

