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

struct Property
{
    char name[30];
    int user;
    int num_houses;
    int num_hotels;
    struct Property *next;
};

struct Property * addProperty(struct Property *propertyListHead, char name[30], int user);
struct Property * deleteProperty(struct Property *propertyListHead, char name[30]);
struct Property * sellProperty(struct Property *propertyListHead, char name[30], int user);
struct Property * buyProperty(struct Property *propertyListHead, char name[30], int user);
struct Property * improveProperty(struct Property *propertyListHead, char name[30], int user);
int checkPropertyOwnership(struct Property *propertyListHead, char name[30], int user);
void viewProperties(struct Property *propertyListHead);

int main(void)
{
    struct Property *propertyListHead; //*propertyListHead is a pointer to the first Property in the linked list
    propertyListHead = NULL; //this ensures that the list is empty

    int playerCount = 0;
    int currentPlayer = 1;
    int menuChoice = 0;
    int validMenuChoice = 0;
    int gameRunning = 1;

    char propertyName[30];

    printf("Welcome to the Monopoly Property Tracker!\n\n");

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

    while (gameRunning == 1)
    {
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

        switch(menuChoice)
        {
            case 1:
                printf("\nWhat is the name of the property you wish to buy?\n");
                scanf("%s", propertyName);
                propertyListHead = buyProperty(propertyListHead, propertyName, currentPlayer);
                break;
            case 2:
                printf("\nWhich property do you wish to improve?\n");
                scanf("%s", propertyName);
                propertyListHead = improveProperty(propertyListHead, propertyName, currentPlayer);
                break;
            case 3:
                printf("\nWhich property do you wish to sell?\n");
                scanf("%s", propertyName);
                propertyListHead = sellProperty(propertyListHead, propertyName, currentPlayer);
                break;
            case 4:
                viewProperties(propertyListHead);
                break;
            case 5:
                if (currentPlayer == playerCount)
                {
                    currentPlayer = 1;
                }
                else
                {
                    currentPlayer++;
                }
                break;
            case 6:
                gameRunning = 0;
                printf("\nList of end-game properties: \n");
                viewProperties(propertyListHead);
                printf("\nThanks for playing! %c\n", 1);
                break;
        }
    }

    return 0;
}

struct Property * addProperty(struct Property *propertyListHead, char name[30], int user)
{
    struct Property * temp = (struct Property *)malloc(sizeof(struct Property)), * helperPointer;
    strcpy(temp->name,name);
    temp->user = user;
    temp->num_houses = 0;
    temp->num_hotels = 0;
    temp->next = NULL;

    if (propertyListHead != NULL)
    {
        helperPointer = propertyListHead;
        while (helperPointer->next != NULL)
        {
            helperPointer = helperPointer->next;
        }
        helperPointer->next = temp;
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

    if (strcmp(propertyListHead->name, name) == 0)
    {
        temp = propertyListHead;
        propertyListHead = propertyListHead->next;
        free(temp);
    }
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
    int propertyOwnershipFlag = checkPropertyOwnership(propertyListHead, name, user);

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
    else
    {
        printf("\nYou don't own that property\n");
    }

    return propertyListHead;
}

struct Property * buyProperty(struct Property *propertyListHead, char name[30], int user)
{
    int propertyOwnershipFlag = checkPropertyOwnership(propertyListHead, name, user);

    if (propertyOwnershipFlag == -1)
    {
        propertyListHead = addProperty(propertyListHead, name, user);
    }
    else if (propertyOwnershipFlag == 0)
    {
        printf("\nYou already own that property\n");
    }
    else
    {
        printf("\nPlayer %d owns that property!\n", propertyOwnershipFlag);
    }

    return propertyListHead;
}

struct Property * improveProperty(struct Property *propertyListHead, char name[30], int user)
{
    int propertyOwnershipFlag = checkPropertyOwnership(propertyListHead, name, user);
    int validMenuChoice = 0;
    int menuChoice;
    int improvementQuantity;

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

        if (menuChoice == 1)
        {
            helperPointer->num_hotels += improvementQuantity;
        }
        else
        {
            helperPointer->num_houses += improvementQuantity;
        }
    }
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
    return -1;
}

void viewProperties(struct Property *propertyListHead)
{
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
    else
    {
        printf("\nThere are no properties to view\n");
    }
}
