//
// File:		Major3-TrentThompson.cpp
// Assignment:	Major 3
// Programmer:	Trent Thompson
// Date:		April 3, 2019
// Description:	This program is an example of what can be done with STL lists, and binary
//				trees. This program has multiple functions built around lists and trees
//				in order to demonstrate the concepts.
//



// Libraries
#include <stdio.h>
#include <list>
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <chrono>

// Disable windows warnings
#pragma warning(disable:4996)

// Using the standard namespace
using namespace std;



// Constants
#define INVALID_FILE -2
#define ERROR -1
#define CODE_DAT "Code.dat"
#define MESSAGE_DAT "Message.dat"
#define STRING_LENGTH 121
#define THOUSAND 1000

// Menu Constants
#define PRINT_LIST 1
#define ENTER_CODE 2
#define READ_MESSAGE 3
#define PLAY_GAME 4
#define EXIT 5



// listNode Struct Definition
typedef struct listNode {
	char character;
	char morseCode[STRING_LENGTH];
} listNode;



// treeNode Struct Definition
typedef struct treeNode {
	char character;
	treeNode *left;
	treeNode *right;
}treeNode;



// Prototypes
void showInterface(void);
char parseString(char *incomingString);
int generateList(list <listNode> &morseCodeList);
int getInt(int *incomingInt);
void showList(list <listNode> &morseCodeList);
treeNode* createBlankNode(void);
int generateTree(list <listNode> &morseCodeList, treeNode *&startingNode);
int getMorseString(char *input);
void printPath(treeNode *&start, char pathString[STRING_LENGTH]);
int printMessage(treeNode *&start);
int validateLine(char *input);
void deleteTree(treeNode *&node);
char randomChar(list <listNode> &morseCodeList);
void randomNumberGame(treeNode *&start, char randomChar);



int main(void) {
	list <listNode> morseCodeList;
	bool exitLoop = false;
	char userString[STRING_LENGTH] = { 0 };
	treeNode *startingNode = NULL;
	time_t t;

	// Seed random number generator
	srand((unsigned) time(&t));

	// If the list generation producs an error, quit the program
	if (generateList(morseCodeList) == ERROR) {
		return ERROR;
	}

	// Allocate a starting node for the tree
	startingNode = createBlankNode();
	// Error check the allocation
	if (startingNode == NULL) {
		printf("Error allocating memory.");
		return ERROR;
	}

	// generate the binary tree
	if (generateTree(morseCodeList, startingNode) == ERROR) {
		printf("Error allocating memory.");
		return ERROR;
	}

	// Loop for menu options
	while (exitLoop != true) {
		int userInput = 0;

		// Print option menu
		showInterface();

		// Get a character from the keyboard
		getInt(&userInput);
		
		// Option for printing out the list
		if (userInput == PRINT_LIST) {
			showList(morseCodeList);
		}
		// Option for entering a morse code string
		else if (userInput == ENTER_CODE) {
			// If the string entered by the user is valid, print the path for that string
			if (getMorseString(userString) != ERROR) {
				printPath(startingNode, userString);
			}
		}
		// Option for reading the message from the file
		else if (userInput == READ_MESSAGE) {
			// Try to read the message from the file
			switch (printMessage(startingNode)) {
			case ERROR:
				return ERROR;
			case INVALID_FILE:
				printf("Invalid input file.\n\n");
				break;
			}
		}
		// Option for playing the game
		else if (userInput == PLAY_GAME) {
			randomNumberGame(startingNode, randomChar(morseCodeList));
		}
		// Option for exiting the program
		else if (userInput == EXIT) {
			deleteTree(startingNode);
			exitLoop = true;
		}
		// Otherwise it is invalid input.
		else {
			printf("Invalid input.\n\n");
		}
	}
	
	return 0;
}



