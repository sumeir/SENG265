#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"

#define MAX_LINE_LEN 132

node_t* create_events (FILE*, int*, node_t*);
node_t* handle_recurring_events (int*, node_t*);
node_t* remove_out_of_range_events (int*, node_t*, char*, char*);
void print_events (node_t*);
void deallocate_list (node_t*);
void dt_format(char*, const char*, const int);
void dt_increment(char*, const char*, int const);
void print_time (char*);
void remove_newline(char*);

int main (int argc, char *argv[])
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

    char from[DT_LEN];
    char to[DT_LEN];
    sprintf(from, "%.4d%.2d%.2dT000000", from_y, from_m, from_d);
    sprintf(to, "%.4d%.2d%.2dT235959", to_y, to_m, to_d);

    FILE* fp = NULL;
    fp = fopen(filename, "r");
    if (fp==NULL) {
        fprintf(stderr, "error: unable to open <%s>\n", filename);
        exit(1);
    }

    node_t* events = NULL;
    int num_events = 0;

    events = create_events(fp, &num_events, events);
    events = handle_recurring_events(&num_events, events);
    events = remove_out_of_range_events(&num_events, events, from, to);
    print_events(events);
    deallocate_list(events);

    fclose(fp);
    exit(0);
}

node_t* create_events (FILE* fp, int* len_list, node_t* list) 
{
    char line[MAX_LINE_LEN];
    event_t* event = (event_t*)emalloc(sizeof(event_t));
    char* tok;
    
    while ( fgets(line, MAX_LINE_LEN, fp) != NULL) {

        if ( strncmp(line, "DTSTART:", 8) == 0 )
        {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(event->dtstart, tok);
            sscanf(event->dtstart, "%dT%*d", &(event->date));
        } 
        else if ( strncmp(line, "DTEND:", 6) == 0 )
        {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(event->dtend, tok);
        }
        else if ( strncmp(line, "LOCATION:", 9) == 0 )
        {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(event->location, tok);
        }
        else if ( strncmp(line, "SUMMARY:", 8) == 0 )
        {
            tok = strtok(line, ":");
            tok = strtok(NULL, ":");
            remove_newline(tok);
            strcpy(event->summary, tok);
        }
        else if ( strncmp(line,"END:VEVENT", 10) == 0 )
        {
            node_t* node = new_node(event);
            list = add_inorder(list, node);
            (*len_list)++;
            event = (event_t*)emalloc(sizeof(event_t));
        }
        else if ( strncmp(line, "RRULE:", 6) == 0 )
        {
            int val;
            char str[MAX_LINE_LEN];
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
            sprintf(event->rrule, "%dT235959", val);
        }
    }

    free((void*)event);

    return list;
}

node_t* handle_recurring_events (int* len_list, node_t* list) 
{   
    node_t* cur = list;

    while (cur != NULL)
    {
        if (strcmp(cur->val->rrule, "") != 0)
        {
            char date[MAX_LINE_LEN];
            strcpy(date, cur->val->dtstart);
            while (strcmp(date, cur->val->rrule) <= 0)
            {
                char incremented_date[MAX_LINE_LEN];
                dt_increment(incremented_date, date, 7);
                strcpy(date, incremented_date);
                if (strcmp(date, cur->val->rrule) <= 0)
                {
                    char incremented_end_date[MAX_LINE_LEN];
                    dt_increment(incremented_end_date, cur->val->dtend, 7);
                    event_t* event = (event_t*)emalloc(sizeof(event_t));
                    strcpy(event->dtstart, date);
                    strcpy(event->dtend, incremented_end_date);
                    strcpy(event->location, cur->val->location);
                    strcpy(event->summary, cur->val->summary);
                    sscanf(event->dtstart, "%dT%*d", &(event->date));
                    node_t* node = new_node(event);
                    list = add_inorder(list, node);
                    (*len_list)++;
                }
            }
        }
        strcpy(cur->val->rrule, "");
        cur = cur->next;
    }
    return list;
}

node_t* remove_out_of_range_events (int* len_list, node_t* list, char* start_date, char* end_date)
{

    node_t* cur = list;
    node_t* temp;

    if (cur != NULL) 
    {
        while (cur->next != NULL) 
        {
            if (!((strcmp(cur->next->val->dtstart, start_date) >= 0) && (strcmp(cur->next->val->dtstart, end_date) <=0)))
            {
                temp = cur->next;
                cur->next = cur->next->next;
                (*len_list)--;
                deallocate(temp);
                continue;
            }
            cur = cur->next;
        }

        cur = list;

        if (!((strcmp(cur->val->dtstart, start_date) >= 0) && (strcmp(cur->val->dtstart, end_date) <=0))) 
        {
            list = list->next;
            (*len_list)--;
            deallocate(cur);
        }

    }
    return list;
}

void print_events (node_t* list)
{
    if (list != NULL)
    {
        node_t* prev = list;
        node_t* cur = list;
        char formatted_date[MAX_LINE_LEN];
        char unformatted_date[MAX_LINE_LEN];

        strcpy(unformatted_date, list->val->dtstart);
        dt_format(formatted_date, unformatted_date, MAX_LINE_LEN);
        printf("%s\n", formatted_date);
        for (int i=0; i<strlen(formatted_date); i++) {
            printf("-");
        }
        printf("\n");
        print_time(list->val->dtstart);
        printf(" to ");
        print_time(list->val->dtend);
        printf(": %s {{%s}}\n", list->val->summary, list->val->location);

        cur = cur->next;

        while (cur != NULL)
        {
            if (cur->val->date == prev->val->date) 
            {
                print_time(cur->val->dtstart);
                printf(" to ");
                print_time(cur->val->dtend);
                printf(": %s {{%s}}\n", cur->val->summary, cur->val->location);
            }
            else 
            {
                printf("\n");
                strcpy(unformatted_date, cur->val->dtstart);
                dt_format(formatted_date, unformatted_date, MAX_LINE_LEN);
                printf("%s\n", formatted_date);
                for (int i=0; i<strlen(formatted_date); i++) {
                    printf("-");
                }
                printf("\n");
                print_time(cur->val->dtstart);
                printf(" to ");
                print_time(cur->val->dtend);
                printf(": %s {{%s}}\n", cur->val->summary, cur->val->location);
            }
            prev = prev->next;
            cur = cur->next;
        }
    }
}

void deallocate_list (node_t* list)
{
    node_t* cur = list;
    node_t* temp;
    while (cur != NULL)
    {
        temp = cur;
        cur = cur->next;
        deallocate(temp);
    }
}

void dt_format (char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}

void dt_increment (char *after, const char *before, int const num_days)
{
    struct tm temp_time;
    time_t    full_time;

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

void print_time (char* str) 
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

void remove_newline (char* ln)
{
    if (ln!=NULL)
    {
        int len = strlen(ln);
        if (ln>0) {
            ln[len-1] = '\0';
        }
    }
}