from sys import argv, exit
import sqlite3
import csv


def main():
    if len(argv) != 2:
        print("Usage: python import.py characters.csv")
        exit(1)

    conn = sqlite3.connect("students.db")
    cur = conn.cursor()

    with open(argv[1], "r") as charactersCsvFile:

        reader = csv.DictReader(charactersCsvFile)

        for row in reader:

            name = row["name"].split()
            house = row["house"]
            birthYear = row["birth"]

            if len(name) == 2:
                firstName = name[0]
                middleName = None
                lastName = name[1]
            else:
                firstName = name[0]
                middleName = name[1]
                lastName = name[2]

            cur.execute("INSERT INTO students (first, middle, last, house, birth) VALUES (?, ?, ?, ?, ?);",
                        (firstName, middleName, lastName, house, birthYear))

    conn.commit()
    conn.close()


if __name__ == "__main__":
    main()