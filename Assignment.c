#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h> // For password input with asterisks

#define MAX_CHASSIS 20
#define MAX_MAKE 50
#define MAX_MODEL 50
#define MAX_OWNER 100
#define MAX_EMAIL 100
#define MAX_PHONE 20
#define MAX_PASSWORD 7
#define MAX_USERNAME 50
#define LOGIN_FILE "login.txt"
#define FLEET_FILE "fleet.txt"
#define REPORT_FILE "report.txt"

// Enum for machine types
typedef enum {
    TRACTOR,
    EXCAVATOR,
    ROLLER,
    CRANE,
    MIXER
} MachineType;

// Enum for breakdown frequency
typedef enum {
    NEVER,
    LESS_THAN_THREE,
    LESS_THAN_FIVE,
    MORE_THAN_FIVE
} BreakdownFrequency;

// Structure for login credentials
typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} Login;

// Structure for machinery details
typedef struct Machinery {
    char chassisNumber[MAX_CHASSIS];
    char make[MAX_MAKE];
    char model[MAX_MODEL];
    int year;
    double cost;
    double currentValuation;
    double currentMileage;
    double nextServiceMileage;
    char ownerName[MAX_OWNER];
    char ownerEmail[MAX_EMAIL];
    char ownerPhone[MAX_PHONE];
    MachineType machineType;
    BreakdownFrequency breakdowns;
	struct Machinery* next;//pointer to the next node
} Machinery;

// Function prototypes(acts as a table of contents)
int authenticateUser(Login logins[], int count);
void loadLogins(Login logins[], int* count);
void loadFleet(Machinery** head);
void saveFleet(Machinery* head);
void addMachine(Machinery** head);
void displayAllMachines(Machinery* head);
void displayMachineDetails(Machinery* head);
void updateMachine(Machinery* head);
void deleteMachine(Machinery** head);
void generateStatistics(Machinery* head);
void printReport(Machinery* head);
void listByValuation(Machinery* head);
void freeList(Machinery** head);
Machinery* createMachineNode();
int validateChassisNumber(Machinery* head, const char* chassisNumber);
int validateEmail(const char* email);
void displayMachine(Machinery* machine);
const char* getMachineTypeName(MachineType type);
const char* getBreakdownFrequencyName(BreakdownFrequency freq);
void sortByValuation(Machinery** head);

// Main function
int main() {
    Machinery* fleet = NULL;
    Login logins[3];
    int loginCount = 0;

    // Load login credentials
    loadLogins(logins, &loginCount);

    // Authenticate user
    if (!authenticateUser(logins, loginCount)) {
        printf("Authentication failed. Exiting program.\n");
        return 1;
    }

    // Load fleet data
    loadFleet(&fleet);

    int choice;
    do {
        printf("\nFleet Management System\n");
        printf("1. Add machine\n");
        printf("2. Display all machines\n");
        printf("3. Display machine details\n");
        printf("4. Update machine details\n");
        printf("5. Delete machine\n");
        printf("6. Generate statistics\n");
        printf("7. Print report\n");
        printf("8. List by valuation\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");

        while (scanf("%d", &choice) != 1 || choice < 0 || choice > 8) {
            printf("Invalid input. Please enter a number between 0 and 8: ");
            while (getchar() != '\n'); // Clear input buffer
        }

        switch (choice)
        {
            case 1:
                addMachine(&fleet);
                break;
            case 2:
                displayAllMachines(fleet);
                break;
            case 3:
                displayMachineDetails(fleet);
                break;
            case 4:
                updateMachine(fleet);
                break;
            case 5:
                deleteMachine(&fleet);
                break;
            case 6:
                generateStatistics(fleet);
                break;
            case 7:
                printReport(fleet);
                break;
            case 8:
                listByValuation(fleet);
                break;
            case 0:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);

    // Save fleet data and free memory
    saveFleet(fleet);
    freeList(&fleet);

    return 0;
}

// Function to load login credentials from file
void loadLogins(Login logins[], int* count) {
    FILE* file = fopen(LOGIN_FILE, "r");
    if (file == NULL) {
        printf("Error opening login file. Using default credentials.\n");
        // Default credentials if file doesn't exist
        strcpy(logins[0].username, "admin");
        strcpy(logins[0].password, "admin1");
        strcpy(logins[1].username, "manager");
        strcpy(logins[1].password, "mgr123");
        strcpy(logins[2].username, "user");
        strcpy(logins[2].password, "user12");
        *count = 3;
        return;
    }

    *count = 0;
    while (fscanf(file, "%s %s", logins[*count].username, logins[*count].password) == 2 && *count < 3) {
        (*count)++;
    }

    fclose(file);
}

//checks if username and password are correct
int authenticateUser(Login logins[], int count) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 3; //gives users only 3 tries

    while (attempts > 0) {
        printf("\nLogin\n");
        printf("Username: ");
        scanf("%s", username);

        printf("Password: ");
		// Hide password input
        int i = 0;
        char ch;
        while ((ch = _getch()) != '\r' && i < MAX_PASSWORD - 1) 
        {
			if (ch == '\b') //hndle backspace
            {
                if (i > 0) 
                {
                    printf("\b \b");
                    i--;
                }
            }
            else {
                password[i++] = ch;
                printf("*");
            }
        }
        password[i] = '\0';
        printf("\n");

		// Check if username and password match
        for (int i = 0; i < count; i++) {
            if (strcmp(username, logins[i].username) == 0 && strcmp(password, logins[i].password) == 0) {
                printf("Authentication successful!\n");
                return 1;
            }
        }

        attempts--;
        if (attempts > 0) {
            printf("Invalid username or password. %d attempts remaining.\n", attempts);
        }
    }

    printf("Too many failed attempts. Access denied.\n");
    return 0;
}

