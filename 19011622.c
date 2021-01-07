//
//  19011622.c
//  19011622
//
//  Created by Mert Arıcan on 25.12.2020.
//  Copyright © 2020 Mert Arıcan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// MARK: - DATA TYPES

// Tiny struct for holding quota information about a class

typedef struct {
    int registered;
    int total;
} Quota;

// Struct for instructor

typedef struct {
    int ID;
    char *name;
    char *surname;
    char *title;
} Instructor;

// Struct for course

typedef struct {
    char *code;
    char *name;
    int credit;
    Quota quota;
    int instructorID;
} Course;

// Struct for student

typedef struct {
    int studentNumber;
    char *name;
    char *surname;
    int numberOfCoursesRegistered;
    int numberOfCreditsTaken;
} Student;

// Struct for registrations

typedef struct {
    int ID;
    int studentNumber;
    char *courseCode;
    bool stillRegistered;
    char *date;
} Registration;

// MARK: - ITEM ABSTRACTION

/* In this assignment, we are asked to build a database that supports addition, removal or update of an instructor, course, student or registration instance. So there should be a function for adding instructor to a database, adding course to a database, adding student to a database, etc. But that means we need the same function, i.e. 'add', with different types of parameters, i.e. 'struct instructor' or 'struct course'. This also applies for both remove and update processes. So, instead of having 4 separate functions for each of these operations, I decided to make an extra layer of abstraction. This abstract struct, called Item, consists of an enum of type 'ItemType' called 'type', and union of type 'ItemUnion' called 'value'. In this union called 'value', either Instructor, Course, Student or Registration instance is hold. And, in enum called 'type', the type of the 'value' is hold. For example, if union has a value of type Instructor, then enum's value is 'InstructorType'. By doing that, I only need to make one function for each operation, i.e. add(Item item), and only parameter of this function is going to be of this new type 'Item'. Inside of these generic functions, i.e. add, remove, update, each process is handled according to 'type' of the 'Item'. */

// Enum for item type

typedef enum { InstructorType, CourseType, StudentType, RegistrationType } ItemType;

// Union for item value

typedef union {
    Instructor instructor;
    Course course;
    Student student;
    Registration registration;
} ItemUnion;

// Abstract struct of 'Item'

typedef struct {
    ItemType type;
    ItemUnion value;
} Item;

// MARK: - OPTIONAL ITEM ABSTRACTION

/* One more layer of abstraction, useful for query functions. It is inspired by Optionals from a higher level language
 called Swift. In Swift, some variable might have optional types, for example Int?. Int?, means that variable is
 either has a value and type of this value is Int, or variable has no value at all. I needed a similar mechanism for query
 i.e. listing functions, and wrapped 'Item' in an 'OptionalItem'. If 'Item' has a value then 'hasValue' property is set to true
 if not, it is set to false. 'OptionalItem's full functionality is explained in query functions below. */

typedef struct {
    Item item;
    bool hasValue;
} OptionalItem;

// MARK: Item Wrappers

/*
 This convenience functions are needed for wrapping an instance of either Instructor, Course, Student or Registration in an 'Item' instance. Only by doing that, these instances can be passed to functions as a parameter.
 Also, there are functions in below, like 'Item wrapInstructorWithID(int ID)', which is also used to wrap an instance in an 'Item' instance. When checking whether an Item is in database or not, i.e. checking whether Instructor is already in Instructors file, only value used, is this unique value 'ID'. So, by using only these unique values and wrapper functions, artificial instances can be created and used to check whether database already consists them. Artificial instance, is an instance that only has this unique key value, but no other values. For example, if we need to check whether student with number 19011001 is in database, 'Item wrapStudentWithStudentNumber(int studentNumber)' function is called with this number, and first, a student instance created with this student number, but no other information is given. So, after first step, there is a student instance with student number 19011001 but no name, no surname or credit etc.. And this Student instance is wrapped in an 'Item' instance, and now can be used to check whether record is in database or not, because only value used in this check is this unique key value of student number. Same applies for Course and courseCode, Instructor and ID etc.
 
 Basically, all of this 'wrapper' functions are just initializers for 'Item' with different parameters.
 */

Item wrapInstructor(Instructor instructor) {
    ItemUnion itemUnion; Item item; itemUnion.instructor = instructor;
    item.type = InstructorType; item.value = itemUnion; return item;
}

Item wrapInstructorWithID(int ID) {
    Instructor instructor = { ID }; return wrapInstructor(instructor);
}

Item wrapCourse(Course course) {
    ItemUnion itemUnion; Item item; itemUnion.course = course;
    item.type = CourseType; item.value = itemUnion; return item;
}

Item wrapCourseWithCode(char *code) {
    Course course = { code }; return wrapCourse(course);
}

Item wrapStudent(Student student) {
    ItemUnion itemUnion; Item item; itemUnion.student = student;
    item.type = StudentType; item.value = itemUnion; return item;
}

Item wrapStudentWithStudentNumber(int studentNumber) {
    Student student = { studentNumber }; return wrapStudent(student);
}

Item wrapRegistration(Registration registration) {
    ItemUnion itemUnion; Item item; itemUnion.registration = registration;
    item.type = RegistrationType; item.value = itemUnion; return item;
}

Item wrapRegistrationsWithID(int ID) {
    Registration registrations = { ID }; return wrapRegistration(registrations);
}

Item wrapRegistrationWithStudentNumberAndCourseCode(int studentNumber, char *courseCode) {
    Registration registration = { -1, studentNumber, courseCode }; return wrapRegistration(registration);
}

// Freeing memory allocated for an Item instance

/* Sometimes, item instance created in function but not deallocated in same function.
 This function is used to clear memory used by those instances. */

void freeItem(Item item) {
    switch (item.type) {
        case InstructorType:
            free(item.value.instructor.name);
            free(item.value.instructor.surname);
            free(item.value.instructor.title); return;
        case CourseType:
            free(item.value.course.code);
            free(item.value.course.name); return;
        case StudentType:
            free(item.value.student.name);
            free(item.value.student.surname); return;
        case RegistrationType:
            free(item.value.registration.courseCode);
            free(item.value.registration.date); return;
    }
}

// Convenience function for printing an Item

void printItem(Item item) {
    Student student = item.value.student;
    Registration registration = item.value.registration;
    Course course = item.value.course;
    Instructor instructor = item.value.instructor;
    switch (item.type) {
        case InstructorType:
            printf("###INSTRUCTOR RECORD###\nID: %d\nName: %s\nSurname: %s\nTitle: %s\n#######################\n\n", instructor.ID, instructor.title, instructor.name, instructor.surname); return;
        case CourseType:
            printf("###COURSE RECORD###\nCourse code: %s\nCourse name: %s\nCredit: %d\nQuota: %d/%d\nInstructor ID: %d\n###################\n\n", course.code, course.name, course.credit, course.quota.registered, course.quota.total, course.instructorID); return;
        case StudentType:
            printf("###STUDENT RECORD###\nStudent number: %d\nName: %s\nSurname: %s\nNumber of courses registered: %d\nNumber of credits taken: %d\n#####################\n\n", student.studentNumber, student.name, student.surname, student.numberOfCoursesRegistered, student.numberOfCreditsTaken); return;
        case RegistrationType:
            printf("###REGISTRATION RECORD###\nID: %d\nStudent number: %d\nCourse code: %s\nStill registered: %s\nRegistration date: %s\n##########################\n\n", registration.ID, registration.studentNumber, registration.courseCode, (registration.stillRegistered) ? "True" : "False", registration.date); return;
    }
}

// MARK: - CONVENIENCE FUNCTIONS

// File names for records

void getFileNameForType(ItemType type, char *fileName) {
    switch (type) {
        case InstructorType: strcpy(fileName, "Instructors.txt"); return;
        case CourseType: strcpy(fileName, "Courses.txt"); return;
        case StudentType: strcpy(fileName, "Students.txt"); return;
        case RegistrationType: strcpy(fileName, "Registrations.txt"); return;
    }
}

// Get record count of a file

int getRecordCountOfAFile(ItemType type) {
    /* Each item type has its own properties, like name and course code, and these properties are stored in
     a line in database files. So, if record type is InstructorType then number of properties it has is 4
     --ID, Name, Surname, Title--and each record occupies 5 line (1 extra because of empty line). So, if we
     divide Instructor records file's number of lines by 5 we get the number of records that are in database.
     This also serves as unique ID creator for registrations. */
    char *fileName = malloc(sizeof(char)*255);
    if (fileName == NULL) { printf("Couldn't allocate memory in 'getRecordCountOfAFile'.\n"); exit(1); }
    int recordLength = (type == InstructorType) ? 5 : 6;
    getFileNameForType(type, fileName);
    if (fileName == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'getRecordCountOfAFile' function.\n"); exit(1);
    }
    FILE *file = fopen(fileName, "r");
    int lineCount = 0; int c;
    if (file == NULL) { return 0; }
    while ((c = getc(file)) != EOF) { if (c == '\n') { lineCount++; } }
    fclose(file); free(fileName);
    return lineCount/recordLength;
}

// MARK: - DECODING FUNCTIONS

// Those functions are used as a subroutine for creating instances from files.

Item readInstructorFromFile(FILE *instructorsFile) {
    Instructor instructor;
    instructor.name = malloc(sizeof(char)*255);
    instructor.surname = malloc(sizeof(char)*255);
    instructor.title = malloc(sizeof(char)*255);
    char *buffer = malloc(sizeof(char)*255);
    if (buffer == NULL || instructor.name == NULL || instructor.surname == NULL || instructor.title == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'readInstructorFromFile' function.\n"); exit(1);
    }
    fgets(buffer, 255, instructorsFile);
    sscanf(buffer, "ID: %d\n", &instructor.ID);
    fgets(buffer, 255, instructorsFile);
    sscanf(buffer, "Name: %s\n", instructor.name);
    fgets(buffer, 255, instructorsFile);
    sscanf(buffer, "Surname: %s\n", instructor.surname);
    fgets(buffer, 255, instructorsFile);
    sscanf(buffer, "Title: %s\n", instructor.title);
    fgets(buffer, 255, instructorsFile);
    free(buffer);
    return wrapInstructor(instructor);
}

Item readCourseFromFile(FILE *coursesFile) {
    Course course;
    char *buffer = malloc(sizeof(char)*255);
    course.code = malloc(sizeof(char)*255);
    course.name = malloc(sizeof(char)*255);
    if (buffer == NULL || course.code == NULL || course.name == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'readCourseFromFile' function.\n"); exit(1);
    }
    fgets(buffer, 255, coursesFile);
    sscanf(buffer, "Course code: %s\n", course.code);
    fgets(buffer, 255, coursesFile);
    sscanf(buffer, "Course name: %[^\n]\n", course.name);
    fgets(buffer, 255, coursesFile);
    sscanf(buffer, "Credit: %d\n", &course.credit);
    fgets(buffer, 255, coursesFile);
    sscanf(buffer, "Quota: %d/%d\n", &course.quota.registered, &course.quota.total);
    fgets(buffer, 255, coursesFile);
    sscanf(buffer, "Instructor ID: %d\n", &course.instructorID);
    fgets(buffer, 255, coursesFile);
    free(buffer);
    return wrapCourse(course);
}

