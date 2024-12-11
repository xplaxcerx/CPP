#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>
#include <locale.h>

typedef struct Club
{
    char *name;
    char *address;
    char *theme;
    int openingHour;
    int openingMinute;
    int closingHour;
    int closingMinute;
    int workingTime;
} data;

void clearInputBuffer(void);
int validateTime(int hour, int minute);
int hasKaraoke(const char *theme);
int addClub(data *element, int data_size);
void printClubData(data element);
void printAllData(data data[], int data_size);
void changeClub(data *element);
int removeClub(data *element, int data_size);
void save(data data[], int size);
int loadData(data data[]);

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int validateTime(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
}

int hasKaraoke(const char *theme) {
    char lower_theme[100];
    strcpy(lower_theme, theme);
    for(int i = 0; lower_theme[i]; i++) {
        lower_theme[i] = tolower(lower_theme[i]);
    }
    return strstr(lower_theme, "karaoke") != NULL;
}

int addClub(data *element, int data_size)
{
    clearInputBuffer();
    char *name = malloc(100);
    char *address = malloc(100);
    char *theme = malloc(100);
    int workingTime = 0;

    int openingHour;
    int openingMinute;
    int closingHour;
    int closingMinute;

    printf("Name: ");
    fgets(name, 100, stdin);
    name[strlen(name) - 1] = '\0';

    printf("Address: ");
    fgets(address, 100, stdin);
    address[strlen(address) - 1] = '\0';

    printf("Theme: ");
    fgets(theme, 100, stdin);
    theme[strlen(theme) - 1] = '\0';

    do {
        printf("Opening time (hours minutes): ");
        if (scanf("%d %d", &openingHour, &openingMinute) != 2) {
            printf("Error: Enter two numbers\n");
            clearInputBuffer();
            continue;
        }
        if (!validateTime(openingHour, openingMinute)) {
            printf("Error: Invalid time. Enter hours (0-23) and minutes (0-59)\n");
            clearInputBuffer();
        }
    } while (!validateTime(openingHour, openingMinute));

    do {
        printf("Closing time (hours minutes): ");
        if (scanf("%d %d", &closingHour, &closingMinute) != 2) {
            printf("Error: Enter two numbers\n");
            clearInputBuffer();
            continue;
        }
        if (!validateTime(closingHour, closingMinute)) {
            printf("Error: Invalid time. Enter hours (0-23) and minutes (0-59)\n");
            clearInputBuffer();
        }
    } while (!validateTime(closingHour, closingMinute));

    int tmpTime = closingHour * 60 + closingMinute - openingHour * 60 - openingMinute;
    if (tmpTime < 0) {
        tmpTime += 24 * 60;
    }

    (*element).name = name;
    (*element).address = address;
    (*element).theme = theme;
    (*element).openingHour = openingHour;
    (*element).openingMinute = openingMinute;
    (*element).closingHour = closingHour;
    (*element).closingMinute = closingMinute;
    (*element).workingTime = tmpTime;
    data_size++;
    return data_size;
}

void printClubData(data element)
{
    printf("| %-20s| %-20s| %-20s| %02d:%02d-%02d:%02d (%d min) ",
           element.name, element.address, element.theme, 
           element.openingHour, element.openingMinute,
           element.closingHour, element.closingMinute,
           element.workingTime);
}

void printAllData(data data[], int data_size)
{
    printf("ID | Name                | Address             | Theme               | Working Hours\n");
    printf("---+--------------------+--------------------+--------------------+------------------\n");
    for (int i = 0; i < data_size; i++)
    {
        printf("%2d ", i);
        printClubData(data[i]);
        printf("\n");
    }
}

void changeClub(data *element)
{
    free(element->name);
    free(element->address);
    free(element->theme);
    element->workingTime = 0;
    addClub(element, 0);
}

int removeClub(data *element, int data_size)
{
    free(element->name);
    free(element->address);
    free(element->theme);
    element->workingTime = 0;
    return data_size - 1;
}

void save(data data[], int size)
{
    FILE *file = fopen("clubs.txt", "w");
    if (file == NULL) {
        printf("Error: Could not open file for saving\n");
        return;
    }
    
    // Write the number of clubs first
    fprintf(file, "%d\n", size);
    
    // Write each club's data
    for (int i = 0; i < size; i++) {
        fprintf(file, "%s\n%s\n%s\n%d %d\n%d %d\n",
                data[i].name, data[i].address, data[i].theme,
                data[i].openingHour, data[i].openingMinute,
                data[i].closingHour, data[i].closingMinute);
    }
    fclose(file);
    printf("Data successfully saved to clubs.txt\n");
}

