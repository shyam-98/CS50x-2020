#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int height = 0;
    int leading_spaces = 0;
    do
    {
        height = get_int("Height: ");
    }
    while (height < 1 || height > 8);
    
    leading_spaces = height - 1;
    
    //The outer loop is reponsible for giving height to the pyramids.
    for (int i = 0; i < height; i++)
    {
        //This loop prints the spaces in-front of hashes
        for (int sp = 0; sp < leading_spaces; sp++)
        {
            printf(" ");
        }
        leading_spaces--;
        
        //This loop prints the first pyramid
        for (int j = 0; j < i + 1; j++)
        {
            printf("#");
        }
        printf("  ");
        //This loop prints the second pyramid
        for (int j = 0; j < i + 1; j++)
        {
            printf("#");
        }
        printf("\n");
    }
}
