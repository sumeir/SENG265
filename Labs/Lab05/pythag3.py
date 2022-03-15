#!/usr/bin/env python3

import sys

def pythag(side_a, side_b):
    return (side_a ** 2 + side_b**2) ** 0.5


def main():
    if len(sys.argv) < 3:
        print("usage:", sys.argv[0], "<length> <length")
        sys.exit(1)

    a = float(sys.argv[1])
    b = float(sys.argv[2])

    print("Sides ", a, " and ", b, ", hypotenuse ", end="", sep="")
    print("%.4f" % pythag(a, b))


if __name__ == "__main__":
    main()
