#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse

###
# parse args
parser = argparse.ArgumentParser()
parser.add_argument("file1", type=str,
                    help="the first file to compare")
parser.add_argument("file2", type=str,
                    help="the second file to compare")
parser.add_argument("-s", "--source", type=str,
                    help="print out with source instruction")
args = parser.parse_args()

###
# open files
f1 = open(args.file1, "r")
f2 = open(args.file2, "r")
if args.source: src = open(args.source, "r")

###
# SOURCE
if args.source:
    from collections import namedtuple
    __TEXT__ = 0x400000
    __DATA__ = 0x10000000
    symbols = dict()
    data = []
    labels = dict()
    instrs = []
    # first pass
    dc = 0
    pc = 0
    for line in src:
        # skip if blankline or comments
        if line.strip() == '': continue
        if line.strip()[0] == '#': continue

        # clean up instruction
        tokens = line.strip().split()
        for idx, tok in enumerate(tokens[1:-1]):
            tokens[idx+1] = tok.strip(',')

        # detect section
        if '.' == tokens[0][0] and len(tokens) == 1:
            section = tokens[0][1:]
            continue

        ###
        # .data section
        # detect symbol
        if section == 'data':
            if tokens[0][-1] == ':':
                symbols[tokens[0][:-1]] = dc
                data.append(tuple(tokens[1:]))
                dc += 1
            else:
                data.append(tuple(tokens))
                dc += 1
            continue

        ###
        # .text section
        if section == 'text':
            if tokens[0][-1] == ':':
                labels[tokens[0][:-1]] = pc
            else:
                if tokens[0] == 'la':
                    # interprete data symbols
                    addr = __TEXT__ + 0x20 * symbols[tokens[2]]
                    upper = addr >> 16
                    lower = addr % (1<<16)
                    # lui upper
                    tokens[0] = "lui"
                    tokens[2] = str(upper)
                    instrs.append(tuple(tokens))
                    pc += 1
                    # ori lower
                    if lower != 0:
                        tokens[0] = "ori"
                        tokens[2] = tokens[1]
                        tokens.append(str(lower))
                        instrs.append(tuple(tokens))
                        pc += 1
                else:
                    instrs.append(tuple(tokens))
                    pc += 1
            continue
        # end for loop
    src.close()
    # end first pass



###
# diff
bs1 = f1.read().strip()
bs2 = f2.read().strip()

print()
if bs1 == bs2:
    print("Exactly same.")
else:
    if len(bs1) != len(bs2):
        print("# of instruction or data is different")
        if len(bs1) % 32 != 0: aligned1 = "not aligned"
        else: aligned1 = ""
        if len(bs2) % 32 != 0: aligned2 = "not aligned"
        else: aligned2 = ""
        print("\t%s: \t%3d   %s" % (args.file1, len(bs1), aligned1))
        print("\t%s: \t%3d   %s" % (args.file2, len(bs2), aligned2))
    else:
        if len(bs1) % 32 != 0:
            print("The object file should be aligned to 32-bit")
        else:
            print("  \t{:32s}     {:32s}".format(args.file1, args.file2))
            print("  \t{:32s}     {:32s}".format('-'*len(args.file1), '-'*len(args.file2)))
            for i in range(len(bs1)//32):
                seg1 = bs1[i*32:(i+1)*32]
                seg2 = bs2[i*32:(i+1)*32]
                if seg1 == seg2:
                    diff = "  "
                    sep = "|"
                else:
                    diff = "!!"
                    sep = "+"
                if args.source and 2 <= i and i < len(instrs) + 2:
                    print((diff + '\t' + seg1 + '  {0}  ' + seg2 + '  {0}  ' + '\t'.join(instrs[i-2])).format(sep))
                elif args.source and i >= len(instrs) + 2:
                    if i == len(instrs) + 2: print("\n  \t{0:32s}     {0:32s}".format(".data", ".data"))
                    print((diff + '\t' + seg1 + '  {0}  ' + seg2 + '  $  ' + '\t'.join(data[i-len(instrs)-2][1:])).format(sep))
                else:
                    print((diff + '\t' + seg1 + '  {0}  ' + seg2).format(sep))

###
# close files
if f1: f1.close()
if f2: f2.close()
if args.source: src.close()
