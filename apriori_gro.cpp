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
	//��ʼ������������Դ���õ�ԭʼ���ݼ���Ƶ��1�
	void init(string fileName);
	//����Ƶ��k�������ֱ�Ӽ�֦���õ�Ƶ��k+1������뵽����item_list
	void apri_gen();
	//�жϺ�ѡ���Ƿ�ΪƵ����
	float calculateSup(vector<string> judge_item);
	//�ж�������Ƿ���Ժϲ���һ���µ��
	vector<string> mergeItem(vector<string> vect1, vector<string> vect2, int round);
	//
	void showItem();
private:
	vector<set<string>> datavec;//ԭʼ���ݼ�
	int trancount;//ԭʼ����������
	vector<vector<pair<vector<string>, float>>> frequentvec;//Ƶ����ļ���
	double minsup;//��С֧�ֶ�
	double minconf;//��С���Ŷ�
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
		set<string> item;//�����ʱset
		int begin, end;
		file >> value;//ȥ�����е�item
		value.clear();
		while (getline(file, value))//һ���еĶ�������
		{
			while (!value.empty()) {
				temp = value;
				temp.erase(0, temp.find_first_of("{") + 1);//ȥ���ַ����ײ���{
				temp.erase(temp.find_first_of("}"));//ȥ���ַ���β����}

				value.erase(0, value.find_first_of("}")+3);//ɾ���Ѿ���ӵ��
				trancount++;
				begin = 0;
				while ((end = temp.find(',', begin)) != string::npos) {//ÿ����ĵ������ԣ�Ϊ�ָ�����
					item.insert(temp.substr(begin, end - begin));//��ÿ�������item��
					begin = end + 1;
				}
				item.insert(temp.substr(begin));//���һ����
				datavec.push_back(item);//��һ���������������Ϊһ���������һ�����vector��
				item.clear();
			}
		}
		map<string, int> items_count;//ͳ�Ƹ��������Ŀ
		for (vector < set<string>>::size_type ix = 0; ix != datavec.size(); ++ix) {
			for (set<string>::iterator iy = datavec[ix].begin(); iy != datavec[ix].end(); ++iy) {
				if (items_count.find(*iy) == items_count.end())
					items_count[*iy] = 1;
				else
					items_count[*iy]++;//����ļ���+1

			}
		}
		vector<string> elem;
		vector<pair<vector<string>, float>> candidatevec;//��ѡ��
		for (map<string, int>::iterator ix = items_count.begin(); ix != items_count.end(); ix++) {
			if ((float(ix->second)) / trancount >= minsup) {//�жϺ�ѡƵ�����еĸ����Ƿ������СƵ����
				elem.push_back(ix->first);
				candidatevec.push_back(make_pair(elem, (float(ix->second)) / trancount));
			}
			elem.clear();
		}
		if (!candidatevec.empty()) {
			frequentvec.push_back(candidatevec);//���õ���Ƶ��һ�����Ƶ�����
			candidatevec.clear();
		}
	}
}

vector<string> Apriori::mergeItem(vector<string> vect1, vector<string> vect2, int round) {
	//��֦
	int count = 0;//ͳ������vector����ͬ�����Ŀ
	vector<string> vect;
	map<string, int> tempMap;//�����ж�����vector���ظ�����
	for (vector<string>::size_type st = 0; st < vect1.size(); st++) {
		tempMap[vect1[st]]++;
		vect.push_back(vect1[st]);
	}
	for (vector<string>::size_type st = 0; st < vect2.size(); st++) {
		tempMap[vect2[st]]++;
		if (tempMap[vect2[st]] == 2) {//��ʾ����������ͬ
			count++;
		}
		else {
			vect.push_back(vect2[st]);
		}
	}
	if ((count + 1) != round) {//Ҫ����������Ŀ��ֻ��һ����Ŀ����ͬ����������ͬ
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
	while (st != frequentvec.size()) {//ѭ���ڴﵽƵ�����ĩβ����
		int i = 0;
		if (frequentvec[round - 1].size() < 2) {//��ǰ�ִε�Ƶ��round�����ĸ���С��2���㷨����
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
	cout << "֧�ֶ�" << minsup << "���Ŷ�" << minconf << endl;
	for (vector<vector<pair<vector<string>, float>>>::size_type st1 = 0; st1 != frequentvec.size(); st1++) {
		cout << "Ƶ��" << st1 + 1 << "�" << endl;
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
	cout << "������ʱ" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	system("pause");
	return 0;
}