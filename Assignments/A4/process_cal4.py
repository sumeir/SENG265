#!/usr/bin/env python3
"""
process_cal4.py

Contains process_cal class and event class. Creates formatted string 
representaion of events in a specific range of dates from an ICS file. 

The get_events_for_day method inside process_cal class is to be invoked by the tester 
on a process_cal instance with a day as a parameter. The events (if any) on the day 
are returned as a formatted string.
"""

import datetime
import re

class process_cal:
    """
    Represents the events from an ICS file. Has attributes: file_name (str), event_list (list),
    events_for_day (dict), and instance methods to read/parse the file, sort events, 
    and store events into appropriate data structures.
    """

    def __init__(self, file_name):
        """
        Instantiates the process_cal class, taking a file name as a parameter. Calls methods
        to perform processing on the file.
        """

        self.file_name = file_name
        self.event_list = []
        self.events_for_day = {}
        self.create_event_list()
        self.handle_recurring_events()
        self.sort_events()
        self.create_events_for_day()

    def get_events_for_day(self, dt):
        """
        Takes as parameter a datetime object, and returns a formatted string 
        containing events on that day (if any). If there are none, returns None.
        """

        if dt in self.events_for_day:
            events = event.format_date(dt) + "\n"
            for i in range(len(self.events_for_day[dt])-1):
                events+= str(self.events_for_day[dt][i]) + "\n"
            events += str(self.events_for_day[dt][len(self.events_for_day[dt])-1])
            return events
        else:
            return None

    def create_event_list(self):
        """
        Reads the file line-by-line, parses each line, creates event objects 
        based on the contents in the lines, and stores the events in the event_list.
        """

        file = open(self.file_name, "r")
        rrule = None
        pattern = re.compile(r".*UNTIL=(\d+T\d+);.*")
        for line in file:
            line = line.rstrip()
            tok = re.split(":", line)
            if tok[0]=="DTSTART":
                dtstart = datetime.datetime.strptime(tok[1], "%Y%m%dT%H%M%S")
            elif tok[0]=="DTEND":
                dtend = datetime.datetime.strptime(tok[1], "%Y%m%dT%H%M%S")
            elif tok[0]=="LOCATION":
                location = tok[1]
            elif tok[0]=="SUMMARY":
                summary = tok[1]
            elif tok[0]=="RRULE":
                rrule = datetime.datetime.strptime(pattern.search(line).group(1), "%Y%m%dT%H%M%S")
            elif line == "END:VEVENT":
                self.event_list.append(event(dtstart,dtend,location,summary,rrule))
                rrule = None
        file.close()
    
    def handle_recurring_events(self):
        """
        Iterates through event_list, and creates appropriate number of updated instances 
        for events that are recurring. The updated instances have start date and end date 
        incremented by 7 days.
        """

        len_event_list = len(self.event_list)
        for i in range(len_event_list):
            if self.event_list[i].rrule is not None:
                date = self.event_list[i].dtstart
                end_date = self.event_list[i].dtend
                while (date <= self.event_list[i].rrule):
                    date += datetime.timedelta(7)
                    end_date += datetime.timedelta(7)
                    if (date <= self.event_list[i].rrule):
                        self.event_list.append(event(date, end_date, self.event_list[i].location, self.event_list[i].summary, None))
                self.event_list[i].rrule = None

    def sort_events(self):
        """
        Sorts event_list by value of dtstart of each event. This effectively organizes the events
        by date first, and for each date, events on same date are organized by the start time.
        """

        self.event_list.sort(key = lambda e : e.dtstart)

    def create_events_for_day(self):
        """
        Creates a hash table events_for_day, where the keys are datetime objects representing
        dates, and values are list of events for a date. This allows fast access to events
        for any day passed into get_events_for_day method, eliminating the need to iterate through
        the entire list of events each time.
        """

        for i in range(len(self.event_list)):
            day = datetime.datetime(self.event_list[i].dtstart.year, self.event_list[i].dtstart.month, self.event_list[i].dtstart.day)
            if day not in self.events_for_day:
                events = [self.event_list[i]]
            else:
                events = self.events_for_day[day]
                events.append(self.event_list[i])
            self.events_for_day[day] = events

class event:
    """
    Represents an event, with attributes dtstart, dtend, location, summary, 
    and rrule. If an event is non-recurring, rrule is set to None.
    """

    def __init__(self, dtstart, dtend, location, summary, rrule):
        """
        Creates an event object, setting the attributes to values passed
        in as parameters.
        """

        self.dtstart = dtstart
        self.dtend = dtend
        self.location = location
        self.summary = summary
        self.rrule = rrule

    def __repr__(self):
        """
        Returns a formatted string representation for an event.
        """

        str = event.format_time(self.dtstart) + " to " + event.format_time(self.dtend) + ": "
        str += self.summary + " {{" + self.location + "}}"
        return str

    def format_date(unformatted_dt):
        """
        Takes a datetime object as a parameter and returns a formatted 
        string representing the date.
        """

        formatted_date = unformatted_dt.strftime("%B %d, %Y (%a)")
        return formatted_date + "\n" + "-"*len(formatted_date)

    def format_time(unformatted_dt):
        """
        Takes a datetime object as a parameter and returns a formatted 
        string representing the time.
        """

        formatted_time = unformatted_dt.strftime("%I:%M %p")
        if (formatted_time[0]=="0"):
            formatted_time = " " + formatted_time[1:]
        return formatted_time
