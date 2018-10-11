//
// Created by Oleksandr Iegorov on 10/11/18.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Do overlapping strcpy safely, by using memmove. */
#define ol_strcpy(dst,src) memmove(dst,src,strlen(src)+1)

void strlwr(char *s);
char *emailtousername(char *userdomaindelimiters, char *emailaddress);
char *trim_whitespace( char* addr );
char *trim_smtp_address( char* addr );
char *trim_header_address( char *addr );
void usage(void);
int HdrEnvFromAuthMatch (char *HdrFromAddr, char *EnvFromAddr, char *AuthUsrAddr, char *AllowedAddrList);
//
void strlwr(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] + 32;
        }
        i++;
    }
}

char *emailtousername(char *userdomaindelimiters, char *emailaddress) {
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
char *trim_whitespace( char* addr ) {
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
char *trim_smtp_address( char* addr ) {
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
char *trim_header_address( char *addr ) {
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

/* HdrEnvFromAuthMatch returns following codes :
 2 - EnvFromAddr is in AllowedAddrList
 1 - EnvFromAddr and AuthUsrAddr match
 0 - none of clauses worked. Which should never happen.
-1 - EnvFromAddr and HdrFromAddr matched, but EnvFromAddr did not match either AllowedAddrList or AuthUsrAddr
-2 - EnvFromAddr and HdrFromAddr do not match
 */
int HdrEnvFromAuthMatch (char *HdrFromAddr, char *EnvFromAddr, char *AuthUsrAddr, char *AllowedAddrList) {
    //Thinking about moving this variable to a define
    char *replacetokens = "@%+\0";
    char *EnvFromAddrTrim=NULL, *EnvFromAddrDotted = NULL;
    char *HdrFromAddrTrim=NULL, *HdrFromAddrDotted = NULL;
    char *AuthUsrAddrDotted=NULL;
    char *AllowedAddrListLwr=NULL;
    int result = 0;

    // Process arguments to remove special symbols from them
    HdrFromAddrTrim=strdup(trim_header_address(HdrFromAddr)); // <-
    strlwr(HdrFromAddrTrim);
    EnvFromAddrTrim=strdup(trim_smtp_address(EnvFromAddr)); // <- cleaned envelope from address
    strlwr(EnvFromAddrTrim);
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
        // Lowercase it
        strlwr(AuthUsrAddrDotted);
        emailtousername(replacetokens, AuthUsrAddrDotted);

        // Convert email address to dotted notation
        EnvFromAddrDotted=strdup(EnvFromAddrTrim);
        emailtousername(replacetokens, EnvFromAddrDotted); // <- this is cleaned envelope from in dotted format

        // Lowercase entire allowed addresses list
        AllowedAddrListLwr=strndup(AllowedAddrList,strlen(AllowedAddrList));
        strlwr(AllowedAddrListLwr);
        // Comparing envelope sender address against conditions
        if (
            // Envelope sender and auth user are the same
                (strncmp(EnvFromAddrDotted, AuthUsrAddrDotted,strlen(EnvFromAddrDotted)) == 0) &&
                // and their sizes are the same
                (strlen(EnvFromAddrDotted) == strlen(AuthUsrAddrDotted))
                ){
            //printf("Match 1 occurred : %s = %s\nWe are fine\n", EnvFromAddrDotted, AuthUsrAddrDotted);
            result = 1;
        } else if (strstr(AllowedAddrListLwr, EnvFromAddrTrim)) {
            result = 2;
            //printf("Match 2 occurred : %s is in %s\nStill fine\n", EnvFromAddrTrim, AllowedAddrList);
        } else {
            result = -1;
            //printf("No Match occurred - We are not fine with it\n");
        }
    }
    else {
        result = -2;
        //printf("Envelope and header from do not match\n");
    }

    //Set vars free
    //free(HdrFromAddr);
    free(HdrFromAddrTrim);
    free(HdrFromAddrDotted);
    //free(EnvFromAddr);
    free(EnvFromAddrTrim);
    free(EnvFromAddrDotted);
    //free(AuthUsrAddr);
    free(AuthUsrAddrDotted);
    //free(AllowedAddrList);
    free(AllowedAddrListLwr);
    return result;
}
