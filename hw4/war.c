/*  \##\ \     _____     /##/ /  /##/   \      |##|  ____  |
     \##\ \   /##/  \   /##/ /  /##/ /#\ \     |##| | |##| |
      \##\ \ /##/    \ /##/ /  /##/ /\##\ \    |##|  ----  /
       \##\ \ #/ ____ \ #/ /  /##/ _______ \   |##|  _____ \
        \##\ \/ / \##\ \/ /  /##/ /    \##\ \  |##| |  \##\ \
         \##\__/   \##\__/  /##/_/      \##\_\ |##|_|   \##\_\
/*
 * ~~~ WAR.C ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~ War card game written by Josiah Nethery       ~
 * ~ For Computer Science 1 (COP3502) Section 0001 ~
 * ~ October 25th, 2012                            ~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*@######################################################################################@
 *@ Author's note: dynamically allocated arrays were considered for this assignment,     @
 *@ but ultimately decided against. Minimizing runtime was valued more than minimizing   @
 *@ memory requirements.                                                                 @
 *@######################################################################################@
 *@/

/*** INCLUDES ***/
#include<stdio.h>
#include<stdlib.h>
/*** END OF INCLUDES ***/

/*** DEFINITIONS ***/
#define MAX_CARDS 52
/*** END OF DEFINITIONS ***/

/*** ENUMS ***/
enum PLAYTYPE //used for determining the type of turn (playing a card or discarding a card)
{
    play,
    discard
};

enum BOOL //boolean datatype
{
    true,
    false
};
/*** END OF ENUMS ***/

/*** STRUCTS ***/
struct card //represents a single playing card
{
    int value; //numeric value of card
    char suit; //suit of card (Hearts, Spades, Diamonds, Clubs)
};

struct cardQueue //queue of cards representing a player's card hand
{
    struct card cards[MAX_CARDS]; //array for storing player's cards
    int numElements; //integer tracking the number of elements in the array
    int front; //front index of the array
};

struct cardStack //stack that represents the area where players put their cards on their turn
{
    struct card cards[MAX_CARDS]; //array for storing cards on the table
    int top; //index of the top of the stack
};
/*** END OF STRUCTS ***/

/*** FUNCTION PROTOTYPES ***/
//INIT
void initializeGame(enum BOOL *gameIsRunning, struct cardQueue *playerCardQueue, struct cardStack *tableCardStack, FILE *inputFile);
void initializeCardQueue(struct cardQueue *playerCardQueue, FILE *inputFile);
void initializeCardStack(struct cardStack *tableCardStack);

//CHECK EMPTINESS
enum BOOL queueIsEmpty(struct cardQueue playerCardQueue);
enum BOOL stackIsEmpty(struct cardStack tableCardStack);

//QUEUE AND STACK FUNCTIONALITY (PUSH, POP, ENQUEUE, DEQUEUE)
void enqueue(struct cardQueue *playerCardQueue, struct card queueElement);
struct card dequeue(struct cardQueue *playerCardQueue);
void push(struct cardStack *tableCardStack, struct card elementToPush);
struct card pop(struct cardStack *tableCardStack);

//GAMEPLAY FUNCTIONS
void playTurn(enum BOOL *gameIsRunning, struct cardQueue *playerCardQueues, struct cardStack *tableCardStack, enum PLAYTYPE turnType, FILE *outputFile);
enum BOOL playCard(struct cardQueue *playerCardQueue, struct cardStack *tableCardStack, int player, FILE *outputFile);
int compareCards(struct card cardOne, struct card cardTwo);
void enqueueStackedCards(struct cardQueue *playerCardQueue, struct cardStack *tableCardStack);
void war(enum BOOL *gameIsRunning, struct cardQueue *playerCardQueues, struct cardStack *tableCardStack, enum PLAYTYPE turnType, FILE *outputFile);
/*** END OF FUNCTION PROTOTYPES ***/

