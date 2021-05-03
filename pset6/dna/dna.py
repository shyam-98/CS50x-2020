from sys import argv, exit
import csv


def main():
    if len(argv) != 3:
        print("Usage: python dna.py data.csv sequence.txt")
        exit(1)

    # reading database file
    with open(argv[1], "r") as csvfile:
        dbList = list()
        for row in csv.DictReader(csvfile):
            dbList.append(dict(row))

    # reading DNA sequence
    with open(argv[2], "r") as txtfile:
        dnaSequence = txtfile.readline()

    # Matches the DNA sequence with all people in the given database
    for person in dbList:
        name = person.pop("name")

        if dnaMatch(person, dnaSequence):
            print(name)
            exit(0)

    print("No match")

# Checks for a DNA sequence match by matching length of longest consecutive sequence of all STRs for each person in the database


def dnaMatch(person, dnaSequence):

    for shortTandemRepeat, strCount in person.items():
        if strSequenceCount(dnaSequence, shortTandemRepeat) != int(strCount):
            return False

    return True

# Returns the length of longest repeating sequence of a given Short Tandem Repeats(STRs) in the dnaSequence


def strSequenceCount(dnaSequence, shortTandemRepeat):
    strLength = len(shortTandemRepeat)
    runningCount = 0  # Count of the STRs sequence currently being processed
    longestCount = 0  # Count of the longest sequence of STRs found so far
    start = 0
    end = -1
    loc = 0

    while True:
        loc = dnaSequence.find(shortTandemRepeat, start, end)
        if loc == -1:
            return longestCount
        else:
            runningCount = 0
            while True:
                if dnaSequence[loc:loc+strLength] == shortTandemRepeat:
                    loc += strLength
                    runningCount += 1
                else:
                    start = loc
                    break
            if runningCount > longestCount:
                longestCount = runningCount


if __name__ == "__main__":
    main()