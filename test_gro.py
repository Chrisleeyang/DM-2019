#!/usr/bin/env python
# encoding: utf-8
"""
Testing code for the FP-growth implementation.
"""
import time
import fp_growth as fpg

if __name__ == '__main__':
    fname = raw_input("Input file name:")
    minsup = float(input("Input min support:"))
    num_list = []
    items_list = []
    infile = open(fname,mode='rU')
    t0 = time.time()
    for line in infile.readlines():
        items_list.append(line.strip().split('",')[1])
    items_list.pop(0)
    for i in range(0,len(items_list)):
        items_list[i] = items_list[i][2:].strip('}"')
    result =[]
    for line in items_list:
        record = line.split(',')
        result.append(record)
    minsup = minsup*len(result)
    frequent_itemsets = fpg.find_frequent_itemsets(result,minsup,include_support=True)
    print (type(frequent_itemsets))

    res = []
    for itemset,support in frequent_itemsets:
        res.append((itemset,support))

    res = sorted(res, key=lambda i: i[1])

    for itemset,support in res:
        support_f = round(float(support)/len(res),3)
        print (str(itemset)+' '+str(support_f))

    print(time.time()-t0)