#!/usr/bin/env python

import sys
import datetime

def main():

    if len(sys.argv)!=4:
        print("usage: ./", sys.argv[0], " --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile", sep="")
        return
    
    start = datetime.datetime.strptime(sys.argv[1][8:]+"/00/00/00", "%Y/%m/%d/%H/%M/%S")
    end = datetime.datetime.strptime(sys.argv[2][6:]+"/23/59/59", "%Y/%m/%d/%H/%M/%S")
    filename = sys.argv[3][7:]

    file = open(filename, "r")
    events = []
    
    create_events(file,events)
    handle_repeating_events(events)
    remove_out_of_range_events(events,start,end)
    sort_events(events)
    print_events(events)

    file.close()

def create_events(file_object, event_list):
    RRULE = None
    for line in file_object:
        line = line.rstrip()
        if line.startswith("DTSTART"):
            DTSTART = datetime.datetime.strptime(line[8:], "%Y%m%dT%H%M%S")
        elif line.startswith("DTEND"):
            DTEND = datetime.datetime.strptime(line[6:], "%Y%m%dT%H%M%S")
        elif line.startswith("LOCATION"):
            LOCATION = line[9:]
        elif line.startswith("SUMMARY"):
            SUMMARY = line[8:]
        elif line.startswith("RRULE"):
            if line.find("WKST")==-1:
                RRULE = datetime.datetime.strptime(line[24:39], "%Y%m%dT%H%M%S")
            else:
                RRULE = datetime.datetime.strptime(line[32:47], "%Y%m%dT%H%M%S")
        elif line == "END:VEVENT":
            event_list.append({
                "DTSTART": DTSTART,
                "DTEND": DTEND,
                "LOCATION": LOCATION,
                "SUMMARY": SUMMARY,
                "RRULE": RRULE
            })
            RRULE = None

def handle_repeating_events(event_list):
    len_list = len(event_list)
    for i in range(len_list):
        if event_list[i]["RRULE"] is not None:
            date = event_list[i]["DTSTART"]
            end_date = event_list[i]["DTEND"]
            while (date <= event_list[i]["RRULE"]):
                delta = datetime.timedelta(7)
                date += delta
                end_date += delta
                if (date <= event_list[i]["RRULE"]):
                    event_list.append({
                        "DTSTART": date,
                        "DTEND": end_date,
                        "LOCATION": event_list[i]["LOCATION"],
                        "SUMMARY": event_list[i]["SUMMARY"]
                    })
    for event in event_list:
        event.pop("RRULE", None)

def sort_events(event_list):
    event_list.sort(key=lambda e:e["DTSTART"])

def remove_out_of_range_events(event_list, start_date, end_date):
    new_list = []
    for i in range(len(event_list)):
        if start_date <= event_list[i]["DTSTART"] <= end_date:
            new_list.append(event_list[i])
    event_list[:] = new_list
    
def print_events(event_list):
    if len(event_list)!=0:
        print(format_date(event_list[0]["DTSTART"]))
        print(format_time(event_list[0]["DTSTART"]), "to", format_time(event_list[0]["DTEND"]), end=": ")
        print(event_list[0]["SUMMARY"], "{{%s}}"%event_list[0]["LOCATION"])
        for i in range(1,len(event_list)):
            if event_list[i]["DTSTART"].date()==event_list[i-1]["DTSTART"].date():
                print(format_time(event_list[i]["DTSTART"]), "to", format_time(event_list[i]["DTEND"]), end=": ")
                print(event_list[i]["SUMMARY"], "{{%s}}"%event_list[i]["LOCATION"])
            else:
                print()
                print(format_date(event_list[i]["DTSTART"]))
                print(format_time(event_list[i]["DTSTART"]), "to", format_time(event_list[i]["DTEND"]), end=": ")
                print(event_list[i]["SUMMARY"], "{{%s}}"%event_list[i]["LOCATION"])

def format_date(unformatted_dt):
    formatted_date = unformatted_dt.strftime("%B %d, %Y (%a)")
    return formatted_date + "\n" + "-"*len(formatted_date)

def format_time(unformatted_dt):
    formatted_time = unformatted_dt.strftime("%I:%M %p")
    if (formatted_time[0]=="0"):
        formatted_time = " " + formatted_time[1:]
    return formatted_time

if __name__ == "__main__":
    main()