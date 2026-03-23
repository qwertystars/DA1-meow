/*
 * ============================================================
 * Project Title : Smart Student Performance Analyzer
 * Student Name  : Daksh Agarwal
 * Register No   : 25BCE5098
 * Department    : BCE (B.Tech Computer Science & Engineering)
 * Course        : Structured and Object Oriented Programming
 * Faculty       : Dr. Dinakaran M
 * ============================================================
 *
 * Description:
 * This is a menu-driven C program to manage student academic
 * records. It supports adding, searching, updating, and
 * displaying student records. It also generates reports like
 * topper detection, class statistics, and grade distribution.
 * All data is saved to a binary file (students.dat) so records
 * are not lost when the program is closed.
 * ============================================================
 */

#include <stdio.h>    /* for printf, scanf, fgets, fopen, fclose, fread, fwrite */
#include <stdlib.h>   /* for general utilities */
#include <string.h>   /* for strcmp, strcpy, strlen, strcspn */
#include <ctype.h>    /* for isdigit, isalpha, toupper */

/* ---- Constants ---- */
#define MAX_STUDENTS 100    /* maximum number of students allowed */
#define NAME_LEN     60     /* max length of student name */
#define REGNO_LEN    10     /* 9 chars (XXYYYXXXX) + 1 null terminator */
#define GRADE_LEN    5      /* "A", "B", "C", "Fail" + null terminator */
#define NUM_SUBJECTS 3      /* number of subjects per student */
#define DATA_FILE    "students.dat"   /* binary file for storing records */

/*
 * Student struct:
 * Holds all information for one student.
 * - regNo  : VIT registration number (format: XXYYYXXXX)
 * - name   : full name of the student
 * - marks  : array of marks for 3 subjects (float, 0-100)
 * - total  : sum of all 3 subject marks (computed automatically)
 * - average: total divided by 3 (computed automatically)
 * - grade  : letter grade assigned based on average (A/B/C/Fail)
 */
typedef struct {
    char  regNo[REGNO_LEN];
    char  name[NAME_LEN];
    float marks[NUM_SUBJECTS];
    float total;
    float average;
    char  grade[GRADE_LEN];
} Student;

/* ---- Global Variables ---- */
Student students[MAX_STUDENTS];  /* array to store all student records in memory */
int     studentCount = 0;        /* tracks how many students are currently stored */

/* ---- Function Prototypes ---- */
int  isValidRegNo(const char *reg);
void computeGrade(Student *s);
int  findStudentIndex(const char *regNo);
void printStudentRecord(const Student *s);
void printTableHeader(void);
void printTableRow(const Student *s);
void saveToFile(void);
void loadFromFile(void);
void addStudent(void);
void searchStudent(void);
void updateMarks(void);
void displayAll(void);
void displayTopper(void);
void classStatistics(void);
void gradeDistribution(void);

/* ============================================================
 * FUNCTION: isValidRegNo
 * PURPOSE : Checks if a registration number matches the VIT
 *           format: XXYYYXXXX (2 digits + 3 letters + 4 digits)
 * INPUT   : const char *reg - the registration number string
 * OUTPUT  : returns 1 if valid, 0 if invalid
 * ============================================================ */
int isValidRegNo(const char *reg) {
    int i;

    /* Check total length must be exactly 9 characters */
    if (strlen(reg) != 9) return 0;

    /* First 2 characters must be digits (batch year, e.g., "25") */
    if (!isdigit(reg[0]) || !isdigit(reg[1])) return 0;

    /* Next 3 characters must be letters (department code, e.g., "BCE") */
    if (!isalpha(reg[2]) || !isalpha(reg[3]) || !isalpha(reg[4])) return 0;

    /* Last 4 characters must be digits (student serial, e.g., "5098") */
    for (i = 5; i < 9; i++) {
        if (!isdigit(reg[i])) return 0;
    }

    return 1;  /* all checks passed — valid format */
}

/* ============================================================
 * FUNCTION: computeGrade
 * PURPOSE : Calculates total, average, and grade for a student
 *           Grade thresholds: A>=85, B>=70, C>=50, else Fail
 * INPUT   : Student *s - pointer to the student struct
 * OUTPUT  : fills s->total, s->average, s->grade
 * ============================================================ */
