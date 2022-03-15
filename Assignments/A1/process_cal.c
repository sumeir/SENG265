#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500

typedef struct {
    char DTSTART[MAX_LINE_LEN+1];
    char DTEND[MAX_LINE_LEN+1];
    char LOCATION[MAX_LINE_LEN+1];
    char SUMMARY[MAX_LINE_LEN+1];
    char RRULE[MAX_LINE_LEN+1];
    int UNTIL;
    int date;
    int flag;
} Event;

// function prototypes
void dt_format(char*, const char*, const int);
void dt_increment(char*, const char*, int const);
void print_events(Event[], int*);
void construct_events (FILE*, int*, Event[]);
void remove_newline(char*);
void handle_repeating_events (int*, Event[]);
int compare (const void*, const void*);
void sortEventsArray (int*, Event[]); 
void printTime (char*);
void toggleFlagsIfWithinRange (int*, Event[], char*, char*, int*);
void populateArrayEventsInRange (int*, Event[], Event[]);


void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;
    char      temp[5];

    /*  
     * Ignore for now everything other than the year, month and date.
     * For conversion to work, months must be numbered from 0, and the 
     * year from 1900.
     */  
    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}

void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time, *p_temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour, 
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));
    strncpy(after + 16, before + 16, MAX_LINE_LEN - 16); 
    after[MAX_LINE_LEN - 1] = '\0';
}

void print_events(Event array[], int* num)
{
    int n = *num;

    if (n!=0) {
        char formatted_date[MAX_LINE_LEN+1];
        char unformatted_date[MAX_LINE_LEN+1];

        strcpy(unformatted_date, array[0].DTSTART);
        dt_format(formatted_date, unformatted_date, MAX_LINE_LEN+1);
        printf("%s\n", formatted_date);
        for (int i=0; i<strlen(formatted_date); i++) {
            printf("-");
        }
        printf("\n");
        printTime(array[0].DTSTART);
        printf(" to ");
        printTime(array[0].DTEND);
        printf(": %s {{%s}}\n", array[0].SUMMARY, array[0].LOCATION);

        for (int i=1; i<n; i++) {
            if ( array[i].date == array[i-1].date ) {
                printTime(array[i].DTSTART);
                printf(" to ");
                printTime(array[i].DTEND);
                printf(": %s {{%s}}\n", array[i].SUMMARY, array[i].LOCATION);
            } else {
                printf("\n");
                strcpy(unformatted_date, array[i].DTSTART);
                dt_format(formatted_date, unformatted_date, MAX_LINE_LEN+1);
                printf("%s\n", formatted_date);
                for (int j=0; j<strlen(formatted_date); j++) {
                    printf("-");
                }
                printf("\n");
                printTime(array[i].DTSTART);
                printf(" to ");
                printTime(array[i].DTEND);
                printf(": %s {{%s}}\n", array[i].SUMMARY, array[i].LOCATION);
            }
        }
    }
}

void remove_newline (char* ln)
{
    if (ln!=NULL) {
        int len = strlen(ln);
        if (ln>0) {
            ln[len-1] = '\0';
        }
    }
}

void construct_events (FILE* fp, int* num, Event array[])
{
    char line[MAX_LINE_LEN+1];
    Event e;
    int n = 0;
    char* tok;
    
    while ( fgets(line, MAX_LINE_LEN+1, fp) != NULL) {
        if ( strncmp(line, "DTSTART:", 8) == 0 ) {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(e.DTSTART, tok);

            sscanf(e.DTSTART, "%dT%*d", &e.date);

        } else if ( strncmp(line, "DTEND:", 6) == 0 ) {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(e.DTEND, tok);
        } else if ( strncmp(line, "LOCATION:", 9) == 0 ) {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(e.LOCATION, tok);
        } else if ( strncmp(line, "SUMMARY:", 8) == 0 ) {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(e.SUMMARY, tok);
        } else if ( strncmp(line,"END:VEVENT", 10) == 0 ) {
            array[n++] = e;
            *(num) = n;
            Event newEvent;
            e = newEvent;
        }else if ( strncmp(line, "RRULE:", 6) == 0 ) {
            int val;
            char str[MAX_LINE_LEN+1];
            strcpy(str, line);
            tok = strtok(str, ":");
            tok = strtok(NULL, ":");
            char* tok1 = strtok(tok, ";");
            tok1 = strtok(NULL, ";");
            char* tok2 = strtok(tok1, "=");
            if (strcmp(tok2, "WKST")==0) {
                sscanf(line, "%*[^;];%*[^;];%*[A-Za-z]=%dT%*d;%*s", &val);
            } else {
                sscanf(line, "%*[^;];%*[A-Za-z]=%dT%*d;%*s", &val);
            }
            e.UNTIL = val;
            sprintf(e.RRULE, "%dT235959", e.UNTIL);
        }
    }
}

