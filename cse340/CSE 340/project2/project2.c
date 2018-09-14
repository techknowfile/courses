#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// LinkedList struct
struct node {
	int line;
	token_type type;
	char *token_value;

	struct node *next;
};

// Forward Declarations
struct node* printInReverse(struct node*);
//struct node* cleanup(struct node*);

// String conversation for token_type ENUMs
char *reserved[] =
    {   "",
        "IF",
        "WHILE",
        "DO",
        "THEN",
        "PRINT",
        "+",
        "-",
        "/",
        "*",
        "=",
        ":",
        ",",
        ";",
        "[",
        "]",
        "(",
        ")",
        "<>",
        ">",
        "<",
        "<=",
        ">=",
        ".",
        "NUM",
        "ID",
        "ERROR"
    };
int main(){
	// Declare and instantiate pointer to head (of linked list) and the current node
	struct node *head = NULL;
	struct node *curr = NULL;
	
	// Loop until end of input
	while(1){

		getToken();
		if (t_type == END_OF_FILE){
			// EOF reached
			break;
		}
		else if (t_type == NUM || (t_type == ID && (strcmp(current_token, "cse340") == 0|| strcmp(current_token, "programming") == 0|| strcmp(current_token, "language") == 0))){
			
			// Declare node and allocate memory
			struct node *n = (struct node *)malloc(sizeof(struct node));

			// Set node values
			n->next = 0;
			n->line = line;
			n->type = t_type;
			// Allocate memory for token_value string
			n->token_value = (char*) malloc(MAX_TOKEN_LENGTH*sizeof(char));
			// Copy current_token into node->token_value
			strcpy(n->token_value, current_token);

			if (head == NULL){
				// This is the first node. Point head and curr to node.
				head = n;
				curr = head;
			}
			curr->next = n;
			curr = n;
		}
	}
	if (head == NULL){
		// Link list is empty. Exit program.
		return 0;
	}
	else{
		// Set current node to head and print list out in reverse order
		curr = head;
		printInReverse(curr); // Initiate recursive loop to print linked list in reverse
		
		//  Cleanup
		while (curr != NULL){
		struct node* temp = curr;
		curr = curr->next;
		free(temp);
		}
		head = NULL;
	}
	
	return 0;
}

/*** Print LinkedList in reverse order ***/
struct node* printInReverse(struct node* p){
	if (p == NULL){
		return;
	}
	printInReverse(p->next); // Recursively call function to reach end of linked list
	printf("%d %s %s\n", p->line, reserved[p->type], p->token_value); // Print data
}