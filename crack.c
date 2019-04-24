#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "md5.h"

const int PASS_LEN=20;        // Maximum any password will be
const int HASH_LEN=33;        // Length of MD5 hash strings

// Given a hash and a plaintext guess, return 1 if
// the hash of the guess matches the given hash.
// That is, return 1 if the guess is correct.
int tryguess(char *hash, char *guess)
{
    int length = strlen(guess);
    
    // Hash the guess using MD5
    char *out = md5(guess, length);
    
    int len = strlen(hash);
    for(int i = 0; i < len; i++)
    {
        if(hash[i] == '\n')
        {
            hash[i] = '\0';
        }
    }

    // Compare the two hashes
    if(strcmp(out, hash) == 0)
    {
        printf("%s\n", guess);
        return 1;
    }

    // Free any malloc'd memory
    free(out);

    return 0;
}

int file_length(char *filename)
{
    struct stat info;
    if (stat(filename, &info) == -1)
    {
        return -1;
    }
    else
    {
        return info.st_size;
    }
}

// Read in the dictionary file and return the array of strings
// and store the length of the array in size.
// This function is responsible for opening the dictionary file,
// reading from it, building the data structure, and closing the
// file.
char **read_dictionary(char *filename, int *size)
{
    *size = 0;
    int len = file_length(filename);
    char *contents = malloc(len);
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        printf("Can't open file for reading\n");
        exit(1);
    }

    fread(contents, 1, len, f);
    fclose(f);
 
    int numwords = 0;
    
    for(int i = 0; i < len; i++)
    {
        if(contents[i] == '\n')
        {
            numwords++;
            contents[i] = '\0';
        }
    }
    
    char **arryWords = malloc(numwords *sizeof(char *));
    
    arryWords[0] = contents;
    int j = 1;
    
    for(int i = 0; i < len-1; i++)
    {
        if(contents[i] == '\0')
        {
            arryWords[j] = &contents[i+1];
            j++;
        }
    }

    *size = numwords;
    return arryWords;
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // Read the dictionary file into an array of strings.
    int dlen;
    char **dict = read_dictionary("rockyou100.txt", &dlen);
    
    // Open the hash file for reading.
    FILE *f = fopen("hashes.txt", "r");
    if (!f)
    {
        printf("Can't open file for reading\n");
        exit(1);
    }
    

    // For each hash, try every entry in the dictionary.
    // Print the matching dictionary entry.
    // Need two nested loops.
    char hashes[1000];
    while(fgets(hashes, 1000, f) != NULL)
    {
        for (int i = 0; i < dlen; i++)
        {
            tryguess(hashes, dict[i]);
        }
    }
}
