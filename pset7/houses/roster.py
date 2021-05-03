from sys import argv, exit
import sqlite3


def main():

    if len(argv) != 2:
        print("Usage: python roster.py Gryffindor")
        exit(1)

    house = argv[1]

    conn = sqlite3.connect("students.db")
    cur = conn.cursor()

    # note the use of comma to convert a grouped expression to tuple
    # source - https://stackoverflow.com/questions/16856647/sqlite3-programmingerror-incorrect-number-of-bindings-supplied-the-current-sta
    cur.execute("SELECT first, middle, last, birth FROM students WHERE house = ? ORDER BY last, first;", (house, ))

    houseRoster = cur.fetchall()

    for row in houseRoster:

        # if the student has no middle name
        if not row[1]:
            print(f"{row[0]} {row[2]}, born {row[3]}")
        else:
            print(f"{row[0]} {row[1]} {row[2]}, born {row[3]}")


if __name__ == "__main__":
    main()