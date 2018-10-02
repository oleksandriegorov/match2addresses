//
// Created by Oleksandr Iegorov on 10/2/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int i;
    int tokennumber;
    char *tokenpointer = NULL;
    char *replacetokens = "@%+\0";
    char *address = NULL;
    printf("Hello World with %d of arguments!\n",argc);
    printf("See arguments:\n");
    // Start from 2-nd argument, thus i is set to 1
    for(i=1;i<argc;i++) {
        address = strndup(argv[i],strlen(argv[i]));
	//Replace any symbols from replacetokens to '.'
        for(tokennumber=0;tokennumber<strlen(replacetokens);tokennumber++) {
            if(tokenpointer=strchr(address,replacetokens[tokennumber])) {
                printf("Replacing %c in %s\n",replacetokens[tokennumber],address);
                *tokenpointer='.';
            }
        }
        printf("Argument %d %s -> %s\n",i,argv[i],address);
	free(address);
    }
}
