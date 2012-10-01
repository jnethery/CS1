/*
 * ~~~ MONOPOLY.C ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~ Basic monopoly game written by Josiah Nethery ~
 * ~ For Computer Science 1 (COP3502) Section 0001 ~
 * ~ September 15th, 2012                          ~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*** STRUCTURE PROTOTYPES ***/
struct Property
{
    char name[30]; //stores the name of the property
    int user; //stores the user# of the property owner
    int num_houses; //stores the # of houses on the property
    int num_hotels; //stores the # of hotels on the property
    struct Property *next; //pointer to the next node in the property list
};
/*** ENDOF STRUCTURE PROTOTYPES ***/

/*** FUNCTION PROTOTYPES ***/
struct Property * addProperty(struct Property *propertyListHead, char name[30], int user); //function for adding nodes to property list
struct Property * deleteProperty(struct Property *propertyListHead, char name[30]); //function for removing nodes from property list
struct Property * sellProperty(struct Property *propertyListHead, char name[30], int user); //function for all processes related to selling a property
struct Property * buyProperty(struct Property *propertyListHead, char name[30], int user); //function for all processes related to buying a property
struct Property * improveProperty(struct Property *propertyListHead, char name[30], int user); //function for all processes related to improving a property
int checkPropertyOwnership(struct Property *propertyListHead, char name[30], int user); //verifies property ownership. return 0 if user parameter matches, -1 if the property does not exist and n > 0 otherwise (user who owns the property)
void viewProperties(struct Property *propertyListHead); //prints the properties in the list
void deleteList(struct Property *propertyListHead); //frees all nodes in the list from memory. to be called at the end of main
/*** ENDOF FUNCTION PROTOTYPES ***/

/*** MAIN ***/
int main(void)
{
    struct Property *propertyListHead; //*propertyListHead is a pointer to the first Property in the linked list
    propertyListHead = NULL; //this ensures that the list is empty

    int playerCount = 0; //number of players in the game
    int currentPlayer = 1; //player whose turn it is
    int menuChoice = 0; //generic int for storing menu choices
    int validMenuChoice = 0; //int functioning as a bool for determining whether or not a menu choice was valid
    int gameRunning = 1; //int functioning as a bool for determining whether or not the game should continue to run

    char propertyName[30]; //int that temporarily stores the property that will be modified by one of the functions

    printf("Welcome to the Monopoly Property Tracker!\n\n");

	//game set-up merely determines the amount of players in the game
    while (validMenuChoice != 1)
    {
        printf("How many people are playing?\n");
        scanf("%d", &playerCount);
        if (playerCount > 0 && playerCount <= 6)
        {
            validMenuChoice = 1;
        }
        else
        {
            printf("Invalid choice. Please enter an integer between 1 and 6\n\n");
        }
    }

	//main menu
    while (gameRunning == 1)
    {
		//get menu choice
        while (validMenuChoice != 1)
        {
            printf("\nPlayer %d, what would you like to do?\n\t1 - Buy Property\n\t2 - Improve Property\n\t3 - Sell Property\n\t4 - View Properties\n\t5 - End Turn\n\t6 - End Game\n", currentPlayer);
            scanf("%d", &menuChoice);
            if (menuChoice > 0 && menuChoice <=6)
            {
                validMenuChoice = 1;
            }
            else
            {
                printf("\nInvalid choice. Please enter an integer between 1 and 6\n");
            }
        }
        validMenuChoice = 0;

		//call functions corresponding to menu choice
        switch(menuChoice)
        {
            case 1: //buyProperty
                printf("\nWhat is the name of the property you wish to buy?\n");
                scanf("%s", propertyName);
                propertyListHead = buyProperty(propertyListHead, propertyName, currentPlayer);
                break;
            case 2: //improveProperty
                printf("\nWhich property do you wish to improve?\n");
                scanf("%s", propertyName);
                propertyListHead = improveProperty(propertyListHead, propertyName, currentPlayer);
                break;
            case 3: //sellProperty
                printf("\nWhich property do you wish to sell?\n");
                scanf("%s", propertyName);
                propertyListHead = sellProperty(propertyListHead, propertyName, currentPlayer);
                break;
            case 4: //viewProperties
                viewProperties(propertyListHead);
                break;
            case 5: //end turn (not a function)
                if (currentPlayer == playerCount)
                {
                    currentPlayer = 1;
                }
                else
                {
                    currentPlayer++;
                }
                break;
            case 6: //quit game (not a function)
                gameRunning = 0;
                printf("\nList of end-game properties: \n");
                viewProperties(propertyListHead);
                printf("\nThanks for playing! %c\n", 1);
                break;
        }
    }
    deleteList(propertyListHead);
    return 0;
}
/*** ENDOF MAIN ***/

/*** FUNCTIONS ***/
struct Property * addProperty(struct Property *propertyListHead, char name[30], int user)
{
	//declaring and initializing a property to insert at the end of the property list
    struct Property * temp = (struct Property *)malloc(sizeof(struct Property)), * helperPointer;
    strcpy(temp->name,name);
    temp->user = user;
    temp->num_houses = 0;
    temp->num_hotels = 0;
    temp->next = NULL;
	//

	//cycle through the list to get to the end
    if (propertyListHead != NULL)
    {
        helperPointer = propertyListHead;
        while (helperPointer->next != NULL)
        {
            helperPointer = helperPointer->next;
        }
        helperPointer->next = temp; //set end->next to temp, effectively adding a new node to the list
    }
    else
    {
        propertyListHead = temp;
    }
    return propertyListHead;
}

struct Property * deleteProperty(struct Property *propertyListHead, char name[30])
{
    struct Property * temp;

