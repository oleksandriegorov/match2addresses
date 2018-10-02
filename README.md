# match2addresses
Sample comparison function to match email address and its authentication username
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
==7935== Memcheck, a memory error detector
==7935== Copyright (C) 2002-2012, and GNU GPL'd, by Julian Seward et al.
==7935== Using Valgrind-3.8.1 and LibVEX; rerun with -h for copyright info
==7935== Command: src/compareaddr alex@email.com todd.email.org
==7935== 
Hello World with 3 of arguments!
See arguments:
Replacing @ in alex@email.com
Argument 1 alex@email.com -> alex.email.com
Argument 2 todd.email.org -> todd.email.org
==7935== 
==7935== HEAP SUMMARY:
==7935==     in use at exit: 30 bytes in 2 blocks
==7935==   total heap usage: 2 allocs, 0 frees, 30 bytes allocated
==7935== 
==7935== 30 bytes in 2 blocks are definitely lost in loss record 1 of 1
==7935==    at 0x4C28A2E: malloc (vg_replace_malloc.c:270)
==7935==    by 0x4EB0FA1: strndup (in /lib64/libc-2.12.so)
==7935==    by 0x400654: main (main.c:18)
==7935== 
==7935== LEAK SUMMARY:
==7935==    definitely lost: 30 bytes in 2 blocks
==7935==    indirectly lost: 0 bytes in 0 blocks
==7935==      possibly lost: 0 bytes in 0 blocks
==7935==    still reachable: 0 bytes in 0 blocks
==7935==         suppressed: 0 bytes in 0 blocks
==7935== 
==7935== For counts of detected and suppressed errors, rerun with: -v
==7935== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 8 from 6)
