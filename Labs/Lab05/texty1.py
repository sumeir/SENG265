#!/usr/bin/env python3

import sys

def main():
    if len(sys.argv) < 2:
        print("usage:", sys.argv[0], "'some,string,with,words'")
        sys.exit(1)

    line = sys.argv[1]
    line = line.strip()

    words = line.split(",")

    for w in words:
        print (w)        


if __name__ == "__main__":
    main()