/*** MAIN ***/
int main(void)
{
    FILE *inputFile, *outputFile;
    inputFile = fopen("games.txt", "r");
    outputFile = fopen("war.out", "w");

    //i is a generic counter for loops
    //numberOfGames stores the amount of games that will be played and consequently how many times to loop
    int i, numberOfGames;

    struct cardQueue playerCardQueue[2]; //player queues declaration. Initialized in game loop (initializeCardQueue)
    struct cardStack tableCardStack; //card stack declaration. initialized in game loop (initializeCardStack)

    enum PLAYTYPE turnType = play; //stores the type of the current turn. initialized to "play", because the game always starts with player one playing a card
    enum BOOL gameIsRunning; //stores the status of the game (whether or not it is running). Initialized in the game loop.

    fscanf(inputFile, "%d", &numberOfGames); //grab the number of games from the first line of the input file

    /*~* GAME LOOP *~*/
    for (i = 0; i < numberOfGames; i++)
    {
        fprintf(outputFile, "Game #%d", i+1);

        //INIT
        initializeGame(&gameIsRunning, playerCardQueue, &tableCardStack, inputFile);

        //play turns until the current game ends
        while(gameIsRunning == true)
        {
            playTurn(&gameIsRunning, playerCardQueue, &tableCardStack, turnType, outputFile);
        }
    }
    /*~* END OF GAME LOOP *~*/

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
/*** END OF MAIN ***/

/*** FUNCTIONS ***/

/*
 * initializeGame: called at the beginning of each game. calls init subprocesses and sets the current game to be running
 * @PARAMS: *gameIsRunning - dictates whether or not the current game loop should continue
 *          *playerCardQueue - array of queues to be initialized
 *          *tableCardStack - the game stack to be initialized
 *          *inputFile - the input file. Passed along to subprocesses
 * @RETURNS: void
 */
void initializeGame(enum BOOL *gameIsRunning, struct cardQueue *playerCardQueue, struct cardStack *tableCardStack, FILE *inputFile)
{
    initializeCardQueue(&playerCardQueue[0], inputFile);
    initializeCardQueue(&playerCardQueue[1], inputFile);
    initializeCardStack(tableCardStack);
    (*gameIsRunning) = true;
}

/*
 * initializeCardQueue: reads in values from the input file into cards to be stored in the player queues
 * @PARAMS: *playerCardQueue - the queue to store the card array
 *          *inputFile - the file from which values are read
 * @RETURNS: void
 */
void initializeCardQueue(struct cardQueue *playerCardQueue, FILE *inputFile)
{
    int i; //generic loop counter
    struct card tempCard; //temporary card object
    (*playerCardQueue).numElements = (*playerCardQueue).front = 0; //set the queue front and numElements to 0, making it "empty"

    for (i = 0; i < MAX_CARDS/2; i++)
    {
        fscanf(inputFile, "%d %c", &tempCard.value, &tempCard.suit);
        enqueue(playerCardQueue, tempCard); //call to enqueue. true to its name, it enqueues playerCardQueue with the tempCard
    }
}

/*
 * initializeCardStack: initializes the stack by setting its top index to -1
 * @PARAMS: *tableCardStack - the stack being initialized
 * @RETURNS: void
 */
void initializeCardStack(struct cardStack *tableCardStack)
{
    (*tableCardStack).top = -1;
}

/*
 * queueIsEmpty: validates if the queue is empty
 * @PARAMS: playerCardQueue - the queue being checked
 * @RETURNS: if the queue is empty, return true
 *           else, return false
 */
enum BOOL queueIsEmpty(struct cardQueue playerCardQueue)
{
    if (playerCardQueue.numElements == 0) //the queue is considered empty if it has no elements
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * stackIsEmpty: validates if the stack is empty
 * @PARAMS: tableCardStack - the stack being checked
 * @RETURNS: if the stack is empty, return true
 *           else, return false
 */
enum BOOL stackIsEmpty(struct cardStack tableCardStack)
{
    if (tableCardStack.top == -1) //the stack is considered empty if its top index is -1 (indicating there are no elements on the stack)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * enqueue: enqueues a queue with an element
 * @PARAMS: *playerCardQueue - the queue to house the new element
 *          queueElement - the element being enqueued
 * @RETURNS: void
 */
void enqueue(struct cardQueue *playerCardQueue, struct card queueElement)
{
    //add the new element to the back of the queue and then increase numElements
    (*playerCardQueue).cards[((*playerCardQueue).front + (*playerCardQueue).numElements)%MAX_CARDS] = queueElement;
    (*playerCardQueue).numElements++;
}

/*
 * dequeue: dequeues a queue
 * @PARAMS: *playerCardQueue - the queue losing an element
 * @RETURNS: void
 */
struct card dequeue(struct cardQueue *playerCardQueue)
{
    //decrement numElements and set the front index to the next index
    (*playerCardQueue).numElements--;
    (*playerCardQueue).front = (((*playerCardQueue).front + 1)%MAX_CARDS);

    //return the card at the previous front index
    if ((*playerCardQueue).front != 0)
    {
        return (*playerCardQueue).cards[(((*playerCardQueue).front - 1)%MAX_CARDS)];
    }
    else
    {
        return (*playerCardQueue).cards[MAX_CARDS - 1];
    }
}

/*
 * push: pushes an element to a stack
 * @PARAMS: *tableCardStack - the stack
 *          elementToPush - the element being pushed
 * @RETURNS: void
 */
void push(struct cardStack *tableCardStack, struct card elementToPush)
{
    //increment the top index of the stack and store the element in that index
    (*tableCardStack).top++;
    (*tableCardStack).cards[(*tableCardStack).top] = elementToPush;
}

/*
 * pop: returns the element from the top of the stack
 * @PARAMS: *tableCardStack - the stack storing the desired element
 * @RETURNS: the card stored in the top index
 */
struct card pop(struct cardStack *tableCardStack)
{
    //decrement the top index and return the card from the former top index
    (*tableCardStack).top--;
    return (*tableCardStack).cards[(*tableCardStack).top + 1];
}

/*
 * playTurn: a turn of play. calls all gameplay subroutines on a case-by-case basis
 * @PARAMS: *gameIsRunning - indicates whether or not the current game should continue
 *          *playerCardQueues - array of player queues
 *          *tableCardStack - the stack for played cards
 *          turnType - the type of turn being called (play or discard)
 *          *outputFile - the output file
 * @RETURNS: void
 */
void playTurn(enum BOOL *gameIsRunning, struct cardQueue *playerCardQueues, struct cardStack *tableCardStack, enum PLAYTYPE turnType, FILE *outputFile)
{
    fprintf(outputFile, "\n");

    //get the results of both players playing their cards. this basically checks if a player can play a card or not.
    enum BOOL ableToPlayCard[2] = {playCard(&playerCardQueues[0], tableCardStack, 1, outputFile), playCard(&playerCardQueues[1], tableCardStack, 2, outputFile)};

    /*~* For play TURNTYPE *~*/
    if (turnType == play)
    {
        //if both players can play their cards, continue
        if (ableToPlayCard[0] == true && ableToPlayCard[1] == true)
        {
            //if player two's card is higher, then player 2 wins the turn and their hand is enqueued with the cards from the stack
            if (compareCards((*tableCardStack).cards[(*tableCardStack).top - 1], (*tableCardStack).cards[(*tableCardStack).top]) > 0)
            {
                enqueueStackedCards(&playerCardQueues[1], tableCardStack);
            }
            //if player one's card is higher, then player 1 wins the turn and their hand is enqueued with the cards from the stack
            else if (compareCards((*tableCardStack).cards[(*tableCardStack).top - 1], (*tableCardStack).cards[(*tableCardStack).top]) < 0)
            {
                enqueueStackedCards(&playerCardQueues[0], tableCardStack);
            }
            //if the cards have the same value, call the war() function
            else
            {
                fprintf(outputFile, "\nWar!!!");
                war(gameIsRunning, playerCardQueues, tableCardStack, turnType, outputFile);
            }
        }
        //if both players can't play their cards, there's a draw
        else if (ableToPlayCard[0] != true && ableToPlayCard[1] != true)
        {
            fprintf(outputFile, "\nThere was a draw!\n");
            (*gameIsRunning) = false;
        }
        //if only player one can't play their card, player 2 wins
        else if (ableToPlayCard[0] != true)
        {
            fprintf(outputFile, "\nPlayer 2 wins!\n");
            (*gameIsRunning) = false;
        }
        //else player 1 wins
        else
        {
            fprintf(outputFile, "\nPlayer 1 wins!\n");
            (*gameIsRunning) = false;
        }
    }

    /*~* For discard PLAYTYPE *~*/
    else
    {
        //if neither player can discard their cards, there is a draw
        if (ableToPlayCard[0] != true && ableToPlayCard[1] != true)
        {
            fprintf(outputFile, "\nThere was a draw!\n");
            (*gameIsRunning) = false;
        }
        //if only player 1 cannot discard their card, player 2 wins
        else if (ableToPlayCard[0] != true)
        {
            fprintf(outputFile, "\nPlayer 2 wins!\n");
            (*gameIsRunning) = false;
        }
        //if only player 2 cannot discard their card, player 1 wins
        else if (ableToPlayCard[1] != true)
        {
            fprintf(outputFile, "\nPlayer 1 wins!\n");
            (*gameIsRunning) = false;
        }
    }
}

/*
 * playCard: attempts to play a card for the player.
 * @PARAMS: *playerCardQueue - the queue where the card is coming from
 *          *tableCardStack - the stack where the card will be placed
 *          player - the player number (1 or 2)
 *          turnType - the type of turn occuring (play or discard)
 *          *outputFile - the output file
 * @RETURNS: true if the player was able to play their card and false if they were not
 */
enum BOOL playCard(struct cardQueue *playerCardQueue, struct cardStack *tableCardStack, int player, FILE *outputFile)
{
    //play the card if the queue isn't empty
    if (queueIsEmpty(*playerCardQueue) != true)
    {
        //print the value and suit to the output, then dequeue and push the dequeued card to the stack
        fprintf(outputFile, "%d%c  ", (*playerCardQueue).cards[(*playerCardQueue).front].value, (*playerCardQueue).cards[(*playerCardQueue).front].suit);
        push(tableCardStack, dequeue(playerCardQueue));
        return true;
    }
    //if the queue is empty, print "NO CARD " and return false
    else
    {
        fprintf(outputFile, "NO CARD  ");
        return false;
    }
}

/*
 * compareCards: compares the value of two cards and returns based on comparison result
 * @PARAMS: cardOne - the first card (to be subtracted from cardTwo)
 *          cardTwo - the second card
 * @RETURNS: Returns 0 if equal, Returns >0 if cardTwo is larger and returns <0 if cardOne is larger
 * Returns -1 if card one wins with an ace and 1 if card two wins with an ace
 */
int compareCards(struct card cardOne, struct card cardTwo)
{
    if (cardTwo.value != 1 && cardOne.value == 1)
    {
        return -1;
    }
    else if (cardTwo.value == 1 && cardOne.value != 1)
    {
        return 1;
    }
    else
    {
        return (cardTwo.value - cardOne.value);
    }
}

/*
 * enqueueStackedCards: continues to enqueue cards from the stack into a queue until the stack is empty
 * @PARAMS: *playerCardQueue - the queue to house the stack elements
 *          *tableCardStack - the stack whose elements are being popped
 * @RETURNS: void
 */
void enqueueStackedCards(struct cardQueue *playerCardQueue, struct cardStack *tableCardStack)
{
    while (stackIsEmpty(*tableCardStack) != true)
    {
        enqueue(playerCardQueue, pop(tableCardStack));
    }
}

/*
 * war: handles all gameplay subroutines related to a war case
 * @PARAMS: *gameIsRunning - determines whether or not the current game should continue
 *          *playerCardQueues - the queues (player hands) from which cards will be drawn
 *          *tableCardStack - stack on which cards will be placed
 *          turnType - determines if the player is playing a card or discarding
 *          *outputFile - the output file
 * @RETURNS: void
 */
void war(enum BOOL *gameIsRunning, struct cardQueue *playerCardQueues, struct cardStack *tableCardStack, enum PLAYTYPE turnType, FILE *outputFile)
{
    turnType = discard;
    playTurn(gameIsRunning, playerCardQueues, tableCardStack, turnType, outputFile);
    turnType = play;
    playTurn(gameIsRunning, playerCardQueues, tableCardStack, turnType, outputFile);
}

/*** END OF FUNCTIONS ***/