Item readStudentFromFile(FILE *studentsFile) {
    Student student;
    student.name = malloc(sizeof(char)*255);
    student.surname = malloc(sizeof(char)*255);
    char *buffer = malloc(sizeof(char)*255);
    if (buffer == NULL || student.name == NULL || student.surname == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'readStudentFromFile' function.\n"); exit(1);
    }
    fgets(buffer, 255, studentsFile);
    sscanf(buffer, "Student number: %d\n", &student.studentNumber);
    fgets(buffer, 255, studentsFile);
    sscanf(buffer, "Name: %[^\n]\n", student.name);
    fgets(buffer, 255, studentsFile);
    sscanf(buffer, "Surname: %[^\n]\n", student.surname);
    fgets(buffer, 255, studentsFile);
    sscanf(buffer, "Number of courses registered: %d\n", &student.numberOfCoursesRegistered);
    fgets(buffer, 255, studentsFile);
    sscanf(buffer, "Number of credits taken: %d\n", &student.numberOfCreditsTaken);
    fgets(buffer, 255, studentsFile);
    return wrapStudent(student);
}

Item readRegistrationFromFile(FILE *registrationsFile) {
    Registration registration;
    registration.courseCode = malloc(sizeof(char)*255);
    registration.date = malloc(sizeof(char)*20);
    char *registrationStatus = malloc(sizeof(char) * 6);
    char *buffer = malloc(sizeof(char)*255);
    if (buffer == NULL || registration.courseCode == NULL || registration.date == NULL || registrationStatus == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'readRegistrationFromFile' function.\n"); exit(1);
    }
    fgets(buffer, 255, registrationsFile);
    sscanf(buffer, "ID: %d\n", &registration.ID);
    fgets(buffer, 255, registrationsFile);
    sscanf(buffer, "Course code: %s\n", registration.courseCode);
    fgets(buffer, 255, registrationsFile);
    sscanf(buffer, "Student number: %d\n", &registration.studentNumber);
    fgets(buffer, 255, registrationsFile);
    sscanf(buffer, "Still registered: %s\n", registrationStatus);
    fgets(buffer, 255, registrationsFile);
    sscanf(buffer, "Registration date: %[^\n]\n", registration.date);
    fgets(buffer, 255, registrationsFile);
    registration.stillRegistered = (strcmp(registrationStatus, "False") == 0) ? false : true;
    free(buffer); free(registrationStatus);
    return wrapRegistration(registration);
}

// MARK: - FUNCTION PROTOTYPES

void prepareForIteration(ItemType type, char *fileName, Item(**decodingFunction)(FILE*));
bool conditionForQuery(Item decodedItem, Item queriedItem);
OptionalItem ItemIterator(ItemType type, OptionalItem(*aimFunction)(Item, Item), Item aimItem);
OptionalItem query(Item decodedItem, Item queriedItem);
bool itemIsInDatabase(Item item);
Item getItem(Item item);
void removeCoursesGivenByInstructor(Item instructorItem);
void invalidateRegistrationsAfterCourseOrStudentRemoval(Item student, int credit);
void updateStudentsCreditStatus(int studentNumber, bool afterRegisteration, int credit, bool changeCourseCount);
OptionalItem registrationCourseOrStudentRemoval(Item registration, Item courseOrStudent);
OptionalItem courseInstructorRemoval(Item courseItem, Item instructor);
void updateRegistrationsAfterCourseOrStudentsUniqueKeyHasChanged(Item courseOrStudentToRemove, Item updatedVersion);
void updateStudentsCreditIfCoursesCreditHasChanged(int creditDifference, Item course);
void updateCoursesAfterInstructorIDChange(Item instructorItem, Item updatedVersion);
void updateItemBase(Item itemToBeUpdated, Item updatedVersion, bool printMessage);
void updateItemSilently(Item itemToBeUpdated, Item updatedVersion);
void updateItem(Item itemToBeUpdated, Item updatedVersion);

// MARK: - ADDING 'Item' TO THE DATABASE

// MARK: Encoding functions

/* These functions are used as a subroutine for writing instances to a file in database.
 Adding Registration record to database is handled separately.
 If function's are called as a subroutine of UPDATE operation, then no message are printed
 if they get called as a subroutine of ADD operation, then message is printed. */

void writeInstructorToAFile(Item instructorItem, FILE *instructorsFile, bool forUpdate) {
    Instructor instructor = instructorItem.value.instructor;
    fprintf(instructorsFile, "ID: %d\n", instructor.ID);
    fprintf(instructorsFile, "Name: %s\n", instructor.name);
    fprintf(instructorsFile, "Surname: %s\n", instructor.surname);
    fprintf(instructorsFile, "Title: %s\n\n", instructor.title);
    if (!forUpdate) {
        printf("Added the instructor '%s %s %s' with ID: %d.\n", instructor.title, instructor.name, instructor.surname, instructor.ID);
    }
    fclose(instructorsFile);
}

void writeCourseToAFile(Item courseItem, FILE *coursesFile, bool forUpdate) {
    Course course = courseItem.value.course;
    fprintf(coursesFile, "Course code: %s\n", course.code);
    fprintf(coursesFile, "Course name: %s\n", course.name);
    fprintf(coursesFile, "Credit: %d\n", course.credit);
    fprintf(coursesFile, "Quota: %d/%d\n", course.quota.registered, course.quota.total);
    fprintf(coursesFile, "Instructor ID: %d\n\n", course.instructorID);
    if (!forUpdate) {
        printf("Added the course '%s %s'.\n", course.code, course.name);
    }
    fclose(coursesFile);
}

void writeStudentToAFile(Item studentItem, FILE *studentsFile, bool forUpdate) {
    Student student = studentItem.value.student;
    fprintf(studentsFile, "Student number: %d\n", student.studentNumber);
    fprintf(studentsFile, "Name: %s\n", student.name);
    fprintf(studentsFile, "Surname: %s\n", student.surname);
    fprintf(studentsFile, "Number of courses registered: %d\n", student.numberOfCoursesRegistered);
    fprintf(studentsFile, "Number of credits taken: %d\n\n", student.numberOfCreditsTaken);
    if (!forUpdate) {
        printf("Added the student '%s %s'.\n", student.name, student.surname);
    }
    fclose(studentsFile);
}

// MARK: Add Item

void updateStudentsCreditStatus(int studentNumber, bool registerationAdded, int credit, bool changeCourseCount) {
    // Helper function for updating students credit status. Assumes student is in database.
    int count = (registerationAdded) ? 1 : -1; credit = (registerationAdded) ? credit : -credit;
    Item studentToUpdate = getItem(wrapStudentWithStudentNumber(studentNumber));
    Item updatedVersion = studentToUpdate;
    if (changeCourseCount) {
        updatedVersion.value.student.numberOfCoursesRegistered += count;
    }
    updatedVersion.value.student.numberOfCreditsTaken += credit;
    updateItemSilently(studentToUpdate, updatedVersion); freeItem(studentToUpdate);
}

void updateCourseQuota(char *courseCode, bool studentAdded) {
    // Updates the quota of the course. Assumes course is in database.
    Item course = getItem(wrapCourseWithCode(courseCode));
    course.value.course.quota.registered += (studentAdded) ? 1 : -1;
    updateItemSilently(course, course); freeItem(course);
}

void prepareForAppend(Item item, char *error, char *error2, char *fileName, void(**writeToAFileFunc)(Item, FILE*, bool)) {
    /* Convenience function for adding an Item to the database. This function assigns error messages, file name and
     appropriate encoding function, to pointers passed as parameters, according to the type of the 'item'.
     Adding 'Registration' record is handled separately. */
    getFileNameForType(item.type, fileName);
    switch (item.type) {
        case InstructorType:
            sprintf(error, "ERROR: Couldn't add the instructor. There is already an instructor with the same ID: %d.\n", item.value.instructor.ID);
            *writeToAFileFunc = &writeInstructorToAFile; return;
        case CourseType:
            sprintf(error, "ERROR: Couldn't add the course. There is already a course with the same course code: %s.\n", item.value.course.code);
            sprintf(error2, "ERROR: Couldn't add the course. There is no instructor with the ID: %d.\n", item.value.course.instructorID);
            *writeToAFileFunc = &writeCourseToAFile; return;
        case StudentType:
            sprintf(error, "ERROR: Couldn't add the student. There is already a student with the number: %d.\n", item.value.student.studentNumber);
            *writeToAFileFunc = &writeStudentToAFile; return;
        case RegistrationType: return; // Registrations handled separately.
    }
}

void addItemBase(Item item, bool forUpdate) {
    /* This function adds 'item' to the database.
     This function, first checks whether 'item' is already in database or not.
     Also, if Item's type is 'CourseType', then function checks whether course's instructor
     is in database or not. If all conditions are met, 'item' is added in database. */
    char *error = malloc(sizeof(char)*511);
    char *error2 = malloc(sizeof(char)*511);
    char *fileName = malloc(sizeof(char)*255);
    void (*writeToAFileFunc)(Item, FILE*, bool) = writeInstructorToAFile;
    if (error == NULL || error2 == NULL || fileName == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'addItemBase' function.\n"); exit(1);
    }
    prepareForAppend(item, error, error2, fileName, &writeToAFileFunc);
    if (itemIsInDatabase(item)) {
        printf("%s", error); free(error); free(error2); free(fileName); return;
    }
    else if (item.type == CourseType) {
        if (!itemIsInDatabase(wrapInstructorWithID(item.value.course.instructorID))) { printf("%s", error2); return; }
    }
    FILE *file = fopen(fileName, "a");
    if (file == NULL) {
        printf("Couldn't open the file '%s' and %s.\n", fileName, error);
        free(error); free(error2); free(fileName); return;
    }
    (*writeToAFileFunc)(item, file, forUpdate);
    free(error); free(error2); free(fileName);
}

/* If 'addItemBase' function should print success message, then its 'forUpdate' parameter
 should be false, otherwise it should be true. Functions below are just convenience functions for calling
 'addItemBase' function. */

void addItem(Item item) { addItemBase(item, false); }
void addItemAsAPartOfUpdateProcess(Item item) { addItemBase(item, true); }

// MARK: Registering student for a course

void getDateForRegistration(char *date) {
    // Helper function for adding date to 'Registration' record.
    time_t t = time(NULL);
    struct tm time = *localtime(&t);
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
}

void registerStudentForCourseBase(char *courseCode, int studentNumber, int MAX_COUNT, int MAX_CREDIT, bool forUpdate) {
    /* Registers student with given 'studentNumber', for the course with given 'courseCode'.
     If course is not in database, or student is not in database, or student is already registered for more courses
     than he/she should have registered, or student is already taken more credits than he/she should have taken, or student is
     already registered for the same course then error is thrown and process failed. Otherwise, registration record is added to
     the database, and student's status is updated. */
    Course course = getItem(wrapCourseWithCode(courseCode)).value.course; bool shouldClearCourse = true;
    Student student = getItem(wrapStudentWithStudentNumber(studentNumber)).value.student; bool shouldClearStudent = true;
    Item registration = getItem(wrapRegistrationWithStudentNumberAndCourseCode(studentNumber, courseCode));
    if (!itemIsInDatabase(wrapCourse(course))) {
        shouldClearCourse = false;
        printf("ERROR: Couldn't register for course. There is no course with code: %s.\n", course.code);
    } else if (!itemIsInDatabase(wrapStudent(student))) {
        shouldClearStudent = false;
        printf("ERROR: Couldn't register for course. There is no student with student number: %d.\n", student.studentNumber);
    } else if (student.numberOfCoursesRegistered >= MAX_COUNT) {
        printf("ERROR: Couldn't register for course. '%s %s' is already registered in %d courses "
               "which is the maximum number of courses a student can register on a quarter.\n", student.name, student.surname, student.numberOfCoursesRegistered);
    } else if (student.numberOfCreditsTaken + course.credit > MAX_CREDIT) {
        printf("ERROR: Couldn't register for course. Maximum number of credits exceeded. A student can \n"
               "take maximum of %d credits in a quarter and '%s %s' takes %d credits already, and course \n"
               "he/she wants to register for is %d credits worth.\n",
               MAX_CREDIT, student.name, student.surname, student.numberOfCreditsTaken, course.credit);
    } else if (course.quota.total - course.quota.registered == 0) {
        printf("ERROR: Couldn't register for course, quota of course is exceeded.\n");
    } else if (itemIsInDatabase(registration)) {
        printf("ERROR: Couldn't register for course. %s %s already registered for %s %s.\n", student.name, student.surname, course.code, course.name); freeItem(registration);
    } else {
        FILE *registrationsFile = fopen("Registrations.txt", "a");
        char *date = malloc(sizeof(char)*20);
        if (date == NULL) { printf("EXCEPTION: Couldn't allocate memory in 'registerStudentForCourseBase' function.\n"); exit(1); }
        getDateForRegistration(date);
        if (registrationsFile == NULL) { printf("ERROR. Couldn't register for course.\n"); return; }
        fprintf(registrationsFile, "ID: %d\n", getRecordCountOfAFile(RegistrationType));
        fprintf(registrationsFile, "Course code: %s\n", course.code);
        fprintf(registrationsFile, "Student number: %d\n", student.studentNumber);
        fprintf(registrationsFile, "Still registered: True \n");
        fprintf(registrationsFile, "Registration date: %s\n\n", date);
        fclose(registrationsFile); free(date);
        if (!forUpdate) {
            /* If we use this function as a part of UPDATE operation, then quota and credit shouldn't get updated
             also no success message should get printed. */
            updateStudentsCreditStatus(studentNumber, true, course.credit, true);
            updateCourseQuota(courseCode, true);
            printf("Successfully registered %s %s for the course %s %s\n", student.name, student.surname, course.code, course.name);
        }
    }
    if (shouldClearCourse) { freeItem(wrapCourse(course)); }
    if (shouldClearStudent) { freeItem(wrapStudent(student)); }
}