// Function to load fleet data from file
void loadFleet(Machinery** head) {
    FILE* file = fopen(FLEET_FILE, "r");
    if (file == NULL) {
        printf("No existing fleet data found. Starting with empty fleet.\n");
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), file)) 
    {
        //creates new machine and fills its data from a file
        Machinery* newMachine = createMachineNode();

        // Parse the line
        sscanf(line, "%[^,],%[^,],%[^,],%d,%lf,%lf,%lf,%lf,%[^,],%[^,],%[^,],%d,%d",
            newMachine->chassisNumber,
            newMachine->make,
            newMachine->model,
            &newMachine->year,
            &newMachine->cost,
            &newMachine->currentValuation,
            &newMachine->currentMileage,
            &newMachine->nextServiceMileage,
            newMachine->ownerName,
            newMachine->ownerEmail,
            newMachine->ownerPhone,
            (int*)&newMachine->machineType,
            (int*)&newMachine->breakdowns);

		//Add to list in sorted order by chassis number
        if (*head == NULL || strcmp(newMachine->chassisNumber, (*head)->chassisNumber) < 0) {
            newMachine->next = *head;
            *head = newMachine;
        }
        else {
            Machinery* current = *head;
            //Finds correct position in list
            while (current->next != NULL && strcmp(newMachine->chassisNumber, current->next->chassisNumber) > 0) {
                current = current->next;
            }
            newMachine->next = current->next;
            current->next = newMachine;
        }
    }

    fclose(file);
    printf("Fleet data loaded successfully.\n");
}

//Saves data to file
void saveFleet(Machinery* head) {
    FILE* file = fopen(FLEET_FILE, "w");
    if (file == NULL) {
        printf("Error opening fleet file for writing.\n");
        return;
    }

    Machinery* current = head;
    while (current != NULL) {
        fprintf(file, "%s,%s,%s,%d,%.2lf,%.2lf,%.2lf,%.2lf,%s,%s,%s,%d,%d\n",
            current->chassisNumber,
            current->make,
            current->model,
            current->year,
            current->cost,
            current->currentValuation,
            current->currentMileage,
            current->nextServiceMileage,
            current->ownerName,
            current->ownerEmail,
            current->ownerPhone,
            current->machineType,
            current->breakdowns);
        current = current->next;
    }

    fclose(file);
    printf("Fleet data saved successfully.\n");
}