// 
// Function:	showInterface
// Description:	Prints the menu interface for the program
// Parameters:	NONE
// Returns:		NONE
//
void showInterface(void) {
	printf("Menu:\n");
	printf("1. Print list\n");
	printf("2. Enter morse code to see path\n");
	printf("3. Read message from file\n");
	printf("4. Play guessing game\n");
	printf("5. Exit\n");
	printf("Option: ");
}



// 
// Function:	generateList
// Description:	Generates the linked list from the file
// Parameters:	list <listNode> &morseCodeList: The list 
//				to populate from the file
// Returns:		Int: ERROR(-1): When there is an error reading to,
//				writing from, or opeing the file.
//				Int 0: on success
//
int generateList(list <listNode> &morseCodeList) {
	FILE *ifp = NULL;	

	// Open the file
	ifp = fopen(CODE_DAT, "r");
	// Error check the file opening was successful
	if (ifp == NULL) {
		printf("\nError opening %s.", CODE_DAT);
		return ERROR;
	}

	// Loop through the file
	while (!feof(ifp)) {
		char buffer[STRING_LENGTH] = { 0 };

		// Create a new list node
		listNode newNode;

		// Get a line from the file to the buffer
		if (fgets(buffer, STRING_LENGTH, ifp) == NULL) {
			// if there was an error reading from the file, close the file and return an error
			if (ferror(ifp)) {
				// Close the file
				if (fclose(ifp) != 0) {
					printf("Error closing file.");
					return ERROR;
				}

				return ERROR;
			}
		}

		// Store the character from the buffer into the new node
		newNode.character = parseString(buffer);

		// Store the morse code string from the buffer into the new node
		strcpy(newNode.morseCode, buffer);

		// Push the new node onto the list
		morseCodeList.push_back(newNode);
	}

	// Close the file and check for error
	if (fclose(ifp) != 0) {
		printf("\nError closing %s.", CODE_DAT);
		return ERROR;
	}

	return 0;
}



// 
// Function:	parseString
// Description:	Parses the string to turn the incoming string into just morse code,
//				and returns the character associated with that line of morse code
// Parameters:	char *incomingString: the string to turn into just morse code
// Returns:		char: the character associated with the morse code string
//
char parseString(char *incomingString) {
	char *pChar = NULL;
	char returnChar = 0;

	// Set the return char to the first index of the incoming string
	returnChar = incomingString[0];

	// Point the character pointer to the comma in the incoming string
	pChar = strchr(incomingString, ',');

	// Increment the character pointer so that it is just to the right of the comma
	pChar++;

	// Copy the char pointer to the incoming string, because here it is just the morse code string
	strcpy(incomingString, pChar);

	// Now let's remove the newline character from our string if there is one
	pChar = strchr(incomingString, '\n');
	if (pChar != NULL) {
		*pChar = '\0';
	}

	return returnChar;
}



// 
// Function:	getInt
// Description:	Gets an integer from the keyboard
// Parameters:	int *incomingInt: the integer to store the entered value to
// Returns:		int: ERROR(-1) if a valid integer was not entered
//				0: On success
//
int getInt(int *incomingInt) {
	char buffer[STRING_LENGTH] = { 0 };
	int input = 0;

	// Get a string from the keyboard to the buffer
	fgets(buffer, STRING_LENGTH, stdin);

	// Check the string for a float
	if (sscanf(buffer, "%d", &input) != 1) {
		// If a float is not found return an error code
		return ERROR;
	}

	// Commit the input float to the incoming float
	*incomingInt = input;

	return 0;
}



// 
// Function:	showList
// Description:	Prints out the morse code list
// Parameters:	list <listNode> &morseCodeList: The list to print out
// Returns:		NONE
//
void showList(list <listNode> &morseCodeList) {
	// Create an iterator for the list
	list <listNode> ::iterator listIterator;

	// Loop throught the list and print out the elements
	for (listIterator = morseCodeList.begin(); listIterator != morseCodeList.end(); listIterator++) {
		printf("%c,%s\n", (*listIterator).character, (*listIterator).morseCode);
	}

	printf("\n\n");
}



