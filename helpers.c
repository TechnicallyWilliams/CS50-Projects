/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
       
#include <cs50.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "helpers.h"

/**
 *  * Returns true if value is in array of n values, else false.
 */
bool binarySearch(int value, int values[], int min, int max);


/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n) //Rewrite search \\\needle, haystack, size
{

    // TODO: implement a searching algorithm
    if ( n > 0 )
    {
        if (binarySearch(value, values, 0, (n - 1)))
        {
            return true;
        }
    } 
    
    return false;

}


bool binarySearch(int value, int values[], int min, int max)
{
        int midPoint;
            
        if (max < min)    
        {
            return false;
        } else 
        {
            
            midPoint = min + ((max - min) / 2); 
            printf("The i: %i\n", midPoint);
            printf("The midpoint: %i\n", values[midPoint]); //127
            
            if (values[midPoint] < value)
            {
                binarySearch(value, values, midPoint + 1, max); //headed towards the max
                                    
            } else if (values[midPoint] > value)
            {
                binarySearch(value, values, min, (midPoint - 1)); //headed towards the minimum
                
            } else  
            {                   
                 printf("The midpoint within: %i\n", values[midPoint]);
                return true; //
            }
        }
}


/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // TODO: implement an O(n^2) sorting algorithm (BubbleSort)
    int i, j, t, counter;
    
    if (n > 0) 
    {
        for ( i = 0; i < n; i++ )
        {
            counter = 0;
            for ( j = 0; j < n - 1; j++ )
            {
                if (values[j] > values[j + 1])
                {
                    t = values[j];
                    values[j] = values[j + 1];
                    values[j + 1] = t;
                    counter++;
                }
            }
            
            if ( counter == 0 )
            break;
        }
    }
    
    
}
    