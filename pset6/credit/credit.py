def main():
    ccnumber = input("Number: ")

    if luhn_check(ccnumber=ccnumber):
        print_card_issuer(ccnumber=ccnumber)
    else:
        print("INVALID")

# Checksum implmented using luhn's algorithm - checks syntactic validity of credit card numbers


def luhn_check(ccnumber):
    sum1 = 0
    sum2 = 0
    switch = False

    for c in reversed(ccnumber):
        if switch:
            temp = int(c) * 2
            sum1 += sum(int(digit) for digit in str(temp))  # adding individual digits which may result from multiplying with 2
            switch = not switch
        else:
            sum2 += int(c)
            switch = not switch

    if ((sum1 + sum2) % 10) == 0:
        return True

    return False

# Determines and Prints the issuer for a given credit card


def print_card_issuer(ccnumber):
    length = len(ccnumber)
    first_digit = int(ccnumber[0:1])
    first_two_digits = int(ccnumber[0:2])

    # AMEX check
    if length == 15 and (first_two_digits == 34 or first_two_digits == 37):
        print("AMEX")
    # MASTERCARD check
    elif length == 16 and (first_two_digits >= 51 and first_two_digits <= 55):
        print("MASTERCARD")
    # VISA check
    elif (length == 13 or length == 16) and first_digit == 4:
        print("VISA")
    else:
        print("INVALID")


if __name__ == "__main__":
    main()