//Create a new machine node
Machinery* createMachineNode() {
    Machinery* newMachine = (Machinery*)malloc(sizeof(Machinery));
    if (newMachine == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
	newMachine->next = NULL;// initialize with no next pointer
    return newMachine;
}

//Checks if chassis number is used before
int validateChassisNumber(Machinery* head, const char* chassisNumber) {
    Machinery* current = head;
    while (current != NULL) {
        if (strcmp(current->chassisNumber, chassisNumber) == 0) {
            return 0; //Already used
        }
        current = current->next;
    }
    return 1; //Not used
}

//Sees if @ nad .com are in the email address
int validateEmail(const char* email) {
    int atFound = 0;
    int dotFound = 0;
    int comFound = 0;

    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            atFound = 1;
        }
        if (atFound && email[i] == '.') {
            dotFound = 1;
        }
    }

    // Check for .com at the end
    int len = strlen(email);
    if (len >= 4 && strcmp(email + len - 4, ".com") == 0) {
        comFound = 1;
    }

    return atFound && dotFound && comFound;
}

// Function to add a new machine to the fleet
void addMachine(Machinery** head) {
    Machinery* newMachine = createMachineNode();

    printf("\nAdd New Machine\n");

	//Gets chassis number
    do {
        printf("Enter chassis number: ");
        scanf("%s", newMachine->chassisNumber);
        if (!validateChassisNumber(*head, newMachine->chassisNumber)) {
            printf("Chassis number must be unique. Please try again.\n");
        }
    } while (!validateChassisNumber(*head, newMachine->chassisNumber));

    printf("Enter make: ");
    scanf(" %[^\n]", newMachine->make);

    printf("Enter model: ");
    scanf(" %[^\n]", newMachine->model);

    printf("Enter year of manufacture: ");
    while (scanf("%d", &newMachine->year) != 1 || newMachine->year < 1900 || newMachine->year > 2025) {
        printf("Invalid year. Please enter a valid year (1900-2025): ");
        while (getchar() != '\n'); // Clear input buffer
    }

    printf("Enter cost: ");
    while (scanf("%lf", &newMachine->cost) != 1 || newMachine->cost <= 0) {
        printf("Invalid cost. Please enter a positive number: ");
        while (getchar() != '\n');
    }

    printf("Enter current valuation: ");
    while (scanf("%lf", &newMachine->currentValuation) != 1 || newMachine->currentValuation < 0) {
        printf("Invalid valuation. Please enter a non-negative number: ");
        while (getchar() != '\n');
    }

    printf("Enter current mileage: ");
    while (scanf("%lf", &newMachine->currentMileage) != 1 || newMachine->currentMileage < 0) {
        printf("Invalid mileage. Please enter a non-negative number: ");
        while (getchar() != '\n');
    }

    printf("Enter next service mileage: ");
    while (scanf("%lf", &newMachine->nextServiceMileage) != 1 || newMachine->nextServiceMileage < newMachine->currentMileage) {
        printf("Invalid service mileage. Must be >= current mileage (%.2lf): ", newMachine->currentMileage);
        while (getchar() != '\n');
    }

    printf("Enter owner name: ");
    scanf(" %[^\n]", newMachine->ownerName);

    // Email validation
    do {
        printf("Enter owner email (must contain @ and .com): ");
        scanf("%s", newMachine->ownerEmail);
        if (!validateEmail(newMachine->ownerEmail)) {
            printf("Invalid email format. Please try again.\n");
        }
    } while (!validateEmail(newMachine->ownerEmail));

    printf("Enter owner phone number: ");
    scanf("%s", newMachine->ownerPhone);

    // Machine type
    printf("Select machine type:\n");
    printf("0. Tractor\n");
    printf("1. Excavator\n");
    printf("2. Roller\n");
    printf("3. Crane\n");
    printf("4. Mixer\n");
    printf("Enter choice (0-4): ");
    int typeChoice;
    while (scanf("%d", &typeChoice) != 1 || typeChoice < 0 || typeChoice > 4) {
        printf("Invalid choice. Please enter a number between 0 and 4: ");
        while (getchar() != '\n');
    }
    newMachine->machineType = (MachineType)typeChoice;

    // Breakdown frequency
    printf("Select breakdown frequency this year:\n");
    printf("0. Never\n");
    printf("1. Less than three times\n");
    printf("2. Less than five times\n");
    printf("3. More than five times\n");
    printf("Enter choice (0-3): ");
    int breakdownChoice;
    while (scanf("%d", &breakdownChoice) != 1 || breakdownChoice < 0 || breakdownChoice > 3) {
        printf("Invalid choice. Please enter a number between 0 and 3: ");
        while (getchar() != '\n');
    }
    newMachine->breakdowns = (BreakdownFrequency)breakdownChoice;

    // Insert into linked list in sorted order by chassis number
    if (*head == NULL || strcmp(newMachine->chassisNumber, (*head)->chassisNumber) < 0) {
        newMachine->next = *head;
        *head = newMachine;
    }
    else {
        Machinery* current = *head;
        while (current->next != NULL && strcmp(newMachine->chassisNumber, current->next->chassisNumber) > 0) {
            current = current->next;
        }
        newMachine->next = current->next;
        current->next = newMachine;
    }

    printf("Machine added successfully.\n");
}

