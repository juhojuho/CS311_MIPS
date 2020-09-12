#!/usr/bin/env python
# -*- coding: utf-8 -*-

print("ASSEMBLER")
print("---------\n")

# initialize
import sys, os, re
__TEXT__ = 0x400000
__DATA__ = 0x10000000
symbols = dict()
data = []
labels = dict()
instrs = []
def bin(num, size):
    digits = []
    while True:
        digits.append(str(num % 2))
        num /= 2
        if not num: break
    digits.extend(['0']*(size - len(digits)))
    digits.reverse()
    return ''.join(digits)

# file open
fname = sys.argv[1]
print(fname)
f = open(fname, 'r')

# first pass
dc = 0
pc = 0
for line in f:
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
                addr = __DATA__ + symbols[tokens[2]] * 4
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

f.close()
# end first pass

# second pass
base = os.path.basename(fname)
fname2 = os.path.splitext(base)[0]
f = open(fname2+'.o', 'w')
###
# initialization
# instruction types
# InstrR = namedtuple('R', 'op rs rt rd shamt funct')
class InstrR(object):
    def __init__(self, op, rs, rt, rd, shamt, funct):
        self.op = op
        self.rs = rs
        self.rt = rt
        self.rd = rd
        self.shamt = shamt
        self.funct = funct
# InstrI = namedtuple('I', 'op rs rt immediate')
class InstrI(object):
    def __init__(self, op, rs, rt, immediate):
        self.op = op
        self.rs = rs
        self.rt = rt
        self.immediate = immediate
# InstrJ = namedtuple('J', 'op address')
class InstrJ(object):
    def __init__(self, op, address):
        self.op = op
        self.address = address
# encoding
def encode(instr):
    if type(instr) == InstrR:
        # ??? should we consider two's complement of shamt?
        return ''.join([bin(instr.op,6), bin(instr.rs,5), bin(instr.rt,5), bin(instr.rd,5), bin(instr.shamt,5), bin(instr.funct,6)])
    elif type(instr) == InstrI:
        # consider two's complement (immediate)
        if instr.immediate < 0:
            immediate = (1<<16) + instr.immediate
        else:
            immediate = instr.immediate
        return ''.join([bin(instr.op,6), bin(instr.rs,5), bin(instr.rt,5), bin(immediate, 16)])
    elif type(instr) == InstrJ:
        return ''.join([bin(instr.op,6), bin(instr.address,26)])
    return None
# reference setup
instr_refs = {
        "addiu" : InstrI(9,None,None,None),
        "addu"  : InstrR(0,None,None,None,0,0x21),
        "and"   : InstrR(0,None,None,None,0,0x24),
        "andi"  : InstrI(0xC,None,None,None),
        "beq"   : InstrI(4,None,None,None),
        "bne"   : InstrI(5,None,None,None),
        "j"     : InstrJ(2,None),
        "jal"   : InstrJ(3,None),
        "jr"    : InstrR(0,None,0,0,0,8),
        "lui"   : InstrI(0xF,0,None,None),
        "lw"    : InstrI(0x23,None,None,None),
        "nor"   : InstrR(0,None,None,None,0,0x27),
        "or"    : InstrR(0,None,None,None,0,0x25),
        "ori"   : InstrI(0xD,None,None,None),
        "sltiu" : InstrI(0xB,None,None,None),
        "sltu"  : InstrR(0,None,None,None,0,0x2B),
        "sll"   : InstrR(0,None,None,None,None,0),
        "srl"   : InstrR(0,None,None,None,None,2),
        "sw"    : InstrI(0x2B,None,None,None),
        "subu"  : InstrR(0,None,None,None,0,0x23),
         }
###
# header
f.write(bin(len(instrs)*4, 32))
f.write(bin(len(data)  *4, 32))
###
# text
for pc, instr in enumerate(instrs):
    if instr[0] in instr_refs:
        ref = instr_refs[instr[0]]
        if type(ref) == InstrR:
            # instr $d, $s, $t
            # op rs rt rd shamt funct
            if instr[0] == "sll" or instr[0] == "srl":
                rd = int(instr[1][1:])
                rs = 0
                rt = int(instr[2][1:])
                shamt = int(instr[3], 0)
            elif instr[0] == "jr":
                rd = ref.rd
                rs = int(instr[1][1:])
                rt = ref.rt
                shamt = ref.shamt
            else:
                rd = int(instr[1][1:])
                rs = int(instr[2][1:])
                rt = int(instr[3][1:])
                shamt = ref.shamt
            _instr = InstrR(ref.op, rs, rt, rd, shamt, ref.funct)
            print(encode(_instr) + '\t' + '\t'.join(instr))
            f.write(encode(_instr))
        elif type(ref) == InstrI:
            # instr $t, $s, C
            # op rs rt immediate
            if instr[0] == "lui":
                rt = int(instr[1][1:])
                rs = 0
                C = int(instr[2],0)
            elif instr[0] == "beq" or instr[0] == "bne":
                rs = int(instr[1][1:])
                rt = int(instr[2][1:])
                # immediate or offset
                if instr[3] in labels:
                    C = labels[instr[3]] - pc - 1
                else:
                    C = int(instr[3],0)
            elif instr[0] == "lw" or instr[0] == "sw":
                rt = int(instr[1][1:])
                C, rs = re.match(r'^(-?\d+)\(\$(\d+)\)', instr[2]).groups()
                C, rs = int(C), int(rs)
            else:
                rt = int(instr[1][1:])
                rs = int(instr[2][1:])
                C  = int(instr[3],0)
            # negative value
            _instr = InstrI(ref.op, rs, rt, C)
            print(encode(_instr) + '\t' + '\t'.join(instr))
            f.write(encode(_instr))
        else:
            # instr C
            # op address
            address = __TEXT__ + labels[instr[1]] * 4
            _instr = InstrJ(ref.op, (address>>2) % (1<<26))
            print(encode(_instr) + '\t' + '\t'.join(instr))
            f.write(encode(_instr))
    else:
        raise NotImplementedError("Unsupported instruction \"%s\"." % instr[0])
###
# data
for datum in data:
    if datum[0] == ".word":
        value = int(datum[1],0)
        if value < 0: value = (1<<32) - value
        f.write(bin(value, 32))

f.close()