void handle_repeating_events (int* num, Event array[])
{
    int n = *num;
    int newNum = *num;
    for (int i=0; i<n; i++) {
        if (array[i].UNTIL!=0) {
            char date[MAX_LINE_LEN+1];
            strcpy(date,array[i].DTSTART);
            while ( strcmp(date, array[i].RRULE) <= 0 ) {
                char incrementedDate[MAX_LINE_LEN+1];
                dt_increment(incrementedDate, date, 7);
                strcpy(date, incrementedDate);
                if ( strcmp(date, array[i].RRULE) <= 0) {
                    char incrementedEndDate[MAX_LINE_LEN+1];
                    dt_increment(incrementedEndDate, array[i].DTEND, 7);
                    Event newOccurence;
                    strcpy(newOccurence.DTSTART, date);
                    strcpy(newOccurence.DTEND, incrementedEndDate);
                    strcpy(newOccurence.LOCATION, array[i].LOCATION);
                    strcpy(newOccurence.SUMMARY, array[i].SUMMARY);
                    sscanf(newOccurence.DTSTART, "%dT%*d", &newOccurence.date);
                    newOccurence.UNTIL = 0;
                    strcpy(newOccurence.RRULE, "");
                    array[newNum] = newOccurence;
                    newNum++;
                }
            }
        }
    }
    *num = newNum;
    for (int i=0; i<n; i++) {
        array[i].UNTIL = 0;
        strcpy(array[i].RRULE, "");
    }
}

int compare (const void * first, const void * second) 
{
    Event* A = (Event*) first;
    Event* B = (Event*) second;

    if ( strcmp((*A).DTSTART,(*B).DTSTART) <= 0 ) {
        return -1;
    } else {
        return 1;
    }
    return 0;
}

void sortEventsArray (int* num, Event array[]) 
{
    int arraySize = *num;
    qsort(array, arraySize, sizeof(Event), compare);
}

void printTime (char* str) 
{
    int time;
    int hour;
    int minute;

    sscanf(str,"%*[0-9]T%d", &time);
    time = time / 100;
    minute = time%100;
    hour = time/100;
    
    if (hour == 0) {
        hour = 12;
        printf("%2d:%.2d AM", hour, minute);
    } else if (hour==12) {
        printf("%2d:%.2d PM", hour, minute);
    } else if (hour > 12) {
        hour = hour-12;
        printf("%2d:%.2d PM", hour, minute);
    } else if (hour < 12) {
        printf("%2d:%.2d AM", hour, minute);
    }
}

void toggleFlagsIfWithinRange (int* num, Event array[], char* from_date, char* to_date, int* N) 
{
    int n = *num;
    int count = 0;

    for (int i=0; i<n; i++) {
        if ((strcmp(array[i].DTSTART, from_date) >= 0) && (strcmp(array[i].DTSTART, to_date) <= 0)) {
            array[i].flag = 1;
            count++;
        } else {
            array[i].flag = 0;
        }
    }
    *N = count;
}

void populateArrayEventsInRange (int* num, Event array[], Event originalArray[]) 
{
    int n = *num;
    int m = 0;
    for (int i=0; i<n; i++) {
        if (originalArray[i].flag==1) {
            array[m] = originalArray[i];
            m++; 
        }
    }
}

int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i; 

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    Event Events[MAX_EVENTS];
    int numEvents = 0;

    FILE* fp = NULL;
    fp = fopen(filename, "r");
    if (fp==NULL) {
        fprintf(stderr, "error: unable to open <%s>\n", filename);
        exit(1);
    }

    construct_events(fp, &numEvents, Events);
    handle_repeating_events(&numEvents, Events);
    sortEventsArray(&numEvents, Events);

    char from[MAX_LINE_LEN+1];
    char to[MAX_LINE_LEN];
    int numEventsInRange;
    sprintf(from, "%.4d%.2d%.2dT000000", from_y, from_m, from_d);
    sprintf(to, "%.4d%.2d%.2dT235959", to_y, to_m, to_d);
    toggleFlagsIfWithinRange(&numEvents, Events, from, to, &numEventsInRange);
    
    Event EventsInRange[numEventsInRange];
    populateArrayEventsInRange(&numEvents,EventsInRange, Events);

    print_events(EventsInRange, &numEventsInRange);

    fclose(fp);
    exit(0);
}