// Function to display all machines
void displayAllMachines(Machinery* head) {
    if (head == NULL) {
        printf("No machines in the fleet.\n");
        return;
    }

    printf("\nFleet Machinery List:\n");
    printf("====================================================================================================================================\n");
    printf("| %-15s | %-10s | %-10s | %-4s | %-10s | %-10s | %-8s | %-8s | %-15s | %-6s | %-10s |\n",
        "Chassis", "Make", "Model", "Year", "Cost", "Valuation", "Mileage", "Next Srv", "Owner", "Type", "Breakdowns");
    printf("====================================================================================================================================\n");

    Machinery* current = head;
    while (current != NULL) {
        printf("| %-15s | %-10s | %-10s | %-4d | %-10.2lf | %-10.2lf | %-8.2lf | %-8.2lf | %-15s | %-6s | %-10s |\n",
            current->chassisNumber,
            current->make,
            current->model,
            current->year,
            current->cost,
            current->currentValuation,
            current->currentMileage,
            current->nextServiceMileage,
            current->ownerName,
            getMachineTypeName(current->machineType),
            getBreakdownFrequencyName(current->breakdowns));
        current = current->next;
    }
    printf("====================================================================================================================================\n");
}

// Function to display a single machine's details
void displayMachine(Machinery* machine) {
    if (machine == NULL) {
        printf("Machine not found.\n");
        return;
    }

    printf("\nMachine Details:\n");
    printf("----------------\n");
    printf("Chassis Number: %s\n", machine->chassisNumber);
    printf("Make: %s\n", machine->make);
    printf("Model: %s\n", machine->model);
    printf("Year of Manufacture: %d\n", machine->year);
    printf("Cost: %.2lf\n", machine->cost);
    printf("Current Valuation: %.2lf\n", machine->currentValuation);
    printf("Current Mileage: %.2lf\n", machine->currentMileage);
    printf("Next Service Mileage: %.2lf\n", machine->nextServiceMileage);
    printf("Owner Name: %s\n", machine->ownerName);
    printf("Owner Email: %s\n", machine->ownerEmail);
    printf("Owner Phone: %s\n", machine->ownerPhone);
    printf("Machine Type: %s\n", getMachineTypeName(machine->machineType));
    printf("Breakdowns this year: %s\n", getBreakdownFrequencyName(machine->breakdowns));
}

// Function to display details of a specific machine
void displayMachineDetails(Machinery* head) {
    if (head == NULL) {
        printf("No machines in the fleet.\n");
        return;
    }

    char chassisNumber[MAX_CHASSIS];
    printf("Enter chassis number of the machine to display: ");
    scanf("%s", chassisNumber);

    Machinery* current = head;
    while (current != NULL) {
        if (strcmp(current->chassisNumber, chassisNumber) == 0) {
            displayMachine(current);
            return;
        }
        current = current->next;
    }

    printf("Machine with chassis number %s not found.\n", chassisNumber);
}

