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


struct _DICTIONARY* alphabetNodes[26]; //Array of Linked List aka separate chaining hashing


/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // TODO: compare word to dictionary and return false if misspelled
    
    
   // char wordBuffer[strlen(word)]; //i think char array adds automatic null terminators and other shit at the end
    char* wordBuffer = malloc(sizeof(char) * strlen(word));
    int wordSize = ((int) strlen(word));
    int possessive = 39;
    
        //printf("word length: %lu\n", strlen(word));
    //printf("Copy length: %lu\n", strlen(wordBuffer));
   
   /* printf("loading length %s\n", wordBuffer);
    printf("Size of word before modification: %lu.\n", strlen(word));
    printf("Size of word after modification: %lu.\n", strlen(wordBuffer)); //possible to get length before assignment?*/
    for (int i = 0; i < wordSize; i++)
    {
        if ( isalpha(word[i]) || (int) word[i] == possessive )
        {
            strcpy(&wordBuffer[i], &word[i]); 
            
            if ( (int) word[i] != possessive )
            {
                wordBuffer[i] = tolower(wordBuffer[i]); 
            }
            //trcpy(&wordBuffer[i], &lowerLetter); //problem may be stemming from returning an integer over a char
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
    
    
  /*      if (strcmp(&wordBuffer[strlen(word) - 1],"\n") == 0)
    {     
        printf("Case #1: %s\n", word);
        //input[ln] = '\0'
    } else if (strcmp(&wordBuffer[strlen(word) - 2],"\n") == 0)
    {
          printf("Case #2: %s\n", word);
    } else if (strcmp(&wordBuffer[strlen(word)],"\n") == 0)
    {
          printf("Case #3: %s\n", word);
    } else if (strcmp(&wordBuffer[strlen(word) + 1],"\n") == 0)
    {
          printf("Case #4: %s\n", word);
    } else if (strcmp(&wordBuffer[strlen(word) + 2],"\n") == 0)
    {
          printf("Case #5: %s\n", word);
    } */

    int hash = (toupper(word[0]) - 'A');
    hash = hash % 25;
    
    //internet just told me to have a temp. node to kick things off
    if (alphabetNodes[hash] != NULL) 
    {
        //what happens after this is the most interesting!    
        struct _DICTIONARY* tempNODE; 
        //tempNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));
        tempNODE = alphabetNodes[hash]; //address sharing, not pointer sharing, making tempNODE point to the value of what array pointer holding; it's holding a struct; i think it become read
        int dicLength = strlen(tempNODE->word);
        int wordLength = strlen(wordBuffer);
        
        for (int i = 0; strlen(tempNODE->word) > 0; i++)
        {
            if (dicLength != wordLength)
            { 
                    if (tempNODE->next != NULL)
                    {
                        tempNODE = tempNODE->next;
                       // printf("Hmmmm %s\n", tempNODE->word);
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
                            tempNODE = tempNODE->next;
                            dicLength = strlen(tempNODE->word);
                            break;
                        } else
                        {
                            free(wordBuffer);
                            return false;
                        }
                    }
                    
                    if ( i == (wordLength - 1) )
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
    // TODO: Databind my complex data structure with the file
    
    //open dictionary
    FILE* fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        unload(); //still not implemented
        return 1;
    }
    
    char stringBuffer[LENGTH+1];
    
    //read word
    while (fscanf(fp, "%s", stringBuffer) == 1) 
    {
        //hash on first letter of string
        int hash = (toupper(stringBuffer[0]) - 'A'); //alphabet 0 - 25

        if (!alphabetNodes[hash]) 
        {
             alphabetNodes[hash] = malloc(sizeof(struct _DICTIONARY));
             alphabetNodes[hash]->next = NULL;
             
             alphabetNodes[hash]->word = malloc((strlen(stringBuffer)) + 1 * sizeof(char)); //this might throw seg fault; No
             
             //printf("loading length %lu\n", strlen(stringBuffer)); //shouldnt include null terminator
             strcpy(alphabetNodes[hash]->word, stringBuffer);
             
        } else 
        {
            struct _DICTIONARY* newNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY)); //my logic flaw was not thinking of this is two steps, pointer then malloc
            newNODE->next = NULL;
            
            //I could up here just make next equal the beginning of the list
            newNODE->word = malloc(strlen(stringBuffer) + 1 * sizeof(char)); //this might throw seg fault; No
            strcpy(newNODE->word, stringBuffer);
        
            //what happens after this is the most interesting!    
            struct _DICTIONARY* tempNODE; 
            //tempNODE;// = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));
            //tempNODE;//->next = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));
            tempNODE = alphabetNodes[hash]; //address sharing, not pointer sharing, making tempNODE point to the value of what array pointer holding; it's holding a struct; i think it become read
            
            //internet just told me to have a temp. node to kick things off
            while (tempNODE->next != NULL) 
            {
               //this copies the address, not the value;  so you are address switching or address hopping
                tempNODE = tempNODE->next;//somehow this doesn't affect the original linked list but the last one does. This zero's the memory of next
            }
            
            //add new node to linked list queue; setting something that was NULL to actually be something
            tempNODE->next = newNODE; //somehow this affects the original linked list but everything in the while loop doesn't.
            //free(tempNODE); //looks lime mallocing a temporary pointer is a bad idea
           
        }
        

    }
    

    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // TODO: Implement this
    int dictSize = 0;

    struct _DICTIONARY* tempNODE;
    //tempNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));
    //tempNODE = alphabetNodes[hash]; 
    
    for ( int i = 0; i < 26; i++ )
    {
        if (alphabetNodes[i])
        {
            dictSize++;
            tempNODE = alphabetNodes[i];
                 
            while (tempNODE->next != NULL) 
            {
                tempNODE = tempNODE->next;
                dictSize++;
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
    // TODO:
    struct _DICTIONARY* tempNODE;
    //tempNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));
            
    for (int i = 0; i < 26; i++)
    {
        if (alphabetNodes[i] != NULL)
        {
            tempNODE = alphabetNodes[i]; //address sharing, not pointer sharing, making tempNODE point to the value of what array pointer holding; it's holding a struct; i think it become read
            
            //internet just told me to have a temp. node to kick things off
            while (tempNODE->next != NULL) 
            {
                //this copies the address, not the value;  so you are address switching or address hopping
                alphabetNodes[i] = tempNODE;
                //tempNODE = alphabetNodes[i];
                tempNODE = tempNODE->next;//somehow this doesn't affect the original linked list but the last one does. This zero's the memory of next
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

//need to work on the capitalization; need to free all the temp nodes and newnodes
//have to find out how to free all the temp nodes now
//then take notes recording my lessons learned and answer the questions