void computeGrade(Student *s) {
    int i;

    /* Sum up all 3 subject marks */
    s->total = 0.0f;
    for (i = 0; i < NUM_SUBJECTS; i++)
        s->total += s->marks[i];

    /* Average = total / number of subjects */
    s->average = s->total / NUM_SUBJECTS;

    /* Assign grade based on average using cascading if-else */
    if      (s->average >= 85.0f) strcpy(s->grade, "A");
    else if (s->average >= 70.0f) strcpy(s->grade, "B");
    else if (s->average >= 50.0f) strcpy(s->grade, "C");
    else                          strcpy(s->grade, "Fail");
}

/* ============================================================
 * FUNCTION: findStudentIndex
 * PURPOSE : Searches for a student by registration number
 *           using linear search through the global array
 * INPUT   : const char *regNo - registration number to find
 * OUTPUT  : returns index (int) if found, -1 if not found
 * ============================================================ */
int findStudentIndex(const char *regNo) {
    int i;

    /* Linear search: compare each stored regNo with the target */
    for (i = 0; i < studentCount; i++) {
        if (strcmp(students[i].regNo, regNo) == 0)
            return i;  /* found — return position in array */
    }
    return -1;  /* not found */
}

/* ============================================================
 * FUNCTION: printStudentRecord
 * PURPOSE : Prints all details of one student in vertical format
 * INPUT   : const Student *s - pointer to the student record
 * OUTPUT  : formatted text printed to stdout
 * ============================================================ */
void printStudentRecord(const Student *s) {
    printf("\n--- Student Record ---\n");
    printf("  Reg No    : %s\n",   s->regNo);
    printf("  Name      : %s\n",   s->name);
    printf("  Subject 1 : %.2f\n", s->marks[0]);
    printf("  Subject 2 : %.2f\n", s->marks[1]);
    printf("  Subject 3 : %.2f\n", s->marks[2]);
    printf("  Total     : %.2f\n", s->total);
    printf("  Average   : %.2f\n", s->average);
    printf("  Grade     : %s\n",   s->grade);
    printf("----------------------\n");
}

/* ============================================================
 * FUNCTION: printTableHeader
 * PURPOSE : Prints the ASCII table header row for student listing
 * INPUT   : none
 * OUTPUT  : header printed to stdout
 * ============================================================ */
void printTableHeader(void) {
    printf("\n+-----------+----------------------+--------+--------+--------+--------+---------+-------+\n");
    printf("| Reg No    | Name                 | Sub 1  | Sub 2  | Sub 3  | Total  | Average | Grade |\n");
    printf("+-----------+----------------------+--------+--------+--------+--------+---------+-------+\n");
}

/* ============================================================
 * FUNCTION: printTableRow
 * PURPOSE : Prints one student as a formatted row inside the table
 * INPUT   : const Student *s - pointer to the student record
 * OUTPUT  : one row printed to stdout
 * ============================================================ */
void printTableRow(const Student *s) {
    printf("| %-9s | %-20.20s | %6.2f | %6.2f | %6.2f | %6.2f | %7.2f | %-5s |\n",
           s->regNo, s->name,
           s->marks[0], s->marks[1], s->marks[2],
           s->total, s->average, s->grade);
}

/* ============================================================
 * FUNCTION: saveToFile
 * PURPOSE : Writes all student records to binary file students.dat
 *           Format: [int studentCount] [Student records...]
 * INPUT   : none (reads from global array)
 * OUTPUT  : binary file written; status message printed
 * ============================================================ */
void saveToFile(void) {
    FILE *fp = fopen(DATA_FILE, "wb");  /* open in write-binary mode */
    if (!fp) {
        printf("[Error] Could not open '%s' for writing.\n", DATA_FILE);
        return;
    }

    /* Write the count first so we know how many records to read back */
    fwrite(&studentCount, sizeof(int), 1, fp);

    /* Write all student records as raw binary data */
    if (studentCount > 0)
        fwrite(students, sizeof(Student), studentCount, fp);

    fclose(fp);
    printf("\n[Info] %d record(s) saved to '%s'.\n", studentCount, DATA_FILE);
}

/* ============================================================
 * FUNCTION: loadFromFile
 * PURPOSE : Reads student records from binary file on startup.
 *           If file doesn't exist, starts with empty database.
 * INPUT   : none
 * OUTPUT  : populates global students[] array and studentCount
 * ============================================================ */