// Function to update machine details
void updateMachine(Machinery* head) {
    if (head == NULL) {
        printf("No machines in the fleet to update.\n");
        return;
    }

    char chassisNumber[MAX_CHASSIS];
    printf("Enter chassis number of the machine to update: ");
    scanf("%s", chassisNumber);

    Machinery* current = head;
    while (current != NULL) {
        if (strcmp(current->chassisNumber, chassisNumber) == 0) {
            printf("\nUpdating machine %s\n", current->chassisNumber);

            // Display current details
            displayMachine(current);

            // Menu for what to update
            int choice;
            do {
                printf("\nWhat would you like to update?\n");
                printf("1. Make\n");
                printf("2. Model\n");
                printf("3. Year of Manufacture\n");
                printf("4. Cost\n");
                printf("5. Current Valuation\n");
                printf("6. Current Mileage\n");
                printf("7. Next Service Mileage\n");
                printf("8. Owner Name\n");
                printf("9. Owner Email\n");
                printf("10. Owner Phone\n");
                printf("11. Machine Type\n");
                printf("12. Breakdown Frequency\n");
                printf("0. Done Updating\n");
                printf("Enter your choice: ");

                while (scanf("%d", &choice) != 1 || choice < 0 || choice > 12) {
                    printf("Invalid input. Please enter a number between 0 and 12: ");
                    while (getchar() != '\n');
                }

                switch (choice) {
                case 1:
                    printf("Enter new make: ");
                    scanf(" %[^\n]", current->make);
                    break;
                case 2:
                    printf("Enter new model: ");
                    scanf(" %[^\n]", current->model);
                    break;
                case 3:
                    printf("Enter new year of manufacture: ");
                    while (scanf("%d", &current->year) != 1 || current->year < 1900 || current->year > 2025) {
                        printf("Invalid year. Please enter a valid year (1900-2025): ");
                        while (getchar() != '\n');
                    }
                    break;
                case 4:
                    printf("Enter new cost: ");
                    while (scanf("%lf", &current->cost) != 1 || current->cost <= 0) {
                        printf("Invalid cost. Please enter a positive number: ");
                        while (getchar() != '\n');
                    }
                    break;
                case 5:
                    printf("Enter new current valuation: ");
                    while (scanf("%lf", &current->currentValuation) != 1 || current->currentValuation < 0) {
                        printf("Invalid valuation. Please enter a non-negative number: ");
                        while (getchar() != '\n');
                    }
                    break;
                case 6:
                    printf("Enter new current mileage: ");
                    while (scanf("%lf", &current->currentMileage) != 1 || current->currentMileage < 0) {
                        printf("Invalid mileage. Please enter a non-negative number: ");
                        while (getchar() != '\n');
                    }
                    break;
                case 7:
                    printf("Enter new next service mileage: ");
                    while (scanf("%lf", &current->nextServiceMileage) != 1 || current->nextServiceMileage < current->currentMileage) {
                        printf("Invalid service mileage. Must be >= current mileage (%.2lf): ", current->currentMileage);
                        while (getchar() != '\n');
                    }
                    break;
                case 8:
                    printf("Enter new owner name: ");
                    scanf(" %[^\n]", current->ownerName);
                    break;
                case 9:
                    do {
                        printf("Enter new owner email (must contain @ and .com): ");
                        scanf("%s", current->ownerEmail);
                        if (!validateEmail(current->ownerEmail)) {
                            printf("Invalid email format. Please try again.\n");
                        }
                    } while (!validateEmail(current->ownerEmail));
                    break;
                case 10:
                    printf("Enter new owner phone number: ");
                    scanf("%s", current->ownerPhone);
                    break;
                case 11:
                    printf("Select new machine type:\n");
                    printf("0. Tractor\n");
                    printf("1. Excavator\n");
                    printf("2. Roller\n");
                    printf("3. Crane\n");
                    printf("4. Mixer\n");
                    printf("Enter choice (0-4): ");
                    int typeChoice;
                    while (scanf("%d", &typeChoice) != 1 || typeChoice < 0 || typeChoice > 4) {
                        printf("Invalid choice. Please enter a number between 0 and 4: ");
                        while (getchar() != '\n');
                    }
                    current->machineType = (MachineType)typeChoice;
                    break;
                case 12:
                    printf("Select new breakdown frequency this year:\n");
                    printf("0. Never\n");
                    printf("1. Less than three times\n");
                    printf("2. Less than five times\n");
                    printf("3. More than five times\n");
                    printf("Enter choice (0-3): ");
                    int breakdownChoice;
                    while (scanf("%d", &breakdownChoice) != 1 || breakdownChoice < 0 || breakdownChoice > 3) {
                        printf("Invalid choice. Please enter a number between 0 and 3: ");
                        while (getchar() != '\n');
                    }
                    current->breakdowns = (BreakdownFrequency)breakdownChoice;
                    break;
                case 0:
                    printf("Updates completed.\n");
                    break;
                }
            } while (choice != 0);

            printf("Machine details updated successfully.\n");
            return;
        }
        current = current->next;
    }

    printf("Machine with chassis number %s not found.\n", chassisNumber);
}

