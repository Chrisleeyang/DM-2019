# **数据挖掘 作业二**
### *161220071   李杨    1103266550@qq.com*

### 1. **实验介绍**
> 1.1 问题介绍：
- 将数据挖掘的相关知识应用到给定的两个数据集上
- 通过改变置信度和支持度，比较*Apriori*、*FP-Growth*和暴力搜索挖掘频繁项集，在生成频繁项集、生成规则时所用的内存和时间消耗
- 通过使用*Apriori*或*FP-Growth*发现一些有趣的关联规则，并讨论这些规则的内在逻辑
>1.2 数据集介绍:
- *GroceryStore*：数据以以下格式存储在 *.scv* 文件中，是一家超市的每一个顾客的每次购买的物品记录  
` id {item1,item2,...,itemn} `
- *UNIX*：数据以以下格式存储分别存储在*USER0~USER8*，记录了8为用户登录*UNIX*的每一次会话与指令操作，由\*\**SOF*\*\*开始，\*\**EOF*\*\*结束的一段指令集，作为一条项目  
`
**SOF**  
command 1
command 2
...
command n
**EOF**
`  
### 2.**实验算法**
>2.1 *Aprioro* 算法 
* 2.1.1 算法原理：  
在介绍*Apriori*原理前，有两个*Apriori*性质的推论：  
性质1：频繁项集的所有非空子集都必须是频繁的  
性质2：若频繁K-项集L<sub>k</sub>中各项可以做链接产生L<sub>k+1</sub>,则L<sub>k</sub>中每个元素在L<sub>k</sub>中出现的次数应大于等于k，若小于k,则删除该项在L<sub>k</sub>中的所有事务集  
 *Aprioro*算法流程：  
①首先单趟扫描数据集，计算各个一项集的支持度，根据给定的最小支持度阈值，得到一项频繁集L1  
②然后通过连接运算，对于每个连接的到项直接进行最小支持度判断，如果大于最小支持度的加入频繁二项集，如果小于则舍弃，循环直到连接完毕；得到二项频繁集L2  
③如此进行下去，直到不能连接产生新的频繁项集为止  
 *(伪代码见《数据挖掘概念与技术》原书第三版 P<sub>163~164</sub>)*  