/* If 'registerStudentForCourseBase' function should print success message, then its 'forUpdate' parameter
 should be false, otherwise it should be true. Functions below are just convenience functions for calling
 'registerStudentForCourseBase' function. */

void registerStudentForCourse(char *courseCode, int studentNumber, int MAX_COUNT, int MAX_CREDIT) {
    registerStudentForCourseBase(courseCode, studentNumber, MAX_COUNT, MAX_CREDIT, false);
}

void registerStudentForCourseAsAPartOfUpdateProcess(char *courseCode, int studentNumber, int MAX_COUNT, int MAX_CREDIT) {
    registerStudentForCourseBase(courseCode, studentNumber, MAX_COUNT, MAX_CREDIT, true);
}

// MARK: - REMOVING 'Item' FROM DATABASE

void prepareForRemoval(Item item, char *fileName, char *checkString, char *error, char *success, int *count) {
    /* Convenience function for removing an Item from database. This function assigns error message, success message,
     file name, and 'count' variable, to pointers passed as parameters, according to the type of the 'item'. */
    getFileNameForType(item.type, fileName);
    switch (item.type) {
        case InstructorType:
            sprintf(error, "ERROR: Couldn't remove the instructor. There is no instructor with the ID: %d.\n", item.value.instructor.ID) ;
            sprintf(success, "Removed the instructor '%s %s %s' with ID: %d.\n",item.value.instructor.title,
                    item.value.instructor.name, item.value.instructor.surname, item.value.instructor.ID);
            sprintf(checkString, "ID: %d\n", item.value.instructor.ID); *count = 3; return;
        case CourseType:
            sprintf(error, "ERROR: Couldn't remove the course. There is no course with the course code: %s.\n", item.value.course.code) ;
            sprintf(success, "Removed the course '%s %s'.\n", item.value.course.code, item.value.course.name);
            sprintf(checkString, "Course code: %s\n", item.value.course.code); *count = 4; return;
        case StudentType:
            sprintf(error, "ERROR: Couldn't remove the student. There is no student with the student number: %d.\n", item.value.student.studentNumber) ;
            sprintf(success, "Removed the student '%s %s'.\n", item.value.student.name, item.value.student.surname);
            sprintf(checkString, "Student number: %d\n", item.value.student.studentNumber); *count = 4; return;
        case RegistrationType:
            sprintf(error, "ERROR: Couldn't remove the registration.\n") ;
            sprintf(success, "Removed the registration.\n");
            sprintf(checkString, "ID: %d\n", item.value.registration.ID); *count = 4; return;
    }
}

void removeItemBase(Item item, bool forUpdate) {
    /* Removes the 'item' from the database, if it is in the database.
     If item's type is 'RegistrationType', then when record found in database,
     'Still registered: True ' expression changed with 'Still registered: False'.
     If item's type is different than 'RegistrationType', then record totally removed from file.
     Removal implemented like this:
     - Create a new temporary file to write.
     - Open the database file that holds records of instances of same type as item's type.
     - Read records from database file and...
     - If record is item's record, then don't write it to a new file.
     - Else, copy record to this new temporary file.
     - After process completed, remove the old database file, and rename the new temporary file to be the same as removed file's name. */
    char *buffer = malloc(sizeof(char)*255);
    char *fileName = malloc(sizeof(char)*255);
    char *checkString = malloc(sizeof(char)*255);
    char *error = malloc(sizeof(char)*511);
    char *success = malloc(sizeof(char)*511);
    int count = 0; int credit = -1;
    
    if (buffer == NULL || fileName == NULL || checkString == NULL || error == NULL || success == NULL) {
        printf("EXCEPTION: Couldn't allocate memory in 'removeItemBase' function.\n"); exit(1);
    }
    
    item = getItem(item); // 'item' might be an artificial instance, so we have to get the rest of the information about 'item'.
    
    prepareForRemoval(item, fileName, checkString, error, success, &count);
    
    if (!itemIsInDatabase(item)) { printf("%s\n", error); return; }
    
    if (item.type == RegistrationType) {
        // If item's type is RegistrationType, then overwrite "False" on "True "
        FILE *registrationsFile = fopen(fileName, "r+");
        if (registrationsFile == NULL) {
            printf("ERROR: Couldn't open 'Registrations.txt'.\n");
            free(buffer); free(fileName); free(checkString); free(error); free(success); return;
        }
        while (fgets(buffer, 255, registrationsFile)) {
            if (strcmp(buffer, checkString) == 0) {
                for (int i = 0; i <= count-2; i++) {
                    fgets(buffer, 255, registrationsFile);
                }
                fseek(registrationsFile, ftell(registrationsFile)-6, SEEK_SET);
                fprintf(registrationsFile, "False\n"); break;
            }
        }; fclose(registrationsFile);
    }
    else {
        // If item's type is InstructorType, CourseType or StudentType...
        if (item.type == CourseType) { credit = item.value.course.credit; }
        FILE *tmp = fopen("tmp.txt", "w");
        FILE *file = fopen(fileName, "r");
        if (tmp == NULL || file == NULL) {
            printf("ERROR: Couldn't open '%s' at 'removeItemBase' function.\n", fileName);
            free(buffer); free(fileName); free(checkString); free(error); free(success); return;
        }
        while (fgets(buffer, 255, file)) {
            if (strcmp(buffer, checkString) != 0) { fprintf(tmp, "%s", buffer); }
            else { for (int i = 0; i <= count; i++) { fgets(buffer, 255, file); } }
        }
        fclose(file); remove(fileName); rename("tmp.txt", fileName); fclose(tmp);
    }
    free(buffer); free(fileName); free(checkString); free(error);
    
    if (!forUpdate) {
        // Make necessary changes in database after removal, if item is not removed as a part of an update process.
        printf("%s", success);
        Course course;
        switch (item.type) {
            case InstructorType:
                /* If removed item is of type InstructorType, then remove all the courses associated with the
                 deleted instructor's instructorID, this also going to trigger all registrations that associated
                 with courses given by this instructor, to get cancelled. */
                removeCoursesGivenByInstructor(item); break;
            case RegistrationType:
                /* If removed item is of type RegistrationType, then student's credit value
                 and course's quota value should get updated according to the registration. */
                course = getItem(wrapCourseWithCode(item.value.registration.courseCode)).value.course;
                updateStudentsCreditStatus(item.value.registration.studentNumber, false, course.credit, true);
                updateCourseQuota(course.code, false); freeItem(wrapCourse(course)); break;
            default:
                /* This will invalidate all registrations associated with 'item'. If 'item' is of type CourseType,
                 then all registrations with the same course code as 'item' will be removed, if 'item' is of type StudentType,
                 then all registrations made with the same studentNumber will be removed from the database. */
                invalidateRegistrationsAfterCourseOrStudentRemoval(item, credit); break;
        }
    }
    free(success); freeItem(item);
}

/* If 'removeItemBase' function should print success message, then its 'forUpdate' parameter
 should be false, otherwise it should be true. Functions below are just convenience functions for calling
 'removeItemBase' function. */

void removeItem(Item item) { removeItemBase(item, false); }
void removeItemAsAPartOfUpdateProcess(Item item) { removeItemBase(item, true); }

// MARK: - UPDATING 'Item'

void prepareForUpdate(Item itemToBeUpdated, Item updatedVersion, bool *uniqueIdentifierHasChanged, char *error1, char *error2) {
    /* Convenience function for updating an Item. This function assigns error messages, and whether item's unique identifier
     has changed with update or not, to pointers passed as parameters, according to the type of the items. */
    switch (itemToBeUpdated.type) {
        case InstructorType:
            *uniqueIdentifierHasChanged = itemToBeUpdated.value.instructor.ID != updatedVersion.value.instructor.ID;
            sprintf(error2, "Update failed. There is already an instructor with the ID: %d.\n", updatedVersion.value.instructor.ID);
            sprintf(error1, "ERROR: Update failed. Couldn't find the instructor to be updated.\n"); return;
        case CourseType:
            *uniqueIdentifierHasChanged = strcmp(itemToBeUpdated.value.course.code, updatedVersion.value.course.code) != 0;
            sprintf(error2, "ERROR: Update failed. There is already a course with the same code: %s.\n", updatedVersion.value.course.code);
            sprintf(error1, "ERROR: Update failed. Couldn't find the course to be updated.\n"); return;
        case StudentType:
            *uniqueIdentifierHasChanged = itemToBeUpdated.value.student.studentNumber != updatedVersion.value.student.studentNumber;
            sprintf(error2, "ERROR: Update failed. There is already a student with student number: %d.\n", updatedVersion.value.student.studentNumber);
            sprintf(error1, "ERROR: Update failed. Couldn't find the student to be updated %d.\n", itemToBeUpdated.value.student.studentNumber); return;
        case RegistrationType: return;
    }
}

