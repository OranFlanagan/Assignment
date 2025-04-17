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
void printPassword(char* pass)
{
    int i = 0;
    char ch;
    while (i < password_Legth)
    {
        ch = _getch();
        if (ch == '\r') break;
        pass[i++] = ch;
        printf("*");
    }
    pass[i] = '\0';
}

void login()
{
    LoginT logins[3] = {
        {"admin1", "pass01"},
        {"admin2", "pass02"},
        {"admin3", "pass03"}
    };

    char enteredUser[20];
    char enteredPass[password_Legth + 1];
    int authenticated = 0;

    printf("Enter username: ");
    scanf("%s", enteredUser);
    printf("Enter password: ");
    printPassword(enteredPass);

    for (int i = 0; i < 3; i++)
    {
        if (strcmp(enteredUser, logins[i].userName) == 0 &&
            strcmp(enteredPass, logins[i].passWord) == 0)
        {
            authenticated = 1;
            break;
        }
    }

    if (!authenticated)
    {
        printf("\nAccess denied.\n");
        exit(0);
    }
    else
    {
        printf("\nLogin successful!\n");
    }
}
