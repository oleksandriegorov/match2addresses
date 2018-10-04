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

struct senderemailaddrs {
    char *authuser;
    char *envelopeuser;
    char *headeruser;
    char *allowedusers;
};

int main(int argc, char *argv[]) {
    int i;
    char *replacetokens = "@%+\0";
    char *address = NULL;
    struct senderemailaddrs originaladdrs,processedaddrs;
//    char *authuser,*envelopefromaddr,*headerfromaddr,*overridelist = NULL;
//    char *processed_authuser,*processed_envelopefromaddr,*headerfromaddr,*overridelist = NULL;

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

    originaladdrs.headeruser=strndup(argv[1],strlen(argv[1]));
    originaladdrs.envelopeuser=strndup(argv[2],strlen(argv[2]));
    originaladdrs.authuser=strndup(argv[3],strlen(argv[3]));
    originaladdrs.allowedusers=strndup(argv[4],strlen(argv[4]));

    // Process arguments to remove special symbols from them
    processedaddrs.headeruser=trim_header_address(originaladdrs.headeruser);
    processedaddrs.envelopeuser=strdup(trim_smtp_address(originaladdrs.envelopeuser));
    processedaddrs.authuser=strndup(originaladdrs.authuser,strlen(originaladdrs.authuser));
    processedaddrs.allowedusers=strndup(originaladdrs.allowedusers,strlen(originaladdrs.allowedusers));

    // Convert email addresses to dotted notation
    emailtousername(replacetokens, processedaddrs.headeruser);
    emailtousername(replacetokens, processedaddrs.envelopeuser);

    printf("%s %s\n",originaladdrs.envelopeuser,originaladdrs.allowedusers);
    if (strcmp(processedaddrs.envelopeuser,processedaddrs.authuser) == 0) {
        printf("Match 1 occurred : %s = %s\nWe are fine\n",processedaddrs.envelopeuser, processedaddrs.authuser);
    }
    else if (strstr(originaladdrs.allowedusers,originaladdrs.envelopeuser)) {
        printf("Match 2 occurred : %s is in %s\nStill fine\n",originaladdrs.envelopeuser,originaladdrs.allowedusers);
    }
    else {
        printf("No Match occurred - We are not fine with it\n");
    }

    free(originaladdrs.headeruser);
    free(originaladdrs.envelopeuser);
    free(originaladdrs.authuser);
    free(originaladdrs.allowedusers);
    free(processedaddrs.authuser);
    free(processedaddrs.envelopeuser);
    free(processedaddrs.allowedusers);

}