void loadFromFile(void) {
    FILE *fp = fopen(DATA_FILE, "rb");  /* open in read-binary mode */

    /* If file doesn't exist yet, start fresh — not an error */
    if (!fp) {
        printf("[Info] No existing data file found. Starting fresh.\n");
        studentCount = 0;
        return;
    }

    /* Read studentCount; validate it is within acceptable bounds */
    if (fread(&studentCount, sizeof(int), 1, fp) != 1 ||
        studentCount < 0 || studentCount > MAX_STUDENTS) {
        printf("[Warning] Data file corrupted. Starting fresh.\n");
        studentCount = 0;
        fclose(fp);
        return;
    }

    /* Read all student records into global array */
    if (studentCount > 0) {
        size_t readCount = fread(students, sizeof(Student), studentCount, fp);
        /* Handle case where file was partially written/corrupted */
        if ((int)readCount != studentCount) {
            printf("[Warning] Could only read %d of %d records.\n",
                   (int)readCount, studentCount);
            studentCount = (int)readCount;
        }
    }

    fclose(fp);
    printf("[Info] Loaded %d record(s) from '%s'.\n", studentCount, DATA_FILE);
}

/* ============================================================
 * FUNCTION: addStudent
 * PURPOSE : Prompts user to enter a new student record.
 *           Validates RegNo format, checks for duplicates,
 *           validates marks range, then saves to file.
 * INPUT   : user input via scanf/fgets
 * OUTPUT  : new record added to array and saved to file
 * ============================================================ */
void addStudent(void) {
    Student s;
    char regNo[REGNO_LEN];
    int i;

    printf("\n===== Add Student Record =====\n");

    /* Check if we have reached the maximum capacity */
    if (studentCount >= MAX_STUDENTS) {
        printf("[Error] Maximum capacity (%d) reached.\n", MAX_STUDENTS);
        return;
    }

    /* Step 1: Read registration number */
    printf("Enter Registration Number (e.g., 25BCE5098): ");
    if (scanf("%9s", regNo) != 1) {
        printf("[Error] Invalid input.\n");
        while (getchar() != '\n');  /* clear input buffer */
        return;
    }

    /* Convert to uppercase so "25bce5098" is treated same as "25BCE5098" */
    for (i = 0; i < (int)strlen(regNo); i++)
        regNo[i] = toupper(regNo[i]);

    /* Validate RegNo format using isValidRegNo() */
    if (!isValidRegNo(regNo)) {
        printf("[Error] Invalid format. Expected XXYYYXXXX (e.g., 25BCE5098).\n");
        while (getchar() != '\n');
        return;
    }

    /* Check for duplicate registration number */
    if (findStudentIndex(regNo) != -1) {
        printf("[Error] Student with Reg No %s already exists.\n", regNo);
        return;
    }
    strcpy(s.regNo, regNo);

    /* Step 2: Read student name using fgets (handles spaces in name) */
    while (getchar() != '\n');  /* clear leftover newline from scanf */
    printf("Enter Student Name: ");
    if (fgets(s.name, NAME_LEN, stdin) == NULL || strlen(s.name) <= 1) {
        printf("[Error] Name cannot be empty.\n");
        return;
    }
    s.name[strcspn(s.name, "\n")] = '\0';  /* remove trailing newline added by fgets */

    /* Step 3: Read and validate marks for each subject */
    for (i = 0; i < NUM_SUBJECTS; i++) {
        printf("Enter marks for Subject %d (0-100): ", i + 1);
        if (scanf("%f", &s.marks[i]) != 1) {
            printf("[Error] Marks must be numeric.\n");
            while (getchar() != '\n');
            return;
        }
        /* Range check: marks must be between 0 and 100 */
        if (s.marks[i] < 0.0f || s.marks[i] > 100.0f) {
            printf("[Error] Marks must be between 0 and 100.\n");
            return;
        }
    }

    /* Step 4: Automatically compute total, average, and grade */
    computeGrade(&s);

    /* Step 5: Add to global array and save immediately */
    students[studentCount++] = s;
    printf("\n[Success] Student added!\n");
    printStudentRecord(&s);
    saveToFile();
}

/* ============================================================
 * FUNCTION: searchStudent
 * PURPOSE : Searches for a student by RegNo and displays record
 * INPUT   : user input via scanf
 * OUTPUT  : prints record if found, or "not found" message
 * ============================================================ */