// Function to delete a machine from the fleet
void deleteMachine(Machinery** head) {
    if (*head == NULL) {
        printf("No machines in the fleet to delete.\n");
        return;
    }

    char chassisNumber[MAX_CHASSIS];
    printf("Enter chassis number of the machine to delete: ");
    scanf("%s", chassisNumber);

    Machinery* current = *head;
    Machinery* prev = NULL;

    // Check if the machine to delete is the head
    if (current != NULL && strcmp(current->chassisNumber, chassisNumber) == 0) {
        *head = current->next;
        free(current);
        printf("Machine deleted successfully.\n");
        return;
    }

    // Search for the machine to delete
    while (current != NULL && strcmp(current->chassisNumber, chassisNumber) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Machine with chassis number %s not found.\n", chassisNumber);
        return;
    }

    // Unlink the node from the linked list
    prev->next = current->next;
    free(current);
    printf("Machine deleted successfully.\n");
}

// Function to generate statistics
void generateStatistics(Machinery* head) {
    if (head == NULL) {
        printf("No machines in the fleet to generate statistics.\n");
        return;
    }

    int totalMachines = 0;
    int totalByType[5] = { 0 }; // One for each machine type
    int neverByType[5] = { 0 };

    Machinery* current = head;
    while (current != NULL) {
        totalMachines++;
        totalByType[current->machineType]++;
        if (current->breakdowns == NEVER) {
            neverByType[current->machineType]++;
        }
        current = current->next;
    }

    printf("\nBreakdown Statistics by Machine Type:\n");
    printf("====================================\n");
    printf("%-15s %-15s %-15s %s\n", "Machine Type", "Total", "No Breakdowns", "% No Breakdowns");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < 5; i++) {
        if (totalByType[i] > 0) {
            double percentage = (double)neverByType[i] / totalByType[i] * 100;
            printf("%-15s %-15d %-15d %.2lf%%\n",
                getMachineTypeName(i),
                totalByType[i],
                neverByType[i],
                percentage);
        }
    }

    printf("\nOverall Statistics:\n");
    printf("==================\n");
    int totalNever = 0;
    for (int i = 0; i < 5; i++) {
        totalNever += neverByType[i];
    }

    double overallPercentage = (double)totalNever / totalMachines * 100;
    printf("Total Machines: %d\n", totalMachines);
    printf("Machines with no breakdowns: %d (%.2lf%%)\n", totalNever, overallPercentage);
}

