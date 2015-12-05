"""
Generates binary features (for hamming space) in two files:
i) data set file
ii) query set file
Guarantees:
Both (i) and (ii) are disjoint (to make it more interesting).

Sample Usage:
python [filename] -d 10 -nd 1000 -df file1 -nq 10 -qf file2

Generates data points in the form {0,1}^d 
"""

import argparse
import math
import random
import sys

def to_binary(n, d):
    # bin(n) -> '0bxxx...'
    binlist = [int(ch) for ch in list(bin(n))[2:]]
    return [0]*(d-len(binlist)) + binlist

def generate_data_sets(d, nd, df, nq, qf):
    numbers = range(2**d)
    random.shuffle(numbers)
    points = [to_binary(num, d) for num in numbers[:(nd + nq)]]

    point_reducer = lambda x, y: str(x) + " " + str(y)
    with open(df, "wb") as f:
        for i in range(nd):
            f.write(reduce(point_reducer, points[i]) + "\r\n")
    with open(qf, "wb") as f:
        for i in range(nd, nd+nq):
            f.write(reduce(point_reducer, points[i]) + "\r\n")

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("-d", "--num_dimensions", dest="d", type=int, required=True,
                        help="Number of dimensions")
    parser.add_argument("-nd", "--number_data_points", dest="nd", type=int, required=True,
                        help="Number of data points")
    parser.add_argument("-df", "--data_set_file", dest="df", type=str, required=True,
                        help="Data set file")
    parser.add_argument("-nq", "--number_query_points", dest="nq", type=int, required=True,
                        help="Number of query points")
    parser.add_argument("-qf", "--query_set_file", dest="qf", type=str, required=True,
                        help="Query set file")

    args = parser.parse_args()
    if args.d < math.ceil(math.log(args.nd + args.nq, 2)): # looser
        print("You need more dimensions. Increase 'd'")
        sys.exit()

    generate_data_sets(args.d, args.nd, args.df, args.nq, args.qf)
    print "Number of dimensions: {0}".format(args.d)
    print "Generated {0} points in data set file {1}".format(args.nd, args.df)
    print "Generated {0} points in query set file {1}".format(args.nq, args.qf)

if __name__ == "__main__":
    main()
