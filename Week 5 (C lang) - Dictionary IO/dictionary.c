/**
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdlib.h> //for malloc
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

typedef struct _DICTIONARY
{
    char* word; //This will have to be malloced
    struct _DICTIONARY* next;
} 
DICTIONARY;


//Array of pointers made for structs to kick off the beginning of 26 linked lists (separate chain hashing)
struct _DICTIONARY* alphabetNodes[26]; 

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    //copy the value of read-only words to a buffer in order to eventually lowercase all letters for case sensitivity
    char* wordBuffer = malloc(sizeof(char) * strlen(word));
    int wordSize = (int) strlen(word);
    int possessive = 39;

    //copies read-only letters to a buffer and lowercases the letters
    for (int i = 0; i < wordSize; i++)
    {
        if ( isalpha(word[i]) || (int) word[i] == possessive )
        {
            strcpy(&wordBuffer[i], &word[i]); 
            
            if ( (int) word[i] != possessive )
            {
                wordBuffer[i] = tolower(wordBuffer[i]); 
            }
        } else
        {
           if (wordBuffer == 0)
           {
               return false;
           }
    
            free(wordBuffer);
            return false;
        }
    }
    
    //hashing function to find the linked-list containing the word 
    int hash = (toupper(word[0]) - 'A');
    hash = hash % 25;
    
    //TODO: compare word to dictionary list of words and return true if found
    if (alphabetNodes[hash] != NULL) 
    {
        struct _DICTIONARY* tempNODE; //create a distinct temporary pointer we can use to shine a light on the same addresses as the linked list (no need to malloc because it's only a pointer)
        tempNODE = alphabetNodes[hash]; //reorients pointer to same address as the first pointer in the linked list
        int dicLength = strlen(tempNODE->word); 
        int wordLength = strlen(wordBuffer); //
        
        for (int i = 0; dicLength > 0; i++)
        {
            if (dicLength != wordLength) //Don't even begin looking at the word if they don't have the same length
            { 
                    if (tempNODE->next != NULL)
                    {
                        tempNODE = tempNODE->next; //this reorients the pointer to the same address as the next pointer; 
                        dicLength = strlen(tempNODE->word); 
                    } else
                    {
                        free(wordBuffer);
                        return false;
                    }
            } else
            {
                for (int i = 0; i < wordLength; i++)
                {
                    if (strcmp(&wordBuffer[i], &tempNODE->word[i]) != 0) 
                    {
                        if (tempNODE->next != NULL)
                        {
                            tempNODE = tempNODE->next; //this reorients the pointer to the same address as the next pointer; 
                            dicLength = strlen(tempNODE->word);
                            break;
                        } else
                        {
                            free(wordBuffer);
                            return false;
                        }
                    }
                    
                    if ( i == (wordLength - 1) ) //if it makes it to the last letter in the list, then you've found the word
                    {
                        free(wordBuffer);
                        return true;
                    }
                }
            }
        }
        
    }

    free(wordBuffer);
    return false;

}


/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    //open dictionary
    FILE* fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        unload();
        return 1;
    }
    
    //create an array to capture n characters read from the file
    char stringBuffer[LENGTH+1];
    
    //read word using fscanf to avoid capturing new line terminators ( Databind my complex data structure with the file )
    while (fscanf(fp, "%s", stringBuffer) == 1) 
    {
        //hashing function to find the corresponding linked-list to place the word
        int hash = (toupper(stringBuffer[0]) - 'A');

        if (!alphabetNodes[hash]) //checks if the first pointer has been malloced
        {
             alphabetNodes[hash] = malloc(sizeof(struct _DICTIONARY));
             alphabetNodes[hash]->next = NULL; //do not malloc because this will only ever be a pointer
             alphabetNodes[hash]->word = malloc((strlen(stringBuffer)) + 1 * sizeof(char));
             strcpy(alphabetNodes[hash]->word, stringBuffer);
             
        } else 
        {
            struct _DICTIONARY* newNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY)); //pointer then malloc for a space in memory for this new node
            newNODE->next = NULL; //do not malloc because this will only ever be a pointer
            newNODE->word = malloc(strlen(stringBuffer) + 1 * sizeof(char));
            strcpy(newNODE->word, stringBuffer);
        
            struct _DICTIONARY* tempNODE; 
            tempNODE = alphabetNodes[hash]; //reorient pointer to the same address as this pointer
            
            while (tempNODE->next != NULL) 
            {
               //this reorients the pointer to the same address as the next pointer; 
                tempNODE = tempNODE->next; 
            }
            
            //add new node to linked list queue by giving pointer an actual address to point to
            tempNODE->next = newNODE;
        }
        

    }
    

    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    int dictSize = 0;

    struct _DICTIONARY* tempNODE;
    
    for ( int i = 0; i < 26; i++ )
    {
        if (alphabetNodes[i])
        {
            dictSize++; //increment size everytime initial node of a linked list is true
            tempNODE = alphabetNodes[i];
                 
            while (tempNODE->next != NULL) 
            {
                tempNODE = tempNODE->next;
                dictSize++; //increment size everytime a node within the linked list is true
            }
        } 
    }
    
    return dictSize;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    struct _DICTIONARY* tempNODE;
            
    for (int i = 0; i < 26; i++)
    {
        if (alphabetNodes[i] != NULL)
        {
            tempNODE = alphabetNodes[i]; ////this reorients the pointer to the same address as the next pointer; 
            
            while (tempNODE->next != NULL) 
            {
                alphabetNodes[i] = tempNODE; //makes it so that the previous pointer can be freed from memory
                tempNODE = tempNODE->next;
                free(alphabetNodes[i]->word);
                free(alphabetNodes[i]);
            }
            
            free(tempNODE->word);
            free(tempNODE);
        }
        
        if ( i == 25 )
        {
            return true;
        }
    }
    
    return false;
}