* 2.12 代码实现  
*Apriori*类:  
```c++
class Apriori {
public:
	void init(string fileName); //初始化，输入数据源，得到原始数据集、频繁1项集
	void apri_gen(); //连接频繁k项集、并且直接剪枝，得到频繁k+1项集，加入到容器item_list
	float calculateSup(vector<string> judge_item);	//判断候选项是否为频繁项
	//判断两个项集是否可以合并成一个新的项集
	vector<string> mergeItem(vector<string> vect1, vector<string> vect2, int round); //判断两个项集是否可以合并成一个新的项集
	void showItem();//输出频繁项集
private:
	vector<set<string>> datavec;//原始数据集
	int trancount;//原始数据项数量
	vector<vector<pair<vector<string>, float>>> frequentvec;//频繁项集的集合
	double minsup;//最小支持度
	double minconf;//最小置信度
};
```  
这里给出两个核心主函数：*void apri_gen()*和*vector<string> mergeItem(vector<string> vect1, vector<string> vect2, int round)*  
*void apri_gen()*：连接频繁k项集、并且直接剪枝，得到频繁k+1项集，加入到容器
```c++
void Apriori::apri_gen() {
	int round = 1;
	vector<vector<vector<string>>>::size_type st = 0;
	vector<pair<vector<string>, float>> tempVec;
	vector<string> tempItem;
	float fre = 0.0;
	while (st != frequentvec.size()) {//循环在达到频繁项集的末尾结束
		int i = 0;
		if (frequentvec[round - 1].size() < 2) {//当前轮次的频繁round项集中项的个数小于2，算法结束
			return;
		}
		vector<vector<string>>::size_type ix, iy;
		for (ix = 0; ix != frequentvec[round - 1].size(); ix++) {
			for (iy = ix + 1; iy != frequentvec[round - 1].size(); iy++) {
				tempItem = mergeItem(frequentvec[round - 1].at(ix).first, frequentvec[round - 1].at(iy).first, round);
				if (!tempItem.empty()) {
					fre = calculateSup(tempItem);
					if (fre >= minsup) {//当支持度大于最小支持度，加入容器
						tempVec.push_back(make_pair(tempItem, fre));
					}
				}
			}
		}
		if (!tempVec.empty()) {//直至容器为空，即不存在更大的频繁项集
			frequentvec.push_back(tempVec);
			tempItem.clear();
			tempVec.clear();
			round++;
			st++;
		}
		else
			return;
	}
}
```  
*vector<string> mergeItem(vector<string> vect1, vector<string> vect2, int round)*：判断两个项集是否可以合成一个新的项集:
```c++
vector<string> Apriori::mergeItem(vector<string> vect1, vector<string> vect2, int round) {
	//剪枝
	int count = 0;//统计两个vector中相同项的数目
	vector<string> vect;
	map<string, int> tempMap;//辅助判断两个vector中重复的项
	for (vector<string>::size_type st = 0; st < vect1.size(); st++) {
		tempMap[vect1[st]]++;
		vect.push_back(vect1[st]);
	}
	for (vector<string>::size_type st = 0; st < vect2.size(); st++) {
		tempMap[vect2[st]]++;
		if (tempMap[vect2[st]] == 2) {//表示这两个项相同
			count++;
		}
		else {
			vect.push_back(vect2[st]);
		}
	}
	if ((count + 1) != round) {//要求链各个项目集只有一个项目不相同，其他都相同
		vect.clear();
	}
	sort(vect.begin(), vect.end());
	return vect;
}
```  
>2.2 *FP-Growth*算法
* 2.2.1 算法原理：  
*FP-Growth*算法采取如下分治策略：将提供频繁项集的数据库压缩到一棵频繁模式树（FP-Tree），但仍保留项集关联信息；该算法和Apriori算法最大的不同有两点：第一，不产生候选集，第二，只需要两次遍历数据库，大大提高了效率  
*FP-Growth*的一般流程如下：  
    ① 先扫描一遍数据集，得到频繁项为1的项目集，定义最小支持度（项目出现最少次数），删除那些小于最小支持度的项目，然后将原始数据集中的条目按项目集中降序进行排列  
    ② 第二次扫描，创建项头表（从上往下降序），以及FP树  
    ③ 对于每个项目（可以按照从下往上的顺序）找到其条件模式基（*CPB，conditional patten base*）,递归调用树结构，删除小于最小支持度的项。如果最终呈现单一路径的树结构，则直接列举所有组合；非单一路径的则继续调用树结构，直到形成单一路径即可  