void updateItemBase(Item itemToBeUpdated, Item updatedVersion, bool printMessage) {
    /* Changes the record of 'itemToBeUpdated' with 'updatedVersion'. If unique identifier
     of the item has changed, i.e. course code of a course, than all records that uses that
     information gets updated, i.e. invalidate all registrations that has old course code and
     add new registrations with new course code, for every student registered for the course so far. */
    char *error1 = malloc(sizeof(char)*511);
    char *error2 = malloc(sizeof(char)*511);
    if (error1 == NULL || error2 == NULL) { printf("Couldn't allocate memory in 'updateItemBase' function.\n"); exit(1); }
    bool uniqueIdentifierHasChanged = false;
    prepareForUpdate(itemToBeUpdated, updatedVersion, &uniqueIdentifierHasChanged, error1, error2);
    
    // If 'itemToBeUpdated' is not in database, throw error
    if (!itemIsInDatabase(itemToBeUpdated)) { printf("%s", error1); }
    // If 'updatedVersion's unique identifier is already used by a record in database, throw error
    else if (uniqueIdentifierHasChanged && itemIsInDatabase(updatedVersion)) {
        printf("%s", error2);
    } else if (itemToBeUpdated.type == CourseType && !itemIsInDatabase(wrapInstructorWithID(updatedVersion.value.course.instructorID))) {
        printf("ERROR: Update failed. There is no instructor with the ID: %d\n", updatedVersion.value.course.instructorID);
    } else {
        int difference = 0;
        if (printMessage) {
            if (itemToBeUpdated.type == CourseType) {
                Item item = getItem(itemToBeUpdated);
                difference = updatedVersion.value.course.credit - item.value.course.credit;
                updatedVersion.value.course.quota.registered = item.value.course.quota.registered; freeItem(item);
            } else if (itemToBeUpdated.type == StudentType) {
                Item item = getItem(itemToBeUpdated);
                updatedVersion.value.student.numberOfCoursesRegistered = item.value.student.numberOfCoursesRegistered;
                updatedVersion.value.student.numberOfCreditsTaken = item.value.student.numberOfCreditsTaken; freeItem(item);
            }
        }
        // Remove old item from database, add new item to the database
        removeItemAsAPartOfUpdateProcess(itemToBeUpdated); addItemAsAPartOfUpdateProcess(updatedVersion);
        if (printMessage) { printf("Updated succesfully!\n"); }
        
        // If unique identifier has changed for an item...
        if (uniqueIdentifierHasChanged) {
            switch (updatedVersion.type) {
                case InstructorType:
                    // If instructor's ID has changed, then reflect that change to course records.
                    updateCoursesAfterInstructorIDChange(itemToBeUpdated, updatedVersion); break;
                case CourseType:
                    // If course's course code has changed, then reflect that change to registration records.
                    updateRegistrationsAfterCourseOrStudentsUniqueKeyHasChanged(itemToBeUpdated, updatedVersion); break;
                case StudentType:
                    // If student's student number has changed, then reflect that change to registration records.
                    updateRegistrationsAfterCourseOrStudentsUniqueKeyHasChanged(itemToBeUpdated, updatedVersion); break;
                case RegistrationType: break; // There is nothing to update about registration records.
            }
        }
        // If updated item is a Course item, and this course's credit has changed, then reflect that change to student records.
        if (itemToBeUpdated.type == CourseType && (itemToBeUpdated.value.course.credit != updatedVersion.value.course.credit)) {
            updateStudentsCreditIfCoursesCreditHasChanged(difference, updatedVersion);
        }
    }
}

/* If 'updateItemBase' function should print success message, then its 'printMessage' parameter
 should be false, otherwise it should be true. Functions below are just convenience functions for calling
 'updateItemBase' function. */

void updateItem(Item itemToBeUpdated, Item updatedVersion) {
    updateItemBase(itemToBeUpdated, updatedVersion, true);
}

void updateItemSilently(Item itemToBeUpdated, Item updatedVersion) {
    updateItemBase(itemToBeUpdated, updatedVersion, false);
}

// MARK: - GENERIC ITERATOR FUNCTION

/*
 'ItemIterator' function is at the heart of many functionality.
 Actually, all of the functionality below, is implemented using this function.
 
 It takes three arguments:
 - type: This parameter indicates which database file should be iterated on, and the type of the return value if needed.
 - aimFunction: This is the function that is applied in every iteration.
 - aimItem: This is passed as second parameter for aimFunction.
 
 Basic strategy works like this: Assume we want to print every student that registered
 for a course. Then we should call iterator with 'type' value of 'RegistarionType' because
 we keep record of students that registered for some course in Registrations file.
 On every iteration, we are going to have a decoded Registration item.
 'aimItem' should be the course that we want to print every student that registered for it.
 And 'aimFunction' takes these two as its parameters, and does the following:
 - Look whether decoded Registration item's course code is same with the aimItem's course code.
 - If that's true, then print students information.
 - If not, do nothing.
 And 'aimFunction' returns 'OptionalItem'. But if we have an operation that doesn't need to return a value--like the one above,
 we don't need to return any value for printing student information--'aimFunction' should return an 'OptionalItem' value that has
 no value--its 'hasValue' bool should be false; otherwise, if 'aimFunction' returns an OptionalItem that has a value, then iteration stops.
 
 With passing appropriate 'aimFunction' and appropriate 'aimItem' parameter values, we can make various types of operations
 with the same strategy, for example listing every course given by a specific instructor, listing every course that a specific student registered for.
 
 This function also can be used to get an actual 'Item' value--and is actually used in 'getItem' and 'itemIsInDatabase' functions.
 If we need to return a value, then 'aimFunction' should set its 'hasValue' bool to true, this will cause 'ItemIterator' to return that 'OptionalItem'.
 */

void prepareForIteration(ItemType type, char *fileName, Item(**decodingFunction)(FILE*)) {
    getFileNameForType(type, fileName);
    switch (type) {
        case InstructorType: *decodingFunction = readInstructorFromFile; return;
        case CourseType: *decodingFunction = readCourseFromFile; return;
        case StudentType: *decodingFunction = readStudentFromFile; return;
        case RegistrationType: *decodingFunction = readRegistrationFromFile; return;
    }
}

OptionalItem ItemIterator(ItemType type, OptionalItem(*aimFunction)(Item, Item), Item aimItem) {
    char *fileName = malloc(sizeof(char)*255);
    if (fileName == NULL) { printf("Couldn't allocate memory in 'ItemIterator' function.\n"); exit(1); }
    Item(*decodingFunction)(FILE*) = readInstructorFromFile;
    prepareForIteration(type, fileName, &decodingFunction);
    OptionalItem optionalItem; optionalItem.hasValue = false;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) { return optionalItem; }
    while (getc(file) != EOF) {
        fseek(file, ftell(file)-1, SEEK_SET);
        Item item = decodingFunction(file);
        optionalItem = aimFunction(item, aimItem);
        optionalItem.item.type = type;
        if (optionalItem.hasValue) {
            free(fileName); fclose(file); return optionalItem;
        }
        freeItem(item);
    }
    free(fileName); fclose(file);
    return optionalItem;
}

// MARK: - ITEM QUERY && GETTING ITEM FROM DATABASE && ITERATIVE REMOVALS && ITERATIVE UPDATES

bool itemIsInDatabase(Item item) {
    /* Returns whether an 'item' is in database or not. If ItemIterator returns an 'OptionalItem'
     that has its 'hasValue' bool set to true, then item is in database, otherwise it's not. */
    OptionalItem optionalItem = ItemIterator(item.type, query, item);
    if (optionalItem.hasValue) { freeItem(optionalItem.item); }
    return optionalItem.hasValue;
}

Item getItem(Item item) {
    /* This function is called with artificial instances, i.e. instances that has only unique key
     value but no other values, and returns the real instance, i.e. the instance that has all values,
     not only unique key value, if it is in database. If instance with unique key is not in database,
     then artificial instance itself will be returned. */
    OptionalItem optionalItem = ItemIterator(item.type, query, item);
    if (!optionalItem.hasValue) { return item; }
    return optionalItem.item;
}

// MARK: Helper functions for detecting whether an Item is in database or not

bool conditionForQuery(Item decodedItem, Item queriedItem) {
    // Conditions for detecting equality between items, depending on type of the items.
    switch (decodedItem.type) {
        case InstructorType: return decodedItem.value.instructor.ID == queriedItem.value.instructor.ID;
        case CourseType: return (strcmp(decodedItem.value.course.code, queriedItem.value.course.code) == 0);
        case StudentType: return decodedItem.value.student.studentNumber == queriedItem.value.student.studentNumber;
        case RegistrationType:
            return
            (decodedItem.value.registration.stillRegistered
             && decodedItem.value.registration.ID == queriedItem.value.registration.ID)
            ||
            (queriedItem.value.registration.courseCode != NULL
             && decodedItem.value.registration.stillRegistered
             && decodedItem.value.registration.studentNumber == queriedItem.value.registration.studentNumber
             && (strcmp(decodedItem.value.registration.courseCode, queriedItem.value.registration.courseCode) == 0));
    }
}

OptionalItem query(Item decodedItem, Item queriedItem) {
    /* This functions returns an OptionalItem, if conditionForQuery is met for decoded item
     and queried item, then optionalItem has a value, if not optionalItem has no value. */
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (conditionForQuery(decodedItem, queriedItem)) {
        optionalItem.hasValue = true;
        optionalItem.item = decodedItem;
        return optionalItem;
    }
    return optionalItem;
}

// MARK: Remove registrations after student or course removal

bool conditionForRemoval(Item registration, Item courseOrStudent) {
    // Helper function for indicating whether registration record's course code or student number matched with the value contained in 'courseOrStudent' parameter
    if (courseOrStudent.type == CourseType) { return strcmp(registration.value.registration.courseCode, courseOrStudent.value.course.code) == 0; }
    else { return registration.value.registration.studentNumber == courseOrStudent.value.student.studentNumber; }
}

OptionalItem registrationCourseOrStudentRemoval(Item registration, Item courseOrStudent) {
    // If conditionForRemoval is met, and registration is an active registration, then return that registration item in OptionalItem.
    // This will be the 'aimFunction' for 'ItemIterator' function, for invalidating all registrations associated with 'courseOrStudent'.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (conditionForRemoval(registration, courseOrStudent)) {
        if (registration.value.registration.stillRegistered) {
            optionalItem.hasValue = true; optionalItem.item = registration;
        }
    }
    return optionalItem;
}

void invalidateRegistrationsAfterCourseOrStudentRemoval(Item courseOrStudent, int credit) {
    // Iterate over the Registrations file, if matching record found, then remove record.
    int count = getRecordCountOfAFile(RegistrationType);
    for (int i = 0; i < count; i++) {
        OptionalItem opt = ItemIterator(RegistrationType, registrationCourseOrStudentRemoval, courseOrStudent);
        if (opt.hasValue) {
            Item course = getItem(wrapCourseWithCode(opt.item.value.registration.courseCode));
            credit = (credit < 0) ? course.value.course.credit : credit;
            removeItemAsAPartOfUpdateProcess(wrapRegistrationsWithID(opt.item.value.registration.ID));
            if (courseOrStudent.type == CourseType) {
                updateStudentsCreditStatus(opt.item.value.registration.studentNumber, false, credit, true);
            } else {
                updateCourseQuota(opt.item.value.registration.courseCode, false); freeItem(course);
            }
            freeItem(opt.item);
        }
    }
}

// MARK: Remove courses given by instructor with ID

OptionalItem courseInstructorRemoval(Item courseItem, Item instructor) {
    // If instructorID's matched, then return that course item in an OptionalItem.
    // This will be the 'aimFunction' for 'ItemIterator' function, for removaing all courses given by 'instructor'.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (courseItem.value.course.instructorID == instructor.value.instructor.ID) {
        optionalItem.hasValue = true; optionalItem.item = courseItem;
    }
    return optionalItem;
}

void removeCoursesGivenByInstructor(Item instructorItem) {
    // Iterate over the Courses file, if record's instructor ID is matched with instructor's ID, then first invalidate registrations associated with that course, and finally remove course from database.
    int count = getRecordCountOfAFile(InstructorType);
    for (int i = 0; i < count; i++) {
        OptionalItem opt = ItemIterator(CourseType, courseInstructorRemoval, instructorItem);
        if (opt.hasValue) {
            invalidateRegistrationsAfterCourseOrStudentRemoval(opt.item, -1);
            removeItemAsAPartOfUpdateProcess(opt.item);
            freeItem(opt.item);
        }
    }
}

// MARK: Update registrations after student or course unique identifier change

