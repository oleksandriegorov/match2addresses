//
// Created by Oleksandr Iegorov on 10/2/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

/* Do overlapping strcpy safely, by using memmove. */
#define ol_strcpy(dst,src) memmove(dst,src,strlen(src)+1)


static char *emailtousername(char *userdomaindelimiters, char *emailaddress) {
    int tokennumber;
    char *tokenpointer = NULL;
    for(tokennumber=0;tokennumber<strlen(userdomaindelimiters);tokennumber++) {
        if(tokenpointer=strchr(emailaddress,userdomaindelimiters[tokennumber])) {
            printf("Replacing %c in %s\n",userdomaindelimiters[tokennumber],emailaddress);
            *tokenpointer='.';
        }
    }
    return emailaddress;
}

// This function is borrowed from sfcmilter
static char *trim_whitespace( char* addr ) {
    int len;

    len = strlen( addr );
    while ( len > 0 && ( addr[0] == ' ' || addr[0] == '\t' || addr[0] == '\012' || addr[0] == '\015' ) )
	{
	++addr;
	--len;
	}
    while ( len > 0 && ( addr[len-1] == ' ' || addr[len-1] == '\t' || addr[len-1] == '\012' || addr[len-1] == '\015' ) )
	{
	--len;
	addr[len] = '\0';
	}

    return addr;
}


// This function is borrowed from sfcmilter
static char *trim_smtp_address( char* addr ) {
    int len;

    /* The milter API sometimes gives us strings with angle brackets. */
    while ( *addr == '<' )
	++addr;
    len = strlen( addr );
    while ( len > 0 && addr[len - 1] == '>' )
	addr[--len] = '\0';

    /* Finally, trim any leading or trailing whitespace. */
    return trim_whitespace( addr );
}


// This function is borrowed from sfcmilter
static char *trim_header_address( char *addr ) {
    char* left;
    char* right;

    /* First, remove any parenthesized comments. */
    while ( ( right = strchr( addr, ')' ) ) != (char*) 0 )
	{
	for ( left = right; ; --left )
	    {
	    if ( left == addr || *left == '(' )
		{
		(void) ol_strcpy( left, right + 1 );
		break;
		}
	    }
	}

    /* If there are angle brackets then the real address is inside them. */
    left = strchr( addr, '<' );
    if ( left != (char*) 0 )
	{
    printf("addr before left + 1 : %s\n",addr);
    addr = left + 1;
    printf("addr before left + 1 : %s\n",addr);
	right = strchr( addr, '>' );
	if ( right != (char*) 0 )
	    *right = '\0';
	}
    /* Finally, trim any leading or trailing whitespace. */
    return trim_whitespace( addr );
}

void usage(void) {
    printf("Usage: <header from address> <envelope from address> <authenticated user address>\n"
           "\"Naadam <info@naadam.co>\"  \"bounces+5960265-daf1-alex.yegorov=gmail.com@rsmail.naadam.co\" \"authenticated_user.naadam.co\"\n");
}



int main(int argc, char *argv[]) {
    int i;
    char *replacetokens = "@%+\0";
    char *address = NULL;
    char *authuser,*envelopefromaddr,*headerfromaddr = NULL;
    if (argc == 4) {
        printf("Hello World with %d of arguments!\n", argc);
        printf("See arguments:\n");
        // Start from 2-nd argument, thus i is set to 1
        for (i = 1; i < argc; i++) {
            address = strndup(argv[i], strlen(argv[i]));
            printf("Argument %d %s -> %s\n", i, argv[i], emailtousername(replacetokens, trim_header_address(address)));
            free(address);
        }
    }
    else {
        usage();
    }
}