* 2.2.2 代码实现：  
首先定义的一个FPNode节点：  
``` python
    def __init__(self, tree, item, count=1):
        self._tree = tree #树节点
        self._item = item #项集
        self._count = count #出现次数
        self._parent = None  #父节点
        self._children = {}  #子节点
        self._neighbor = None  #邻居节点

    def add(self, child):
        #Add the given FPNode `child` as a child of this node.

        if not isinstance(child, FPNode):
            raise TypeError("Can only add other FPNodes as children")

        if not child.item in self._children:
            self._children[child.item] = child
            child.parent = self

    def search(self, item):
        #Check whether this node contains a child node for the given item.If so, that node is returned; otherwise, `None` is returned.
        try:
            return self._children[item]
        except KeyError:
            return None

    def __contains__(self, item):
        return item in self._children
```  
*find_frequent_itemsets(transactions, minimum_support, include_support=False)*:对于给定的数据，应用*FP-Growth*寻找频繁项集
``` python
def find_frequent_itemsets(transactions, minimum_support, include_support=False):
 
    items = defaultdict(lambda: 0) # mapping from items to their supports

    # Load the passed-in transactions 
    for transaction in transactions:
        for item in transaction:
            items[item] += 1

    # Remove infrequent items 
    items = dict((item, support) for item, support in items.iteritems()
        if support >= minimum_support)

    # Build FP-tree and surviving items must be sorted in decreasing order of frequency.
    def clean_transaction(transaction):
        transaction = filter(lambda v: v in items, transaction)
        transaction.sort(key=lambda v: items[v], reverse=True)
        return transaction

    master = FPTree()
    for transaction in imap(clean_transaction, transactions):
        master.add(transaction)

    def find_with_suffix(tree, suffix):
        for item, nodes in tree.items():
            support = sum(n.count for n in nodes)
            if support >= minimum_support and item not in suffix:
                # New winner!
                found_set = [item] + suffix
                yield (found_set, support) if include_support else found_set

                # Build a conditional tree and recursively search for frequent itemsets within it.
                cond_tree = conditional_tree_from_paths(tree.prefix_paths(item))
                for s in find_with_suffix(cond_tree, found_set):
                    yield s # pass along the good news to our caller

    # Search for frequent itemsets, and yield the results we find.
    for itemset in find_with_suffix(master, []):
        yield itemset
```  
> 2.3 *Brute Force*  
* 2.3.1 算法原理：  
BF算法，即暴风(Brute Force)算法，是普通的模式匹配算法，BF算法的思想就是将目标串S的第一个字符与模式串T的第一个字符进行匹配，若相等，则继续比较S的第二个字符和 T的第二个字符；若不相等，则比较S的第二个字符和T的第一个字符，依次比较下去，直到得出最后的匹配结果。BF算法是一种蛮力算法。  
* 2.3.2 代码实现：
```python
start = time.time()
dataSet = loadData()
minSup = 0.05
minSup = minSupRate * len(dataSet)
maxItemLength = max([len(each) for each in dataSet])
allItems = []
for i in dataSet:
    for j in i:
        if not j in allItems:
            allItems.append(j)
dic = {}
for i in range(maxItemLength):
    nr = i + 1
    for each in combinations(allItems, nr):
        if not each in dic.keys():
            dic[each] = 1
        else:
            dic[each] += 1
for each in list(filter(lambda x: dic[x] > minSup, dic.keys())):
    print(each, dic[each])
end = time.time()
print((end - start).seconds)
```
### 3.**实验环境与数据处理**  
> 3.1 实验环境：
* 3.1.1 *Apriori算法*：VS 2017 运用*c++*
* 3.1.2 *FP-Growth算法*：PyCharm 运用*python2.8*
* 3.1.3 *Brute Force算法*：PyCharm 运用*python2.8*  
> 3.2 数据处理:
* 3.2.1 *Groceries.csv*：对于*Groceries.csv*，由于该文件本身就是表格形式，只需要在读入数据的时候，对数据格式进行修改即可，具体操作为，在每读入一条数据后，删去其‘{’之前的所有字符，并删去‘}’之后的所有字符，剩下的即为多个item组成的一个项集，再依据每个item之间以‘，’为分割符，可以从每个项集中抽出每个item并加入到容器之中
* 3.2.2 *UNIX_usage*：对于*UNIX_usage*，由于其从\*\**SOF*\*\*开始，到\*\**EOF*\*\*结束的所有区间的指令集，作为一条项目，所有要对文件进行重新操作。具体操作为，从源文件读入数据，每一个\*\**SOF*\*\*~\*\**EOF*\*\*为一条项目，插入一个备用的表格形式的文件中；再全部转换后，使用备用文件读取数据，其操作与*Groceries.csv*一致  
### 4.**实验结果**  
> 4.1 *Apriori算法* ：
* 4.1.1 *Groceries*  
<table>
  <tr>
    <th>支持度</th>
    <th>置信度</th>
    <th>频繁项集数</th>
	<th>耗时</th>
	<th>内存消耗</th>
  </tr>
  <tr>
    <td>0.05</td>
    <td>0.10</td>
    <td>31</td>
	<td>27.243s</td>
	<td>18MB</td>
  </tr>
  <tr>
    <td>0.05</td>
    <td>0.20</td>
    <td>27</td>
	<td>28.199s</td>
	<td>16MB</td>
  </tr>
    <tr>
    <td>0.10</td>
    <td>0.10</td>
    <td>8</td>
	<td>4.929s</td>
	<td>18MB</td>
  </tr>
  <tr>
    <td>0.10</td>
    <td>0.20</td>
    <td>8</td>
	<td>5.307s</td>
	<td>18MB</td>
  </tr>
  <tr>
    <td>0.15</td>
    <td>0.10</td>
    <td>4</td>
	<td>3.522s</td>
	<td>16MB</td>
  </tr>
</table> 

* 4.1.2 *UNIX*  
<table>
  <tr>
    <th>USER</th>
	<th>耗时</th>
	<th>内存消耗</th>
  </tr>
  <tr>
    <td>0</td>
	<td>1.373s</td>
	<td>4MB</td>
  </tr>
  <tr>
    <td>1</td>
	<td>1.37s</td>
	<td>9MB</td>
  </tr>
    <tr>
    <td>2</td>
	<td>2.252s</td>
	<td>8MB</td>
  </tr>
  <tr>
    <td>ALL</td>
	<td>21.102s</td>
	<td>93MB</td>
  </tr>
</table>  

