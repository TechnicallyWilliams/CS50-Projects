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

//DICTIONARY->word = malloc(sizeof(char));
//DICTIONARY->next = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));


struct _DICTIONARY* alphabetNodes[26]; //Array of Linked List aka separate chaining hashing
//my logic flaw was thinking of this is two steps, pointer then malloc. It just pointer, no mallocing



//alphabetNodes[0]->word = NULL;


//DICTIONARY alphabetNodes[26];




/* NOTES */
//Should I do a linked list of nodes with each list starting with the beg. of the alphabet?
//How should I use the "try"? Well try will look into it's existing nodes and see if it complete a path.
//Am I searching the same data structure that I loaded values into? yes 143, 091 / 26
// I think it's a choice between a try and dictionary. Well it looks like they want both so that we can document performance improvements
//How to make a linked list have it's own unique path? Every instantiation should be the beginning of a new list
//Hash --> key ---> value
//Hashing would only take you to the beginning of the list, I can't think of hashing that would take you somewhere in the list


//Adding a word to a character of words

/*
n = strlen(s);

t = malloc((strlen(s) + 1 * sizeof(char));

for ( i < =n…)
{
  t[i] = s[i];
}

*/

//Generic Hasing Function
/*
int  hash_function(char* key)
{
   //hash on first letter of string
   int hash = toupper(key[0]) – ‘A’; //alphabet 0 - 25
   return hash % SIZE;
}

Take complex input and outputs a number.

it will be i in this case

*/



/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // TODO
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
    
    /*for (int i = 0; i < 26; i++) //looks like the key to this was realizing that the pointers themselves have to be malloced, then what they point to have to be malloced
    { //Now i'm testing if this is true in more cases. rule: declare pointer, then malloc it. 
        //alphabetNodes[i]->word = malloc(sizeof(char)); //This threw a seg faul
    }*/
    
    char stringBuffer[LENGTH+1];
    
    //read word
    while ( fgets(stringBuffer, sizeof(stringBuffer), fp) != 0 ) 
    {
        //struct _DICTIONARY* newNODE = malloc(sizeof(struct _DICTIONARY));
        
        //hash on first letter of string
        int hash = (toupper(stringBuffer[0]) - 'A'); //alphabet 0 - 25
        
        if (!alphabetNodes[hash]) 
        {
             alphabetNodes[hash] = malloc(sizeof(struct _DICTIONARY));
           //  alphabetNodes[hash]->next = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY)); //you also have to malloc this, the pointer was setup in the creation of the struct
             alphabetNodes[hash]->next = NULL;
             
             alphabetNodes[hash]->word = malloc(strlen(stringBuffer) + 1 * sizeof(char)); //this might throw seg fault; No
             strcpy(alphabetNodes[hash]->word, stringBuffer);
        } else 
        {
            struct _DICTIONARY* newNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY)); //my logic flaw was not thinking of this is two steps, pointer then malloc
            // newNODE->next = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY)); //you also have to malloc this, the pointer was setup in the creation of the struct
            newNODE->next = NULL;
            
            //I could up here just make next equal the beginning of the list
            newNODE->word = malloc(strlen(stringBuffer) + 1 * sizeof(char)); //this might throw seg fault; No
            strcpy(newNODE->word, stringBuffer);
        
            //what happens after this is the most interesting!    
           struct _DICTIONARY* tempNODE; 

           tempNODE = (struct _DICTIONARY *) malloc(sizeof(struct _DICTIONARY));
           int r = (int) tempNODE;
          // printf("tempnode before: %i.\n", r);
           r = (int) alphabetNodes[hash]->next;
           printf("alphabetNode before: %i.\n", r);
           tempNODE = alphabetNodes[hash]; //address sharing, not pointer sharing, making tempNODE point to the value of what array pointer holding; it's holding a struct; i think it become read
           r = (int) tempNODE;
          // printf("tempnode afteR: %i.\n", r);
          // r = (int) alphabetNodes[hash];
          // printf("alphabetNode after: %i.\n", r);
            
            //internet just told me to have a temp. node to kick things off
            //search for next empty node but it's throwing a segmentation fault
            while (tempNODE->next != NULL) //because the hash is 2, i'm starting at node 2 but it doesn't exist yet, however, everything is in order in the large file
            {
                 r = (int) alphabetNodes[hash];
                 printf("alphabetNode during: %i.\n", r);
                 r = (int) tempNODE->next;
                  printf("tempNODE->next: %i.\n", r); //this is not zero
                  
                //this copies the address, not the value;  so you are address switching or address hopping
               tempNODE = tempNODE->next;//somehow this doesn't affect the original linked list but the last one does. This zero's the memory of next
                r = (int) tempNODE->next;
                 printf("is now 0: %i.\n", r);
                  r = (int) tempNODE;
                  printf("next during: %i.\n", r);
            // printf("each word: %s", alphabetNodes[hash]->word);  //the fact that this doesn't print out until cat makes sense because there is no next node for the beg. of each linked list
            //   printf("2nd word: %s", alphabetNodes[hash]->next->word); //never anything
            // alphabetNodes[hash] = alphabetNodes[hash]->next; //the last thing the base hash is looking at is caterpillar's node
            }
            
            //add new node to linked list queue; setting something that was NULL to actually be something
           tempNODE->next = newNODE; //somehow this affects the original linked list but everything in the while loop doesn't.
            
                   // printf("new node word again: %s", alphabetNodes[hash]->word);
            
            //I might have to create a temporary node during the addition process
           /* next node address
            struct DICTIONARY
            
            next node value
            *alphabetNodes[hash]->next*/
            
            
            //create space in memory for next node and space in memory for member "word" of this node   
           // alphabetNodes[hash]->next = newNODE;//removed "struct"
            
            //looks like you never malloced the original node, unless the array did that
            
            //alphabetNodes[hash]->next = malloc(sizeof(struct _DICTIONARY));
            
            
            //alphabetNodes[hash].word = malloc(sizeof(stringBuffer)); //This is definitely diff. then what's above. caterpiller was the only words thats big enough?
            //printf("the node's word right before malloc %s   ", alphabetNodes[hash].word);
           // alphabetNodes[hash].word = malloc(strlen(stringBuffer) + 1 * sizeof(char)); //It took me awhile to get here but this was a good move. It's about the size of the type
    
            //strcpy(alphabetNodes[hash].word, stringBuffer);
        }

       /* for (int i = 0; i < strlen(stringBuffer); i++)
        {   alphabetNodes[hash].word[i] = stringBuffer[i];  } */
        
        //guaranteeing end of queue points to NULL
        //alphabetNodes[hash]->next->next = NULL; //This may be the wrong syntax
        //alphabetNodes[hash].word = stringBuffer;

    }
    
    /* my main issue is building a linked list, not a dictionary right now */
    printf("med dict. word: %s   ", alphabetNodes[2]->word);
    printf("med dict. word: %s   ", alphabetNodes[2]->next->word);
        printf("med dict. word: %s   ", alphabetNodes[2]->next->next->word); //caterpillar is overwriting cat for some reason; catatrophe is overwriting caterpillar

     
    
    return false;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // TODO
    return 0;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // TODO
    return false;
}
