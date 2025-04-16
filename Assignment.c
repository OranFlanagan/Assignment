#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define password_Legth 6
#define max_Machines 100
#define max_Users 100

typedef enum {tractor = 1, Excavator, Roller, Crane, Mixer} MachineType;
typedef enum { never = 0, LessThanThree, LessThanFive, MoreThanFive }BreakdownLevel;

typedef struct
{
	char chassisNumber[20];
	char model[20];
	char male[20];
	char ownerName[30];
	char email[50];
	char phone[15];
	int year;
	int milage;
	float cost;
	float value;
	MachineType type;
	BreakdownLevel breakdowns;

	struct machinery* next;
} MachineryT;

typedef struct
{
	char userName[20];
	char passWord[password_Legth + 1];
} LoginT;

MachineryT head = NULL;

void login();
void displayMenu();
void addMachine();
void displayAllMachines();
void displayMachineByChassis();
void clearBuffer();
int validateEmail(char* email);
void printPassword(char* pass);

void main()
{
	login();
	displayMenu();
}
