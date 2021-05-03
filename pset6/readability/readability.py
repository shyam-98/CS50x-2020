def main():
    text = input("Text: ")

    n_letters = 0
    n_spaces = 0
    n_words = 0
    n_sentences = 0
    Grade = 0

    for c in text:
        if c.isalpha():
            n_letters += 1

        if c == " ":
            n_spaces += 1

        # counting the number of sentences in the text.
        # not really accurate though since it counts Mr. or Mrs. as different senteces but,
        # these cases can be ingnored as per the assignment instructions.
        if c == "." or c == "?" or c == "!":
            n_sentences += 1

    n_words = n_spaces + 1  # Since one space means there are two words in a sentence.

    # Calculating the Coleman-Liau index using the formula:
    # index = 0.0588 * L - 0.296 * S - 15.8 where,
    # L is the average number of letters per 100 words in the text
    # and, S is the average number of sentences per 100 words in the text

    L = (100 * n_letters) / n_words
    S = (100 * n_sentences) / n_words
    index = 0.0588 * L - 0.296 * S - 15.8
    grade = round(index)

    if grade < 1:
        print("Before Grade 1")
    elif grade > 16:
        print("Grade 16+")
    else:
        print(f"Grade {grade}")


if __name__ == "__main__":
    main()