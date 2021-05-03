// Implements a dictionary's functionality

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
const unsigned int N = 190788;

// Hash table
node *table[N] = {NULL};

// Number of words loaded in the dictionary
unsigned int lcount = 0;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    bool isPresent = false;

    // Converting `word` to lowercase, for calculating hash value
    char copy[LENGTH + 1];
    strcpy(copy, word);
    for (int i = 0; copy[i] != '\0'; i++)
    {
        copy[i] = tolower(copy[i]);
    }
    
    unsigned int index = hash(copy);
    node *cursor = table[index];

    while (cursor != NULL)
    {
        if (strcasecmp(cursor -> word, word) == 0)
        {
            isPresent = true;
            break;
        }
        cursor = cursor -> next;
    }
    
    return isPresent;
}

// Hashes word to a number
// Implements one_at_a_time hash function developed by Bob Jenkins.
// Source: https://en.wikipedia.org/wiki/Jenkins_hash_function
unsigned int hash(const char *word)
{
    size_t i = 0, length = strlen(word);
    unsigned int hash = 0;

    while (i < length)
    {
        hash += word[++i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    FILE *dictp = fopen(dictionary, "r");
    if (dictp == NULL)
    {
        return false;
    }

    char ulword[LENGTH + 1];

    while (fscanf(dictp, "%s", ulword) != EOF)
    {
        unsigned int index = hash(ulword);
        
        node *tnode = malloc(sizeof(node));
        if (tnode == NULL)
        {
            return false;
        }

        strcpy(tnode -> word, ulword);

        tnode -> next = table[index];
        table[index] = tnode;

        lcount++;
    }

    fclose(dictp);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return lcount;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    for (int index = 0; index < N; index++)
    {
        node *cursor = table[index];
        table[index] = NULL;

        while (cursor != NULL)
        {
            node *tmp = cursor;
            cursor = cursor -> next;
            free(tmp);
        }
    }
    return true;
}