void updateRegistrationsAfterCourseOrStudentsUniqueKeyHasChanged(Item courseOrStudentToRemove, Item updatedVersion) {
    // Iterate over the Registrations file, if it is a matching record then update record.
    int count = getRecordCountOfAFile(RegistrationType);
    for (int i = 0; i < count; i++) {
        OptionalItem opt = ItemIterator(RegistrationType, registrationCourseOrStudentRemoval, courseOrStudentToRemove);
        if (opt.hasValue) {
            removeItemAsAPartOfUpdateProcess(wrapRegistrationsWithID(opt.item.value.registration.ID));
            /* Since we already removed registration (temporarily), and know that no credit information changed,
             we can register student without caring about the actual number of courses limit or credit limit.
             That's why 'registerStudentForCourse' function called with value 1000, it's a lot bigger than any realizable limit,
             and won't prevent the registration of student for course. */
            if (courseOrStudentToRemove.type == CourseType) {
                registerStudentForCourseAsAPartOfUpdateProcess(updatedVersion.value.course.code, opt.item.value.registration.studentNumber, 1000, 1000);
            } else {
                registerStudentForCourseAsAPartOfUpdateProcess(opt.item.value.registration.courseCode, updatedVersion.value.student.studentNumber, 1000, 1000);
            }
            freeItem(opt.item);
        }
    }
}

// MARK: Update courses given by specific instructor after instructor's ID has changed

void updateCoursesAfterInstructorIDChange(Item instructorItem, Item updatedInstructor) {
    // Iterate over the Courses file, if record's 'instructorID' is matched with instructor's 'ID', then update course item.
    int count = getRecordCountOfAFile(InstructorType);
    for (int i = 0; i < count; i++) {
        OptionalItem opt = ItemIterator(CourseType, courseInstructorRemoval, instructorItem);
        if (opt.hasValue) {
            Item updatedCourse = opt.item;
            updatedCourse.value.course.instructorID = updatedInstructor.value.instructor.ID;
            updateItemSilently(opt.item, updatedCourse);
            freeItem(opt.item);
        }
    }
}

// MARK: Update student's credit status if course's credit has changed

OptionalItem updateStudentCredit(Item registration, Item course) {
    // This will be the 'aimFunction' for 'ItemIterator' function, updating every student's credit info, that is registered for the 'course'.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (registration.value.registration.stillRegistered &&
        strcmp(registration.value.registration.courseCode, course.value.course.code) == 0) {
        int creditDifference = course.value.course.credit;
        bool creditIncremented = creditDifference > 0;
        updateStudentsCreditStatus(registration.value.registration.studentNumber, creditIncremented, abs(creditDifference), false);
    }
    return optionalItem;
}

void updateStudentsCreditIfCoursesCreditHasChanged(int creditDifference, Item course) {
    // Iterate over the Courses file, if record's 'instructorID' is matched with instructor's 'ID', then update course item.
    Course trickyCourse = { course.value.course.code, "", creditDifference };
    ItemIterator(RegistrationType, updateStudentCredit, wrapCourse(trickyCourse));
}

// MARK: - LISTING FUNCTIONS

// MARK: List courses given by instructor with ID

OptionalItem courseInstructor(Item course, Item instructor) {
    // This will be the 'aimFunction' for 'ItemIterator' function, for listing all courses given by 'instructor'.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (course.value.course.instructorID == instructor.value.instructor.ID) { printItem(course); }
    return optionalItem;
}

void listCoursesGivenByInstructor(Item instructor) {
    if (!itemIsInDatabase(instructor)) { printf("There is no instructor with ID: %d", instructor.value.instructor.ID); return; }
    Instructor instructorRecord = getItem(instructor).value.instructor;
    printf("\nHere is all the courses given by instructor %s %s %s:\n\n", instructorRecord.title, instructorRecord.name, instructorRecord.surname);
    freeItem(wrapInstructor(instructorRecord));
    ItemIterator(CourseType, courseInstructor, instructor);
}

// MARK: List courses registered by student

OptionalItem registrationStudent(Item registration, Item student) {
    // This will be the 'aimFunction' for 'ItemIterator' function, for listing every course that 'student' is registered for.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (registration.value.registration.stillRegistered &&
        registration.value.registration.studentNumber == student.value.student.studentNumber) {
        Item course = getItem(wrapCourseWithCode(registration.value.registration.courseCode));
        printItem(course); freeItem(course);
    }
    return optionalItem;
}

void listCoursesRegisteredByStudent(Item student) {
    if (!itemIsInDatabase(student)) { printf("There is no student with student number: %d.\n", student.value.student.studentNumber); return; }
    Student studentRecord = getItem(student).value.student;
    printf("\nHere is the list of all of the courses that '%s %s' is registered for: \n\n", studentRecord.name, studentRecord.surname);
    freeItem(wrapStudent(studentRecord));
    ItemIterator(RegistrationType, registrationStudent, student);
}

// MARK: List students registered for course

OptionalItem registrationCourse(Item registration, Item course) {
    // This will be the 'aimFunction' for 'ItemIterator' function, for listing every student that is registered for 'course'.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    if (registration.value.registration.stillRegistered &&
        strcmp(registration.value.registration.courseCode, course.value.course.code) == 0) {
        Item student = getItem(wrapStudentWithStudentNumber((registration.value.registration.studentNumber)));
        printItem(student); freeItem(student);
    }
    return optionalItem;
}

void listStudentsRegisteredForCourse(Item course) {
    if (!itemIsInDatabase(course)) { printf("There is no course with code: %s.\n", course.value.course.code); return; }
    Course courseRecord = getItem(course).value.course;
    printf("\nHere is the list of all the students that is registered for the course %s %s: \n\n", courseRecord.code, courseRecord.name);
    freeItem(wrapCourse(courseRecord));
    ItemIterator(RegistrationType, registrationCourse, course);
}

// MARK: Print course's students list to a file.

OptionalItem registrationCourseEncode(Item registration, Item courseItem) {
    // This will be the 'aimFunction' for 'ItemIterator' function, for printing student list of a given class.
    OptionalItem optionalItem; optionalItem.hasValue = false;
    char *fileName = malloc(sizeof(char)*255);
    if (fileName == NULL) { printf("Couldn't allocate memory in 'registrationCourseEncode' function.\n"); exit(1); }
    sprintf(fileName, "%s_STUDENTSLIST.txt", courseItem.value.course.code);
    FILE *studentList = fopen(fileName, "a");
    if (studentList == NULL) { free(fileName); return optionalItem; }
    if (registration.value.registration.stillRegistered &&
        strcmp(registration.value.registration.courseCode, courseItem.value.course.code) == 0) {
        Student student = getItem(wrapStudentWithStudentNumber((registration.value.registration.studentNumber))).value.student;
        fprintf(studentList, "- %s %s\n", student.name, student.surname); freeItem(wrapStudent(student));
    }
    fclose(studentList); free(fileName); return optionalItem;
}

void printStudentListOfACourseToAFile(Item courseItem) {
    Course course = getItem(courseItem).value.course;
    char *fileName = malloc(sizeof(char)*255);
    if (fileName == NULL) { printf("Couldn't allocate memory in 'printStudentListOfACourseToAFile' function.\n"); exit(1); }
    sprintf(fileName, "%s_STUDENTSLIST.txt", course.code);
    remove(fileName);
    FILE *studentList = fopen(fileName, "w");
    if (studentList == NULL) { free(fileName); return; }
    fprintf(studentList, "%s %s Course Students List\n\n", course.code, course.name); fclose(studentList);
    ItemIterator(RegistrationType, registrationCourseEncode, courseItem);
    printf("\nSuccessfully printed the students list to file %s_STUDENTLIST.txt\n", course.code);
    free(fileName); freeItem(wrapCourse(course));
}

void applyTests(void);
Item createItemOfType(ItemType type, bool forUpdate);
void deleteItemOfType(ItemType type);
void updateItemOfType(ItemType type);
void menu(void);

int main(int argc, const char * argv[]) {
//    applyTests();
    menu();
    return 0;
}

// MARK: Menu

Item createItemOfType(ItemType type, bool forUpdate) {
    char *buffer = malloc(sizeof(char)*255);
    char *memoryError = "Couldn't allocate memory in 'createItemOfType'.\n";
    if (buffer == NULL) { printf("%s", memoryError); exit(1); }
    Item item; item.type = InstructorType;
    if (type == InstructorType) {
        Instructor instructor;
        instructor.name = malloc(sizeof(char)*255);
        instructor.surname = malloc(sizeof(char)*255);
        instructor.title = malloc(sizeof(char)*255);
        if (instructor.name == NULL || instructor.surname == NULL || instructor.title == NULL) { printf("%s", memoryError); exit(1); }
        printf("\nEnter the ID for the instructor: ");
        scanf("%d", &instructor.ID); getchar();
        printf("Enter the name of the instructor: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%[^\n]", instructor.name);
        printf("Enter the surname of the instructor: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%[^\n]", instructor.surname);
        printf("Enter the title of the instructor: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%[^\n]", instructor.title); printf("\n");
        item = wrapInstructor(instructor);
        if (!forUpdate) { addItem(wrapInstructor(instructor)); freeItem(wrapInstructor(instructor)); }
    } else if (type == CourseType) {
        Course course;
        course.code = malloc(sizeof(char)*255);
        course.name = malloc(sizeof(char)*255);
        if (course.code == NULL || course.name == NULL) { printf("%s", memoryError); exit(1); }
        Quota quota = { 0, 0 };
        printf("\nEnter the code for the course: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%s\n", course.code);
        printf("Enter the name of the course: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%[^\n]", course.name);
        printf("Enter the quota for the course: ");
        scanf("%d", &quota.total); getchar();
        printf("Enter the credit of the course: ");
        scanf("%d", &course.credit); getchar();
        printf("Enter the instructor ID for the course: ");
        scanf("%d", &course.instructorID); getchar(); course.quota = quota; printf("\n");
        item = wrapCourse(course);
        if (!forUpdate) { addItem(wrapCourse(course)); freeItem(wrapCourse(course)); }
    } else if (type == StudentType) {
        Student student;
        student.name = malloc(sizeof(char)*255);
        student.surname = malloc(sizeof(char)*255);
        if (student.name == NULL || student.surname == NULL) { printf("%s", memoryError); exit(1); }
        printf("\nEnter student number for student: ");
        scanf("%d", &student.studentNumber); getchar();
        printf("Enter the name of the student: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%[^\n]", student.name);
        printf("Enter the surname of the student: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%[^\n]", student.surname);
        student.numberOfCoursesRegistered = 0;
        student.numberOfCreditsTaken = 0; printf("\n");
        item = wrapStudent(student);
        if (!forUpdate) { addItem(wrapStudent(student)); freeItem(wrapStudent(student)); }
    }
    return item;
}

void createRegistration(int *MAX_COUNT, int *MAX_CREDIT) {
    if (*MAX_COUNT < 0) {
        printf("\nEnter the maximum number of courses that a student can register in a quarter: ");
        scanf("%d", MAX_COUNT); getchar();
        printf("Enter the maximum number of credits that a student can use in a quarter: ");
        scanf("%d", MAX_CREDIT); getchar();
    }
    int studentNumber = 0;
    char *buffer = malloc(sizeof(char)*255);
    char *courseCode = malloc(sizeof(char)*255);
    if (buffer == NULL || courseCode == NULL) { printf("Couldn't allocate memory in 'createRegistration' function.\n"); exit(1); }
    printf("\nEnter the student number of the student: ");
    scanf("%d", &studentNumber); getchar();
    printf("Enter the course code: ");
    fgets(buffer, 255, stdin);
    sscanf(buffer, "%s\n", courseCode); printf("\n");
    registerStudentForCourse(courseCode, studentNumber, *MAX_COUNT, *MAX_CREDIT);
    free(courseCode); free(buffer);
}

