from cs50 import get_int


def main():
    while True:
        height = get_int("Height: ")
        if height >= 1 and height <= 8:
            break

    # prints the actual pyramids
    for i in range(height):
        leading_spaces = " " * (height - i - 1)  # stores empty spaces
        hashes = "#" * (i + 1)  # stores hashses

        print(leading_spaces + hashes + "  " + hashes)


if __name__ == "__main__":
    main()