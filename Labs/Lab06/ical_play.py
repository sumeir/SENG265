#!/usr/bin/env python3

import sys
import datetime


def init():
    data =  ['19880720T093835', '19771111T045913', '20061011T023203',
            '19521230T025101', '19730203T110551', '19961110T025409',
            '19451207T022926', '19710712T124423', '19920312T123642',
            '19430917T101024', '19760916T053459', '19930413T050936',
            '20180714T071825', '20170718T081406', '19690819T084908',
            '19990413T044420', '19701109T120652', '20100703T073320',
            '19680213T051641', '19700623T125307']

    return data


"""
The following won't work yet as you must as first add parameters to
the function definition. 
"""

def icals_to_datetimes():
    """
    Given a list of strings encoding dates and times in the
    iCalendar form, return a list of datatimes where each original
    string corresponds to a new and equivalent datetime.
    """

    return []



def main():
    ical_dts = init()

    result = [ icals_to_datetimes(i) for i in ical_dts ]
    whitespace = ""
    for dt in result:
        print(whitespace, end="")
        print(dt)
        print("<something appropriate here?>")
        whitespace = "\n"


if __name__ == "__main__":
    main()