void deleteItemOfType(ItemType type) {
    char *buffer = malloc(sizeof(char)*255);
    char *courseCode = malloc(sizeof(char)*255);
    if (buffer == NULL || courseCode == NULL) { printf("Couldn't allocate memory in 'deleteItemOfType' function.\n"); exit(1); }
    if (type == InstructorType) {
        int ID = 0;
        printf("CAUTION: All of the courses taught by this instructor, and all the registrations made for\n"
               "that courses will also get removed from database.\n");
        printf("Enter the ID of the instructor you want to delete: ");
        scanf("%d", &ID); getchar();  printf("\n");
        removeItem(wrapInstructorWithID(ID));
    } else if (type == CourseType) {
        printf("CAUTION: All of the registrations made for the course, will also get removed from database.\n");
        printf("Enter the code of the course you want to delete: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%s\n", courseCode); printf("\n");
        removeItem(wrapCourseWithCode(courseCode));
    } else if (type == StudentType) {
        int studentNumber = 0;
        printf("CAUTION: All of the registration records of the student, will also get removed from database.\n");
        printf("Enter the student number of the student you want to delete: ");
        scanf("%d", &studentNumber); getchar(); printf("\n");
        removeItem(wrapStudentWithStudentNumber(studentNumber));
    } else if (type == RegistrationType) {
        int studentNumber = 0;
        printf("Enter the student number for the registration you want to delete: ");
        scanf("%d", &studentNumber); getchar();
        listCoursesRegisteredByStudent(wrapStudentWithStudentNumber(studentNumber));
        printf("Enter the course code for removing registration: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%s\n", courseCode); printf("\n");
        removeItem(wrapRegistrationWithStudentNumberAndCourseCode(studentNumber, courseCode));
    }
    free(buffer); free(courseCode);
}

void updateItemOfType(ItemType type) {
    char *buffer = malloc(sizeof(char)*255);
    if (buffer == NULL) { printf("Couldn't allocate memory in 'updateItemOfType' function.\n"); exit(1); }
    if (type == InstructorType) {
        int ID = 0;
        printf("Enter the ID of the instructor you want to update: ");
        scanf("%d", &ID); getchar();
        printf("\nEnter values for the updated version of the instructor:\n");
        Item updatedInstructor = createItemOfType(InstructorType, true); printf("\n");
        updateItem(wrapInstructorWithID(ID), updatedInstructor);
        freeItem(updatedInstructor);
    } else if (type == CourseType) {
        char *courseCode = malloc(sizeof(char)*255);
        if (courseCode == NULL) { printf("Couldn't allocate memory in 'updateItemOfType' function.\n"); exit(1); }
        printf("Enter the code of the course that you want to update: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%s\n", courseCode);
        printf("\nEnter values for the updated version of the course:\n");
        Item updatedCourse = createItemOfType(CourseType, true); printf("\n");
        updateItem(wrapCourseWithCode(courseCode), updatedCourse);
        freeItem(updatedCourse); free(courseCode);
    } else if (type == StudentType) {
        int studentNumber = 0;
        printf("Enter the student number of the student that you want to update: ");
        scanf("%d", &studentNumber); getchar();
        printf("\nEnter values for the updated version of the student:\n");
        Item updatedStudent = createItemOfType(StudentType, true); printf("\n");
        updateItem(wrapStudentWithStudentNumber(studentNumber), updatedStudent);
        freeItem(updatedStudent);
    }
    free(buffer);
}

void list(int mode) {
    char *buffer = malloc(sizeof(char)*255);
    char *courseCode = malloc(sizeof(char)*255);
    if (buffer == NULL || courseCode == NULL) { printf("Couldn't allocate memory in 'list' function.\n"); exit(1); }
    if (mode == 1) {
        int ID = 0;
        printf("Enter the ID of the instructor: ");
        scanf("%d", &ID); getchar(); printf("\n");
        listCoursesGivenByInstructor(wrapInstructorWithID(ID));
    } else if (mode == 2) {
        printf("Enter the code of the course: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%s\n", courseCode); printf("\n");
        listStudentsRegisteredForCourse(wrapCourseWithCode(courseCode));
    } else if (mode == 3) {
        int studentNumber = 0;
        printf("Enter the student number: ");
        scanf("%d", &studentNumber); getchar(); printf("\n");
        listCoursesRegisteredByStudent(wrapStudentWithStudentNumber(studentNumber));
    } else if (mode == 4) {
        list(1);
        printf("Enter the code of the course: ");
        fgets(buffer, 255, stdin);
        sscanf(buffer, "%s\n", courseCode); printf("\n");
        printStudentListOfACourseToAFile(wrapCourseWithCode(courseCode));
    } else {
        printf("Invalid selection.\n");
    }
    free(buffer); free(courseCode);
}

void menu() {
    int option = -1;
    int MAX_COUNT = -1; int MAX_CREDIT = -1;
    printf("Select the operation you want to perform:\n");
    printf("Enter '1', for adding Instructor, Course, Student or Registration record to the database.\n");
    printf("Enter '2', for removing Instructor, Course, Student or Registration record from database.\n");
    printf("Enter '3', for updating Instructor, Course, Student or Registration record from database.\n");
    printf("Enter '4', for listing operations.\n");
    printf("Enter '5', for terminating the program.\n");
    while (true) {
        printf("\n('1' -> Add, '2' -> Remove, '3' -> Update, '4' -> List, '5' -> Exit)\n");
        printf("Please enter a valid operation number: ");
        scanf("%d", &option); getchar();
        if (option == 1) {
            printf("\nEnter '1', for adding Instructor.\n");
            printf("Enter '2', for adding Course.\n");
            printf("Enter '3', for adding Student.\n");
            printf("Enter '4', for adding Registration.\n\n");
            printf("Enter value: ");
            scanf("%d", &option); getchar();
            if (option == 1) { createItemOfType(InstructorType, false); }
            else if (option == 2) { createItemOfType(CourseType, false); }
            else if (option == 3) { createItemOfType(StudentType, false); }
            else if (option == 4) { createRegistration(&MAX_COUNT, &MAX_CREDIT); }
            else { printf("Invalid operation number.\n"); }
        } else if (option == 2) {
            printf("\nEnter '1', for removing Instructor.\n");
            printf("Enter '2', for removing Course.\n");
            printf("Enter '3', for removing Student.\n");
            printf("Enter '4', for removing Registration.\n\n");
            printf("Enter value: ");
            scanf("%d", &option); getchar();
            if (option == 1) { deleteItemOfType(InstructorType); }
            else if (option == 2) { deleteItemOfType(CourseType); }
            else if (option == 3) { deleteItemOfType(StudentType); }
            else if (option == 4) { deleteItemOfType(RegistrationType); }
            else { printf("Invalid operation number.\n"); }
        } else if (option == 3) {
            printf("\nEnter '1', for updating Instructor.\n");
            printf("Enter '2', for updating Course.\n");
            printf("Enter '3', for updating Student.\n\n");
            printf("Enter value: ");
            scanf("%d", &option); getchar();
            if (option == 1) { updateItemOfType(InstructorType); }
            else if (option == 2) { updateItemOfType(CourseType); }
            else if (option == 3) { updateItemOfType(StudentType); }
            else { printf("Invalid operation number.\n"); }
        } else if (option == 4) {
            printf("\nEnter '1', for listing courses given by an specific instructor.\n");
            printf("Enter '2', for listing students that is registered for specific course.\n");
            printf("Enter '3', for listing all courses that specific student is registered for.\n");
            printf("Enter '4', for printing student list of a course taught by specific instructor, to a file.\n\n");
            printf("Enter value: ");
            scanf("%d", &option); getchar();
            list(option);
        } else if (option == 5) {
            printf("Program terminated.\n"); break;
        } else {
            printf("Invalid operation number.\n");
        }
        printf("\n");
    }
}

// MARK: Tests