> 4.2 *FP-Growth算法*
* 4.2.1 *Groceries*  
<table>
  <tr>
    <th>支持度</th>
    <th>频繁项集数</th>
	<th>耗时</th>
	<th>内存消耗</th>
  </tr>
  <tr>
    <td>0.01</td>
    <td>333</td>
	<td>1.411s</td>
	<td>72.5MB</td>
  </tr>
  <tr>
    <td>0.05</td>
    <td>31</td>
	<td>0.062s</td>
	<td>46.5MB</td>
  </tr>
    <tr>
    <td>0.10</td>
    <td>8</td>
	<td>0.390s</td>
	<td>18.9MB</td>
  </tr>    
</table>

* 4.2.2 *UNIX* 
<table>
  <tr>
    <th>USER</th>
	<th>耗时</th>
	<th>内存消耗</th>
  </tr>
  <tr>
    <td>0</td>
	<td>0.014s</td>
	<td>38.6MB</td>
  </tr>
  <tr>
    <td>1</td>
	<td>0.082s</td>
	<td>46.3MB</td>
  </tr>
    <tr>
    <td>2</td>
	<td>0.025s</td>
	<td>42.9MB</td>
  </tr>
</table> 

>*Brute Force算法*  
  * 由于暴力搜索太过朴素、数据集过大，会导致产生的频繁集过多，而使得程序崩溃，故无法估计该算法的时间和空间代价   

### 5.**关联规则**
> 5.1 *Groceries*  

利用*Apriori*算法，支持度为0.05、置信度为0.10时，得到以下数据：
```text
频繁2项集
other vegetables,whole milk,:0.0748348
rolls/buns,whole milk,:0.0566345
whole milk,yogurt,:0.0560244
```
再查询一项集中的对应数据，利用置信度公式，可以手动推导出以下的内容：
```text
other vegetables ==> whole milk, 0.387
whole milk ==> other vegetables, 0.293
rolls/buns ==> whole milk, 0.308
whole milk ==> rolls/buns, 0.222
whole milk ==> yogurt, 0.219
yogurt ==> whole milk, 0.402
```
再根据上述数据排序，可以得出以下结论：  
* 买酸奶的人，大概率会同时购买牛奶
* 买蔬菜的人，大概率会同时购买牛奶
* 买面包、小蛋糕的人，大概率同时购买牛奶  
>5.2 *UNIX*  

先使用cat函数将9个文件合并之后，利用*FP-Growth*算法，在支持度为0.1时，得到以下数据:  
```text
[''] 145.571
['cd'] 262.571
['cd', 'ls'] 167.714
['cd', 'ls', 'vi'] 87.5
['cd', 'rm'] 90.0
['cd', 'vi'] 122.143
['elm'] 191.214
['exit'] 141.143
['ll'] 79.643
['ls'] 201.857
['ls', 'vi'] 95.286
['rm'] 102.857
['vi'] 139.0
['z'] 136.714
```  
再对上述数据进行分析，可以大致得出以下结论：  
* 'cd'和'ls': 说明用户会经常查看目录并进入文件 或者 用户会经常进入某一文件，并查看其子目录  
* 'cd'、'ls'和'vi': 同上，在上述基础上，会同时存在编辑文件操作
* 'cd'和'rm'：进入某一目录，并进行文件删除操作
* 'ls'和'vi'： 查看目录，并新建文件  

### 6.**实验总结**  
对于*Apriori*算法和*FP-Growth*算法，由于我使用的是两种不同的语言实现的，其对比性有一定的减弱，但还是可以得到一定的结论。
* *Apriori*算法，在时间消耗上，远大于*FP-Growth*算法，但其优点是占用的内存较*FP-Growth*算法会少很多。这是因为*Apriori*算法需要在每次生成候选集时遍历一次频繁项集，在递归中花费很多时间
* *FP-Growth*算法，在内存消耗上，远大于*Apriori*算法，但其优点是花费的时间少。这是因为*FP-Growth*算法需要先建立FP-tree，所以需要消耗更多内存，但在生成候选集时，由于省去了遍历频繁集的过程，会使得时间大大减小

> 结论：*Apriori*算法空间代价小，*FP-Growth*算法时间代价小；但由于在增加的空间消耗并不是量级的提升，综上*FP-Growth*算法的效率更高  

### **参考**：
注：
* *FP-Growth实现*是直接使用github上面的源码，自己实现了输入与输出
https://github.com/enaeseth/python-fp-growth 
* 提交源码包括*apriori_gro.cpp*、*apriori_unix.cpp*、*test_gro.py*、*test_unxi.py*