// 
// Function:	createBlankNode
// Description:	returns a pointer to a blank node
// Parameters:	NONE
// Returns:		treeNode*: A pointer to the newly allocated node
//
treeNode* createBlankNode(void) {
	treeNode *tmp = NULL;

	// Allocate a new node
	tmp = (treeNode*)malloc(sizeof(treeNode));

	// Set the new nodes character to integer value 0
	tmp->character = 0;

	// Set the new nodes pointers to NULL
	tmp->left = NULL;
	tmp->right = NULL;

	return tmp;
}



// 
// Function:	generateTree
// Description:	Generates the binary tree from the linked list
// Parameters:	list <listNode> &morseCodeList: The list to derive the binary tree from
//				treeNode *&startingNode: a pointer to the start of the binary tree
// Returns:		int: ERROR(-1) if there was an error allocating memory for a node
//				0: On success
//
int generateTree(list <listNode> &morseCodeList, treeNode *&startingNode) {
	char *pChar = NULL;
	treeNode *tmp = NULL;

	// Create an iterator to traverse the list
	list <listNode> ::iterator listIterator;

	// Loop through each index of the linked list
	for (listIterator = morseCodeList.begin(); listIterator != morseCodeList.end(); listIterator++) {
		char morseCode[STRING_LENGTH] = { 0 };

		// Set the tmp node to the beginning of the tree
		tmp = startingNode;

		// start by copying the morse code string 
		strcpy(morseCode, (*listIterator).morseCode);

		// Set a pointer to the morse code string
		pChar = morseCode;

		// Loop through the morse code string
		while (*pChar != '\0') {
			// If the character is a dot, move to the left
			if (*pChar == '.') {
				// If the left node is null, create a new node
				if (tmp->left == NULL) {
					tmp->left = createBlankNode();
					// Error check the allocation of the new node
					if (tmp->left == NULL) {
						return ERROR;
					}
				}

				// Move the tmp node one space to the left
				tmp = tmp->left;
			}
			// IF the character is a dash, move to the right
			else if (*pChar == '-') {
				// If the right node is null, create a new node
				if (tmp->right == NULL) {
					tmp->right = createBlankNode();
					// Error check the allocation of the new node
					if (tmp->right == NULL) {
						return ERROR;
					}
				}

				// Move the tmp node one space to the right
				tmp = tmp->right;
			}

			// Increment the character pointer
			pChar++;
		}

		// Store the character from the list node into the binary tree node
		tmp->character = (*listIterator).character;
	}

	return 0;
}



// 
// Function:	getMorseString
// Description:	Gets a string of morse code from the keyboard
// Parameters:	char *input: The string to store the entered string in
// Returns:		int: ERROR(-1) on invalid input
//				0: On success
//
int getMorseString(char *input) {
	char buffer[STRING_LENGTH] = { 0 };
	char *newlinePointer = NULL;

	// Prompt for user input
	printf("Enter a string of morse code: ");

	// Get a string from the keyboard
	fgets(buffer, STRING_LENGTH, stdin);

	// Remove newline character
	newlinePointer = strchr(buffer, '\n');
	if (newlinePointer != NULL) {
		*newlinePointer = 0;
	}

	// Loop through string to validate input
	for (int i = 0; i < (int)strlen(buffer); i++) {
		if ((buffer[i] != '-') && (buffer[i] != '.')) {
			printf("Invalid input.\n\n");
			return ERROR;
		}
	}

	// Copy the buffer to the incoming string
	strcpy(input, buffer);

	return 0;
}



