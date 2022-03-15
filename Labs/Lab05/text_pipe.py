#!/usr/bin/env python3

import sys

def main():
    all_words = []
    
    for line in sys.stdin:
        line = line.strip()
        words = line.split(",")
        all_words.extend(words)
        
    all_unique_words = []
    
    for word in all_words:
        if word not in all_unique_words:
            all_unique_words.append(word)

    all_unique_words.sort()
    
    shortest_word_length = len(all_unique_words[0])
    longest_word_length = len(all_unique_words[0])
    
    for word in all_unique_words:
        if len(word) < shortest_word_length:
            shortest_word_length = len(word)

    for word in all_unique_words:
        if len(word) > longest_word_length:
            longest_word_length = len(word)

    for word in all_unique_words:
        print(word)

    print()
    print("="*12)

    print("SHORTEST:")
    for word in all_unique_words:
        if len(word) == shortest_word_length:
            print(word)
    
    print()

    print("LONGEST:")
    for word in all_unique_words:
        if len(word) == longest_word_length:
            print(word)

    print("="*12)
    print()

if __name__ == "__main__":
    main()

