/*
Pascal Lexical Analyzer based on C/C++
Author: Wang Yuwei
Operating Environment: VS2013
*/
#include <cmath>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;

const unsigned __int64 INF = ((1ull << 64) - 1);
const string str[33] =
{ "", "PROGRAM", "CONST", "VAR", "INTEGER", "LONG", "PROCEDURE", "IF", "THEN",
"WHILE", "DO", "READ", "WRITE", "BEGIN", "END", "ODD", "+", "-", "*", "/", "=",
"<>", "<", "<=", ">", ">=", ".", ",", ";", ":", ":=", "(", ")" };

map <string, int> myMap;
map <string, int> symMap;
vector <pair<string, string> > myVct;
int row, col;
int cntSym;

void GetMap() {
	/*
	put all the reserve words and symbol in myMap
	*/
	for (int i = 1; i <= 32; ++i) {
		myMap[str[i]] = i;
	}
	myMap["#"] = 35;
}
int GetValue(string &str) {
	/*
	return 0: normal ending
	return -1: find a number beyond the max uint64
	*/

	/*
	delete the leading 0
	*/
	while (true) {
		if (str.size() <= 1 || str[0] != '0') {
			break;
		}
		str = str.substr(1);
	}
	/*
	check if it is beyond the largeset uint64
	yes: return the error code -1
	*/
	ostringstream oss;
	oss << INF;
	string strInf = oss.str();
	if ((int)str.size() >= 21 || ((int)str.size() == 20 && str > strInf)) {
		return -1;
	}
	return 0;
}
int GetPair(string sym) {
	/*
	return 0: normal ending
	return -1: find a number beyond the max uint64
	*/
	int value = myMap[sym];
	pair<string, string> myPr;
	/*
	check if it is in myMap
	*/
	if (value > 0) {
		ostringstream oss;
		oss << value;
		myPr.first = oss.str();
	}
	/*
	check whether it is number or variable
	*/
	else if (isdigit(sym[0]) != 0) {
		string ret;
		if (GetValue(sym) == -1) {
			return -1;
		}
		myPr.first = "33";
	}
	/*
	it is known variable
	no need to write down
	*/
	else if (symMap[sym.substr(0, min(20, (int)sym.size()))] != 0) {
		return 0;
	}
	/*
	it is unknown variable
	write down it in symMap
	lexicographic order for binary search
	*/
	else {
		symMap[sym.substr(0, min(20, (int)sym.size()))] = cntSym++;
		myPr.first = "34";
	}
	myPr.second = sym;
	myVct.push_back(myPr);
	return 0;
}
bool Match(const char &a, const char &b) {
	/*
	check whether it is double-symbol word or single
	return true: double-symbol
	return false: single-symbol
	*/
	if (a == ':' && b == '=') {
		return true;
	}
	if (a == '>' && b == '=') {
		return true;
	}
	if (a == '<' && (b == '>' || b == '=')) {
		return true;
	}
	return false;
}
int Input() {
	/*
	return 0: normal ending
	return -1: find an unknown symbol
	return -2: find a number beyond the max uint64
	*/
	string sub, input;
	while (getline(cin, sub)) {
		input += sub;
		input += "\n";
	}
	row = col = 1;
	cntSym = 1;
	/*
	preHead is the relative loction of the lastest '\n'
	as for get the loction of current column
	*/
	int head, tail, preHead = -1;
	for (head = 0; head < (int)input.size(); head = tail) {
		/*
		ch is space(' ' or '\t' or '\r' or '\n' or '\v' or '\f')
		do until ch is not space
		*/
		if (isspace(input[head]) != 0) {
			if (input[head] == '\n') {
				++row;
				preHead = head;
			}
			for (tail = head + 1; tail < (int)input.size(); ++tail) {
				if (input[tail] == '\n') {
					++row;
					preHead = tail;
				}
				if (isspace(input[tail]) == 0) {
					break;
				}
			}
		}
		/*
		ch is letter
		do until ch is not letter or number
		*/
		else if (isalpha(input[head]) != 0) {
			for (tail = head + 1; tail < (int)input.size(); ++tail) {
				if (isalnum(input[tail]) == 0) {
					break;
				}
			}
		}
		/*
		ch is number
		do until ch is not number
		*/
		else if (isdigit(input[head]) != 0) {
			for (tail = head + 1; tail < (int)input.size(); ++tail) {
				if (isdigit(input[tail]) == 0) {
					break;
				}
			}
		}
		/*
		ch is known symbol
		check the next symbol
		*/
		else if (myMap[input.substr(head, 1)] > 0) {
			if (head != (int)input.size() - 1 && Match(input[head], input[head + 1]) == true) {
				tail = head + 2;
				col += 2;
			}
			else {
				tail = head + 1;
			}
		}
		/*
		ch is unkown symbol
		error information
		*/
		else {
			col = head - preHead;
			return -1;
		}
		if (isspace(input[head]) == 0) {
			if (GetPair(input.substr(head, tail - head)) == -1) {
				col = head - preHead;
				return -2;
			}
		}
	}
	return 0;
}
void Output(const int &info) {
	if (info == 0) {
		if (myVct.size() <= 0) {
			cout << "no input" << endl;
			return;
		}
		vector <pair<string, string> > ::iterator itVct;
		map <string, int> ::iterator itMap;
		cout << "Lexical analyze successful" << endl;
		/*
		internal code output
		*/
		for (itVct = myVct.begin(); itVct != myVct.end(); ++itVct) {
			cout << "< " << itVct->first << ", " << itVct->second << " >" << endl;
		}
		/*
		variable output
		*/
		cout << endl;
		cout << symMap.size() << " variable" << endl;
		for (itMap = symMap.begin(); itMap != symMap.end(); ++itMap) {
			cout << "{ " << itMap->first << ", " << itMap->second << " }" << endl;
		}
		return;
	}
	cout << "Lexical analyze failed" << endl;
	if (info == -1) {
		cout << "Unknown symbol at ( " << row << ", " << col << " )" << endl;
		return;
	}
	else if (info == -2) {
		cout << "Too large number begin at ( " << row << ", " << col << " )" << endl;
		return;
	}
	else {
		cout << "Unknown error" << endl;
	}
}
void Init() {
	myMap.clear();
	symMap.clear();
	myVct.clear();
}
int main() {
	freopen("in.txt", "r", stdin);
	freopen("out.txt", "w", stdout);
	Init();
	GetMap();
	cout << endl;
	Output(Input());
	cout << endl;
	//system("pause");
	//fclose(stdin);
	//fclose(stdout);
	return 0;
}