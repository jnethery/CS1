/*
 * ~~~ JUMBLE.C ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~ Anagram solver written by Josiah Nethery      ~
 * ~ For Computer Science 1 (COP3502) Section 0001 ~
 * ~ October 8th, 2012                             ~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*** INCLUDES ***/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*** END OF INCLUDES ***/

/*** DEFINITIONS ***/
#define MAX_STRLEN 20
#define FLUSH_INPUT(x) while(x!='\n'){x=getchar();}
#define SWAP(x, y, z) z = x; x = y; y = z;
/*** END OF DEFINITIONS ***/

/*** FUNCTION PROTOTYPES ***/
char ** populateDictionary(char **dictionary, int *dictionarySize);
int findDictionaryWord(char **dictionary, char *word, int low, int high);
int isUppercase(char *word);
void generateWordPermutations(char **dictionary, char *word, char *originalWord, int k, int *existenceFlag, int dictionarySize, FILE *output);
void freeDictionary(char **dictionary, int dictionarySize);
/*** END OF FUNCTION PROTOTYPES ***/

/*** MAIN ***/
int main(void)
{
    FILE *output;
    char **dictionary = (char **)malloc(sizeof(char *)); //declaration of the dictionary
    char word[MAX_STRLEN], originalWord[MAX_STRLEN], menuChoice; //declaring the temp arrays word and originalWord and the temp char menuChoice
    int dictionarySize, existenceFlag, isRunning = 1, isUpperFlag; //declaring menu flags and dictionarySize

    output = fopen("jumbleout.txt", "w");
    dictionary = populateDictionary(dictionary, &dictionarySize); //populate the dictionary

    /*
     * MAIN MENU
     */
    printf("Welcome to the jumble puzzle solver!\n\n");
    while (isRunning == 1)
    {
        printf("Would you like to enter a jumbled word? (y/n)\n");
        menuChoice = getchar();
        if (getchar() == '\n') //checks to see if the user entered only 1 character. if so, then getchar() == '\n'
        {
            if (menuChoice == 'y' || menuChoice == 'Y')
            {
                existenceFlag = 0; //assume that the word does not exist in the dictionary
                isUpperFlag = 0; //assume that the input word is not uppercase
                while (isUpperFlag == 0) //loop while the user inputs a non-uppercase string
                {
                    printf("\nWhat word would you like scored?\n");
                    scanf("%s", word);
                    if (strlen(word) <= (MAX_STRLEN - 1) && isUppercase(word) == 0) //break from the loop when the input string is shorter than (MAX_STRLEN - 1) and is uppercase
                    {
                        isUpperFlag = 1;
                    }
                    else
                    {
                        printf("Invalid input. Please enter an UPPERCASE string less than 19 characters long\n"); //notify the user that their input is unacceptable
                    }
                }
                FLUSH_INPUT(menuChoice); //flush the input so that the program doesn't get confused by junk characters in the stream. this is necessary, because it is possible for the user to enter too many characters and overflow "word"
                strcpy(originalWord, word); //copy the contents of word into originalWord. necessary for string permutation algorithm
                generateWordPermutations(dictionary, word, originalWord, 0, &existenceFlag, dictionarySize, output);
                if (existenceFlag == 0) //existenceFlag is modified by generateWordPermutations. if existenceFlag == 0, then there are no valid permutations of the input string
                {
                    printf("Sorry, no permutations of %s form a valid word.\n\n", word);
                    fprintf(output, "Sorry, no permutations of %s form a valid word.\n", word);
                }
                else
                {
                    printf("\n");
                }
            }
            else if (menuChoice == 'n' || menuChoice == 'N')
            {
                isRunning = 0; //quit the menu loop if the input is 'n' or 'N'
            }
            else
            {
                printf("Invalid menu choice. Please enter 'y' or 'n'\n\n"); //notify the user that their input is unacceptable
            }
        }
        else //if the user entered more than one character
        {
            printf("Invalid menu choice. Please enter a single letter 'y' or 'n'\n\n");
            FLUSH_INPUT(menuChoice); //flush the input so that the program doesn't get confused by junk characters in the stream. this is necessary, because the user input more than 1 character
        }
    }
    /*
     * END OF MAIN MENU
     */

    printf("\nQuitting...\n");
    freeDictionary(dictionary, dictionarySize); //free the dictionary from memory
    fclose(output); //close the output file
    return 0;
}
/*** END OF MAIN ***/

/*** FUNCTIONS ***/

/*
 * Allocate space for the strings stored in the input file.
 * PARAMETERS: dictionary - an array of strings, dictionarySize - stores the number of arrays in the dictionary
 * RETURNS: the filled dictionary array.
 */
