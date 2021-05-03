#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];
// true by default, because locking the pair first is necessary to detect cycles
bool lock = true;

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // Traversing the triangle on the left side of the diagonal of the preferences array.
    for (int i = 1; i < candidate_count; i++)
    {
        for (int j = 0; j < i; j++)
        {
            int v1 = preferences[i][j];
            int v2 = preferences[j][i];

            if (v1 > v2)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (v2 > v1)
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int n_swap = 1;
    int passes = 0;
    while (n_swap > 0)
    {
        n_swap = 0;
        for (int i = 0; i < pair_count - 1 - passes; i++)
        {
            int v1 = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
            int v2 = preferences[pairs[i + 1].winner][pairs[i + 1].loser] - preferences[pairs[i + 1].loser][pairs[i + 1].winner];;
            if (v2 > v1)
            {
                int p1_winner = pairs[i].winner;
                int p1_loser = pairs[i].loser;

                pairs[i].winner = pairs[i + 1].winner;
                pairs[i].loser = pairs[i + 1].loser;

                pairs[i + 1].winner = p1_winner;
                pairs[i + 1].loser = p1_loser;

                n_swap++;
            }
        }
        passes++;
    }

    return;
}

// function for detecting cycles in the locked matrix by,
// recursively checking submatrices: if rank(matrix) < size(matrix) there are no cycles present
void detect_cycles(int size)
{
    if (size == 0)
    {
        return;
    }

    // detect cycles in all the submatrices
    detect_cycles(size - 1);

    int rank_count = 0;
    bool rank[size];
    for (int i = 0; i < size; i++)
    {
        rank[i] = false;
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (locked[i][j] == true)
            {
                rank[i] = true;
                break;
            }
        }
    }

    for (int i = 0; i < size; i++)
    {
        if (rank[i] == true)
        {
            rank_count++;
        }
    }

    if (rank_count == size)
    {
        lock = false;
    }
}

// To Do:
// 1. find proof for the above approach
// 2. try linear algebra approach where G is acyclic if trace(L) == 2 * rank(L)

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        locked[pairs[i].winner][pairs[i].loser] = true;

        detect_cycles(candidate_count);

        // remove the locked pair if cycles detected;
        if (!lock)
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
        lock = true; // resetting
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int winner;

    for (int i = 0; i < candidate_count; i++)
    {
        int t = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                t++;
            }
        }
        if (t == 0)
        {
            printf("%s\n", candidates[i]);
        }
    }

    return;
}

