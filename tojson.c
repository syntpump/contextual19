#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Size of the buffer
#define BUFFERSIZE 50
// Size of the char array where property name will be written
#define MAXPROPSIZE 15
// Size of the char array where property value will be written
#define MAXVALSIZE 30
// Size of the char array where position of selector will be written. 3 is
// enough to contain two number and terminator
#define MAXSELPOS 3


// Read n characters and check whether it's \t (if n==1) or \t\t (if n==2)
#define catchTabs(n) \
	fgets(buffer, n+1, inpfile) && \
	!strcmp(buffer, n == 1 ? "\t" : "\t\t")

#define catchBlock(name) \
	!strcmp( \
		buffer, \
		name \
		"\n" \
	)


// Write JSON-style property: \t...\t"prop": value; without quotes
#define printProperty(indent, name, value) \
	fprintf( \
		outpfile, \
		"%s\"%s\": %s,\n", \
		indent, \
		name, \
		value \
	);


// Write JSON-style property: \t...\t"prop": "value"; with quotes
#define printPropertyQuotes(indent, name, value) \
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
	/* Fills buffer with spaces. */
	for (int i = 0; i < BUFFERSIZE; i++)
		buffer[i] = ' ';
}


short readUntil (char what, FILE *fp, char *buffer) {
	/* Read file until specific symbol, put the string into buffer and return
	*  its length.*/
	char c;
	short len = 0;
	while ((c = fgetc(fp)) != EOF && c != what)
		buffer[len++] = c;

	if (feof(fp)) {
		fprintf(stderr, "Unexpected end of the file.\n");
		exit(EXIT_FAILURE);
	}

	return len;
}

void pasteAfter (char *string, const char *substring, int a, int b, int n) {
	/* Paste n characters from substring[a:] into string[b:] */
	do
		string[a++] = substring[b++];
	while (n-- > 0);
}

void getSelector (char *buffer, char *position) {
	/* Parse selectors to position and name. Example:
	*  "first next": buffer -> "next", position -> "1"
	*  "second previous": buffer -> "previous", position -> "2"
	*  "6th next": buffer -> "next", position -> "6"
	*  Write selector name to buffer, position to *position.
	* 
	*  Args:
	*      buffer: String to read.
	*      position: Char array to write position of selector.
	*
	*/
	// Position of 'next', 'previous', 'token', etc.
	short c = -1;
	if (isdigit(buffer[0])) {
		// String starts with [0-9]+th
		// Copy all until 't'
		while (buffer[c++ + 1] != 't')
			position[c] = buffer[c];

		position[c] = '\0';
		// Selector name will start after 3 symbols: 'th '
		c += 2;
	} else {
		// String starts with one of words: first, second, third, fourth,
		// fifth, end, beginning, token.
		// Third character in each of words is different, so we can recognize
		// them using that.
		switch(buffer[2]){
			case 'r':
				// 'fi -r- st'
				strcpy(position, "1\0");
				c = 5;
				break;
			case 'c':
				// 'se -c- ond'
				strcpy(position, "2\0");
				c = 6;
				break;
			case 'i':
				// 'th -i- rd'
				strcpy(position, "3\0");
				c = 5;
				break;
			case 'u':
				// 'fo -u- rth'
				strcpy(position, "4\0");
				c = 6;
				break;
			case 'f':
				// 'fi -f- th'
				strcpy(position, "5\0");
				c = 5;
				break;
			default:
				// If there's no number-word, it's considered to be 'first'
				strcpy(position, "1\0");
				break;
		}
	}
	if (c != 0) {
		// Rewrite selector into buffer end terminate the word.
		short i = -1;
		do
			buffer[i++] = buffer[c++];
		while
			(buffer[c] != ' ');
		buffer[i] = '\0';
	} else {
		// String starts from 'end', 'beginning', or 'token' word. Put
		// terminator after them.
		switch(buffer[0]){
			case 'e':
				buffer[3] = '\t';
			case 'b':
				buffer[9] = '\t';
			case 't':
				buffer[5] = '\t';
		}
	}
}

void getProperty (char *buffer, char *name, char *value) {
	/* Parse property to name and value. Example:
	*  "name is value": value -> '[true, "value"]', name -> 'name'
	*  "name is not value": value -> '[false, "value"]', name -> 'name'
	*  "name becomes value": value -> 'value', name -> 'name'
	*
	*  Args:
	*	buffer: String to read.
	*   name: Char array to write property name in.
	*
	*/
	// Position counter.
	short c = -1;
	// I've avoid using strncpy here in order to clarify the code.
	while (buffer[c++ + 1] != ' ')
		name[c] = buffer[c];
	name[c] = '\0';
	if (buffer[c+1] == 'b') {
		// Next word is 'becomes'.
		c += 8;
		// Write value and terminate it.
		short i = -1;
		do
			value[i++] = buffer[c++];
		while
			(buffer[c] != ' ');
		buffer[i] = '\0';
	} else {
		// Next is 'is' or 'is not'. It should be written in the following
		// format:
		// [true, value]	if "name is value"
		// [false, value]	if "name is not value"
		unsigned char isTrue;

		if (buffer[c+6] == 't' && buffer[c+7] == ' '){
			// After 'is no-t -'
			// isTrue becomes false
			isTrue = !(c += 8);
		else
			// isTrue becomes true
			isTrue = !!(c += 4);

		strcpy(value, isTrue ? "[true, \"" : "[false, \"");
		short i = isTrue ? 8 : 9;
		do
			value[i++] = buffer[c++];
		while
			(buffer[c] != ' ');
		pasteAfter(value, "\"]\0", i, 0, 4);
	}
}


int main (int argc, char** argv) {

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

			if (!strcmp(argv[i], "--output"))
				outpfile = fopen(argv[i+1], "w");
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

		// Catch 'if' block.
		fgets(buffer, 4, inpfile);
		if (catchBlock("if")) {
			printDict("\t\t", "if");

			// Catch selector.
			while(catchTabs(1)){

				clear(buffer);
				// Enough to contain two numbers and '\0'
				char selectorPosition[MAXSELPOS];
				// Name of selector will be written into buffer.
				readUntil('\n', inpfile, buffer);
				getSelector(
					buffer,
					selectorPosition
				);
				printDict("\t\t\t", buffer);
				printProperty("\t\t\t\t", "position", selectorPosition);
				clear(buffer);

				// Catch property
				while(catchTabs(2)){
					clear(buffer);
					readUntil('\n', inpfile, buffer);
					char propertyName[MAXPROPSIZE];
					char propertyValue[MAXVALSIZE];
					getProperty(
						buffer,
						propertyName,
						propertyValue
					);
					printProperty("\t\t\t\t", propertyName, propertyValue);
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
			if (catchBlock("then")){
				printDict("\t\t", "then");
				clear(buffer);

				// Caught property
				while(catchTabs(1)){
					clear(buffer);
					readUntil('\n', inpfile, buffer);
					char propertyName[MAXPROPSIZE];
					char propertyValue[MAXVALSIZE];
					getProperty(
						buffer,
						propertyName,
						propertyValue
					);
					printPropertyQuotes("\t\t\t", propertyName, buffer);
					clear(buffer);
				}

				// End of 'then' block.
				closeDict("\t\t");
			}


			// End of rule block
			closeDict("\t");
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
