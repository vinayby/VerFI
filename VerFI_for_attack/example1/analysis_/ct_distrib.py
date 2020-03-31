#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#

"""

"""
from scipy import stats
import seaborn as sns
import matplotlib.pyplot as plt
import pdb
import sys
"""
Usage: ./THIS [cm_|o_]
"""
def print2f_dist(x, out_dir, name, index):
   # sns_plot = sns.distplot(x, kde=True, fit=stats.uniform) 
    sns_plot = sns.distplot(x, hist=True, fit=stats.uniform, bins=16) 
    fname = '/tmp/{}/{}_{}_.png'.format(out_dir, name, index)
    sns_plot.figure.savefig(fname)
    sns_plot.figure.clf()

def analysis(ll, out_dir, name):
    print("name={}\tlen={}\toutdir={}".format( name, len(ll), out_dir))
    lhex = [int(l, 16) for l in ll]
    for nibble in range(0, 64//4):
        x = []
        for ct in lhex:
            x.append(ct&0xf)
        print2f_dist(x, out_dir, name, nibble)
        lhex = [l>>4 for l in lhex]

sns.set(color_codes=True)

def do_thing(out_dir, resultFile):
    with open(resultFile, 'r') as fh:
        ll = fh.readlines()
        ll = [l.strip().split() for l in ll]
        
        def get_cointoss_value(line):
            def msb_bit(twenty1nibble):
                x = int(twenty1nibble[0],16) 
                return x & 1;
            r = 0 if len(line[1]) == 20 else msb_bit(line[1])
            return r
        
        ll = [(l[10][4:], l[12], l[4][4:], get_cointoss_value(l)) for l in ll] # faultySelectCC, ineff?, fault-free, cointoss value

        ol =  [e[0] for e in ll if e[1] == '1'] 
        analysis(ol, out_dir, 'r30ineff')
        if out_dir == 'xcm_':
            ol =  [e[0] for e in ll if e[1] == '1' and e[3] == 1] 
            analysis(ol, out_dir + '1_', 'r30ineff')
            ol =  [e[0] for e in ll if e[1] == '1' and e[3] == 0] 
            analysis(ol, out_dir + '0_', 'r30ineff')
        ol = [e[2] for e in ll]
        #analysis(ol, out_dir, 'r30all')
def do_things():
    #input: beed7b6eb25c5406cd75 (fault-free) data_o: 64'hdb2083ff911b3373 (faulty) data_o: 64'hdb2083ff911b3373 (faultySelectCC) data_o: 64'ha2e7d15c321692c8 ineffective: 1
    do_thing('o_', 'o_/Result1.txt')
    do_thing('cm_', 'cm_/Result1.txt')

if sys.argv[1] == 'o_':
    do_thing('o_', 'o_/Result1.txt')   

if sys.argv[1] == 'cm_':
    do_thing('cm_', 'cm_/Result1.txt')   

if len(sys.argv) == 1:
    do_things()

 
