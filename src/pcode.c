#include "pcode.h"
#include "error.h"
#include <string.h>
#include <stdio.h>

int stack[STACK_SIZE] = { -1 };

int base = 1;
int stackTop = 0;
int programCounter = 0;
struct instruction instructionRegister;
char * instructionString[] = { "LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC", "WRT" };
char * operationString[] = { "RTN", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD", "MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ" };

void run(int studyMode){
	stack[1] = stack[2] = stack[3] = 0;
	
	while ( base > 0 ){
		if ( studyMode == 1 ){
			printStack();
		}
		executeInstruction();
	}
	if ( studyMode == 1 ){
		printStack();
	}else{
		printResult();
	}
}

void executeInstruction(){
	instructionRegister = instructions[programCounter];
	programCounter++;
	switch ( instructionRegister.operation ){
		case LIT:
			stackTop++;
			stack[stackTop] = instructionRegister.argument;
			break;
		case OPR:
			stackOperation();
			break;
		case LOD:
			stackTop++;
			stack[ stackTop ] = stack[ getBase(instructionRegister.level) + instructionRegister.argument ];
			break;
		case STO:
			stack[ getBase(instructionRegister.level) + instructionRegister.argument ] = stack[stackTop];
			stackTop--;
			break;
		case CAL:
			stack[stackTop+1] = getBase( instructionRegister.level );
			stack[stackTop+2] = base;
			stack[stackTop+3] = programCounter;
			base = stackTop+1;
			programCounter = instructionRegister.argument;
			break;
		case INT:
			stackTop += instructionRegister.argument;
			break;
		case JMP:
			programCounter = instructionRegister.argument-1;
			break;
		case JPC:
			if ( stack[stackTop] == 0 ){
				programCounter = instructionRegister.argument-1;
			}
			stackTop--;	
			break;
		default:
			notImplemented( instructionRegister.operation,ERROR_INSTRUCTION_NOT_IMPLEMENTED, programCounter);
	}
}

void stackOperation(){
	switch(instructionRegister.argument){
		case RTN:
			stackTop = base - 1;
			programCounter = stack[stackTop+3];
			base = stack[stackTop+2];
			break;
		case ADD:
			stackTop--;
			stack[stackTop] += stack[stackTop+1];
			break;
		case SUB:
			stackTop--;
			stack[stackTop] -= stack[stackTop+1];
			break;
		case MUL:
			stackTop--;
			stack[stackTop] *= stack[stackTop+1];
			break;
		case NEQ:
			stackTop--;
			stack[stackTop] = ( stack[stackTop] != stack[stackTop+1] );
			break;
		case GTR:
			stackTop--;
			stack[stackTop] = ( stack[stackTop] > stack[stackTop+1] );
			break;
		default:
			notImplemented(instructionRegister.argument, ERROR_STACK_OPERATION_NOT_IMPLEMENTED, programCounter);
			break;
	}
}

int getBase(int level){
	int newBase;
	newBase = base;
	while ( level>0 ){
		newBase = stack[newBase];
		level--;
	}
	return newBase;
}

int getInstructionCode(char *str){
	int i = 0;
	while ( i<INSTRUCTION_COUNT ){
		if ( strcmp(str,instructionString[i])==0 ){
			return i;
		}
		i++;
	}
	error(ERROR_UNKNOWN_OPERATION);
	return -1;
}

char * getInstructionName(int code){
	if ( code >= 0 && code <INSTRUCTION_COUNT ){
		return instructionString[code];
	}
	return "UNKNOWN INSTRUCTION";
}

char * getOperationName(int code){
	if ( code >= 0 && code <OPERATION_COUNT ){
		return operationString[code];
	}
	return "UNKNOWN OPERATION";
}

void printStack(){
	int i=1;
	//Add spacing, to clear the prompt
	for(i=0; i<20; i++){
		printf("\n");
	}
	
	//Prints the stack
	for(i=1; i<=stackTop; i++){
		printf("%d: %d", i, stack[i]);
		if ( i==base ){
			printf(" BASE");
		}
		if ( i==stackTop ){
			printf(" TOP");
		}
		printf("\n");
	}
	//Prints the next instruction
	if ( base == 0 ){
		printf("Stack end: %d\n", stack[1]);
	}else{
		printf("Next instruction: %s %d %d\n", getInstructionName(instructions[programCounter].operation),
							instructions[programCounter].level,
							instructions[programCounter].argument);
		
		char temp;
		printf("(press enter to continue)\n");
		scanf("%c", &temp);
	}
	
}

void printResult(){
	printf("The pcode-machine returned: %d\n", stack[1]);
}