void searchStudent(void) {
    char regNo[REGNO_LEN];
    int index, i;

    printf("\n===== Search Student =====\n");

    /* Cannot search if there are no records */
    if (studentCount == 0) {
        printf("[Info] No records available.\n");
        return;
    }

    printf("Enter Registration Number: ");
    if (scanf("%9s", regNo) != 1) {
        printf("[Error] Invalid input.\n");
        while (getchar() != '\n');
        return;
    }

    /* Convert to uppercase before searching */
    for (i = 0; i < (int)strlen(regNo); i++)
        regNo[i] = toupper(regNo[i]);

    /* Use findStudentIndex for linear search */
    index = findStudentIndex(regNo);
    if (index == -1)
        printf("[Info] No student found with Reg No %s.\n", regNo);
    else
        printStudentRecord(&students[index]);
}

/* ============================================================
 * FUNCTION: updateMarks
 * PURPOSE : Updates all 3 subject marks for an existing student
 *           and automatically recomputes grade
 * INPUT   : user input via scanf
 * OUTPUT  : updated record saved to file
 * ============================================================ */
void updateMarks(void) {
    char regNo[REGNO_LEN];
    int index, i;
    float mark;

    printf("\n===== Update Marks =====\n");

    if (studentCount == 0) {
        printf("[Info] No records available.\n");
        return;
    }

    printf("Enter Registration Number: ");
    if (scanf("%9s", regNo) != 1) {
        printf("[Error] Invalid input.\n");
        while (getchar() != '\n');
        return;
    }

    /* Convert to uppercase before searching */
    for (i = 0; i < (int)strlen(regNo); i++)
        regNo[i] = toupper(regNo[i]);

    index = findStudentIndex(regNo);
    if (index == -1) {
        printf("[Info] No student found with Reg No %s.\n", regNo);
        return;
    }

    /* Show current record before updating */
    printf("\nCurrent record:\n");
    printStudentRecord(&students[index]);

    /* Read new marks with validation */
    printf("\nEnter new marks:\n");
    for (i = 0; i < NUM_SUBJECTS; i++) {
        printf("  Subject %d (0-100): ", i + 1);
        if (scanf("%f", &mark) != 1) {
            printf("[Error] Invalid input. Update cancelled.\n");
            while (getchar() != '\n');
            return;
        }
        if (mark < 0.0f || mark > 100.0f) {
            printf("[Error] Marks must be 0-100. Update cancelled.\n");
            return;
        }
        students[index].marks[i] = mark;
    }

    /* Recompute grade after marks are updated */
    computeGrade(&students[index]);
    printf("\n[Success] Marks updated!\n");
    printStudentRecord(&students[index]);
    saveToFile();
}

/* ============================================================
 * FUNCTION: displayAll
 * PURPOSE : Displays all student records in a formatted ASCII table
 * INPUT   : none
 * OUTPUT  : full table printed to stdout
 * ============================================================ */
void displayAll(void) {
    int i;
    printf("\n===== All Student Records =====\n");

    if (studentCount == 0) {
        printf("[Info] No records available.\n");
        return;
    }

    /* Print header row, then each student as a table row */
    printTableHeader();
    for (i = 0; i < studentCount; i++)
        printTableRow(&students[i]);

    /* Print closing border of table */
    printf("+-----------+----------------------+--------+--------+--------+--------+---------+-------+\n");
    printf("Total records: %d\n", studentCount);
}

/* ============================================================
 * FUNCTION: displayTopper
 * PURPOSE : Finds and displays the student(s) with the highest
 *           total marks. Uses 2-pass approach to handle ties.
 * INPUT   : none
 * OUTPUT  : topper record(s) printed to stdout
 * ============================================================ */
void displayTopper(void) {
    int i;
    float maxTotal;

    printf("\n===== Class Topper(s) =====\n");

    if (studentCount == 0) {
        printf("[Info] No records available.\n");
        return;
    }

    /* Pass 1: Find the maximum total across all students */
    maxTotal = students[0].total;
    for (i = 1; i < studentCount; i++)
        if (students[i].total > maxTotal)
            maxTotal = students[i].total;

    printf("Highest Total: %.2f\n\nTopper(s):\n", maxTotal);

    /* Pass 2: Print all students whose total equals the maximum (handles ties) */
    for (i = 0; i < studentCount; i++)
        if (students[i].total == maxTotal)
            printStudentRecord(&students[i]);
}