// 
// Function:	printPath
// Description:	Prints out the path to a certain node in the tree, based off
//				the input string
// Parameters:	treeNode *&start: The starting node in the binary tree
//				char pathString[STRING_LENGTH]: The string to navigate the tree with
// Returns:		NONE
//
void printPath(treeNode *&start, char pathString[STRING_LENGTH]) {
	treeNode *tmp = NULL;
	char *pChar = NULL;
	// Set the tmp pointer to the start of the binary tree
	tmp = start;
	// Set the character pointer to the beginning of the string
	pChar = pathString;

	// Loop until the character pointer is at the end of the string
	while (*pChar != '\0') {
		// If the character we are looking at is a dot
		if (*pChar == '.') {
			// If the left pointer is not null, move to the left
			if (tmp->left != NULL) {
				tmp = tmp->left;
			}
			// Otherwise, we are at the end of the tree, so print a message and break out
			else {
				printf("End of tree");
				break;
			}
				
		}
		// If the character we are looking at is a dash
		else if (*pChar == '-') {
			// If the right pointer is not null, move to the right
			if (tmp->right != NULL) {
				tmp = tmp->right;
			}
			// Otherwise, we are at the end of the tree, so print a message and break out
			else {
				printf("End of tree");
				break;
			}
		}

		// If the node is null, print "NULL"
		if (tmp->character == 0) {
			printf("NULL");
		}
		// Otherwise print out the character
		else {
			printf("%c", tmp->character);
		}

		// Avoid a comma on the last character
		if (*(pChar + 1) != '\0') {
			printf(",");
		}

		// Increment the character pointer
		pChar++;
	}

	printf("\n\n");	
}



// 
// Function:	printMessage
// Description:	Prints a message from the file message.dat
// Parameters:	treeNode *&start: the start of the binary tree
// Returns:		int: ERROR(-1): on file related errores
//				INVALID_FILE(-2): if the file used has invalid characters
//				0: On success
//
int printMessage(treeNode *&start) {
	FILE *ifp = NULL;
	char buffer[STRING_LENGTH] = { 0 };
	char *newlinePointer = NULL;
	treeNode *tmp = NULL;

	// Open the file
	ifp = fopen(MESSAGE_DAT, "r");
	// Error check the file opening was successful
	if (ifp == NULL) {
		printf("\nError opening %s.", MESSAGE_DAT);
		return ERROR;
	}

	// Get the line from the file
	fgets(buffer, STRING_LENGTH, ifp);

	// Remove newline character
	newlinePointer = strchr(buffer, '\n');
	if (newlinePointer != NULL) {
		*newlinePointer = 0;
	}

	// Validate the line from the file
	if (validateLine(buffer) == ERROR) {
		printf("Invalid input file.\n\n");
		return INVALID_FILE;
	}

	// Set the tmp node to the beginning of the tree
	tmp = start;

	for (int i = 0; i < (int)strlen(buffer); i++) {
		// If the current character is a slash
		if (buffer[i] == '/') {
			// Print a space and continue
			printf(" ");
			continue;
		}

		// If the current character is a space
		if (buffer[i] == ' ') {
			continue;
		}

		// If the current character is a dot
		if (buffer[i] == '.') {
			// Move to the left if the node is not NULL
			if (tmp->left != NULL) {
				tmp = tmp->left;
			}
			// If the node is null, it means it was not a valid character
			else {
				return INVALID_FILE;
			}
		}

		// If the current character is a dash
		if (buffer[i] == '-') {
			// Move to the right if the node is not NULL
			if (tmp->right != NULL) {
				tmp = tmp->right;
			}
			// If the node is null, it means it was not a valid character
			else {
				return INVALID_FILE;
			}
		}

		// If the next character is a space, or the end of the string
		if (buffer[i + 1] == ' ' || buffer[i + 1] == 0) {
			// print the character pointed at by the tmp
			printf("%c", tmp->character);
			// reset tmp pointer to start of the tree
			tmp = start;
		}
	}

	// Close the file and check for error
	if (fclose(ifp) != 0) {
		printf("\nError closing %s.", CODE_DAT);
		return ERROR;
	}

	printf("\n\n");
	
	return 0;
}



