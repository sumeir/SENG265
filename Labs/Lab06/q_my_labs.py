#!/usr/bin/env python3

import datetime

def main():

    todays_date = datetime.date(2021, 10, 20)
    date_list = every_lab(todays_date)

    for date in date_list:
        print(date.strftime("%a, %d %b %y"))

    
def every_lab(todays_date):

    end_date = datetime.date(2021, 12, 6)
    lab_list = []
    dt = todays_date
    delta = datetime.timedelta(7)

    while dt <= end_date:
        lab_list.append(dt)
        dt += delta
    
    return lab_list
    
    
if __name__ == "__main__":
    main()
