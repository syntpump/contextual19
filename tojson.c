#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFERSIZE 50


// Read n characters and check whether it's \t (if n==1) or \t\t (if n==2)
#define catchTabs(n) \
	fgets(buffer, n+1, inpfile) && \
	!strcmp(buffer, n == 1 ? "\t" : "\t\t")


// Write JSON-style property: \t...\t"prop": "value"
#define printProperty(indent, name, value) \
	fprintf( \
		outpfile, \
		"%s\"%s\": \"%s\",\n", \
		indent, \
		name, \
		value \
	);


// Write dict JSON-style property: \t...\t"prop": {
#define printDict(indent, name) \
	fprintf( \
		outpfile, \
		"%s\"%s\": {\n", \
		indent, \
		name \
	);

// Close dict block with "}"
#define closeDict(indent) \
	fprintf( \
		outpfile, \
		"%s},\n", \
		indent \
	);


void clear(char *buffer){
	for (int i = 0; i < BUFFERSIZE; i++)
		buffer[i] = ' ';
}


short readUntil (char what, FILE *fp, char *buffer){
	/*Read file until specific symbol, put the string into buffer and return
	* its length.*/
	char c;
	short len = 0;
	while ((c = fgetc(fp)) != EOF && c != what) {
		buffer[len] = c;
		len++;
	}
	if (feof(fp)){
		fprintf(stderr, "Unexpected end of the file.\n");
		exit(EXIT_FAILURE);
	}

	return len;
}


int main (int argc, char** argv){

	FILE *inpfile, *outpfile;

	if (argc == 1) {
		printf(
"This parser can convert .ctx19 files to .json, but some parameters are "
"needed.\n\n"
"Expected parameters:\n"
"Name            Default     Description\n"
"--file          *requiered  Name of the file to be converted. Type it with\n"
"                            .ctx19 extension. E.g.: \"rules.ctx19\".\n"
"--output        *requiered  Name of the file output to be written. If file\n"
"                            doesn't exists, if will be created. Otherwise,\n"
"                            it will be overwritten.\n"
		);
		exit(EXIT_SUCCESS);
	} else if (argc == 5) {
		for (int i = 1; i < argc; i+=2){
			// Grab --file and --output. Assume that parameters was passed
			// correctly.
			if (!strcmp(argv[i], "--file"))
				inpfile = fopen(argv[i+1], "r");

			if (!strcmp(argv[i], "--output")){
				outpfile = fopen(argv[i+1], "w");
			}
		}
	} else {
		fprintf(stderr, "No --file and --output specified.\n");
		exit(EXIT_FAILURE);

	}
	if(inpfile == NULL || outpfile == NULL){
		fprintf(stderr, "Error when creating file stream.\n");
		exit(EXIT_FAILURE);
	}

	// Start of array of rules.
	fprintf(outpfile, "[\n");

	while (1) {

		fprintf(outpfile, "\t{\n");

		char buffer[BUFFERSIZE] = {' '};
		short len; // Size of read symbols.

		// Catch 'if' block.
		fgets(buffer, 4, inpfile);
		if (!strcmp(buffer, "if\n")) {
			printDict("\t\t", "if");

			// Catch selector.
			while(catchTabs(1)){
				clear(buffer);
				len = readUntil('\n', inpfile, buffer);
				printDict("\t\t\t", "selector");
				clear(buffer);

				// Caught property
				while(catchTabs(2)){
					clear(buffer);
					len = readUntil('\n', inpfile, buffer);
					printProperty("\t\t\t\t", "prop", "value")
					clear(buffer);
				}
				fseek(inpfile, -2, SEEK_CUR);

				// End of selector
				closeDict("\t\t\t");

			}
			// End of 'if' block.
			closeDict("\t\t");
			fseek(inpfile, -1, SEEK_CUR);
			clear(buffer);

			// Catch 'then' block
			fgets(buffer, 12, inpfile);
			if (!strcmp(buffer, "then\n")){
				clear(buffer);
				printDict("\t\t", "then");

				// Caught property
				while(catchTabs(1)){
					clear(buffer);
					len = readUntil('\n', inpfile, buffer);
					printProperty("\t\t\t", "propthen", "value")
					clear(buffer);
				}

				// End of 'then' block.
				closeDict("\t\t");
			}


			// End of rule block
			closeDict("\t");
			//fprintf(outpfile, "\n");
		}

		if(feof(inpfile))
			break;

	}

	// End of array of rules.
	fprintf(outpfile, "]\n");

	fclose(inpfile);
	fclose(outpfile);

	return 0;
}
