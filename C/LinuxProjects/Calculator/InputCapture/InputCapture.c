#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//definitions
#define INPUTBUFSIZE 1048
#define INPUTWORDBUFSIZE 256
#define TRUE 1
#define FALSE 0

//imports
#include "../MainFiles/MathChars.h"
#ifndef FILE_DEFINITIONS
#define FILE_DEFINITIONS
#include "../MainFiles/definitions.c"
#include "../Misc/Sorts/countingSort.c"
#endif


// operation functions and enums
typedef enum OperationsPrec{
	OPERERROR = -1,
	EXPONENTIAL = 0,
       	MULTIPLICATION = 1,
       	DIVISION = 1,
       	ADDITION = 2,
	SUBTRACTION = 2
} OperationsPrec;

const static struct{
	OperationsPrec oper;
	int symbol;
} conversion[] = {
	{EXPONENTIAL, '^'},
	{MULTIPLICATION, '*'  },
	{DIVISION, '/'  },
	{ADDITION   , '+'  },
	{SUBTRACTION, '-'  },
};


term* captureInput(void);

//identify function declarations
OperationsPrec operation2Enum(int symbol);
int isOperation(int symbol);
int hasPrecedence(int oper1, int oper2);

//input identifiers
int GetInputIdentifier(char c);
int isLetter(const int letter);
int isBracket(const int bracket);
int isDigit(const int digit);
int isActualEndBracket(const int openBracket, const int closeBracket);
int isEndBracket(const int bracket);
int isOpenBracket(const int bracket);
int isAssignmentOperation(const int c);

//appending to number
void appendTerm(term* expression, term addTerm,int* posi, int* tempi, int* alpai);

//creating terms
term createTerm(char* coefficient, char* alphanumeric, int coeffPos, int alphanumPos, int boolOperation, int boolbracket, int Termination);

// redo capture input
// only works with Ints no floats as of yet
term* captureInput(void){
	int bufsize = INPUTBUFSIZE;
	int pos = 0;
	term* input = (term*)malloc(sizeof(term) * bufsize);
	int c;
	char* tempString = malloc(sizeof(char) * bufsize);
	int tempPos = 0;
	char* alphaString = malloc(sizeof(char) * bufsize);
	int alphaPos = 0;
	char* BracketChecking = malloc(sizeof(char) * bufsize);
	int BracketCheckingPos = 0;

	int decimalNumber = FALSE;
	int is_negative = 0; // mod 2 : =0 then positive : 1 negative
       	int operationinputed = 0;	

	if(!input || !tempString || !BracketChecking || !alphaString){
		printf("Allocation Error\n");
		exit(EXIT_FAILURE);
	}
	
	//always encapsulate with brackets
	tempString[0] = '(';
	tempString[1] = '\0';
	input[pos++] = createTerm(tempString, alphaString, 1, alphaPos, 0, 1, 0);
	while(1){
		c = getchar();
		// printf("Tokenizing \"%c\"\n", c);
		switch(GetInputIdentifier(c)){
			case INPUT_ERROR:
				printf("Error Tokenizing char \"%c\"", c);
			break;
			case INPUT_END:
			{
				//create term adds the zeroes for us
				if(tempPos > 0 || alphaPos > 0){
					term toAdd = createTerm(tempString, alphaString, tempPos, alphaPos, FALSE, FALSE, FALSE);
					//add term
					appendTerm(input, toAdd, &pos, &tempPos, &alphaPos);
				}				

				//add termination Term
				term termination = createTerm(tempString, alphaString, tempPos, alphaPos, FALSE, FALSE, TRUE);
				appendTerm(input, termination, &pos, &tempPos, &alphaPos);

				printf("Captured Normally\n");
				return input;
			}
			break;
			case INPUT_SPACE:
				continue;
			break;
			case INPUT_CHAR:
			{
				alphaString[alphaPos++] = c;
				pos--;
			}
			break;
			case INPUT_DIGIT:
			{
				if(is_negative % 2 == 1) 
					tempString[tempPos++] = '-';
				operationinputed = 0;
				is_negative = 0;
				if(c == DECIMALPOINT) 
					decimalNumber = TRUE;
				tempString[tempPos++] = c;
				pos--;
			}
			break;
			case INPUT_OPERATION:
			{
				if(c == '-'){
					is_negative++;
					c = '+'; // then let c be + to replace the -
					if(operationinputed == 1)
						continue;
					//this - will be kept the the next number coming after the +
				}else{
					operationinputed = 1;
				}

				if(is_negative > 1) //hence we have more than one do not make more +
					continue;
				if(tempPos > 0 || alphaPos > 0){
					term toAdd = createTerm(tempString, alphaString, tempPos, alphaPos, FALSE, FALSE, FALSE);
					appendTerm(input, toAdd, &pos, &tempPos, &alphaPos);
				}
				tempString[0] = c;
				term operation = createTerm(tempString , alphaString, 1, alphaPos, TRUE, FALSE, FALSE); //this is so we can input the operation
				appendTerm(input, operation, &pos, &tempPos, &alphaPos);
				pos--; //because we add one in the append term
			}
			break;
			case INPUT_BRACKET:
			{
				
				if(tempPos > 0 || alphaPos > 0){
					term toAdd = createTerm(tempString, alphaString, tempPos, alphaPos, FALSE, FALSE, FALSE);
					appendTerm(input, toAdd, &pos, &tempPos, &alphaPos);
				}

				//input the bracket
				tempString[0] = c;
				term bracket = createTerm(tempString, alphaString, 1, alphaPos, FALSE, TRUE, FALSE); //for a bracket
				appendTerm(input, bracket, &pos, &tempPos, &alphaPos);
				
				pos--;
				//assume BracketChecking never reaches its max allocation size
				//check that bracket that was added is valid
				//check if opening bracket first
				//allow un closed open brackets
				if(isOpenBracket(c)){
					BracketChecking[BracketCheckingPos++] = c;
					break;
				}
				if(BracketCheckingPos == 0 && isEndBracket(c)){
					printf("Syntax Error No Opening Corresponding Bracket\n");
					return input;
				}
				if(!(isActualEndBracket(BracketChecking[BracketCheckingPos-1] ,c))){
					printf("Doesnt Correspond ti opening Bracket Error\n");
					return input;
				}else{ // then is is an actual end bracket
					BracketCheckingPos--;
				}
			}
			break;
			case INPUT_ASSIGNMENT:
				continue;
			break;
			default:
				continue;
		}

		
		//if our string is out of space add more memory
		if(pos >= bufsize){
			bufsize += INPUTBUFSIZE;
			input = realloc(input, bufsize);
			if(!input) {
				printf("Allocation Error\n");
				exit(EXIT_FAILURE);
			}
		}
		pos++;
	}
	free(alphaString);
	free(tempString);
	free(BracketChecking);
	return input;
}






