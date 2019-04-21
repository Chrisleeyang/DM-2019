#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <set>
#include <utility>
#include <time.h>

using namespace std;

class Apriori {
public:
	//初始化，输入数据源，得到原始数据集、频繁1项集
	void init(string fileName);
	//连接频繁k项集、并且直接剪枝，得到频繁k+1项集，加入到容器item_list
	void apri_gen();
	//判断候选项是否为频繁项
	float calculateSup(vector<string> judge_item);
	//判断两个项集是否可以合并成一个新的项集
	vector<string> mergeItem(vector<string> vect1, vector<string> vect2, int round);
	//
	void showItem();
private:
	vector<set<string>> datavec;//原始数据集
	int trancount;//原始数据项数量
	vector<vector<pair<vector<string>, float>>> frequentvec;//频繁项集的集合
	double minsup;//最小支持度
	double minconf;//最小置信度
};

void Apriori::init(string fileName) {
	//to be changed
	minsup = 0.05;
	minconf = 0.10;
	trancount = 0;
	ifstream file(fileName);
	if (!file) {
		cout << "fail to open data file" << endl;
	}
	else {
		string temp;
		string value;
		set<string> item;//项集的临时set
		int begin, end;
		file >> value;//去除首行的item
		value.clear();
		while (getline(file, value))//一行行的读入数据
		{
			while (!value.empty()) {
				temp = value;
				temp.erase(0, temp.find_first_of("{") + 1);//去除字符串首部的{
				temp.erase(temp.find_first_of("}"));//去除字符串尾部的}

				value.erase(0, value.find_first_of("}")+3);//删除已经添加的项集
				trancount++;
				begin = 0;
				while ((end = temp.find(',', begin)) != string::npos) {//每个项集的的项是以，为分隔符的
					item.insert(temp.substr(begin, end - begin));//将每个项插入item中
					begin = end + 1;
				}
				item.insert(temp.substr(begin));//最后一个项
				datavec.push_back(item);//讲一个事务的所有项作为一个整体插入一个大的vector中
				item.clear();
			}
		}
		map<string, int> items_count;//统计各个项集的数目
		for (vector < set<string>>::size_type ix = 0; ix != datavec.size(); ++ix) {
			for (set<string>::iterator iy = datavec[ix].begin(); iy != datavec[ix].end(); ++iy) {
				if (items_count.find(*iy) == items_count.end())
					items_count[*iy] = 1;
				else
					items_count[*iy]++;//该项集的计数+1

			}
		}
		vector<string> elem;
		vector<pair<vector<string>, float>> candidatevec;//候选集
		for (map<string, int>::iterator ix = items_count.begin(); ix != items_count.end(); ix++) {
			if ((float(ix->second)) / trancount >= minsup) {//判断候选频繁集中的各项是否大于最小频繁度
				elem.push_back(ix->first);
				candidatevec.push_back(make_pair(elem, (float(ix->second)) / trancount));
			}
			elem.clear();
		}
		if (!candidatevec.empty()) {
			frequentvec.push_back(candidatevec);//将得到的频繁一项集加入频繁项集中
			candidatevec.clear();
		}
	}
}

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

float Apriori::calculateSup(vector<string> judgeVect) {
	int count = 0;
	vector<string>::iterator iter;
	vector<string>::iterator iterBegin = judgeVect.begin();
	vector<string>::iterator iterEnd = judgeVect.end();
	for (vector<set<string>>::size_type st = 0; st != datavec.size(); st++) {
		iter = iterBegin;
		for (; iter != iterEnd; iter++) {
			if (datavec[st].find(*iter) == datavec[st].end())
				break;
		}
		if (iter == iterEnd)
			count++;
	}
	float frequent = (float)count / trancount;
	return frequent;
}

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
			for (iy = ix+1; iy != frequentvec[round - 1].size(); iy++) {
				tempItem = mergeItem(frequentvec[round - 1].at(ix).first, frequentvec[round - 1].at(iy).first, round);
				if (!tempItem.empty()) {
					fre = calculateSup(tempItem);
					if (fre >= minsup) {
						tempVec.push_back(make_pair(tempItem, fre));
					}
				}
			}
		}
		if (!tempVec.empty()) {
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

void Apriori::showItem() {
	cout << "支持度" << minsup << "置信度" << minconf << endl;
	for (vector<vector<pair<vector<string>, float>>>::size_type st1 = 0; st1 != frequentvec.size(); st1++) {
		cout << "频繁" << st1 + 1 << "项集" << endl;
		for (vector<pair<vector<string>, float>>::size_type st2 = 0; st2 != frequentvec[st1].size(); st2++) {
			for (vector<string>::size_type st3 = 0; st3 != frequentvec[st1].at(st2).first.size(); st3++) {
				cout << frequentvec[st1].at(st2).first[st3] << ",";
			}
			cout << ":" << frequentvec[st1].at(st2).second;
			cout << endl;
		}

	}
}

int main() {
	time_t startTime, endTime;
	startTime = clock();
	string infilename=".\\Groceries.csv";
	Apriori calculation;
	calculation.init(infilename);
	calculation.apri_gen();
	calculation.showItem();
	endTime = clock();
	cout << "程序用时" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	system("pause");
	return 0;
}