typedef struct _DICTIONARY
{
    char* word; //This will have to be malloced
    struct _DICTIONARY* next;
} 
DICTIONARY;


DICTIONARY alphabetNodes[26]; //Array of Linked List aka separate chaining hashing

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
    while ( fgets(stringBuffer, sizeof(stringBuffer), fp) != NULL ) 
    {
        
        //hash on first letter of string
        int hash = (toupper(stringBuffer[0]) - 'A'); //alphabet 0 - 25
        
        //search for next empty node
        while (alphabetNodes[hash].next != NULL)
        {
            alphabetNodes[hash] = *alphabetNodes[hash].next; //Unforunaely, latest node in the linked list is overwriting current node
        }
        
        //create space in memory for next node and space in memory for member "word" of this node   
        alphabetNodes[hash].next = (struct _DICTIONARY*) malloc(sizeof(DICTIONARY));//removed "struct"
        
        alphabetNodes[hash].word = malloc(strlen(stringBuffer) + 1 * sizeof(char)); //It took me awhile to get here but this was a good move. It's about the size of the type

        strcpy(alphabetNodes[hash].word, stringBuffer);

    }
    
    /* my main issue is building a linked list, not a dictionary right now */
    
    
    for (int i = 0; i < 4; i++)
    {
        printf("med dict. word: %s   ", alphabetNodes[i].word); //caterpillar is overwriting cat for some reason
    }
     
    
    return false;
}