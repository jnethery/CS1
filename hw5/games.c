/*
 * ~~~ GAMES.C ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~ Binary search tree and merge sort written by Josiah Nethery ~
 * ~ For Computer Science 1 (COP3502) Section 0001               ~
 * ~ November 16th, 2012                                         ~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//print condition is used to make calls to printTree more flexible.
enum printCondition
{
    none, //no condition means just print the names of the tree inorder
    gameType, //print names inorder, checking if the players' games match the game passed
    addToPlayers //instead of printing names, add players to an array (for part 2)
};

//the type of binary tree node
enum nodeType
{
    hasOnlyLeftChild,
    hasOnlyRightChild,
    isLeaf
};

struct bstNode
{
    char name[20];
    char game[20];
    struct bstNode *left;
    struct bstNode *right;
};

struct player
{
    char name[20];
    char game[20];
};

struct playerArray
{
    struct player *players;
    int numElements;
};

struct bstNode * readFromFile(FILE *ofp, struct bstNode *bst, struct playerArray *playerArray);
struct bstNode * createNode (char name[20], char game[20]);
struct bstNode * addNode (struct bstNode *bst, struct bstNode *newNode);
struct bstNode * removeNode (struct bstNode *bst, char name[20]);
struct bstNode * findNode (struct bstNode *bst, char name[20]);
struct bstNode * getParent (struct bstNode *bst, struct bstNode *child);
enum nodeType getNodeType (struct bstNode *node);
struct bstNode * getMaxNode (struct bstNode *bst);
struct bstNode * getMinNode (struct bstNode *bst);
void printTree (FILE *ofp, struct bstNode *bst, enum printCondition condition, char game[20], struct playerArray *playerArray);
struct bstNode * freeTree(struct bstNode *bst);
struct player * createPlayer(char name[20], char game[20]);
void addPlayer(struct playerArray *playerArray, struct player *player);
void mergeSort(struct player *players, int start, int end);
void merge(struct player *players, int start, int mid, int end);
void printOrderedArray(FILE *ofp, struct player *players, int length);

int main(void)
{
    struct bstNode *bst; //binary search tree
    struct playerArray playerArray; //array of players for part 2
    enum printCondition addPlayersCondtion = addToPlayers; //addToPlayers print condition for adding binary tree nodes to array
    FILE *ofp; //output file

    ofp = fopen("partygames.out", "w");
    bst = NULL;
    playerArray.players = NULL;
    playerArray.numElements = 0;

    bst = readFromFile(ofp, bst, &playerArray); //reads commands from a file and calls binary tree processes. essentially all of part 1
    printTree(ofp, bst, addToPlayers, NULL, &playerArray); //copies data from tree nodes into an array for part 2
    bst = freeTree(bst); //frees the binary tree
    mergeSort(playerArray.players, 0, playerArray.numElements-1); //does a merge sort on the array
    printOrderedArray(ofp, playerArray.players, playerArray.numElements); //prints out the array
    free(playerArray.players); //frees the player array

    fclose(ofp);
    return 0;
}

/*
 * readFromFile: reads commands from a file and calls bst functions based on those commands
 */
struct bstNode * readFromFile(FILE *ofp, struct bstNode *bst, struct playerArray *playerArray)
{
    FILE *ifp;
    char command[10];
    char name[20];
    char game[20];
    enum printCondition condition = none;

    ifp = fopen("party.txt", "r");

    while (strcmp(command, "PART2") != 0)
    {
        fscanf(ifp, "%s", command);
        if (strcmp(command, "ADD") == 0)
        {
            fscanf(ifp, "%s %s", name, game);
            bst = addNode(bst, createNode(name, game));
        }
        else if (strcmp(command, "REMOVE") == 0)
        {
            fscanf(ifp, "%s", name);
            bst = removeNode(bst, name);
        }
        else if (strcmp(command, "PRINTGAME") == 0)
        {
            fscanf(ifp, "%s", game);
            fprintf(ofp, "\n%s:\n", game);
            condition = gameType;
            printTree(ofp, bst, condition, game, playerArray);
        }
        else if (strcmp(command, "PRINT") == 0)
        {
            fprintf(ofp, "\nPRINT:\n");
            condition = none;
            printTree(ofp, bst, condition, NULL, playerArray);
        }
        else if (strcmp(command, "PART2") == 0)
        {
            fprintf(ofp, "\n---PART2---\n");
            break;
        }
    }

    fclose(ifp);
    return bst;
}

/*
 * createNode: creates a new node to place in the bst
 */
