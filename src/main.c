//
// Created by Oleksandr Iegorov on 10/2/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Do overlapping strcpy safely, by using memmove. */
#define ol_strcpy(dst,src) memmove(dst,src,strlen(src)+1)


static char *emailtousername(char *userdomaindelimiters, char *emailaddress) {
    int tokennumber;
    char *tokenpointer = NULL;
    for(tokennumber=0;tokennumber<strlen(userdomaindelimiters);tokennumber++) {
        if(tokenpointer=strchr(emailaddress,userdomaindelimiters[tokennumber])) {
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
// Reminder for my future self : this function returns a pointer to original string, so either before of
// a double free or just strndup the result
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
// Reminder for my future self : this function returns a pointer to original string, so either before of
// a double free or just strndup the result
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
    addr = left + 1;
	right = strchr( addr, '>' );
	if ( right != (char*) 0 )
	    *right = '\0';
	}
    /* Finally, trim any leading or trailing whitespace. */
    return trim_whitespace( addr );
}

void usage(void) {
    printf("Usage: <header from address> <envelope from address> <authenticated user address> <override list of explicitly allowed addresses>\n"
           "\"Naadam <info@naadam.co>\"  \"bounces+5960265-daf1-alex.yegorov=gmail.com@rsmail.naadam.co\" \"authenticated_user.naadam.co\" \"someaddress@gmail.com,someaddress1@gmail.com\"\n");
}

int main(int argc, char *argv[]) {
    int i;
    char *replacetokens = "@%+\0";
    char *address = NULL;
    char *EnvFromAddr=NULL, *EnvFromAddrTrim=NULL, *EnvFromAddrDotted = NULL;
    char *HdrFromAddr=NULL, *HdrFromAddrTrim=NULL, *HdrFromAddrDotted = NULL;
    char *AuthUsrAddr=NULL, *AuthUsrAddrDotted=NULL;
    char *AllowedAddrList=NULL;

    if (argc == 5) {
        printf("Hello World with %d of arguments!\n", argc);
        printf("See arguments:\n");
        // Start from 2-nd argument, thus i is set to 1
        for (i = 1; i < argc; i++) {
            address = strndup(argv[i], strlen(argv[i]));
            //printf("Argument %d %s -> %s\n", i, argv[i], emailtousername(replacetokens, trim_header_address(address)));
            free(address);
        }
    }
    else {
        usage();
        return 0;
    }

    // Get variables to work with
    HdrFromAddr=strndup(argv[1],strlen(argv[1]));
    EnvFromAddr=strndup(argv[2],strlen(argv[2]));
    AuthUsrAddr=strndup(argv[3],strlen(argv[3]));
    AllowedAddrList=strndup(argv[4],strlen(argv[4]));

    // Process arguments to remove special symbols from them
    HdrFromAddrTrim=strdup(trim_header_address(HdrFromAddr)); // <-
    EnvFromAddrTrim=strdup(trim_smtp_address(EnvFromAddr)); // <- cleaned envelope from address
    // Header and envelope senders *SHOULD* match in order to continue
    // Reason is we need to ensure that recipient is not deceived by sender address being different in
    // header and envelope
    if (
            //Header and envelope sender are the same one to one
            (strncmp(HdrFromAddrTrim,EnvFromAddrTrim,strlen(HdrFromAddrTrim)) == 0 ) &&
            // and their sizes are the same as well: to avoid user@example.com and user@example.command match
            (strlen(HdrFromAddrTrim) == strlen(EnvFromAddrTrim))
    ){
        //Makes sense to proceed further

        // Convert auth user address to dotted notation
        AuthUsrAddrDotted = strndup(AuthUsrAddr, strlen(AuthUsrAddr));
        emailtousername(replacetokens, AuthUsrAddrDotted);

        // Convert email address to dotted notation
        EnvFromAddrDotted=strdup(EnvFromAddrTrim);
        emailtousername(replacetokens, EnvFromAddrDotted); // <- this is cleaned envelope from in dotted format

        // Comparing envelope sender address against conditions
        if (
                // Envelope sender and auth user are the same
                (strncmp(EnvFromAddrDotted, AuthUsrAddrDotted,strlen(EnvFromAddrDotted)) == 0) &&
                // and their sizes are the same
                (strlen(EnvFromAddrDotted) == strlen(AuthUsrAddrDotted))
        ){
            printf("Match 1 occurred : %s = %s\nWe are fine\n", EnvFromAddrDotted, AuthUsrAddrDotted);
        } else if (strstr(AllowedAddrList, EnvFromAddrTrim)) {
            printf("Match 2 occurred : %s is in %s\nStill fine\n", EnvFromAddrTrim,
                   AllowedAddrList);
        } else {
            printf("No Match occurred - We are not fine with it\n");
        }
    }
    else {
        printf("Envelope and header from do not match\n");
    }

    //Set vars free
    free(HdrFromAddr);
    free(HdrFromAddrTrim);
    free(HdrFromAddrDotted);
    free(EnvFromAddr);
    free(EnvFromAddrTrim);
    free(EnvFromAddrDotted);
    free(AuthUsrAddr);
    free(AuthUsrAddrDotted);
    free(AllowedAddrList);

}