// 
// Function:	validateLine
// Description:	Validates that the line from the file only includes ' ', '/', '.', or '-'
// Parameters:	char *input: the line to validate
// Returns:		0 on success, ERROR(-1) on an invalid line
//
int validateLine(char *input) {
	// Loop through the string
	for (int i = 0; i < (int)strlen(input); i++) {
		// Check if each character is part of the valid character set
		if (input[i] != ' ' && input[i] != '-' && input[i] != '.' && input[i] != '/') {
			return ERROR;
		}
	}

	return 0;
}



// 
// Function:	deleteTree
// Description:	Deletes the entire binary tree, by passing in the head node
// Parameters:	treeNode *&node: the node to start deleting from
// Returns:		NONE
//
void deleteTree(treeNode *&node) {
	// If the current node's left pointer is not null
	if (node->left != NULL) {
		// call delete on the current node's left node
		deleteTree(node->left);
	}
	// If the current node's right pointer is not null
	if (node->right != NULL) {
		// call delete on the current node's right node
		deleteTree(node->right);
	}

	// Free the memory allocated to the node
	free(node);
}



// 
// Function:	randomChar
// Description:	Generates a random character from an STL list
// Parameters:	list <listNode> &morseCodeList: the list to generate a character from
// Returns:		char: The randomly generated character
//
char randomChar(list <listNode> &morseCodeList) {
	int randomIndex = 0;

	// Create an interator for the linked list
	list <listNode> ::iterator index;

	// Set the random index to a random number, based on the size of the list
	randomIndex = rand() % morseCodeList.size();

	// Set the iterator to the beginning of the list
	index = morseCodeList.begin();

	// Iterate throught the linked list
	for (int i = 0; i < randomIndex; i++) {
		index++;
	}

	// Return the character at the random index
	return (*index).character;
}



//
// Function:	randomNumberGame
// Description: A game for the user to guess the morse code path of a random character through the tree
// Parameters:	treeNode *&start: The start of the binary tree
//				char randomChar: The char for the user to guess the path for
// Returns:		NONE
//
void randomNumberGame(treeNode *&start, char randomChar) {
	treeNode *tmp = NULL;
	bool exit = false;
	char userInput = 0;

	// Tell the user their random character
	printf("Your character to guess is: %c\n", randomChar);

	// Set a temporary pointer to the start of the list
	tmp = start;

	// Take the time before the guessing starts
	auto startTime = chrono::steady_clock::now();

	// Loop until being told to exit
	while (exit != true) {
		// Get a character from the keyboard
		userInput = getch();

		// If the userInput was a dot, move to the left
		if (userInput == '.') {
			// If the user tries to go left, but the node is null, tell them they can't go that way
			if (tmp->left == NULL) {
				printf("You can't go that way!\n");
				continue;
			}
			// Move one node to the left
			tmp = tmp->left;
		}	
		// If the userInput was a dash, move to the right
		else if (userInput == '-') {
			// If the user tries to go right, but the node is null, tell them they can't go that way
			if (tmp->right == NULL) {
				printf("You can't go that way!\n");
				continue;
			}
			// Move one node to the right
			tmp = tmp->right;
		}	
		// Otherwise just continue
		else {
			continue;
		}

		// If the character at the pointers position is NULL
		if (tmp->character == 0) {
			printf("NULL\n");
		}
		else {
			// Print the character at that location
			printf("%c\n", tmp->character);
		}

		// If the character is a match, the user wins the game
		if (randomChar == tmp->character) {
			printf("It's a match! You win!\n");
			break;
		}

		// If the tmp pointer has nowhere else to go, the user loses the game
		if (tmp->right == NULL && tmp->left == NULL) {
			printf("No more moves! You lose!\n");
			break;
		}
	}
	// Take the time from after the guessing ends
	auto endTime = chrono::steady_clock::now();

	// Find the difference in time from start to end
	std::chrono::duration<double> elapsed = endTime - startTime;

	// Print the time taken to finish the game
	printf("Time taken: %d ms\n\n", (int)(elapsed.count() * THOUSAND));
}