struct bstNode * createNode (char name[20], char game[20])
{
    struct bstNode *tempNode;
    tempNode = (struct bstNode *)malloc(sizeof(struct bstNode));
    strcpy(tempNode->name, name);
    strcpy(tempNode->game, game);
    tempNode->left = NULL;
    tempNode->right = NULL;

    return tempNode;
}

/*
 * addNode: places a new node in the bst
 */
struct bstNode * addNode (struct bstNode *bst, struct bstNode *newNode)
{
    //recursive algorithm for adding nodes. determines node order by numerical value of strings.
    if (bst != NULL)
    {
        if (strcmp(bst->name, newNode->name) > 0)
        {
            bst->left = addNode(bst->left, newNode);
        }
        else
        {
            bst->right = addNode(bst->right, newNode);
        }
    }
    else
    {
        bst = newNode;
    }
    return bst;
}

/*
 * removeNode: removes a node from the bst
 */
struct bstNode * removeNode (struct bstNode *bst, char name[20])
{
    struct bstNode *nodeToRemove;
    nodeToRemove = findNode(bst, name);
    if (nodeToRemove != NULL)
    {
        struct bstNode *parent;
        struct bstNode *newNodePath;

        parent = getParent(bst, nodeToRemove);
        switch(getNodeType(nodeToRemove))
        {
            case (isLeaf):
                newNodePath = NULL;
                break;
            case (hasOnlyLeftChild):
                newNodePath = nodeToRemove->left;
                break;
            case (hasOnlyRightChild):
                newNodePath = nodeToRemove->right;
                break;
            //the default case is if the node being removed has more than 1 child. in this case, the node needs
            //to be replaced by either the minimum node in the right sub-tree or the maximum node in the left sub-tree.
            //I chose the minimum node in the right sub-tree as the replacement node.
            default:
                struct bstNode *replacementNode, *replacementParent;
                replacementNode = getMinNode(nodeToRemove->right);
                replacementParent = getParent(nodeToRemove, replacementNode);

                //this check was necessary because in testing I noticed that if a node being removed had
                //two children but its right sub-tree only had one element, then that element was the minimum node
                if (replacementParent == nodeToRemove)
                {
                    replacementNode->left = nodeToRemove->left;
                    newNodePath = replacementNode;
                }
                else
                {
                    replacementNode->left = nodeToRemove->left;
                    replacementNode->right = nodeToRemove->right;
                    newNodePath = replacementNode;
                    replacementParent->left = NULL;
                }
                break;
        }
        if (parent != NULL)
        {
            if (strcmp(parent->name, nodeToRemove->name) > 0)
            {
                parent->left = newNodePath;
            }
            else
            {
                parent->right = newNodePath;
            }
            free(nodeToRemove);
            return bst;
        }
        else
        {
            bst = newNodePath;
            free(nodeToRemove);
            return bst;
        }
    }
}

/*
 * findNode: finds a node within the bst based on its name
 */
struct bstNode * findNode (struct bstNode *bst, char name[20])
{
    if (bst != NULL)
    {
        if (strcmp(bst->name, name) == 0)
        {
            return bst;
        }
        else if (strcmp(bst->name, name) > 0)
        {
            bst = findNode(bst->left, name);
        }
        else
        {
            bst = findNode(bst->right, name);
        }
    }
    else
    {
        return NULL;
    }
}

/*
 * getParent: find the parent of a given node in the bst
 */