// Function to print report to file
void printReport(Machinery* head) {
    if (head == NULL) {
        printf("No machines in the fleet to generate report.\n");
        return;
    }

    FILE* file = fopen(REPORT_FILE, "w");
    if (file == NULL) {
        printf("Error opening report file for writing.\n");
        return;
    }

    // Write header
    fprintf(file, "Machinery Management Ltd. - Fleet Report\n");
    fprintf(file, "=======================================\n\n");

    // Write all machine details
    fprintf(file, "Machine Details:\n");
    fprintf(file, "----------------\n");

    Machinery* current = head;
    while (current != NULL) {
        fprintf(file, "Chassis Number: %s\n", current->chassisNumber);
        fprintf(file, "Make: %s\n", current->make);
        fprintf(file, "Model: %s\n", current->model);
        fprintf(file, "Year: %d\n", current->year);
        fprintf(file, "Cost: %.2lf\n", current->cost);
        fprintf(file, "Current Valuation: %.2lf\n", current->currentValuation);
        fprintf(file, "Current Mileage: %.2lf\n", current->currentMileage);
        fprintf(file, "Next Service Mileage: %.2lf\n", current->nextServiceMileage);
        fprintf(file, "Owner: %s\n", current->ownerName);
        fprintf(file, "Email: %s\n", current->ownerEmail);
        fprintf(file, "Phone: %s\n", current->ownerPhone);
        fprintf(file, "Type: %s\n", getMachineTypeName(current->machineType));
        fprintf(file, "Breakdowns: %s\n", getBreakdownFrequencyName(current->breakdowns));
        fprintf(file, "----------------------------------------\n");
        current = current->next;
    }

    // Generate and write statistics
    fprintf(file, "\nPerformance Statistics:\n");
    fprintf(file, "======================\n");

    int totalMachines = 0;
    int totalByType[5] = { 0 };
    int neverByType[5] = { 0 };

    current = head;
    while (current != NULL) {
        totalMachines++;
        totalByType[current->machineType]++;
        if (current->breakdowns == NEVER) {
            neverByType[current->machineType]++;
        }
        current = current->next;
    }

    fprintf(file, "Breakdown Statistics by Machine Type:\n");
    fprintf(file, "------------------------------------\n");
    fprintf(file, "%-15s %-15s %-15s %s\n", "Machine Type", "Total", "No Breakdowns", "% No Breakdowns");
    fprintf(file, "------------------------------------------------------------\n");

    for (int i = 0; i < 5; i++) {
        if (totalByType[i] > 0) {
            double percentage = (double)neverByType[i] / totalByType[i] * 100;
            fprintf(file, "%-15s %-15d %-15d %.2lf%%\n",
                getMachineTypeName(i),
                totalByType[i],
                neverByType[i],
                percentage);
        }
    }

    int totalNever = 0;
    for (int i = 0; i < 5; i++) {
        totalNever += neverByType[i];
    }

    double overallPercentage = (double)totalNever / totalMachines * 100;
    fprintf(file, "\nOverall Statistics:\n");
    fprintf(file, "------------------\n");
    fprintf(file, "Total Machines: %d\n", totalMachines);
    fprintf(file, "Machines with no breakdowns: %d (%.2lf%%)\n", totalNever, overallPercentage);

    fclose(file);
    printf("Report generated successfully in %s.\n", REPORT_FILE);
}

// Function to list machines by valuation
void listByValuation(Machinery* head) {
    if (head == NULL) {
        printf("No machines in the fleet.\n");
        return;
    }

    // Create a copy of the list to sort
    Machinery* sortedList = NULL;
    Machinery* current = head;

    while (current != NULL) {
        Machinery* newNode = createMachineNode();
        *newNode = *current; // Copy the data
        newNode->next = NULL;

        // Insert into the new list in sorted order by valuation
        if (sortedList == NULL || newNode->currentValuation >= sortedList->currentValuation) {
            newNode->next = sortedList;
            sortedList = newNode;
        }
        else {
            Machinery* temp = sortedList;
            while (temp->next != NULL && newNode->currentValuation < temp->next->currentValuation) {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }

        current = current->next;
    }

    // Display the sorted list
    printf("\nMachines Ordered by Current Valuation (Highest to Lowest):\n");
    printf("============================================================================================================\n");
    printf("| %-15s | %-10s | %-10s | %-10s | %-15s | %-15s |\n",
        "Chassis", "Make", "Model", "Valuation", "Owner", "Type");
    printf("============================================================================================================\n");

    current = sortedList;
    while (current != NULL) {
        printf("| %-15s | %-10s | %-10s | %-10.2lf | %-15s | %-15s |\n",
            current->chassisNumber,
            current->make,
            current->model,
            current->currentValuation,
            current->ownerName,
            getMachineTypeName(current->machineType));
        current = current->next;
    }
    printf("============================================================================================================\n");

    // Free the temporary sorted list
    freeList(&sortedList);
}

// Helper function to get machine type name
const char* getMachineTypeName(MachineType type) {
    switch (type) {
    case TRACTOR: return "Tractor";
    case EXCAVATOR: return "Excavator";
    case ROLLER: return "Roller";
    case CRANE: return "Crane";
    case MIXER: return "Mixer";
    default: return "Unknown";
    }
}

// Helper function to get breakdown frequency name
const char* getBreakdownFrequencyName(BreakdownFrequency freq) {
    switch (freq) {
    case NEVER: return "Never";
    case LESS_THAN_THREE: return "<3 times";
    case LESS_THAN_FIVE: return "<5 times";
    case MORE_THAN_FIVE: return ">5 times";
    default: return "Unknown";
    }
}

// Function to free the linked list
void freeList(Machinery** head) {
    Machinery* current = *head;
    Machinery* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}