/* ============================================================
 * FUNCTION: classStatistics
 * PURPOSE : Computes and displays class-wide statistics:
 *           highest total, lowest total, and class average
 * INPUT   : none
 * OUTPUT  : statistics printed to stdout
 * ============================================================ */
void classStatistics(void) {
    int i;
    float highest, lowest, sum;

    printf("\n===== Class Statistics =====\n");

    if (studentCount == 0) {
        printf("[Info] No records available.\n");
        return;
    }

    /* Initialize with the first student's total */
    highest = lowest = sum = students[0].total;

    /* Single pass to find min, max, and accumulate sum */
    for (i = 1; i < studentCount; i++) {
        if (students[i].total > highest) highest = students[i].total;
        if (students[i].total < lowest)  lowest  = students[i].total;
        sum += students[i].total;
    }

    printf("  Number of Students    : %d\n",   studentCount);
    printf("  Highest Total         : %.2f\n",  highest);
    printf("  Lowest Total          : %.2f\n",  lowest);
    printf("  Class Average (Total) : %.2f\n",  sum / studentCount);
}

/* ============================================================
 * FUNCTION: gradeDistribution
 * PURPOSE : Counts and displays how many students are in each
 *           grade category: A, B, C, and Fail
 * INPUT   : none
 * OUTPUT  : grade counts printed to stdout
 * ============================================================ */
void gradeDistribution(void) {
    int i;
    int a = 0, b = 0, c = 0, f = 0;  /* counters for each grade */

    printf("\n===== Grade Distribution =====\n");

    if (studentCount == 0) {
        printf("[Info] No records available.\n");
        return;
    }

    /* Count each student into the correct grade bucket */
    for (i = 0; i < studentCount; i++) {
        if      (strcmp(students[i].grade, "A") == 0) a++;
        else if (strcmp(students[i].grade, "B") == 0) b++;
        else if (strcmp(students[i].grade, "C") == 0) c++;
        else                                          f++;  /* "Fail" */
    }

    printf("  Grade A (Avg >= 85) : %d student(s)\n", a);
    printf("  Grade B (Avg >= 70) : %d student(s)\n", b);
    printf("  Grade C (Avg >= 50) : %d student(s)\n", c);
    printf("  Fail    (Avg <  50) : %d student(s)\n", f);
    printf("  Total               : %d student(s)\n", a + b + c + f);
}

/* ============================================================
 * FUNCTION: main
 * PURPOSE : Entry point. Loads saved data, shows main menu in
 *           a loop, and calls appropriate function for each choice.
 * ============================================================ */
int main(void) {
    int choice;
    
    /* Welcome banner */
    printf("============================================\n");
    printf("  Smart Student Performance Analyzer\n");
    printf("  Daksh Agarwal | 25BCE5098\n");
    printf("============================================\n\n");

    /* Load any previously saved records from file */
    loadFromFile();

    /* Main menu loop — keeps running until user chooses Exit (8) */
    while (1) {
        printf("\n========== MAIN MENU ==========\n");
        printf("  1. Add Student Record\n");
        printf("  2. Search Student by RegNo\n");
        printf("  3. Update Marks\n");
        printf("  4. Display All Students\n");
        printf("  5. Display Topper(s)\n");
        printf("  6. Class Statistics\n");
        printf("  7. Grade Distribution\n");
        printf("  8. Exit\n");
        printf("===============================\n");
        printf("Enter your choice (1-8): ");

        /* Validate menu input — handle non-numeric input gracefully */
        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Please enter a number between 1 and 8.\n");
            while (getchar() != '\n');  /* clear bad input from buffer */
            continue;
        }

        /* Switch-case routes to the correct function */
        switch (choice) {
            case 1: addStudent();        break;
            case 2: searchStudent();     break;
            case 3: updateMarks();       break;
            case 4: displayAll();        break;
            case 5: displayTopper();     break;
            case 6: classStatistics();   break;
            case 7: gradeDistribution(); break;
            case 8:
                saveToFile();
                printf("\n[Info] Goodbye!\n");
                return 0;  /* exit program */
            default:
                printf("\n[Error] Invalid choice. Please enter 1-8.\n");
                break;
        }
    }

    return 0;
}