OperationsPrec operation2Enum(int symbol){
	int j;
	for(j = 0; j< sizeof(conversion)/sizeof(conversion[0]); ++j)
		if(symbol == conversion[j].symbol)
			return conversion[j].oper;
	return OPERERROR;
}


int isOperation(int symbol){
	if(operation2Enum(symbol) >= 0)
		return TRUE;
	return FALSE;
}

int hasPrecedence(int oper1, int oper2){
	if(operation2Enum(oper1) < operation2Enum(oper2))
		return TRUE;
	return FALSE;
}

int isLetter(const int letter){
	if((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))
		return TRUE;
	return FALSE;
}

int isDigit(const int digit){
	if(digit >= '0' && digit <= '9')
		return TRUE;
	return FALSE;
}

int isBracket(const int bracket){
	for(int i = 0; i < sizeof(BRACKETS)/sizeof(char); ++i)
		if(bracket == BRACKETS[i])
			return TRUE;
	return FALSE;
}

int GetInputIdentifier(char c){
	if(c == ' ') return INPUT_SPACE;
	if(isLetter(c)) return INPUT_CHAR;
	if(c == EOF || c == '\n') return INPUT_END;
	if(isOperation(c)) return INPUT_OPERATION;
	if(isDigit(c) || c == DECIMALPOINT) return INPUT_DIGIT;
	if(isBracket(c)) return INPUT_BRACKET;
	if(isAssignmentOperation(c)) return INPUT_ASSIGNMENT;
	return INPUT_ERROR;
}

int isEndBracket(const int bracket){
	for(int i = 0; i < sizeof(ENDBRACKET)/sizeof(char); ++i)
		if(bracket == ENDBRACKET[i])
			return TRUE;
	return FALSE;
}

int isOpenBracket(const int bracket){
	for(int i = 0; i < sizeof(OPENBRACKET)/sizeof(char); ++i)
		if(bracket == OPENBRACKET[i])
			return TRUE;
	return FALSE;
}


int isActualEndBracket(const int openBracket, const int closeBracket){
	switch(closeBracket){
		case ')':
			if(openBracket == '(')
				return TRUE;
			return FALSE;
		break;
		case ']':
			if(openBracket == '[')
				return TRUE;
			return FALSE;
		break;
		case '}':
			if(openBracket == '{')
				return TRUE;
			return FALSE;
		break;
		default:
			return FALSE;
	}
}

int isAssignmentOperation(const int c){
	for(int i = 0; i < sizeof(ASSIGNMENTCHAR)/sizeof(char); i++)
		if(c == ASSIGNMENTCHAR[i])
			return TRUE;
	return FALSE;
}


void appendTerm(term* expression, term addTerm,int* posi, int* tempi, int* alpai){
	expression[(*posi)++] = addTerm;
	
	//set back to zero
	*tempi = 0;
	*alpai = 0;
}



term createTerm(char* coefficient, char* alphanumeric, int coeffPos, int alphanumPos, int boolOperation, int boolBracket, int Termination){
	//all alpnamumerics have coefficient of 1
	if(coeffPos == 0)
		coefficient[coeffPos++] = '1';
	//add null chars to coeff and alpha first
	coefficient[coeffPos] = '\0';
	alphanumeric[alphanumPos] = '\0';
	
	//sort alphanumeric with No count
	if(alphanumPos > 0){
		printf("Sup %d\n", alphanumPos);
		char* sorted = CountingSortNC(alphanumeric);
		strcpy(alphanumeric, sorted);
		//printf("--- %s here \n" , CountingSortC(alphanumeric));
		//printf("Create Term with Alpha ->  %s -> %s \n", alphanumericIn, alphanumeric);	
	}
	
	
	term toAdd;
	toAdd.boolisOperation = boolOperation;
	toAdd.isTermination = Termination;
	toAdd.boolBracket = boolBracket;
	if(coeffPos == 0){
		toAdd.boolhascoeff = 0; // no coefficient
	}else{
		toAdd.coefficient = (char*)malloc(sizeof(coefficient)+sizeof(char));
		strcpy(toAdd.coefficient, coefficient);
		toAdd.boolhascoeff = 1;
	}

	if(alphanumPos == 0){
		toAdd.boolhasalpha = 0;
	}else{
		toAdd.alphanumeric = (char*)malloc(sizeof(alphanumeric) + sizeof(char));
		strcpy(toAdd.alphanumeric, alphanumeric);
		toAdd.boolhasalpha = 1;
	}

	return toAdd;
}





