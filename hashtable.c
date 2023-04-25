#include "trove.h"

//  'CREATE' A NEW, EMPTY LIST - JUST A NULL POINTER
LIST *list_new(void)
{
    return NULL;
}

//  DETERMINE IF A PATHNAME IS STORED IN A GIVEN LIST
bool list_find(LIST *list, char *wantedWord, char *wantedPathname)
{
    while(list != NULL) {
        if(strcmp(list->word, wantedWord) == 0) {
            if(strcmp(list->pathname, wantedPathname) == 0){
                return true;
            }
        }
        list	= list->next;
    }
    return false;
}

//  ALLOCATE SPACE FOR A NEW LIST ITEM, TESTING THAT ALLOCATION SUCCEEDS
LIST *list_new_item(char *newWord, char *newPathname)
{
    LIST *new       = calloc(1, sizeof(LIST) );
    CHECK_ALLOC(new);
    strcpy(new->word, newWord);
    strcpy(new->pathname, newPathname);
    new->next       =  NULL;
    return new;
}

//  ADD A NEW (STRING) ITEM TO AN EXISTING LIST
LIST *list_add(LIST *list, char *newWord, char *newPathname)
{
    if(list_find(list, newWord, newPathname)) {            // only add each item once
        return list;
    }
    else {                                      // add new item to head of list
        // printf("    %s\n", newWord); // For debugging purposes: allows you to read the actual words that are added to the trove-file
        LIST *new   = list_new_item(newWord, newPathname);
        new->next   = list;
        return new;
    }
}

// simple function to hash a string
uint32_t hash_string(char *word)
{
    uint32_t hash = 0;

    while(*word != '\0') {
        hash = hash*33 + *word;
        ++word;
    }
    return hash;
}

// creates and returns a new hashtable that is empty 
HASHTABLE *hashtable_new(void)
{
    HASHTABLE   *new = calloc(HASHTABLE_SIZE, sizeof(LIST *));

    CHECK_ALLOC(new);
    return new;
}

//  ADD A NEW STRING TO A GIVEN HASHTABLE
void hashtable_add(HASHTABLE *hashtable, char *word, char *filename)
{
    uint32_t h   = hash_string(word) % HASHTABLE_SIZE;    // choose list

    hashtable[h] = list_add(hashtable[h], word, filename);
}