struct bstNode * getParent (struct bstNode *bst, struct bstNode *child)
{
    if (strcmp(bst->name, child->name) > 0)
    {
        if (bst->left != NULL)
        {
            if (strcmp(bst->left->name, child->name) == 0)
            {
                return bst;
            }
            else
            {
                bst = getParent(bst->left, child);
            }
        }
        else
        {
            return NULL;
        }
    }
    else if (strcmp(bst->name, child->name) < 0)
    {
        if (bst->right != NULL)
        {
            if (strcmp(bst->right->name, child->name) == 0)
            {
                return bst;
            }
            else
            {
                bst = getParent(bst->right, child);
            }
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

/*
 * getNodeType: returns the type of node
 */
enum nodeType getNodeType (struct bstNode *node)
{
    enum nodeType nodeType;
    if (node->left == NULL && node->right == NULL)
    {
        nodeType = isLeaf;
    }
    else if (node->left == NULL)
    {
        nodeType = hasOnlyRightChild;
    }
    else if (node->right == NULL)
    {
        nodeType = hasOnlyLeftChild;
    }
    return nodeType;
}

/*
 * getMaxNode: finds the maximum node in a tree (I kept this in, even though I didn't end up using it)
 */
struct bstNode * getMaxNode (struct bstNode *bst)
{
    if (bst != NULL)
    {
        while (bst->right != NULL)
        {
            bst = bst->right;
        }
        return bst;
    }
    else
    {
        return NULL;
    }
}

/*
 * getMinNode: finds the minimum node in a tree
 */
struct bstNode * getMinNode (struct bstNode *bst)
{
    if (bst != NULL)
    {
        while (bst->left != NULL)
        {
            bst = bst->left;
        }
        return bst;
    }
    else
    {
        return NULL;
    }
}

/*
 * printTree: prints the nodes of a tree in order
 */
void printTree (FILE *ofp, struct bstNode *bst, enum printCondition condition, char game[20], struct playerArray *playerArray)
{
    if (bst != NULL)
    {
        if (bst->left != NULL)
        {
            printTree(ofp, bst->left, condition, game, playerArray);
        }
        if (condition == none)
        {
            fprintf(ofp, "\t%s %s\n", bst->name, bst->game);
        }
        else if (condition == gameType && strcmp(bst->game, game) == 0)
        {
            fprintf(ofp, "\t%s\n", bst->name);
        }
        else if (condition == addToPlayers)
        {
            addPlayer(playerArray, createPlayer(bst->name, bst->game));
        }
        if (bst->right != NULL)
        {
            printTree(ofp, bst->right, condition, game, playerArray);
        }
    }
}

/*
 * freeTree: frees a tree from memory
 */
struct bstNode * freeTree(struct bstNode *bst)
{
    if (bst == NULL)
    {
        return NULL;
    }
    else
    {
        return freeTree(removeNode(bst, bst->name));
    }
}

/************************/
/*** PART 2 FUNCTIONS ***/
/************************/

/*
 * createPlayer: creates a new node to add to the player array
 */
struct player * createPlayer(char name[20], char game[20])
{
    struct player *newPlayer;
    newPlayer = (struct player *)malloc(sizeof(struct player));
    strcpy(newPlayer->name, name);
    strcpy(newPlayer->game, game);
    return newPlayer;
}

/*
 * addPlayer: adds a node to the player array
 */
void addPlayer(struct playerArray *playerArray, struct player *player)
{
    if ((*playerArray).players == NULL)
    {
        (*playerArray).players = (struct player *)malloc(sizeof(struct player));
        (*playerArray).players[0] = (*player);
    }
    else
    {
        (*playerArray).players = (struct player *)realloc((*playerArray).players, ((*playerArray).numElements+1)*sizeof(struct player));
        (*playerArray).players[(*playerArray).numElements] = (*player);
    }
    (*playerArray).numElements++;

}

/*
 * mergeSort: recursive algorithm for merge sorting the player array
 */
void mergeSort(struct player *players, int start, int end)
{
    if (start < end)
    {
        int mid = (start + end)/2;
        mergeSort(players, start, mid);
        mergeSort(players, mid+1, end);
        merge(players, start, mid+1, end);
    }
}

/*
 * merge: merges to sub-arrays together to form a sorted array
 */
void merge(struct player *players, int start, int mid, int end)
{
    struct player *temp = (struct player *)calloc(end+1, sizeof(struct player));
    int i = start, j = start, k = mid;

    //this happens while each of the two arrays being merged still has elements.
    //I added this because if one of the arrays runs out of elements, then no more
    //comparisons need to be made and all remaining elements can simply be added
    while (j < mid && k < end+1 && i < end+1)
    {
        if (strcmp(players[j].game, players[k].game) < 0)
        {
            temp[i] = players[j];
            j++;
        }
        else if (strcmp(players[j].game, players[k].game) > 0)
        {
            temp[i] = players[k];
            k++;
        }
        else
        {
            if (strcmp(players[j].name, players[k].name) < 0)
            {
                temp[i] = players[j];
                j++;
            }
            else
            {
                temp[i] = players[k];
                k++;
            }
        }
        i++;
    }

    //adding trailing elements
    while (j < mid && k > end)
    {
        temp[i] = players[j];
        i++;
        j++;
    }
    while (j >= mid && k <= end)
    {
        temp[i] = players[k];
        i++;
        k++;
    }

    //copying these values back into the original array
    for (i = start; i < end+1; i++)
    {
        players[i] = temp[i];
    }
    free(temp);
}

/*
 * printOrderedArray: prints out the names in the array
 */
void printOrderedArray(FILE *ofp, struct player *players, int length)
{
    char game[20] = "";

    int i;
    for (i = 0; i < length; i++)
    {
        if (strcmp(players[i].game, game) != 0)
        {
            strcpy(game, players[i].game);
            fprintf(ofp, "%s\n", game);
        }
        fprintf(ofp, "\t%s\n", players[i].name);
    }
}
