#include <cs50.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

bool luhn_check(long, int);
int get_nth_digit(long, int);

int main(void)
{
    long card_number = get_long("Number: ");
    int length = 0;

    // Since log10(0) returns -inf
    if (card_number != 0)
    {
        length = floor(log10(card_number)) + 1;
    }

    if (luhn_check(card_number, length))
    {
        int first_digit = get_nth_digit(card_number, length - 1);
        int first_two_digits = (first_digit * 10) + get_nth_digit(card_number, length - 2);

        // AMEX Check
        if (length == 15 && (first_two_digits == 34 || first_two_digits == 37))
        {
            printf("AMEX\n");
        }
        // MasterCard Check
        else if (length == 16 && (first_two_digits >= 51 && first_two_digits <= 55))
        {
            printf("MASTERCARD\n");
        }
        // VISA Check
        else if ((length == 13 || length == 16) && first_digit == 4)
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}

// Checksum implmented using luhn's algorithm - used to check syntactic validity of credit card numbers
bool luhn_check(long card_number, int length)
{
    bool is_syntactically_valid = false;
    int sum_step_1 = 0, sum_step_2 = 0;

    for (int i = 0; i < length; i++)
    {

        if (i % 2 == 0) // calulates sum of even-located digits
        {
            sum_step_2 += (card_number % 10);
            card_number /= 10;
        }
        else // calulates sum of odd-located digits
        {
            int temp1 = 0, temp2 = 0;

            temp1 = 2 * (card_number % 10);
            card_number /= 10;

            // adding individual digits of double digit numbers
            if (temp1 > 9)
            {
                // probably an overkill since temp1 can't have more than two digits (9 * 2 = 18)
                while (temp1 > 0)
                {
                    temp2 += (temp1 % 10);
                    temp1 /= 10;
                }
            }
            else
            {
                temp2 = temp1;
            }
            
            sum_step_1 += temp2;
        }
    }

    int sum = sum_step_1 + sum_step_2;

    if (sum % 10 == 0)
    {
        is_syntactically_valid = true;
    }

    return is_syntactically_valid;
}

// returns nth digit of a given number where the rightmost digit is n=0.
// not yet sure if this is the best way.
int get_nth_digit(long number, int n)
{
    long t1, t2;
    
    t1 = pow(10, n + 1);
    t2 = number % t1;
    
    if (n > 0)
    {
        t2 = t2 / pow(10, n);
    }
    
    return t2;
}
