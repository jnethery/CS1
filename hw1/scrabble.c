/*
 * ~~~ SCRABBLE.C ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~ Basic scrabble game written by Josiah Nethery ~
 * ~ For Computer Science 1 (COP3502)              ~
 * ~ August 27th, 2012                             ~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

//Definition of the maximum string length for scrabble strings
#define MAX_STRLEN 7

//Letter structure is used to link a point value and virtual tile distribution for each letter of the alphabet
typedef struct
{
    char character;
    unsigned short amount;
    unsigned short value;
}
Letter;

void setLetterAttributes(Letter* letters);
void randomizeString(char* randomString, Letter* letters);
unsigned short wordValidation(char* bestWord, char* enteredString, char* randomString, char dictionary[][MAX_STRLEN], unsigned short dictionarySize, unsigned short *bestWordScore, Letter* letters);
unsigned short pointCalculator(char* enteredString, Letter* letters);

int main(void)
{
    Letter letters[26]; //26 letters corresponding to the 26 letters of the alphabet
    char* randomString = (char*)malloc(MAX_STRLEN*sizeof(char)); //A random string, which represents the tiles that the player recieves at the start of the game
    char enteredWord[MAX_STRLEN]; //Stores the string that the player enters as their word submission
    char bestWord[MAX_STRLEN]; //Stores the current best word submission
    unsigned short bestWordScore = 0; //Stores the current best word score
    unsigned short dictionarySize; //Stores the size of the dictionary of words
    unsigned short quitFlag = 0; //Boolean variable that allows/prohibits the program from quitting
    unsigned short i; //Basic counter for iterations

    FILE *fin;
    fin = fopen("dictionary.txt", "r"); //'dictionary.txt' stores every valid word for the game
    fscanf(fin, "%hu", &dictionarySize); //retrieves the size of the dictionary from the first line of 'dictionary.txt'

    char dictionary[dictionarySize][MAX_STRLEN]; //declaration of the dictionary array

    //Loop fills the dictionary with words from 'dictionary.txt'
    for (i = 0; i < dictionarySize; i++)
    {
        fscanf(fin, "%s", &dictionary[i]);
    }
    fclose(fin); //close the file

    setLetterAttributes(letters); //sets the point values and virtual tile distribution for every letter of the alphabet

    randomizeString(randomString, letters); //creates a string of characters which will serve as the player's tiles
    printf("Here are your letters: %s\n", randomString);

    while (quitFlag == 0)
    {
        unsigned short menuChoice;
        unsigned short validChoiceFlag = 0; //determines whether or not a player's input on the menu is valid

        while (validChoiceFlag == 0)
        {
            printf("What would you like to do?\n\t1-Enter word\n\t2-Quit\n\n");
            scanf("%hu", &menuChoice);
            switch(menuChoice)
            {
                case 1:
                    validChoiceFlag = 1;
                    printf("Word: ");
                    scanf("%s", &enteredWord);
                    printf("\nYou entered the word %s\n", enteredWord);
                    wordValidation(bestWord, enteredWord, randomString, dictionary, dictionarySize, &bestWordScore, letters); //determines if the word that the player enters is valid
                    break;
                case 2:
                    validChoiceFlag = 1;
                    quitFlag = 1;
                    break;
                default:
                    printf("Invalid choice.\nPlease enter '1' or '2'\n");
                    break;
            }
        }
    }

    //tells the player their best word and score when they quit the program (alternatively, informs the player that they played no words)
    if (bestWordScore > 0)
    {
        printf("\nYour best word was %s for %hu points\n", bestWord, bestWordScore);
    }
    else
    {
        printf("\nYou entered no words\n");
    }

    free(randomString); //free memory
    return 0;
}

/*
 * Sets the attributes of each tile type
 */
void setLetterAttributes(Letter* letters)
{
    unsigned short i; //basic counter for iterations
    unsigned short value; //temporarily stores the value of a piece
    unsigned short amount; //temporarily stores the amount of a piece
    for (i = 0; i < 26; i++)
    {
        letters[i].character = 'A' + i;
        switch(i)
        {
            case 0: case 8:
                value = 1;
                amount = 9;
                break;
            case 1: case 2: case 12: case 15:
                value = 3;
                amount = 2;
                break;
            case 3:
                value = 2;
                amount = 4;
                break;
            case 4:
                value = 1;
                amount = 12;
                break;
            case 5: case 7: case 21: case 22: case 24:
                value = 4;
                amount = 2;
                break;
            case 6:
                value = 2;
                amount = 3;
                break;
            case 9: case 23:
                value = 8;
                amount = 1;
                break;
            case 10:
                value = 5;
                amount = 1;
                break;
            case 11: case 18: case 20:
                value = 1;
                amount = 4;
                break;
            case 13: case 17: case 19:
                value = 1;
                amount = 6;
                break;
            case 14:
                value = 1;
                amount = 8;
                break;
            case 16: case 25:
                value = 10;
                amount = 1;
                break;
        }
        letters[i].value = value;
        letters[i].amount = amount;
    }
}

