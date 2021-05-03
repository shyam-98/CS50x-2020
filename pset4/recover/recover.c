#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define BLOCK_SIZE 512

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    char *sourcefile = argv[1];

    FILE *sourcefilep = fopen(sourcefile, "r");
    if (sourcefilep == NULL)
    {
        printf("Cannot access the image file.\n");
        return 1;
    }
    
    BYTE buffer[BLOCK_SIZE];
    int count = 0; // keeps count of the number of images recovered
    
    char targetfile[8];
    FILE *targetfilep = NULL;

    while (true)
    {
        size_t bytesRead = fread(buffer, sizeof(BYTE) * BLOCK_SIZE, 1, sourcefilep);
        if (bytesRead == 0 || feof(sourcefilep) || ferror(sourcefilep))
        {
            break;
        }

        /*
        Check first 4 bytes for JPEG signature.
        Notice the use of bitwise & to shorten the fourth byte condition check.
        Bitwise & with f0 effectively preserves the first 4 bits and discards the last 4 bits since,
        the condition can be effectively condensed from checking the range 0xe0 -> 0xef,
        to checking just 0xe[0-f].
        */
        bool isJPEG = buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0;

        // looking for new JPEG files
        if (isJPEG && targetfilep != NULL)
        {
            fclose(targetfilep);
            sprintf(targetfile, "%03d.jpg", count);
            targetfilep = fopen(targetfile, "w");
            count++;
        }
        
        // checking for the initial JPEG file
        if (isJPEG && targetfilep == NULL)
        {
            sprintf(targetfile, "%03d.jpg", count);
            targetfilep = fopen(targetfile, "w");
            count++;
        }

        if (targetfilep != NULL)
        {
            fwrite(buffer, sizeof(BYTE) * BLOCK_SIZE, 1, targetfilep);
        }
    }

    fclose(sourcefilep);
    fclose(targetfilep);
    
    return 0;
}