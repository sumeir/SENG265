#!/usr/bin/env python3

import sys

def main():
    all_words = []

    for line in sys.stdin:
        line = line.strip()
        words = line.split(",")
        all_words.extend(words)

    all_words.sort()

    for w in all_words:
        print(w) 


if __name__ == "__main__":
    main()
