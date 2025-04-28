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
        printf("Welcome %s!\n", enteredUser);
    }
}

int main()
{
    login();
    return 0;
}

void clearBuffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) 
    {
        ;
    }
}

void displayMenu() {
    printf("\nFleet Management Menu:\n");
    printf("1) Add machine\n");
    printf("2) Display all machines\n");
    printf("3) Display machine details\n");
    printf("4) Update machine\n");
    printf("5) Delete machine\n");
    printf("6) Generate statistics\n");
    printf("7) Print report\n");
    printf("8) Sort by valuation\n");
    printf("0) Exit\n");
}

void menuLoop() {
    int choice;
    do {
        displayMenu();
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
        case 1: addMachine(); break;
        case 2: displayAllMachines(); break;
        case 3: displayMachineByChassis(); break;
        case 4: updateMachine(); break;
        case 5: deleteMachine(); break;
        case 6: generateStatistics(); break;
        case 7: printReport(); break;
        case 8: sortByValuation(); break;
        case 0: printf("Exiting...\n"); break;
        default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

int main() {
    login();
    menuLoop();
    return 0;
}
