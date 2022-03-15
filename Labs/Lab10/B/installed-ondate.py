#!/usr/bin/env python3

import re
import sys

def main():
    if len(sys.argv) < 2:
        sys.exit(0)

    date = sys.argv[1]

    pattern = re.compile(date + ".* installed ((.+):(.+)) .*")

    for line in sys.stdin:
        line = line.rstrip()

        m = pattern.search(line)
        if m:
            print("%s: %s" % (date, m.group(2)))

if __name__ == "__main__":
    main()
