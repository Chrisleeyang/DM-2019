#!/usr/bin/env python
# encoding: utf-8
"""
Testing code for the FP-growth implementation.
"""
import time
import fp_growth as fpg

if __name__ == '__main__':
    minsup = float(input("Input min support:"))
    infile = open("UNIX_usage/unix_all.981115184025",mode='r')
    with open('unix_data.csv','w')as outfile:
        startOfLine = False
        for line in infile.readlines():
            content = line.strip('\n')
            if content == "**SOF**":
                startOfLine = True;
            elif content == "**EOF**":
                outfile.write('\n')
            else:
                if startOfLine == True:
                    if content != "<1>" and content != "<2>" and content != "<3>":
                        outfile.write(content)
                    startOfLine = False
                else:
                    if content != "<1>" and content != "<2>" and content != "<3>":
                        outfile.write(',')
                        outfile.write(content)
        outfile.close()

    items = []
    t0 = time.time()
    csvfile  = open("unix_data.csv",mode='r')
    for line in csvfile.readlines():
        line = line.strip().rstrip('\n')
        filtered_line = line.split(',')
        ss=[]
        for i in filtered_line:
            if i not in ss:
                ss.append(i)
            else:
                pass
        items.append(ss)
    minsup = minsup*len(items)

    frequent_itemsets = fpg.find_frequent_itemsets(items,minsup,include_support=True)
    print (type(frequent_itemsets))

    res = []
    for itemset,support in frequent_itemsets:
        res.append((itemset,support))

    res = sorted(res, key=lambda i: i[0])
    for itemset,support in res:
        support_f = round(float(support)/len(res),3)
        print (str(itemset)+' '+str(support_f))

    print (time.time()-t0)