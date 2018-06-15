#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    char* loadedWord = nextWord(file);
    while(loadedWord != NULL){
        hashMapPut(map, loadedWord, 1);
        free(loadedWord);
        loadedWord = nextWord(file);
    }
    free(loadedWord);
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
    char inputBuffer[256];
    char* suggestion;
    int suggestionDistance;
    int quit = 0;
    struct HashLink* searcher;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);
        
        if(hashMapContainsKey(map, inputBuffer)){
            printf("Your word was found in the dictionary\n");
        } else {
            int lowestDistances[5] = {256};
            char* topPicks[5] = {NULL};
            printf("Did you mean one of these words...?\n");
            for(int i=0; i<map->capacity; ++i){
                if(map->table[i] != NULL){
                    searcher = map->table[i];
                    suggestion = searcher->key;
                    suggestionDistance = levenshtein(&inputBuffer, suggestion);
                    if(suggestionDistance < lowestDistances[5]){
                        for(int i=0; i<5; ++i){
                            if(suggestionDistance < lowestDistances[i]){
                                for(int j=4; j>i; --j){
                                    lowestDistances[j] = lowestDistances[j-1];
                                    topPicks[j] = topPicks[j-1];
                                }
                                lowestDistances[i] = suggestionDistance;
                                topPicks[i] = suggestion;
                                break;
                            }
                        }
                    }
                    searcher = searcher->next;
                    while(searcher != NULL){
                        suggestion = searcher->key;
                        suggestionDistance = levenshtein(&inputBuffer, suggestion);
                        if(suggestionDistance < lowestDistances[5]){
                            for(int i=0; i<5; ++i){
                                if(suggestionDistance < lowestDistances[i]){
                                    for(int j=4; j>i; --j){
                                        lowestDistances[j] = lowestDistances[j-1];
                                        topPicks[j] = topPicks[j-1];
                                    }
                                    lowestDistances[i] = suggestionDistance;
                                    topPicks[i] = suggestion;
                                    break;
                                }
                            }
                        }
                        searcher = searcher->next;
                    }
                }
            }
            printf("The lowest distances found in the dictionary are: \n");
            for(int i=0; i<5; ++i){
                printf("%s :: %d\n", topPicks[i], lowestDistances[i]);
            }

        }
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }
    
    hashMapDelete(map);
    return 0;
}