int loadData(data data[])
{
    FILE *file = fopen("clubs.txt", "r");
    if (file == NULL) {
        // If file doesn't exist, create it
        file = fopen("clubs.txt", "w");
        if (file == NULL) {
            printf("Error: Could not create clubs.txt\n");
            return 0;
        }
        fprintf(file, "0\n"); // Initialize with 0 clubs
        fclose(file);
        printf("Created new clubs.txt file\n");
        return 0;
    }
    
    int size;
    if (fscanf(file, "%d\n", &size) != 1) {
        printf("Error: Invalid file format\n");
        fclose(file);
        return 0;
    }

    for (int i = 0; i < size; i++) {
        char *name = malloc(100);
        char *address = malloc(100);
        char *theme = malloc(100);

        // Read club data
        if (fgets(name, 100, file) == NULL || 
            fgets(address, 100, file) == NULL || 
            fgets(theme, 100, file) == NULL) {
            printf("Error: Could not read club data\n");
            free(name);
            free(address);
            free(theme);
            fclose(file);
            return i;
        }

        // Remove newline characters
        name[strcspn(name, "\n")] = 0;
        address[strcspn(address, "\n")] = 0;
        theme[strcspn(theme, "\n")] = 0;

        int openingHour, openingMinute, closingHour, closingMinute;
        if (fscanf(file, "%d %d\n%d %d\n", 
                   &openingHour, &openingMinute,
                   &closingHour, &closingMinute) != 4) {
            printf("Error: Could not read time data\n");
            free(name);
            free(address);
            free(theme);
            fclose(file);
            return i;
        }

        // Calculate working time
        int tmpTime = closingHour * 60 + closingMinute - openingHour * 60 - openingMinute;
        if (tmpTime < 0) {
            tmpTime += 24 * 60;
        }

        // Store the data
        data[i].name = name;
        data[i].address = address;
        data[i].theme = theme;
        data[i].openingHour = openingHour;
        data[i].openingMinute = openingMinute;
        data[i].closingHour = closingHour;
        data[i].closingMinute = closingMinute;
        data[i].workingTime = tmpTime;
    }

    fclose(file);
    printf("Successfully loaded %d clubs from clubs.txt\n", size);
    return size;
}

int main()
{
    data data[100];
    int data_size = loadData(data);
    int changed;
    int removed;

    while (1)
    {
        printf("\nClub Management System:\n");
        printf("[1] - Add new club\n");
        printf("[2] - Show all clubs\n");
        printf("[3] - Find club with karaoke and longest working hours\n");
        printf("[4] - Edit club data\n");
        printf("[5] - Delete club\n");
        printf("[q] - Exit\n>");
        
        char key = _getch();
        printf("\n");
        
        switch (key)
        {
        case '1':
            if (data_size >= 100) {
                printf("Error: Maximum number of clubs reached\n");
                break;
            }
            data_size = addClub(&data[data_size], data_size);
            save(data, data_size);  // Save after adding
            break;
        case '2':
            if (data_size == 0) {
                printf("Club list is empty\n");
                break;
            }
            printAllData(data, data_size);
            break;
        case '3': {
            int maxWorkingTime = -1;
            int foundIndex = -1;
            
            for (int i = 0; i < data_size; i++) {
                if (hasKaraoke(data[i].theme) && data[i].workingTime > maxWorkingTime) {
                    maxWorkingTime = data[i].workingTime;
                    foundIndex = i;
                }
            }
            
            if (foundIndex != -1) {
                printf("Found club with karaoke and longest working hours:\n");
                printf("ID | Name                | Address             | Theme               | Working Hours\n");
                printf("---+--------------------+--------------------+--------------------+------------------\n");
                printf("%2d ", foundIndex);
                printClubData(data[foundIndex]);
                printf("\n");
            } else {
                printf("No clubs with karaoke found\n");
            }
            break;
        }
        case '4':
            if (data_size == 0) {
                printf("Club list is empty\n");
                break;
            }
            printAllData(data, data_size);
            printf("Enter club number to edit: ");
            scanf("%d", &changed);
            if (changed < data_size && changed >= 0) {
                changeClub(&data[changed]);
                save(data, data_size);  // Save after editing
            } else {
                printf("Error: Invalid club number\n");
            }
            break;
        case '5':
            if (data_size == 0) {
                printf("Club list is empty\n");
                break;
            }
            printAllData(data, data_size);
            printf("Enter club number to delete: ");
            scanf("%d", &removed);
            if (removed < data_size && removed >= 0) {
                data_size = removeClub(&data[removed], data_size);
                for (int i = removed; i < data_size; i++)
                    data[i] = data[i + 1];
                printf("Club successfully deleted\n");
                save(data, data_size);  // Save after deleting
            } else {
                printf("Error: Invalid club number\n");
            }
            break;
        case 'q':
            save(data, data_size);
            printf("Data saved. Goodbye!\n");
            return 0;
        case '\n':
            break;
        default:
            printf("Unknown command\n");
            break;
        }
    }
}