void applyTests() {
    char c = 0;
    printf("!!!!!!!!!! ALL FILES WILL BE REMOVED TO APPLY TESTS !!!!!!!!!!\n");
    printf("DO YOU WANT TO CONTINUE? Enter 'y' for yes, 'n' for no: ");
    scanf("%c", &c);
    if (c != 'y') { printf("Cancelled the tests.\n"); exit(1); }
    
    remove("Instructors.txt"); remove("Courses.txt"); remove("Students.txt"); remove("Registrations.txt"); printf("\n");
    
    printf("################################################################################## ADDING ITEMS TESTS #########################################################################################\n\n");
    
    printf("######################################## TEST FOR ADDING INSTRUCTORS TO THE DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### 5 INSTRUCTORS SHOULD GET ADDED TO DATABASE SUCCESSFULLY.\n");
    printf("######### HERE IS THE RESULTS OF OUR ATTEMPT TO ADD 5 INSTRUCTORS TO THE DATABASE:\n");
    // Creating 'Instructor' instances to use in tests.
    Instructor paulHegarty = { 1, "Paul", "Hegarty", "Instructor" };
    Instructor johnGuttag = { 2, "John", "Guttag", "Professor" };
    Instructor ericGrimson = { 3, "Eric", "Grimson", "Professor" };
    Instructor davidMalan = { 4, "David", "Malan", "Professor" };
    Instructor casperHare = { 5, "Casper", "Hare", "Professor" };
    addItem(wrapInstructor(paulHegarty)); addItem(wrapInstructor(johnGuttag)); addItem(wrapInstructor(ericGrimson));
    addItem(wrapInstructor(davidMalan)); addItem(wrapInstructor(casperHare));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING COURSES TO THE DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### 6 COURSES SHOULD GET ADDED TO DATABASE SUCCESSFULLY.\n");
    printf("######### HERE IS THE RESULTS OF OUR ATTEMPT TO ADD 6 COURSES TO THE DATABASE:\n");
    // Creating 'Course' instances to use in tests.
    Course cs193p = { "CS193P", "Developing Apps For iOS", 4, {0, 80}, 1 };
    Course mit60001 = { "6.0001", "Introduction to Computer Science and Programming in Python", 2, {0, 250}, 2 };
    Course mit60002 = { "6.0002", "Introduction to Computational Thinking and Data Science", 2, {0, 250}, 3 };
    Course cs50 = { "CS50", "CS50's Introduction to Computer Science", 4, {0, 300}, 4 };
    Course cs50ut = { "CS50UT", "CS50's Understanding Technology", 3, {0, 9}, 4 };
    Course mit2400 = { "24.00", "Introduction to Philosophy: God, Knowledge and Consciousness", 2, {0, 100}, 5 };
    addItem(wrapCourse(cs193p)); addItem(wrapCourse(mit60001)); addItem(wrapCourse(mit60002));
    addItem(wrapCourse(cs50)); addItem(wrapCourse(cs50ut)); addItem(wrapCourse(mit2400));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING STUDENTS TO THE DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### 9 STUDENTS SHOULD GET ADDED TO DATABASE SUCCESSFULLY.\n");
    printf("######### HERE IS THE RESULTS OF OUR ATTEMPT TO ADD 9 STUDENTS TO THE DATABASE:\n");
    // Creating 'Student' instances to use in tests.
    Student socrates = { 0, "Socrates", "of Athens", 0, 0 };
    Student plato = { 1, "Platon", "of Athens", 0, 0 };
    Student aristotle = { 2, "Aristoteles", "of Athens", 0, 0 };
    Student alexander = { 356, "Alexander", "the Great", 0, 0 };
    Student michelangelo = { 1475, "Michelangelo", "Buonarroti", 0, 0 };
    Student raffaello = { 1482, "Raffaello", "Sanzio", 0, 0 };
    Student descartes = { 1596, "Rene", "Descartes", 0, 0 };
    Student spinoza = { 1632, "Baruch", "Spinoza", 0, 0 };
    Student denis = { 1713, "Denis", "Diderot", 0, 0 };
    addItem(wrapStudent(socrates)); addItem(wrapStudent(plato)); addItem(wrapStudent(aristotle));
    addItem(wrapStudent(alexander)); addItem(wrapStudent(michelangelo)); addItem(wrapStudent(raffaello));
    addItem(wrapStudent(descartes)); addItem(wrapStudent(spinoza)); addItem(wrapStudent(denis));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING REGISTRATIONS TO THE DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### EVERY STUDENT, IS GOING TO REGISTER FOR EVERY COURSE AVAILABLE.\n");
    printf("######### THERE ARE 9 STUDENTS IN DATABASE AND 6 COURSES, SO WE SHOULD GET 54 REGISTRATIONS TOTAL.\n");
    printf("######### 54 REGISTRATIONS SHOULD GET ADDED TO THE DATABASE SUCCESSFULLY.\n");
    printf("######### HERE IS THE RESULTS OF OUR ATTEMPT TO ADD 54 REGISTRATIONS TO THE DATABASE:\n");
    char *allCourses[] = { mit2400.code, cs193p.code, cs50.code, cs50ut.code, mit60001.code, mit60002.code };
    int allStudents[] = { socrates.studentNumber, plato.studentNumber, aristotle.studentNumber,
        alexander.studentNumber, michelangelo.studentNumber, raffaello.studentNumber,
        descartes.studentNumber, spinoza.studentNumber, denis.studentNumber };
    const int MAX_NUMBER_OF_COURSES = 10;
    const int MAX_NUMBER_OF_CREDITS = 25;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 9; j++) {
            registerStudentForCourse(allCourses[i], allStudents[j], MAX_NUMBER_OF_COURSES, MAX_NUMBER_OF_CREDITS);
        }
    }
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING AN INSTRUCTOR WITH DUPLICATE ID (SHOULD FAIL) ##################################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD AN INSTRUCTOR WITH DUPLICATE ID.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD AN INSTRUCTOR WITH DUPLICATE ID:\n");
    // Creating an 'Instructor' instance with duplicate ID.
    Instructor duplicateInstructor = { 1, "Duplicate", "Instructor", "Instructor" };
    addItem(wrapInstructor(duplicateInstructor));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A COURSE WITH DUPLICATE COURSE CODE (SHOULD FAIL) ##################################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A COURSE WITH DUPLICATE COURSE CODE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A COURSE WITH DUPLICATE COURSE CODE:\n");
    // Creating a 'Course' instance with duplicate course code.
    Course duplicateCourse = { "CS193P", "Duplicate Course", 4, {0, 80}, 1 };
    addItem(wrapCourse(duplicateCourse));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A COURSE WITH INVALID INSTRUCTOR ID (SHOULD FAIL) ##################################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A COURSE WITH INVALID INSTRUCTOR ID.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A COURSE WITH INVALID INSTRUCTOR ID:\n");
    // Creating a 'Course' instance with invalid instructor ID.
    Course invalidInstructorIDCourse = { "INVALIDINSTRUCTOR", "Invalid Instructor Course", 4, {0, 80}, -1 };
    addItem(wrapCourse(invalidInstructorIDCourse));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A STUDENT WITH DUPLICATE STUDENT NUMBER (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A STUDENT WITH DUPLICATE STUDENT NUMBER.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A STUDENT WITH DUPLICATE STUDENT NUMBER:\n");
    // Creating a 'Student' instance with duplicate student number.
    Student duplicateStudent = { 0, "Duplicate", "Student", 0, 0 };
    addItem(wrapStudent(duplicateStudent));
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A REGISTRATION WITH INVALID COURSE CODE (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A REGISTRATION WITH INVALID COURSE CODE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A REGISTRATION WITH INVALID COURSE CODE:\n");
    registerStudentForCourse("INVALID", denis.studentNumber, MAX_NUMBER_OF_COURSES, MAX_NUMBER_OF_CREDITS);
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A REGISTRATION WITH INVALID STUDENT NUMBER (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A REGISTRATION WITH INVALID STUDENT NUMBER.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A REGISTRATION WITH INVALID STUDENT NUMBER:\n");
    registerStudentForCourse("CS193P", -1000, MAX_NUMBER_OF_COURSES, MAX_NUMBER_OF_CREDITS);
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A REGISTRATION WITH EXCEEDED NUMBER OF COURSES (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A REGISTRATION WITH EXCEEDED NUMBER OF COURSES.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A REGISTRATION WITH EXCEEDED NUMBER OF COURSES:\n");
    registerStudentForCourse("CS193P", denis.studentNumber, denis.numberOfCoursesRegistered, MAX_NUMBER_OF_CREDITS);
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A REGISTRATION WITH EXCEEDED CREDITS (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A REGISTRATION WITH EXCEEDED CREDITS.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A REGISTRATION WITH EXCEEDED CREDITS:\n");
    registerStudentForCourse("CS193P", denis.studentNumber, MAX_NUMBER_OF_COURSES, 17);
    printf("\n\n");
    
    printf("######################################## TEST FOR ADDING A REGISTRATION WITH EXCEEDED QUOTA (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO ADD A REGISTRATION WITH EXCEEDED QUOTA.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO ADD A REGISTRATION WITH EXCEEDED QUOTA:\n");
    // CS50UT course has a quota of 9, and 9 students are registered for it already, so there is no space available for a new student.
    registerStudentForCourse("CS50UT", denis.studentNumber, MAX_NUMBER_OF_COURSES, MAX_NUMBER_OF_CREDITS);
    printf("\n\n");
    
    printf("######################################## TEST FOR REGISTERING SAME COURSE TWICE (SHOULD FAIL) ########################################\n");
    printf("######### THIS SHOULD FAIL FOR TRYING TO REGISTER FOR A COURSE THAT STUDENT IS ALREADY REGISTERED FOR.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO REGISTER FOR THE SAME COURSE TWICE:\n");
    registerStudentForCourse(cs193p.code, denis.studentNumber, MAX_NUMBER_OF_COURSES, MAX_NUMBER_OF_CREDITS);
    printf("\n\n");
    
    printf("######################################## ALL TESTS ARE COMPLETED FOR ADDING ITEMS TO THE DATABASE ########################################\n");
    printf("######### YOU CAN CHECK THE DATABASE FILES.\n");
    printf("######### THERE SHOULD BE 5 INSTRUCTORS IN THE DATABASE.\n");
    printf("######### THERE SHOULD BE 6 COURSES IN THE DATABASE.\n");
    printf("######### THERE SHOULD BE 9 STUDENTS IN THE DATABASE.\n");
    printf("######### THERE SHOULD BE 54 REGISTRATIONS IN THE DATABASE.\n");
    printf("######### ALL STUDENTS SHOULD BE REGISTERED FOR 6 COURSES AND TAKEN 17 CREDITS.\n");
    printf("######### ALL COURSES SHOULD BE USING 9 OF THEIR QUOTA.\n");
    printf("\n\n");
    
    printf("AFTER CHECKING FILES, ENTER AN ENGLISH LETTER TO CONTINUE: ");
    scanf(" %c", &c);
    printf("\n\n");
    
    // MARK: Tests about removal of an 'Item'
    
    printf("################################################################################## REMOVAL TESTS #########################################################################################\n\n");
    
    printf("######################################## REMOVING AN INSTRUCTOR FROM DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### TEST FOR TRYING TO REMOVE THE INSTRUCTOR 'DAVID MALAN'\n");
    printf("######### WE SHOULD BE ABLE TO REMOVE 'DAVID MALAN' FROM DATABASE SUCCESSFULLY.\n");
    printf("######### ALSO ALL COURSES TAUGHT BY 'DAVID MALAN' SHOULD GET REMOVED.\n");
    printf("######### AND AS A RESULT OF THE REMOVAL OF THESE COURSES, ALL REGISTRATIONS MADE FOR THESE COURSES SHOULD GET INVALIDATED.\n");
    printf("######### ALSO RECORDS OF ALL STUDENTS THAT WERE REGISTERED FOR THESE COURSES SHOULD GET UPDATED.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO REMOVE 'DAVID MALAN' FROM DATABASE:\n");
    removeItem(wrapInstructor(davidMalan));
    printf("\n\n");
    
    printf("######################################## REMOVING COURSE FROM DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### WE SHOULD BE ABLE TO REMOVE THE COURSE 'INTRODUCTION TO COMPUTATIONAL THINKING AND DATA SCIENCE'\n");
    printf("######### ALSO, ALL REGISTRATIONS MADE FOR THAT COURSE SHOULD GET INVALIDATED.\n");
    printf("######### AS A RESULT OF THAT, RECORDS OF ALL OF THE STUDENTS THAT WERE REGISTERED FOR THIS COURSE SHOULD GET UPDATED.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO REMOVE THE COURSE 'INTRODUCTION TO COMPUTATIONAL THINKING AND DATA SCIENCE' FROM DATABASE:\n");
    removeItem(wrapCourse(mit60002));
    printf("\n\n");
    
    printf("######################################## REMOVING STUDENT FROM DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### WE SHOULD BE ABLE TO REMOVE THE STUDENT 'RENE DESCARTES'\n");
    printf("######### ALSO, ALL REGISTRATIONS THAT 'RENE DESCARTES' MADE SHOULD GET INVALIDATED.\n");
    printf("######### AS A RESULT OF THAT, QUOTA OF ALL OF THE COURSES THAT DESCARTES REGISTERED SHOULD GET UPDATED.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO REMOVE THE STUDENT 'RENE DESCARTES' FROM DATABASE:\n");
    removeItem(wrapStudent(descartes));
    printf("\n\n");
    
    printf("######################################## REMOVING (INVALIDATING) REGISTRATION FROM DATABASE (SHOULD SUCCEED) ########################################\n");
    printf("######### WE SHOULD BE ABLE TO INVALIDATE THE REGISTRATION OF 'MICHELANGELO' FOR THE COURSE 'INSTRODUCTION TO PHILOSOPHY'\n");
    printf("######### COURSE'S QUOTA AND MICHELANGELO'S CREDIT SHOULD ALSO GET UPDATED.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO REMOVE THE REGISTRATION OF 'MICHELANGELO' FOR THE COURSE 'INTRODUCTION TO PHILOSOPHY', FROM DATABASE:\n");
    removeItem(wrapRegistrationWithStudentNumberAndCourseCode(michelangelo.studentNumber, mit2400.code));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO REMOVE INSTRUCTOR THAT IS NOT IN DATABASE (SHOULD FAIL) ################################################\n");
    printf("######### THIS SHOULD FAIL BECAUSE INSTRUCTOR WE ARE TRYING TO REMOVE IS NOT IN DATABASE.\n");
    removeItem(wrapInstructor(davidMalan));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO REMOVE COURSE THAT IS NOT IN DATABASE (SHOULD FAIL) ################################################\n");
    printf("######### THIS SHOULD FAIL BECAUSE COURSE WE ARE TRYING TO REMOVE IS NOT IN DATABASE.\n");
    removeItem(wrapCourse(mit60002));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO REMOVE STUDENT THAT IS NOT IN DATABASE (SHOULD FAIL) ################################################\n");
    printf("######### THIS SHOULD FAIL BECAUSE STUDENT WE ARE TRYING TO REMOVE IS NOT IN DATABASE.\n");
    removeItem(wrapStudent(descartes));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO REMOVE REGISTRATION THAT IS NOT IN DATABASE (SHOULD FAIL) ################################################\n");
    printf("######### THIS SHOULD FAIL BECAUSE REGISTRATION WE ARE TRYING TO REMOVE IS NOT IN DATABASE.\n");
    removeItem(wrapRegistrationWithStudentNumberAndCourseCode(michelangelo.studentNumber, mit2400.code));
    printf("\n\n");
    
    printf("######################################## ALL TESTS ARE COMPLETED FOR REMOVING ITEMS FROM THE DATABASE ########################################\n");
    printf("######### YOU CAN CHECK THE RECORD FILES OF THE DATABASE.\n");
    printf("######### THERE SHOULD BE 4 INSTRUCTORS IN THE DATABASE. ('DAVID MALAN' GOT REMOVED.)\n");
    printf("######### THERE SHOULD BE 3 COURSES IN THE DATABASE. ('CS50', 'CS50UT', '6.0002' GOT REMOVED.)\n");
    printf("######### THERE SHOULD BE 8 STUDENTS IN THE DATABASE. ('RENE DESCARTES' GOT REMOVED.)\n");
    printf("######### THERE SHOULD BE 23 VALID (AND 54 TOTAL) REGISTRATIONS IN THE DATABASE.\n");
    printf("######### AFTER CHECK, PRESS ENTER TO CONTINUE TO TESTS FOR UPDATING ITEMS.\n\n");
    printf("AFTER CHECKING FILES, ENTER AN ENGLISH LETTER TO CONTINUE: ");
    scanf(" %c", &c);
    printf("\n\n");
    
    printf("################################################################################## UPDATE TESTS #########################################################################################\n\n");
    
    printf("######################################### UPDATING INSTRUCTOR'S TITLE (SHOULD SUCCEED) ################################################\n");
    printf("######### 'PAUL HEGARTY'S TITLE SHOULD GET UPDATED SUCCESSFULLY.\n");
    printf("######### IF WE CAN UPDATE TITLE SUCCESSFULLY, THAT MEANS WE CAN ALSO UPDATE ANY NON-UNIQUE VALUE (LIKE NAME AND SURNAME ETC.).\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'PAUL HEGARTY'S TITLE:\n");
    // Creating 'Paul Hegarty' instance with 'Professor' title.
    Instructor professorPaulHegarty = { 1, "Paul", "Hegarty", "Professor" };
    updateItem(wrapInstructor(paulHegarty), wrapInstructor(professorPaulHegarty));
    printf("\n\n");
    
    printf("######################################### UPDATING INSTRUCTOR'S UNIQUE ID (SHOULD SUCCEED) ################################################\n");
    // Creating 'Paul Hegarty' instance with different 'ID'.
    Instructor paulHegartyNewID = { 0, "Paul", "Hegarty", "Professor" };
    printf("######### WE SHOULD BE ABLE TO CHANGE INSTRUCTOR'S ID UNLESS, NEW ID IS ALREADY USED IN DATABASE BY OTHER INSTRUCTOR.\n");
    printf("######### AFTER CHANGING THAT, ALL COURSES TAUGHT BY INSTRUCTOR ALSO SHOULD GET UPDATED ACCORDING TO THE NEW ID.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'PAUL HEGARTY'S UNIQUE ID:\n");
    updateItem(wrapInstructor(paulHegarty), wrapInstructor(paulHegartyNewID));
    printf("\n\n");
    
    printf("######################################## UPDATING COURSE'S QUOTA (SHOULD SUCCEED) ########################################\n");
    // Creating a new 'Quota' instance with 'total' value of 30 (instead of 100).
    Quota newQuota = { getItem(wrapCourseWithCode("24.00")).value.course.quota.registered, 30 };
    // Creating a new 'Introduction to Philosophy' course with new 'quota' value.
    Course newQuota2400 = { "24.00", "Introduction to Philosophy: God, Knowledge and Consciousness", 2, newQuota, 5 };
    printf("######### WE SHOULD BE ABLE TO CHANGE COURSE'S QUOTA.\n");
    printf("######### IF WE CAN UPDATE QUOTA SUCCESSFULLY, THAT MEANS WE CAN ALSO UPDATE ANY NON-UNIQUE VALUE (LIKE NAME, INSTRUCTOR ID ETC.).\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'INTRODUCTION TO PHILISOPHY' COURSE'S QUOTA:\n");
    updateItem(getItem(wrapCourseWithCode("24.00")), wrapCourse(newQuota2400));
    printf("\n\n");
    
    printf("######################################## UPDATING COURSE'S CREDIT (SHOULD SUCCEED) ########################################\n");
    // Creating a new 'Introduction to Philosophy' course with new 'credit' value of 4 (instead of 2).
    Course newCredit2400 = { "24.00", "Introduction to Philosophy: God, Knowledge and Consciousness", 4, newQuota, 5 };
    printf("######### WE SHOULD BE ABLE TO CHANGE COURSE'S CREDIT.\n");
    printf("######### AFTER CHANGING THAT, CREDIT INFO OF ALL OF THE STUDENTS THAT REGISTERED FOR THE COURSE SHOULD GET UPDATED.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'INTRODUCTION TO PHILOSOPHY' COURSE'S CREDIT:\n");
    updateItem(getItem(wrapCourseWithCode("24.00")), wrapCourse(newCredit2400));
    printf("\n\n");
    
    printf("######################################## UPDATING COURSE'S CODE (SHOULD SUCCEED) ########################################\n");
    // Creating a new 'Introduction to Philosophy' course with new 'code' value.
    Course newCode2400 = { "24.00x", "Introduction to Philosophy: God, Knowledge and Consciousness", 4, newQuota, 5 };
    printf("######### WE SHOULD BE ABLE TO CHANGE COURSE'S CODE UNLESS, NEW CODE IS ALREADY USED IN DATABASE BY OTHER COURSE.\n");
    printf("######### AFTER CHANGING THAT, ALL REGISTRATION MADE FOR THAT COURSE SHOULD ALSO GET UPDATED.\n");
    printf("######### ALL REGISTRATIONS WITH OLD CODE; WILL GET INVALIDATED.\n");
    printf("######### AND ALL STUDENTS REGISTERED FOR THE COURSE WILL HAVE NEW REGISTRATION RECORD WITH THE NEW COURSE CODE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'INTRODUCTION TO PHILISOPHY' COURSE'S CODE:\n");
    updateItem(wrapCourse(newCredit2400), wrapCourse(newCode2400));
    printf("\n\n");
    
    printf("######################################## UPDATING STUDENT'S SURNAME (SHOULD SUCCEED) ########################################\n");
    // Getting the current record of 'Aristotle' from database.
    Item aristotleItem = getItem(wrapStudent(aristotle));
    // Create a new 'Aristotle' instance with different 'surname' ('of Stagira' instead of 'of Athnes').
    Student aristotleNewSurname = { 2, "Aristoteles", "of Stagira", aristotleItem.value.student.numberOfCoursesRegistered, aristotleItem.value.student.numberOfCreditsTaken };
    printf("######### WE SHOULD BE ABLE TO CHANGE STUDENT'S SURNAME.\n");
    printf("######### IF WE CAN UPDATE SURNAME SUCCESSFULLY, THAT MEANS WE CAN ALSO UPDATE ANY NON-UNIQUE VALUE (LIKE NAME, CREDIT ETC.).\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'ARISTOTLE OF ATHENS'S SURNAME:\n");
    updateItem(getItem(wrapStudent(aristotle)), wrapStudent(aristotleNewSurname));
    printf("\n\n");
    
    printf("######################################## UPDATING STUDENT'S STUDENT NUMBER (SHOULD SUCCEED) ########################################\n");
    // Getting the current record of 'Aristotle' from database.
    aristotleItem = getItem(wrapStudent(aristotle));
    // Creating a new 'Aristotle' instance with different student number (384 instead of 2).
    Student aristotleNewNumber = { 384, "Aristoteles", "of Stagira", aristotleItem.value.student.numberOfCoursesRegistered, aristotleItem.value.student.numberOfCreditsTaken };
    printf("######### WE SHOULD BE ABLE TO CHANGE STUDENT'S STUDENT NUMBER.\n");
    printf("######### AFTER CHANGING THAT, ALL REGISTRATION MADE WITH THAT STUDENT NUMBER SHOULD ALSO GET UPDATED.\n");
    printf("######### ALL REGISTRATIONS WITH OLD STUDENT NUMBER; WILL GET INVALIDATED.\n");
    printf("######### AND ALL INVALIDATED REGISTRATIONS WILL BE RENEWED BY THE REGISTRATIONS WITH NEW STUDENT NUMBERS.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'ARISTOTLE OF STAGIRA'S STUDENT NUMBER:\n");
    updateItem(getItem(wrapStudent(aristotle)), wrapStudent(aristotleNewNumber));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO UPDATE INSTRUCTOR'S ID WITH DUPLICATE VALUE (SHOULD FAIL) ################################################\n");
    // Creating a new 'Paul Hegarty' instance with duplicate ID.
    Instructor paulHegaryInvalidID = { 3, "Paul", "Hegarty", "Professor" };
    printf("######### THIS SHOULD FAIL, BECAUSE NEW INSTRUCTOR ID IS ALREADY USED BY ANOTHER INSTRUCTOR IN DATABASE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'PAUL HEGARTY'S ID WITH DUPLICATE VALUE:\n");
    updateItem(wrapInstructor(paulHegartyNewID), wrapInstructor(paulHegaryInvalidID));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO UPDATE COURSE'S CODE WITH DUPLICATE VALUE (SHOULD FAIL) ################################################\n");
    // Creating a new 'Introduction to Philosophy' instance with duplicate course code.
    Course invalidCode2400 = { "CS193P", "Introduction to Philosophy: God, Knowledge and Consciousness", 4, newQuota, 5 };
    printf("######### THIS SHOULD FAIL, BECAUSE NEW COURSE CODE IS ALREADY USED BY ANOTHER COURSE IN DATABASE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'INTRODUCTION TO PHILOSOPHY'S COURSE CODE WITH DUPLICATE VALUE:\n");
    updateItem(wrapCourse(newCode2400), wrapCourse(invalidCode2400));
    printf("\n\n");
    
    printf("######################################## ATTEMPT TO UPDATE COURSE'S INSTRUCTOR ID WITH INVALID VALUE (SHOULD FAIL) ########################################\n");
    // Creating a new 'Introduction to Philosophy' course with new 'instructorID' value of -1 (instead of 5).
    Course invalidInstructor2400 = { "24.00x", "Introduction to Philosophy: God, Knowledge and Consciousness", 4, newQuota, -1 };
    printf("######### THIS SHOULD FAIL BECAUSE NEW INSTRUCTOR ID IS NOT USED BY  ANY INSTRUCTOR IN DATABASE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'INTRODUCTION TO PHILOSOPHY' COURSE'S INSTRUCTOR ID:\n");
    updateItem(getItem(wrapCourseWithCode("24.00x")), wrapCourse(invalidInstructor2400));
    printf("\n\n");
    
    printf("######################################### ATTEMPT TO UPDATE STUDENT'S STUDENT NUMBER WITH DUPLICATE VALUE (SHOULD FAIL) ################################################\n");
    // Creating a new 'Aristotle' instance with different student number (384 instead of 2).
    Student aristotleInvalidNumber = { 1475, "Aristoteles", "of Stagira", aristotle.numberOfCoursesRegistered, aristotle.numberOfCreditsTaken };
    printf("######### THIS SHOULD FAIL, BECAUSE NEW STUDENT NUMBER IS ALREADY USED BY ANOTHER STUDENT IN DATABASE.\n");
    printf("######### HERE IS THE RESULT OF OUR ATTEMPT TO UPDATE 'ARISTOTLE OF STAGIRA'S STUDENT NUMBER WITH DUPLICATE VALUE:\n");
    updateItem(wrapStudent(aristotleNewNumber), wrapStudent(aristotleInvalidNumber));
    printf("\n\n");
    
    printf("######################################## ALL TESTS ARE COMPLETED FOR UPDATING ITEMS ########################################\n");
    printf("######### YOU CAN CHECK WHETHER DATABASE FILES REFLECT THOSE CHANGES.\n");
    printf("\n\n");
}
