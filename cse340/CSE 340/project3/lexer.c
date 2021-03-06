#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

#define KEYWORDS  5

char current_token[MAX_TOKEN_LENGTH];
int token_length;
token_type t_type;
int line = 1;


static int active_token = 0;
static char *reserved[] =
    {   "ID",
        "HASH",
        "DOUBLEHASH"
        "ARROW",
        "ERROR"
    };


static void skip_space()
{
    char c;

    c = getchar();
    line += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line += (c == '\n');
    }
    // return character to input buffer if eof is not reached
    if (!feof(stdin))
    {
        ungetc(c,stdin);
    }
}

static token_type scan_id()
{
    char c;
    int k;
    token_type the_type;

    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c) && token_length < MAX_TOKEN_LENGTH)
        {
            current_token[token_length] = c;
            token_length++;
            c = getchar();
        }
        current_token[token_length] = '\0';
        ungetc(c, stdin);
        the_type = ID;
        return the_type;
    }
    else
    {
        return ERROR;
    }
}

token_type getToken()
{
    char c;

    if (active_token)
    {
        active_token = 0;
        return t_type;
    }
    skip_space();
    token_length = 0;
    current_token[0] = '\0';
    c = getchar();
    switch (c)
    {
        case '#': 
            c = getchar();
            if (c == '#'){
                t_type = DOUBLEHASH;
            }
            else{
                ungetc(c, stdin);
                t_type = HASH;
            }
            return t_type;
        case '-':
            c = getchar();
            if (c == '>')
            {
                t_type = ARROW;
            }
            else
            {;
                t_type = ERROR;
            }
            return t_type;
       default:
            if (isalpha(c)) // token is ID
            {
                ungetc(c, stdin);
                t_type = scan_id();
            }
            else if (c == EOF)
            {
                t_type = END_OF_FILE;
            }
            else
            {
                t_type = ERROR;
            }
            return t_type;
    }
}

void ungetToken()
{
    active_token = 1;
}

/*
 * Write your code in a SEPARATE FILE, do NOT edit this file.
 *
 *
 * If using C, include lexer.h in your code and compile and
 * link your code like this:
 *
 * $ gcc -Wall lexer.c your_file.c
 *
 *
 *
 * If using C++, include lexer.h like this in your code:
 *
 * extern "C" {
 *     #include "lexer.h"
 * }
 *
 * And compile and link your code like this:
 *
 * $ gcc -Wall -c lexer.c
 * $ g++ -Wall lexer.o your_file.cpp
 *
 * The first command just compiles the C code. The second
 * command compiles your C++ code and links it with the
 * compiled lexer.
 *
 */