/*
 * Produces a valid random string for the player to use as their tile pieces
 */
void randomizeString(char* randomString, Letter* letters)
{
    srand(time(NULL)); //seeding the random number generator

    unsigned short i, j; //basic counters for iterations
    unsigned short validLetterFlag = 0; //determines if a letter is available for use (recall that there is a finite distribution of 'tiles')
    unsigned short randomNum; //stores a random number

    for (i = 0; i < MAX_STRLEN; i++)
    {
        while (validLetterFlag == 0)
        {
            randomNum = rand()%26;
            if (letters[randomNum].amount > 0) //validating that there are enough tile pieces
            {
                letters[randomNum].amount--;
                validLetterFlag = 1;
            }
        }
        validLetterFlag = 0;
        randomString[i] = 'A' + randomNum;
    }
}

/*
 * Determines if a word submission is valid (TODO: comment this section of code)
 */
unsigned short wordValidation(char* bestWord, char* enteredString, char* randomString, char dictionary[][MAX_STRLEN], unsigned short dictionarySize, unsigned short *bestWordScore, Letter* letters)
{
    unsigned short i, j, arrayPointer = 0, elementExistenceFlag = 0;

    Letter* lettersInRandomString = (Letter*)malloc(sizeof(Letter));
    for (i = 0; i < MAX_STRLEN; i++)
    {
        if (arrayPointer > 0)
        {
            elementExistenceFlag = 0;
            for (j = 0; j < arrayPointer; j++)
            {
                if (randomString[i] == lettersInRandomString[j].character)
                {
                    lettersInRandomString[j].amount++;
                    elementExistenceFlag = 1;
                }
            }
            if (elementExistenceFlag == 0)
            {
                lettersInRandomString = (Letter*)realloc(lettersInRandomString, (arrayPointer+1)*sizeof(Letter));
                lettersInRandomString[arrayPointer].character = randomString[i];
                lettersInRandomString[arrayPointer].amount = 1;
                arrayPointer++;
            }
        }
        else
        {
            lettersInRandomString[0].character = randomString[i];
            lettersInRandomString[0].amount = 1;
            arrayPointer++;
        }
    }

    elementExistenceFlag = 0;
    for (i = 0; i < strlen(enteredString); i++)
    {
        for (j = 0; j < arrayPointer; j++)
        {
            if (lettersInRandomString[j].amount > 0)
            {
                if (enteredString[i] == lettersInRandomString[j].character)
                {
                    lettersInRandomString[j].amount--;
                    elementExistenceFlag++;
                }
            }
        }
    }

    if (elementExistenceFlag != strlen(enteredString))
    {
        printf("\nINVALID WORD\nEntered word doesn't match available letters %s\n\n", randomString);
        return 0;
    }

    free(lettersInRandomString);

    for (i = 0; i < dictionarySize; i++)
    {
        if (strcmp(enteredString, dictionary[i]) == 0)
        {
            unsigned short wordValue = pointCalculator(enteredString, letters);
            printf("That word is worth %hu points\n\n", wordValue);

            if (wordValue > *bestWordScore)
            {
                *bestWordScore = wordValue;
                strcpy(bestWord, enteredString);
            }
            return 1;
        }
    }

    printf("\nINVALID WORD\nEntered word is not in the dictionary\n\n");
    return 0;
}

/*
 * Determines how many points a word is worth
 */
unsigned short pointCalculator(char* enteredString, Letter* letters)
{
    unsigned short i, j, pointTotal = 0; //i and j are used for iterations and pointTotal is used for storing the total amount of points a word is worth

    for (i = 0; i < strlen(enteredString); i++)
    {
        pointTotal += letters[(int)enteredString[i] - 65].value; //retrieves the value for ith letter of the submitted word and adds it to the point total
    }

    return pointTotal;
}
