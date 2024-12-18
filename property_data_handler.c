#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ROWS 1000
#define MAX_COLS 10
#define MAX_LINE_LENGTH 500
#define MAX_FIELD_LENGTH 100

// Global data structure
char csvData[MAX_ROWS][MAX_COLS][MAX_FIELD_LENGTH];
char columnNames[MAX_COLS][MAX_FIELD_LENGTH];
int totalRows = 0;
int totalCols = 0;

// Function to clean whitespace
void trim(char *str)
{
    // Remove leading whitespace
    int start = 0;
    while (isspace((unsigned char)str[start]))
        start++;

    // Remove trailing whitespace
    int end = strlen(str) - 1;
    while (end > start && isspace((unsigned char)str[end]))
        end--;

    // Shift string
    int i;
    for (i = 0; i <= end - start; i++)
    {
        str[i] = str[start + i];
    }
    str[i] = '\0';
}

// Function to read CSV file with cleaning
int readCSVFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Failed to open file %s\n", filename);
        return 0;
    }

    char line[MAX_LINE_LENGTH];

    // Read header
    if (fgets(line, sizeof(line), file))
    {
        char *token = strtok(line, ",");
        totalCols = 0;
        while (token && totalCols < MAX_COLS)
        {
            trim(token);
            strcpy(columnNames[totalCols], token);
            strcpy(csvData[0][totalCols], token);
            totalCols++;
            token = strtok(NULL, ",");
        }
    }

    // Read data
    totalRows = 1;
    while (fgets(line, sizeof(line), file) && totalRows < MAX_ROWS)
    {
        char *token = strtok(line, ",");
        int col = 0;
        while (token && col < totalCols)
        {
            trim(token);
            strcpy(csvData[totalRows][col], token);
            col++;
            token = strtok(NULL, ",");
        }
        totalRows++;
    }

    totalRows--; // Correct row count
    fclose(file);
    return 1;
}

// Function to find column index case-insensitive
int findColumnIndex(const char *columnName)
{
    for (int i = 0; i < totalCols; i++)
    {
        if (strcasecmp(columnNames[i], columnName) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Function to display data
void displayData(int numRows)
{
    if (numRows > totalRows)
        numRows = totalRows;

    // Print header
    for (int col = 0; col < totalCols; col++)
    {
        printf("%-20s", columnNames[col]);
    }
    printf("\n");

    // Print data rows
    for (int row = 1; row <= numRows; row++)
    {
        for (int col = 0; col < totalCols; col++)
        {
            printf("%-20s", csvData[row][col]);
        }
        printf("\n");
    }
}

// Function to search rows
void searchData(const char *columnName, const char *query)
{
    int colIndex = findColumnIndex(columnName);

    if (colIndex == -1)
    {
        printf("Column not found!\n");
        return;
    }

    int foundCount = 0;

    // Print header
    for (int col = 0; col < totalCols; col++)
    {
        printf("%-20s", columnNames[col]);
    }
    printf("\n");

    // Search and print data
    for (int row = 1; row <= totalRows; row++)
    {
        if (strcasecmp(csvData[row][colIndex], query) == 0)
        {
            for (int col = 0; col < totalCols; col++)
            {
                printf("%-20s", csvData[row][col]);
            }
            printf("\n");
            foundCount++;
        }
    }

    if (foundCount == 0)
    {
        printf("Data not found!\n");
    }
}

// Function to sort data
void sortData(const char *columnName, const char *order)
{
    int colIndex = findColumnIndex(columnName);

    if (colIndex == -1)
    {
        printf("Column not found!\n");
        return;
    }

    // Bubble sort with numeric and string comparison
    for (int i = 1; i < totalRows; i++)
    {
        for (int j = 1; j < totalRows - i + 1; j++)
        {
            int compareResult = 0;

            // Check if column is numeric (Price or Rooms)
            if (colIndex == 2 || colIndex == 3) // Assuming column 2 is Price and 3 is Rooms
            {
                // Compare as integers
                int num1 = atoi(csvData[j][colIndex]);
                int num2 = atoi(csvData[j + 1][colIndex]);
                compareResult = num1 - num2;
            }
            else
            {
                // Compare as strings (default case)
                compareResult = strcmp(csvData[j][colIndex], csvData[j + 1][colIndex]);
            }

            // Determine sorting order
            if ((strcmp(order, "asc") == 0 && compareResult > 0) ||
                (strcmp(order, "desc") == 0 && compareResult < 0))
            {
                // Swap rows
                char temp[MAX_COLS][MAX_FIELD_LENGTH];
                memcpy(temp, csvData[j], sizeof(temp));
                memcpy(csvData[j], csvData[j + 1], sizeof(temp));
                memcpy(csvData[j + 1], temp, sizeof(temp));
            }
        }
    }

    // Display first 10 data after sorting
    printf("Sorted Data (first 10 rows):\n");
    for (int col = 0; col < totalCols; col++)
    {
        printf("%-20s", columnNames[col]);
    }
    printf("\n");

    int displayRows = (totalRows < 10) ? totalRows : 10;
    for (int row = 1; row <= displayRows; row++)
    {
        for (int col = 0; col < totalCols; col++)
        {
            printf("%-20s", csvData[row][col]);
        }
        printf("\n");
    }
}

// Function to export data
void exportData(const char *filename)
{
    char fullFilename[MAX_FIELD_LENGTH];
    snprintf(fullFilename, sizeof(fullFilename), "%s.csv", filename);

    FILE *file = fopen(fullFilename, "w");
    if (!file)
    {
        printf("Failed to create file %s\n", fullFilename);
        return;
    }

    // Write header
    for (int col = 0; col < totalCols; col++)
    {
        fprintf(file, "%s%s", columnNames[col], (col < totalCols - 1) ? "," : "\n");
    }

    // Write data
    for (int row = 1; row <= totalRows; row++)
    {
        for (int col = 0; col < totalCols; col++)
        {
            fprintf(file, "%s%s", csvData[row][col], (col < totalCols - 1) ? "," : "\n");
        }
    }

    fclose(file);
    printf("Data successfully written to file %s\n", fullFilename);
}

int main()
{
    const char *filename = "property_data.csv";

    if (!readCSVFile(filename))
    {
        return 1;
    }

    int choice;
    char input[MAX_FIELD_LENGTH];
    char column[MAX_FIELD_LENGTH];

    while (1)
    {
        printf("\nWhat do you want to do?\n");
        printf("1. Display Data\n");
        printf("2. Search Data\n");
        printf("3. Sort Data\n");
        printf("4. Export Data\n");
        printf("5. Exit\n");
        printf("Your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline

        switch (choice)
        {
        case 1:
            printf("How many rows to display? ");
            scanf("%d", &choice);
            getchar(); // Clear newline
            displayData(choice);
            break;
        case 2:
            printf("Select column: ");
            fgets(column, sizeof(column), stdin);
            column[strcspn(column, "\n")] = 0;

            printf("Data to search: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            searchData(column, input);
            break;
        case 3:
            printf("Select column to sort: ");
            fgets(column, sizeof(column), stdin);
            column[strcspn(column, "\n")] = 0;

            printf("Sort ascending (asc) or descending (desc)? ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            sortData(column, input);
            break;
        case 4:
            printf("Export filename: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            exportData(input);
            break;
        case 5:
            printf("Exiting program...\n");
            return 0;
        default:
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