	// set the list head to the next node if the list head is the node being deleted
    if (strcmp(propertyListHead->name, name) == 0)
    {
        temp = propertyListHead;
        propertyListHead = propertyListHead->next;
        free(temp);
    }
	// otherwise, cycle through the list and find the node that matches the name parameter passed in and remove it
    else
    {
        struct Property * helperPointer;
        helperPointer = propertyListHead;

        while (strcmp(helperPointer->next->name, name) != 0)
        {
            helperPointer = helperPointer->next;
        }
        temp = helperPointer->next;
        helperPointer->next = helperPointer->next->next;
        free(temp);
    }
    return propertyListHead;
}

struct Property * sellProperty(struct Property *propertyListHead, char name[30], int user)
{
	//first, call checkPropertyOwnership to see if "user" owners property "name"
    int propertyOwnershipFlag = checkPropertyOwnership(propertyListHead, name, user);

	//if the user owns the property, calculate the sell price of the property based on # of houses and hotels and then call deleteProperty to remove the property from the list
    if (propertyOwnershipFlag == 0)
    {
        struct Property * helperPointer;
        helperPointer = propertyListHead;

        while (strcmp(helperPointer->name, name) != 0)
        {
            helperPointer = helperPointer->next;
        }
        printf("\nYou sold %s for $%d.00!\n", helperPointer->name, 100 + 50*helperPointer->num_hotels + 25*helperPointer->num_houses);
        propertyListHead = deleteProperty(propertyListHead, name);
    }
	//if the user does not own the property, notify the user
    else
    {
        printf("\nYou don't own that property\n");
    }

    return propertyListHead;
}

struct Property * buyProperty(struct Property *propertyListHead, char name[30], int user)
{
	//first, call checkPropertyOwnership to see if "user" owners property "name"
    int propertyOwnershipFlag = checkPropertyOwnership(propertyListHead, name, user);

	//if the property is not owned by anyone, add the property to the list
    if (propertyOwnershipFlag == -1)
    {
        propertyListHead = addProperty(propertyListHead, name, user);
    }
	//if the property is owned by the player, notify the player that they already own it
    else if (propertyOwnershipFlag == 0)
    {
        printf("\nYou already own that property\n");
    }
	//if the property is owned by another user, tell the player the user who owns it
    else
    {
        printf("\nPlayer %d owns that property!\n", propertyOwnershipFlag);
    }

    return propertyListHead;
}

struct Property * improveProperty(struct Property *propertyListHead, char name[30], int user)
{
	//first, call checkPropertyOwnership to see if "user" owners property "name"
    int propertyOwnershipFlag = checkPropertyOwnership(propertyListHead, name, user);
    int validMenuChoice = 0;
    int menuChoice;
    int improvementQuantity; //# of houses or hotels to be added to the property

	//if the property is owned by the player, get the property from the list and add the specified number or hotels or houses to it
    if (propertyOwnershipFlag == 0)
    {
        struct Property * helperPointer;
        helperPointer = propertyListHead;

        while (strcmp(helperPointer->name, name) != 0)
        {
            helperPointer = helperPointer->next;
        }

        while (validMenuChoice == 0)
        {
            printf("\nWhich improvement do you wish to purchase?\n\t1 - Hotel\n\t2 - House\n");
            scanf("%d", &menuChoice);
            if (menuChoice == 1 || menuChoice == 2)
            {
                validMenuChoice = 1;
            }
            else
            {
                printf("\nInvalid choice. Please enter a 1 or a 2\n");
            }
        }
        printf("\nHow many do you wish to purchase?\n");
        scanf("%d", &improvementQuantity);

        if (menuChoice == 1) //add hotels
        {
            helperPointer->num_hotels += improvementQuantity;
        }
        else //add houses
        {
            helperPointer->num_houses += improvementQuantity;
        }
    }
	//if the player does not own the property, notify them
    else
    {
        printf("\nYou do not own that property\n");
    }
    return propertyListHead;
}

/*
 * Returns 0 if the user parameter matches the user who owns the Property with name name[30]
 * Returns -1 if the Property does not exist
 * Returns Property's user if the user parameter does not match the user who owns the Property
 */
int checkPropertyOwnership(struct Property *propertyListHead, char name[30], int user)
{
    struct Property * helperPointer;
    helperPointer = propertyListHead;

	//cycle through the property list. if a property's name matches the "name" parameter,
	//check if the property's user matches the "user" parameter. If it does, then return 0,
	//otherwise return the property's user#
    while (helperPointer != NULL)
    {
        if (strcmp(helperPointer->name, name) == 0)
        {
            if (helperPointer->user == user)
            {
                return 0;
            }
            else
            {
                return helperPointer->user;
            }
        }
        helperPointer = helperPointer->next;
    }
	//if the whole list is cycled through and n >= 0 is not yet returned, return -1, indicating that the property is not owned by anyone
    return -1;
}

void viewProperties(struct Property *propertyListHead)
{
	//cycle through the list and print the name of each property and the user who owns it
    if (propertyListHead != NULL)
    {
        printf("\n");
        struct Property * helperPointer;
        helperPointer = propertyListHead;

        while (helperPointer != NULL)
        {
            printf("~*** %s, owned by Player %d ***~\n", helperPointer->name, helperPointer->user);
            helperPointer = helperPointer->next;
        }
    }

	//if the property list is empty, notify the player
    else
    {
        printf("\nThere are no properties to view\n");
    }
}

void deleteList(struct Property *propertyListHead)
{
    if (propertyListHead != NULL)
    {
        if (propertyListHead->next != NULL)
        {
            deleteList(propertyListHead->next);
        }
        free(propertyListHead);
    }
}
/*** ENDOF FUNCTIONS ***/
