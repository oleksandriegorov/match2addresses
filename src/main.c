//
// Created by Oleksandr Iegorov on 10/2/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    printf("Match result is %d\n",HdrEnvFromAuthMatch(argv[1],argv[2],argv[3],argv[4]));
    return 0;

}
