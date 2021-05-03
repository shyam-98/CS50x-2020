#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int main(void)
{
    string text = get_string("Text: ");
    int len = strlen(text);

    int n_letters = 0;
    int n_spaces = 0;
    int n_words = 0;
    int n_sentences = 0;
    int Grade = 0;

    // iterating over the text character-wise to calculate properties
    for (int i = 0; i < len; i++)
    {
        if (isalpha(text[i])) // counting number of characters in the text.
        {
            n_letters++;
        }

        if (text[i] == ' ') // required for counting number of words in the text.
        {
            n_spaces++;
        }

        // counting the number of sentences in the text.
        // not really accurate though since it counts Mr. or Mrs. as different senteces but,
        // these cases can be ingnored as per the assignment instructions.
        if (text[i] == '.' || text[i] == '?' || text[i] == '!')
        {
            n_sentences++;
        }
    }

    n_words = n_spaces + 1; // Since one space means there are two words in a sentence.

    // Calculating the Coleman-Liau index using the formula:
    // index = 0.0588 * L - 0.296 * S - 15.8 where,
    // L is the average number of letters per 100 words in the text
    // and, S is the average number of sentences per 100 words in the text

    float L = (100.0 * n_letters) / n_words;
    float S = (100.0 * n_sentences) / n_words;
    float index = 0.0588 * L - 0.296 * S - 15.8;
    int grade = round(index);

    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", grade);
    }

}