char ** populateDictionary(char **dictionary, int *dictionarySize)
{
    FILE *ifp;
    int i;
    char tempString[MAX_STRLEN]; //tempString will temporarily store strings read from the input file

    ifp = fopen("dictionary.txt", "r");
    fscanf(ifp, "%d", dictionarySize); //determine the size of the dictionary from the first line of the inpute file

    dictionary = (char **)realloc(dictionary, (*dictionarySize)*sizeof(char *)); //allocate enough space for each char array required for the dictionary

    for (i = 0; i < *dictionarySize; i++)
    {
        fscanf(ifp, "%s", &tempString);
        dictionary[i] = (char *)malloc((strlen(tempString))*sizeof(char)); //allocate exactly enough space to fit the string read in
        strcpy(dictionary[i], tempString); //copy the string into the allocated memory
    }

    fclose(ifp);
    return dictionary;
}

/*
 * Search the dictionary for a specific string. Since it employs a recursive binary search, low and high bounds are
 * required parameters.
 * PARAMETERS: dictionary - an array of strings, word - string to be found in dictionary, low - low bound of binary search, high - high bound of binary search
 * RETURNS: 0 if the word is found, -1 if the word is not found
 */
int findDictionaryWord(char **dictionary, char *word, int low, int high)
{
    int mid = ((low + high)/2);

    //first, check to see if the middle element is the string being searched for.
    //if it is, then no other steps are necessary
    if (strcmp(word, dictionary[mid]) == 0)
    {
        return 0;
    }
    //check if low == mid or high == mid, indicating that no more divisions can be made
    //(i.e., the string being searched for does not exist in the dictionary)
    else if (low == mid || high == mid)
    {
        return -1;
    }
    else
    {
        //if the string is greater numerically than the middle element of the dictionary, then the
        //middle index is set to the low bound. Otherwise, the middle index is set to the high bound.
        if (strcmp(word, dictionary[mid]) > 0)
        {
            findDictionaryWord(dictionary, word, mid, high);
        }
        else
        {
            findDictionaryWord(dictionary, word, low, mid);
        }
    }
}

/*
 * Verify that a string is uppercase.
 * PARAMETERS: word - the string being examined
 * RETURNS: 0 if the string is uppercase, -1 otherwise
 */
int isUppercase(char *word)
{
    int i;

    for (i = 0; i < strlen(word); i++)
    {
        //if any character of the string is greater than or equal to 'a', it is a lowercase element
        //if any character is less than 'A', then it is not alphabetic
        if (word[i] >= 'a' || word[i] < 'A')
        {
            return -1;
        }
    }
    return 0;
}

/*
 * Generates string permutations of "word" and prints out the permutations that exist in the dictionary
 * PARAMETERS: dictionary - array of strings, word - string to generate permutations of,
 * originalWord - same as word, except it will be unaffected by the SWAPs, k - root index of substring,
 * existenceFlag - used in main for determining if a word has any valid permutations, dictionarySize - number of rows in dictionary
 * output - output file pointer
 * RETURNS: (null)
 */
void generateWordPermutations(char **dictionary, char *word, char *originalWord, int k, int *existenceFlag, int dictionarySize, FILE *output)
{
    int j; //used to loop through substrings of word
    char temp; //temporary character use for element SWAP

    if (k == strlen(word)) //if the substring root index is equal to strlen(word), then there are no more substrings
    {
        //if the anagram exists as a string in the dictionary, then print acknowledgement
        //to the screen and to the output file
        if (findDictionaryWord(dictionary, word, 0, dictionarySize) == 0)
        {
            printf("A valid permutation of %s is %s.\n", originalWord, word);
            fprintf(output, "A valid permutation of %s is %s.\n", originalWord, word);
            (*existenceFlag)++;
        }
    }
    else
    {
        //if the substring root index is not equal to strlen(word), then more substrings must exist
        for (j = k; j <= strlen(word); j++)
        {
            //SWAP characters j and k and then call the function recursively, so that all permutations can be checked.
            SWAP(word[j], word[k], temp);
            generateWordPermutations(dictionary, word, originalWord, k+1, existenceFlag, dictionarySize, output);
            SWAP(word[j], word[k], temp);
        }
    }
}

/*
 * Frees the dictionary from memory
 * PARAMETERS: dictionary - the array block being freed, dictionarySize - the amount of arrays in the dictionary
 * RETURNS: (null)
 */
void freeDictionary(char **dictionary, int dictionarySize)
{
    int i;
    for (i = 0; i < dictionarySize; i++)
    {
        free(dictionary[i]); //free each row of the dictionary
    }
    free(dictionary); //finally, free the dictionary itself
}
/*** END OF FUNCTIONS ***/
