#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "files.h"

void readUntil(char what, FILE *fp, char *buffer){
    /*Read file until given character and write line to buffer.*/
	char c;
	int len = 0;
	while((c = fgetc(fp)) != EOF && c != what)
		buffer[len++] = c;

	if(feof(fp)){
		fprintf(stderr, "Unexpected end of the file.\n");
		exit(EXIT_FAILURE);
	}

	return len;
}

int seekUntil(const char* what, FILE *fp){
    /*Read file until given substring and returns position of cursor.*/
    char buffer[COMPARING_BUFFERSIZE];

    while(fgets(buffer, COMPARING_BUFFERSIZE, fp))
        if(strstr(buffer, what) != NULL)
                return ftell(fp);

    return NULL;
}
