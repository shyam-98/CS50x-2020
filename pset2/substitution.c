#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int keyValidation(string);

int main(int argc, string argv[])
{

    if (argc != 2)
    {
        printf("Usage: ./substitution key");
        return 1;
    }

    string key = argv[1];

    if (keyValidation(key) == 1)
    {
        printf("Key must contain 26 unique alphabets.");
        return 1;
    }

    string plaintext = get_string("plaintext: ");
    printf("ciphertext: ");
    for (int i = 0, len = strlen(plaintext); i < len; i++)
    {
        char c = plaintext[i];
        char ec; // enciphered character

        if (isalpha(c))
        {
            if (isupper(c))
            {
                ec = toupper(key[((int) c) - 65]);
                printf("%c", ec);
            }
            else
            {
                ec = tolower(key[((int) c) - 97]);
                printf("%c", ec);
            }
        }
        else
        {
            printf("%c", c);
        }
    }
    printf("\n");
    return 0;
}

int keyValidation(string key)
{
    bool isCorrectLength = 0; // denotes whether the key is 26 characters long or not
    bool isOnlyAlphabetic = 0; // denotes whether the key has only alphabets or not
    bool hasUniqueCharacters = 0; // denotes whether the key has all unique characters or not

    if (strlen(key) == 26)
    {
        isCorrectLength = 1;
        isOnlyAlphabetic = 1; // temporarily flipping
        hasUniqueCharacters = 1; // temporarily flipping

        int charCount[26] = {0};
        for (int i = 0; i < 26; i++)
        {
            // logic to check if all the characters in the key are unique or not

            char c = key[i];
            charCount[((int) toupper(c) - 65)]++;

            // flips back the flag if the character is not an alphabet
            if (!isalpha(c))
            {
                isOnlyAlphabetic = 0;
                break;
            }
        }
        for (int k = 0; k < 26; k++)
        {
            if (charCount[k] != 1)
            {
                hasUniqueCharacters = 0;
                break;
            }
        }
        
    }


    // returns 0 only if the key is valid i.e. all 3 conditions are met
    if (isCorrectLength && isOnlyAlphabetic && hasUniqueCharacters)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}