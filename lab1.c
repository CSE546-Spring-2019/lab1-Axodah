#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

/*
 * Function to get the number of occurrences of a
 * search string
 *
 *  @param buffer the buffer to count duplicates in
 *
 *  @param numChars the number of characters to parse in the buffer
 *
 *  @param dup the string to check for
 *
 */
long getNumDups(char *buffer, int numChars, char *dup)
{
    long count = 0;
    int checkPos = 0;

    FILE *log = fopen("log.txt", "a");

    //printf("buffer: %.*s|\n\n", numChars, buffer);
    while (checkPos < numChars)
    {
        if (buffer[checkPos] == dup[0])
        {
            fprintf(log, "May be a match in here! \nString:%.*s\nDup:   %s\nLen: %d\nNum: %d\n", strlen(dup), &buffer[checkPos], dup, strlen(dup), numChars);
            fprintf(log, "Starting at %d, char: %c\n", checkPos, buffer[checkPos]);
            // A match could be here, check next characters
            int i = 1;
            int match = 1;
            while(i < strlen(dup))
            {
                if ((checkPos + i) < numChars)
                {
                    fprintf(log, "comparing %c to reference char %c\n", buffer[checkPos + i], dup[i]);
                }
                else
                {
                    // Reached the end of the buffer
                    break;
                }
                // If the next position is valid and doesn't match the string
                if(((checkPos + i) < numChars) && (buffer[checkPos + i] != dup[i]))
                {
                    fprintf(log, "no match!\n\n");
                    match = 0;
                    break;
                }
                i++;
            }


            if ((match == 1) && (i == strlen(dup)))
            {
                fprintf(log, "match found! i=%d\n\n", i);
                count++;
            }
            else
            {
                fprintf(log, "no match!\n\n");
            }
        }
    checkPos++;
    }
    fclose(log);
    return count;
}

/* String presence counter */
    // Don't read entire file into memory, but efficiency isn't an
    // issue here. Don't forget overlapping boundaries of file reads

int main(int argc, char **argv)
{


    if (argc != 4) // Incorrect args, inform user of structure
    {
        printf("Expected 4 arguments, got %d. The syntax is:\n ./count <input-filename> <search-string> <output-filename>\n", argc);
    }
    else // Correct number of args, can continue
    {
        // Verify that both files can be opened and the search string is
        // 20 characters or less
        FILE *inFile = fopen(argv[1], "r");
        FILE *outFile = fopen(argv[3], "a");

        if ((inFile == NULL) || (outFile == NULL))
        {
            if (inFile == NULL)
            {
                printf("ERROR: Input file %s could not be opened!\n", argv[1]);
            }
            else
            {
                fclose(inFile);
            }
            if (outFile == NULL)
            {
                printf("ERROR: Output file %s could not be opened!\n", argv[3]);
            }
            else
            {
                fclose(outFile);
            }
        }
        else if (strlen(argv[2]) > 20)
        {
            // Both files have successfully opened by here
            fprintf(outFile, "ERROR: Length of search string >20 chars!\n");
            fclose(inFile);
            fclose(outFile);
        }
        else // Files can be opened, can continue.
        {
            // Write command to analyze to output file
            fprintf(outFile, "$ %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);

            // Check file size
            fseek(inFile, 0, SEEK_END);
            long fileSize = ftell(inFile);
            fprintf(outFile, "Size of file is %ld\n", fileSize);
            printf("Size of file is %ld\n", fileSize);
            fseek(inFile, 0, SEEK_SET);


            // Count string matches
            long curPos = 0;
            int compLen = strlen(argv[2]);
            char buffer[100];
            long dupsFound = 0;

            if (fileSize < 100) // Buffer only needs filled once
            {
                fread(buffer, 1, fileSize, inFile);
                dupsFound = getNumDups(buffer, fileSize, argv[2]);
                fprintf(outFile, "Number of matches = %ld\n", dupsFound);
                printf("Number of matches = %ld\n", dupsFound);
            }
            else // Buffer needs filled multiple times
            {
                // Initial fill of 100 bytes
                fread(buffer, 1, 100, inFile);
                dupsFound = getNumDups(buffer, 100, argv[2]);

                curPos += 100;
                long sizeRemain = fileSize - curPos;
                /*
                 * Perform subsequent fills.
                 * Since the string is compLen characters, we know that
                 * the last compLen -1 characters are safe to preserve
                 * without causing a duplicate result
                 *
                 */
                while (sizeRemain > 100)
                {
                    // Shift buffer so that only compLen -1 bytes remain
                    int i = 1;
                    int j = compLen - 1;
                    while (i <= j)
                    {
                        buffer[i-1] = buffer[99 - j + i];
                        i++;
                    }

                    i--; //position to start adding data

                    // Read in remaining bytes
                    int numToFill = 100 - i;
                    fread(&buffer[i], 1, numToFill, inFile);

                    // Increment count of duplicates
                    dupsFound += getNumDups(buffer, 100, argv[2]);

                    // Increment loop values
                    curPos += numToFill;
                    sizeRemain = fileSize - curPos;
                }
                // last read, then write to file
                fread(buffer, 1, sizeRemain, inFile);
                dupsFound += getNumDups(buffer, sizeRemain, argv[2]);
                fprintf(outFile, "Number of matches = %ld\n", dupsFound);
                printf("Number of matches = %ld\n", dupsFound);
            }

            // Close files
            fclose(inFile);
            fclose(outFile);
        }

    }

    return 0;
}
