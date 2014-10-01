#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <map>
#include <strings.h>
#include <streambuf>
#include <sstream>
#include <queue>
using namespace std;

string PC = "";
bool BASE_ON = false;
string BASE_address = "";
vector<pair<string, string> > object_code;
map<string, string> optab;
map<string, pair<string, string> > SYMTAB;
vector<string> for_list_file;
bool mnemonics_loaded = false;
bool symtab_loaded = false;
string program_name;
string fixSixDigitHexa(string hexa) {
	int size = 6 - hexa.size();
	string fill = "";
	for (int i = 0; i < size; i++) {
		fill += '0';
	}
	hexa = fill + hexa;
	return hexa;
}

bool equalStrings(string str1, string str2) {
	if (str1.size() != str2.size())
		return false;
	for (int i = 0; i < (int) str1.size(); i++) {
		if (str1[i] != str2[i])
			return false;
	}
	return true;
}

string toLower(string str) {

	string s = "";
	for (int i = 0; i < (int) str.length(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			s += (char) (str[i] - 'A' + 'a');
		} else
			s += str[i];
	}
	return s;
}

string reverseString(string str) {
	string reversed = "";
	for (int i = str.length() - 1; i > -1; i--) {
		reversed += str[i];
	}
	return reversed;
}

string intToBinary(int number) {
	string binary = "";
	while (number > 0) {
		if (number % 2 == 1)
			binary += '1';
		else
			binary += '0';
		number = number >> 1;
	}
	binary = reverseString(binary);
	return binary;
}

string invertBits(string bits) {
	for (int i = 0; i < (int) bits.length(); i++) {
		if (bits[i] == '1') {
			bits[i] = '0';
		} else {
			bits[i] = '1';
		}
	}
	return bits;
}

string increment(string binaryNumber) {
	for (int i = (binaryNumber.length() - 1); i > -1; i--) {
		if (binaryNumber[i] == '1') {
			binaryNumber[i] = '0';
		} else {
			binaryNumber[i] = '1';
			break;
		}
	}
	return binaryNumber;
}

/*this method takes the displacement
 * without caring about pc/base relativeness
 * returns the binary string according to the
 * format in the parameter
 * >>IT RETURNS AN EMPTY STRING IF THERE IS AN ERROR<<
 * */
string dispToString(int disp, int format) {
	if (format == 4) {
		string binaryStr = intToBinary(disp);
		if (binaryStr.length() <= 20 && disp >= 0) {
			string zeros = "";
			for (int i = 0; i < (20 - binaryStr.length()); i++) {
				zeros += '0';
			}
			binaryStr = zeros + binaryStr;
			return binaryStr;
		} else
			return "";
	} else if (format == 3) {
		if (disp >= 0) {
			string binaryStr = intToBinary(disp);
			if (binaryStr.length() <= 12) {
				string zeros = "";
				for (int i = 0; i < (12 - binaryStr.length()); i++) {
					zeros += '0';
				}
				binaryStr = zeros + binaryStr;
				return binaryStr;
			} else
				return "";
		} else {
			disp *= (-1);
			if (disp <= 2048) {
				string binaryStr = intToBinary(disp);
				string zeros = "";
				for (int i = 0; i < (12 - binaryStr.length()); i++) {
					zeros += '0';
				}
				binaryStr = zeros + binaryStr;
				binaryStr = invertBits(binaryStr);
				binaryStr = increment(binaryStr);
				return binaryStr;
			} else {
				return "";
			}
		}
	} else
		return "";
}

//string dispToString(int disp, int format) {
//	return "";
//}

string get(string form, int index) {
	if (form.at(index) == '0')
		return "0";
	else
		return "1";

}

string toHexa(int in) {
	if (in == 0)
		return "0";
	string str;
	char current;
	bool is_negative = false;

	if (in < 0) {
		in *= -1;
		is_negative = true;
	}

	while (in > 0) {
		int rem = in % 16;

		if (rem == 10)
			current = 'A';
		else if (rem == 11)
			current = 'B';
		else if (rem == 12)
			current = 'C';
		else if (rem == 13)
			current = 'D';
		else if (rem == 14)
			current = 'E';
		else if (rem == 15)
			current = 'F';
		else
			current = (char) (rem + '0');

		str += current;

		in /= 16;

	}

	string ans = "";
	for (int i = str.length() - 1; i >= 0; i--)
		ans += str[i];

	if (is_negative) {
		ans = "-" + ans;
	}
	return ans;
}

bool isHexa(string str) {
	for (int i = 0; i < (int) str.length(); i++) {
		if (!((str[i] >= '0' && str[i] <= '9')
				|| (str[i] >= 'A' && str[i] <= 'F')))
			return false;
	}
	return true;
}

int toInt(string num) {

	int ans = 0;
	int p = 0;
	bool is_negative = false;
	if (num.at(0) == '-') {
		is_negative = true;
		num = num.substr(1, (int) num.size() - 1);
	}
	//cout << "lo" << endl;

	for (int i = num.length() - 1; i >= 0; i--) {
		int current;
		if (num[i] == 'A' || num[i] == 'a')
			current = 10;
		else if (num[i] == 'B' || num[i] == 'b')
			current = 11;
		else if (num[i] == 'C' || num[i] == 'c')
			current = 12;
		else if (num[i] == 'D' || num[i] == 'd')
			current = 13;
		else if (num[i] == 'E' || num[i] == 'e')
			current = 14;
		else if (num[i] == 'F' || num[i] == 'f')
			current = 15;

		else
			current = num[i] - '0';

		ans += pow(16, p++) * current;
	}
	if (is_negative)
		ans *= -1;
	return ans;
}

string get_binary(char c) {
	if (c == '0')
		return "0000";
	else if (c == '1')
		return "0001";
	else if (c == '2')
		return "0010";
	else if (c == '3')
		return "0011";
	else if (c == '4')
		return "0100";
	else if (c == '5')
		return "0101";
	else if (c == '6')
		return "0110";
	else if (c == '7')
		return "0111";
	else if (c == '8')
		return "1000";
	else if (c == '9')
		return "1001";
	else if (c == 'A' || c == 'a')
		return "1010";
	else if (c == 'B' || c == 'b')
		return "1011";
	else if (c == 'C' || c == 'c')
		return "1100";
	else if (c == 'D' || c == 'd')
		return "1101";
	else if (c == 'E' || c == 'e')
		return "1110";
	else if (c == 'F' || c == 'f')
		return "1111";

	return "error";

}

string hex_to_binary(string s) {

	string ans = "";

	for (int i = 0; i < (int) s.size(); i++)
		ans += get_binary(s.at(i));

	return ans;

}

bool is_special_(vector<string> v, string &s, int &position) {

	for (int i = 0; i < (int) v.size(); i++) {
		if (v.at(i).compare("RSUB") == 0 || v.at(i).compare("LTORG") == 0
				|| v.at(i).compare("NOBASE") == 0 || v.at(i).compare("END") == 0
				|| v.at(i).compare("BASE") == 0 || v.at(i).compare("WORD") == 0
				|| v.at(i).compare("BYTE") == 0 || v.at(i).compare("RESW") == 0
				|| v.at(i).compare("RESB") == 0
				|| v.at(i).compare("EQU") == 0) {
			s = v.at(i);
			position = i;
			return true;
		}

	}
	return false;

}

string get_hexa_from_binary(string s) {

	if (s.compare("0000") == 0)
		return "0";
	else if (s.compare("0001") == 0)
		return "1";
	else if (s.compare("0010") == 0)
		return "2";
	else if (s.compare("0011") == 0)
		return "3";
	else if (s.compare("0100") == 0)
		return "4";
	else if (s.compare("0101") == 0)
		return "5";
	else if (s.compare("0110") == 0)
		return "6";
	else if (s.compare("0111") == 0)
		return "7";
	else if (s.compare("1000") == 0)
		return "8";
	else if (s.compare("1001") == 0)
		return "9";
	else if (s.compare("1010") == 0)
		return "A";
	else if (s.compare("1011") == 0)
		return "B";
	else if (s.compare("1100") == 0)
		return "C";
	else if (s.compare("1101") == 0)
		return "D";
	else if (s.compare("1110") == 0)
		return "E";
	else if (s.compare("1111") == 0)
		return "F";

	return "error";

}

string get_hexa_string_binary(string s) {

	string ans = "";

	for (int ii = 0; ii <= (int) s.size() - 4; ii += 4) {
		string substring = s.substr(ii, 4);
		ans += get_hexa_from_binary(substring);

	}
	return ans;

}

void fill_optab() {

	ifstream optab_file("optab.txt");
	string reader;
	string buff;

	if (optab_file.is_open()) {

		while (getline(optab_file, reader)) {
			if (reader.compare("") == 0)
				continue;

			stringstream ss5(reader);
			vector<string> now_parsed;
			while (ss5 >> buff)
				now_parsed.push_back(buff);

			optab.insert(make_pair(now_parsed.at(0), now_parsed.at(1)));

		}
		mnemonics_loaded = true;

	} else {
		cout << "*** could not load mnemonics" << endl;
	}

}

void fill_symtab() {

	ifstream file("SYMTAB.txt");
	string read;
	string buffer;
	if (file.is_open()) {
		while (getline(file, read)) {
			if (read.compare("") == 0)
				continue;

			stringstream ss6(read);
			vector<string> parsed;
			while (ss6 >> buffer)
				parsed.push_back(buffer);

			SYMTAB.insert(
					make_pair(parsed.at(0),
							make_pair(parsed.at(1), parsed.at(2))));

		}
		symtab_loaded = true;

	} else {
		cout << "*** could not load symtab" << endl;
	}

}

bool is_Digit_string(string s) {

	for (int i = 0; i < (int) s.size(); i++)
		if (!(s.at(i) >= '0' && s.at(i) <= '9'))
			return false;

	return true;

}

/*
 * v.at(i).first = OBJECT code
 * v.at(i).second = line Address
 * first == break --> Resw or Reswb
 * BYTE mnemonic obcode will end with '@'
 * */
//void printObjectCode(vector<pair<string, string> > v, string programName) {
//	pair<string, string> p;
//	pair<string, string> dummy;
//	/************************************************/
//	/*Header Record*/
//	/************************************************/
//	cout << "H" << programName;
//	int fillBlankSize = (6 - programName.size());
//	string fill = "";
//	for (int i = 0; i < fillBlankSize; i++) {
//		fill += ' ';
//	}
//	cout << fill;
//	p = v.at(0);
//	dummy = v.at(v.size() - 1);
//	int endAddressInt = toInt(dummy.second);
//	int startAddressInt = toInt(p.second);
//	int lengthProgramInt = endAddressInt - startAddressInt + 1;
//	string lengthProgStr = toHexa(lengthProgramInt);
//	lengthProgStr = fixSixDigitHexa(lengthProgStr);
//	p.second = fixSixDigitHexa(p.second);
//	cout << p.second << lengthProgStr << endl;
//	/************************************************/
//	/*Text Records*/
//	/************************************************/
//	string startingAddress = "";
//	string lengthRecord = "";
//	string opcode = "";
//	for (int i = 1; i < (int) v.size(); i++) {
//		if (!equalStrings(v.at(i).first, "break")
//				&& v.at(i).first[v.at(i).first.size() - 1] != '@')
//			v.at(i).first = fixSixDigitHexa(v.at(i).first);
//
//		if (!equalStrings(v.at(i).first, "break")
//				&& (opcode.size() + v.at(i).first.size()) <= 60) { // break could be capital letter
//			if (opcode.size() == 0) {
//				v.at(i).second = fixSixDigitHexa(v.at(i).second);
//				startingAddress += v.at(i).second;
//			}
//
//			opcode += v.at(i).first;
//
//		} else {
//			while (equalStrings(v.at(i).first, "break")) {
//				i++;
//			}
//			i--;
//			//print line in file
//
//			lengthRecord = toHexa(opcode.size() / 2);
//			cout << "T" << startingAddress << lengthRecord << opcode << endl;
//
//			/*Initialize the variables*/
//			startingAddress = "";
//			lengthRecord = "";
//			opcode = "";
//		}
//
//	}
//
//	/************************************************/
//	/*End Record*/
//	/************************************************/
//	p = v.at(1);
//	p.second = fixSixDigitHexa(p.second);
//	cout << "E" << p.second << endl;
//}
//void printObjectCode(vector<pair<string, string> > v, string programName) {
//	pair<string, string> p;
//	pair<string, string> dummy;
//	/************************************************/
//	/*Header Record*/
//	/************************************************/
//	cout << "H" << programName;
//	int fillBlankSize = (6 - programName.size());
//	string fill = "";
//	for (int i = 0; i < fillBlankSize; i++) {
//		fill += ' ';
//	}
//	cout << fill;
//	p = v.at(0);
//	dummy = v.at(v.size() - 1);
//	int endAddressInt = toInt(dummy.second);
//	int startAddressInt = toInt(p.second);
//	int lengthProgramInt = endAddressInt - startAddressInt + 1;
//	string lengthProgStr = toHexa(lengthProgramInt);
//	lengthProgStr = fixSixDigitHexa(lengthProgStr);
//	p.second = fixSixDigitHexa(p.second);
//	cout << p.second << lengthProgStr << endl;
//	/************************************************/
//	/*Text Records*/
//	/************************************************/
//	string startingAddress = "";
//	string lengthRecord = "";
//	string opcode = "";
//	for (int i = 0; i < v.size(); i++) {
//		if (!equalStrings(v.at(i).first, "break")
//				&& v.at(i).first[v.at(i).first.size() - 1] != '@')
//			v.at(i).first = fixSixDigitHexa(v.at(i).first);
//
//		if (!equalStrings(v.at(i).first, "break")
//				&& (opcode.size() + v.at(i).first.size()) <= 60) { // break could be capital letter
//			if (opcode.size() == 0) {
//				v.at(i).second = fixSixDigitHexa(v.at(i).second);
//				startingAddress += v.at(i).second;
//			}
//
//			opcode += v.at(i).first;
//
//		} else {
//			while (equalStrings(v.at(i).first, "break")) {
//				i++;
//			}
//			i--;
//			//print line in file
//
//			lengthRecord = toHexa(opcode.size() / 2);
//			cout << "T" << startingAddress << lengthRecord << opcode << endl;
//
//			/*Initialize the variables*/
//			startingAddress = "";
//			lengthRecord = "";
//			opcode = "";
//		}
//
//	}
//
//	/************************************************/
//	/*End Record*/
//	/************************************************/
//	p = v.at(0);
//	p.second = fixSixDigitHexa(p.second);
//	cout << "E" << p.second << endl;
//}
//void printObjectCode(vector<pair<string, string> > v, string programName) {
//	pair<string, string> p;
//	pair<string, string> dummy;
//	/************************************************/
//	/*Header Record*/
//	/************************************************/
//	cout << "H" << programName;
//	int fillBlankSize = (6 - programName.size());
//	string fill = "";
//	for (int i = 0; i < fillBlankSize; i++) {
//		fill += ' ';
//	}
//	cout << fill;
//	p = v.at(0);
//	dummy = v.at(v.size() - 1);
//	int endAddressInt = toInt(dummy.second);
//	int startAddressInt = toInt(p.second);
//	int lengthProgramInt = endAddressInt - startAddressInt + 1;
//	string lengthProgStr = toHexa(lengthProgramInt);
//	lengthProgStr = fixSixDigitHexa(lengthProgStr);
//	p.second = fixSixDigitHexa(p.second);
//	cout << p.second << lengthProgStr << endl;
//	/************************************************/
//	/*Text Records*/
//	/************************************************/
//	string startingAddress = "";
//	string lengthRecord = "";
//	string opcode = "";
//	for (int i = 0; i < v.size(); i++) {
//
//		if (!equalStrings(v.at(i).first, "break")
//				&& (opcode.size() + v.at(i).first.size()) <= 60) {
//			if (opcode.size() == 0) {
//				v.at(i).second = fixSixDigitHexa(v.at(i).second);
//				startingAddress += v.at(i).second;
//			}
//			if (v.at(i).first[v.at(i).first.size() - 1] == '@') {
//				for (int k = 0; k < v.at(i).first.size() - 1; k++) {
//					opcode += v.at(i).first[k];
//				}
//			} else
//				opcode += v.at(i).first;
//
//		} else {
//			while (equalStrings(v.at(i).first, "break")) {
//				i++;
//			}
//			i--;
//			//print line in file
//
//			lengthRecord = toHexa(opcode.size() / 2);
//			cout << "T" << startingAddress << lengthRecord << opcode << endl;
//
//			/*Initialize the variables*/
//			startingAddress = "";
//			lengthRecord = "";
//			opcode = "";
//		}
//
//	}
//
//	/************************************************/
//	/*End Record*/
//	/************************************************/
//	p = v.at(0);
//	p.second = fixSixDigitHexa(p.second);
//	cout << "E" << p.second << endl;
//}
//
//void printObjectCode(vector<pair<string, string> > v, string programName) {
//	pair<string, string> p;
//	pair<string, string> dummy;
//	/************************************************/
//	/*Header Record*/
//	/************************************************/
//	cout << "H" << programName;
//	int fillBlankSize = (6 - programName.size());
//	string fill = "";
//	for (int i = 0; i < fillBlankSize; i++) {
//		fill += ' ';
//	}
//	cout << fill;
//	p = v.at(0);
//	dummy = v.at(v.size() - 1);
//	int endAddressInt = toInt(dummy.second);
//	int startAddressInt = toInt(p.second);
//	int lengthProgramInt = endAddressInt - startAddressInt + 1;
//	string lengthProgStr = toHexa(lengthProgramInt);
//	lengthProgStr = fixSixDigitHexa(lengthProgStr);
//	p.second = fixSixDigitHexa(p.second);
//	cout << p.second << lengthProgStr << endl;
//	/************************************************/
//	/*Text Records*/
//	/************************************************/
//	string startingAddress = "";
//	string lengthRecord = "";
//	string opcode = "";
//	for (int i = 0; i < v.size(); i++) {
//
//		if (!equalStrings(v.at(i).first, "break")
//				&& (opcode.size() + v.at(i).first.size()) <= 60) {
//			if (opcode.size() == 0) {
//				v.at(i).second = fixSixDigitHexa(v.at(i).second);
//				startingAddress += v.at(i).second;
//			}
//
//			opcode += v.at(i).first;
//
//		} else {
//			while (equalStrings(v.at(i).first, "break")) {
//				i++;
//			}
//			i--;
//			//print line in file
//
//			lengthRecord = toHexa(opcode.size() / 2);
//			cout << "T" << startingAddress << lengthRecord << opcode << endl;
//
//			/*Initialize the variables*/
//			startingAddress = "";
//			lengthRecord = "";
//			opcode = "";
//		}
//
//	}
//
//	/************************************************/
//	/*End Record*/
//	/************************************************/
//	p = v.at(0);
//	p.second = fixSixDigitHexa(p.second);
//	cout << "E" << p.second << endl;
//}
void printObjectCode(vector<pair<string, string> > v, string programName) {
	pair<string, string> p;
	pair<string, string> dummy;
	/************************************************/
	/*Header Record*/
	/************************************************/
	cout << "H" << programName;
	int fillBlankSize = (6 - programName.size());
	string fill = "";
	for (int i = 0; i < fillBlankSize; i++) {
		fill += ' ';
	}
	cout << fill;
	p = v.at(0);
	dummy = v.at(v.size() - 1);
	int endAddressInt = toInt(dummy.second);
	int startAddressInt = toInt(p.second);
	int lengthProgramInt = endAddressInt - startAddressInt + 2;
	string lengthProgStr = toHexa(lengthProgramInt);
	lengthProgStr = fixSixDigitHexa(lengthProgStr);

	p.second = fixSixDigitHexa(p.second);
	cout << p.second << lengthProgStr << endl;
	/************************************************/
	/*Text Records*/
	/************************************************/
	string startingAddress = "";
	string lengthRecord = "";
	string opcode = "";
	for (int i = 0; i < (int) v.size(); i++) {

		if (!equalStrings(v.at(i).first, "break")
				&& ((int) opcode.size() + (int) v.at(i).first.size()) <= 60) {
			if ((int) opcode.size() == 0) {
				v.at(i).second = fixSixDigitHexa(v.at(i).second);
				startingAddress += v.at(i).second;
			}

			opcode += v.at(i).first;

		} else {
			while (equalStrings(v.at(i).first, "break")) {
				i++;
			}
			i--;
			//print line in file

			lengthRecord = toHexa(opcode.size() / 2);
			if ((int) lengthRecord.size() == 1) {
				lengthRecord = "0" + lengthRecord;
			}
			cout << "T" << startingAddress << lengthRecord << opcode << endl;

			/*Initialize the variables*/
			startingAddress = "";
			lengthRecord = "";
			opcode = "";
		}

	}
	if (opcode.size() != 0) {
		lengthRecord = toHexa(opcode.size() / 2);
		if ((int) lengthRecord.size() == 1) {
			lengthRecord = "0" + lengthRecord;
		}
		cout << "T" << startingAddress << lengthRecord << opcode << endl;
	}
	/************************************************/
	/*End Record*/
	/************************************************/
	p = v.at(0);
	p.second = fixSixDigitHexa(p.second);
	cout << "E" << p.second << endl;
}

void print_for_list_file() {

	for (int i = 0; i < (int) for_list_file.size(); i++)
		cout << for_list_file.at(i) << endl;

}

string handle_equ(string exp, bool &boolean) {
	int indexSign = -1;
	bool valid = true;
	char sign;
	string ret = "";
	string errorMess = "";
	for (int i = 0; i < (int) exp.size(); i++) {
		if (exp[i] == '+') {
			indexSign = i;
			sign = '+';
		} else if (exp[i] == '-') {
			indexSign = i;
			sign = '-';
		} else if (exp[i] == '*') {
			indexSign = i;
			valid = false;
			sign = '*';
		} else if (exp[i] == '/') {
			indexSign = i;
			valid = false;
			sign = '/';
		}
	}
	if (indexSign != -1 && valid) {

		string labelOne = "";
		string labelTwo = "";
		for (int i = 0; i < indexSign; i++) {
			labelOne += exp[i];
		}
		for (int i = indexSign + 1; i < (int) exp.size(); i++) {
			labelTwo += exp[i];
		}
		if ((int) labelOne.size() != 0 && (int) labelTwo.size() != 0) {
			string add_lab1 = "-1 ";
			string add_lab2 = "-1";

			if (SYMTAB.count(labelOne) != 0) {
				add_lab1 = (*SYMTAB.find(labelOne)).second.first;

			}

			if (SYMTAB.count(labelTwo) != 0) {
				add_lab2 = (*SYMTAB.find(labelTwo)).second.first;

			}

			if (labelOne.compare("-1") == 0 || labelTwo.compare("-1") == 0) {
				errorMess = "***label in expression not found ";
				cout << errorMess << endl;
			} else {

				if (sign == '+') {
					boolean = false;
					ret = (toHexa(toInt(add_lab1) + toInt(add_lab2)));

				} else {
					boolean = true;
					ret = (toHexa(toInt(add_lab1) - toInt(add_lab2)));

				}
				return ret;

			}

		} else {
			errorMess = "***Invalid Expression **";
			cout << errorMess << endl;
		}
	} else {
		errorMess = "***Invalid Expression";
		cout << errorMess << endl;
	}
	return ret;
}

int main() {

	ifstream file("read.txt");
	fill_optab();
	fill_symtab();
	bool ready_to_assemble = false;
	if (file.is_open() && mnemonics_loaded && symtab_loaded) {
		vector<string> parsed;
		string read;
		string read2;
		bool continue_program = true;
		getline(file, read);
		stringstream ss(read);
		string buffer;
		while (ss >> buffer)
			parsed.push_back(buffer);
		PC = parsed.at((int) parsed.size() - 1);
		if (parsed.at(2).compare("START") == 0) {
			program_name = "NONAME";

		} else {
			program_name = parsed.at(2);

		}

		ready_to_assemble = true;

		if (ready_to_assemble) {

			getline(file, read);

			while (getline(file, read2) && continue_program) {

				if (read.at(0) == '*' && read.at(1) == '*') {
					read = read2;
					continue;

				}

				parsed.clear();
				stringstream ss2(read);
				while (ss2 >> buffer) {
					parsed.push_back(buffer);
				}
				bool label_exist = false;

				/*
				 * warning : we still need to handle
				 * mnemonics with no operands like RSUB  and so on
				 * will be handled soon in sha2 allah :)
				 *
				 *  */
				bool operand_exist = false;
				int format_type = -1;
				string line_number = "-1";
				string address = "-1";
				string nmennic = "-1";
				string operand = "-1";
				string label = "-1";

				string special_mnemonic = "-1";
				int position_special_mnemonic = -1;
				bool is_special = is_special_(parsed, special_mnemonic,
						position_special_mnemonic);

				if (is_special) {

					if (special_mnemonic.compare("RSUB") == 0) {
						string objecter_code = "4c0000";
						string address__ = parsed.at(1);
						string lister = objecter_code + "  " + read;
						for_list_file.push_back(lister);
						object_code.push_back(
								make_pair(objecter_code, address__));
						read = read2;
						continue;

					}

					else if (special_mnemonic.compare("EQU") == 0) {

						string label_to_insert = parsed.at(
								position_special_mnemonic - 1);
						int position_operand = position_special_mnemonic + 1;
						string special_operand = parsed.at(position_operand);
						bool boolean = true;
						string newadress = handle_equ(special_operand, boolean);
						string ss = "";
						if (boolean) {
							ss = "RELOCATABLE";
						} else {
							ss = "ABSOLUTE";
						}
						if ((int) newadress.size() != 0) {

							SYMTAB.erase(label_to_insert);
							SYMTAB.insert(
									make_pair(label_to_insert,
											make_pair(newadress, ss)));

						}
						read = read2;
						continue;

					}

					else if (special_mnemonic.compare("END") == 0) {
						//cout << "88" << endl;
						if (position_special_mnemonic == (int) parsed.size()) {
							operand_exist = false;
						} else
							operand_exist = true;

						if (position_special_mnemonic == 2) {
							label_exist = false;
						} else
							label_exist = true;

						line_number = parsed.at(0);
						address = parsed.at(1);
						nmennic = special_mnemonic;
						continue_program = false;
						continue;

					}

					else if (special_mnemonic.compare("BYTE") == 0) {

						int position_operand = position_special_mnemonic + 1;
						string special_operand = parsed.at(position_operand);
						address = parsed.at(1);

						if (special_operand.at(0) == 'X'
								|| special_operand.at(0) == 'x') {
							string temp = "";
							for (int i = 2;
									i < (int) special_operand.size() - 1; i++)
								temp += special_operand.at(i);

							//cout << read << " -- > " << temp << endl;
							string lister = temp + " " + read;
							for_list_file.push_back(lister);
							//temp += "@";

							object_code.push_back(make_pair(temp, address));
						} else {

							string temp = "";
							for (int i = 1;
									i < (int) special_operand.size() - 1; i++)
								temp += special_operand.at(i);

							string temp2 = "";
							for (int i = 0; i < (int) temp.size(); i++) {
								temp2 += toHexa(temp.at(i));

							}
							//cout << read << " -- > " << temp2 << endl;
							string lister = temp2 + " " + read;
							for_list_file.push_back(lister);
							//temp2 += "@";

							object_code.push_back(make_pair(temp2, address));

						}

						read = read2;
						continue;

					}

					else if (special_mnemonic.compare("WORD") == 0) {

						int position_operand = position_special_mnemonic + 1;
						address = parsed.at(1);
						string word_operand = parsed.at(position_operand);
						string word_operand_hexa = toHexa(
								atoi(word_operand.c_str()));
						int len = (int) word_operand_hexa.size();
						string temp_ = "";
						int len_imp = 6 - len;
						for (int ii = 0; ii < len_imp; ii++) {
							temp_ += "0";
						}
						string word_object_code = temp_ + word_operand_hexa;
						//cout << read << " -- > " << word_object_code << endl;
						object_code.push_back(
								make_pair(word_object_code, address));
						string lister = word_object_code + " " + read;
						for_list_file.push_back(lister);

						read = read2;
						continue;

					} else if (special_mnemonic.compare("RESW") == 0
							|| special_mnemonic.compare("RESB") == 0) {
						object_code.push_back(make_pair("break", address));
						read = read2;
						continue;

					}

					else {
						if (position_special_mnemonic == 2) {
							label_exist = false;
							operand_exist = false;
							line_number = parsed.at(0);
							address = parsed.at(1);
							nmennic = special_mnemonic;
						} else {
							label_exist = true;
							operand_exist = false;
							line_number = parsed.at(0);
							address = parsed.at(1);
							nmennic = special_mnemonic;
							label = parsed.at(2);
						}

						read = read2;
						continue;

					}

				}

				if ((int) parsed.size() == 4) {
					label_exist = false;
					line_number = parsed.at(0);
					address = parsed.at(1);
					nmennic = parsed.at(2);
					operand = parsed.at(3);
					operand_exist = true;

				} else {

					label_exist = true;
					line_number = parsed.at(0);
					address = parsed.at(1);
					label = parsed.at(2);
					nmennic = parsed.at(3);
					operand = parsed.at(4);
					operand_exist = true;

				}

				/*
				 * Handling format 2 here :)
				 *
				 *
				 *  */

				if (nmennic.compare("ADDR") == 0 || nmennic.compare("MULR") == 0
						|| nmennic.compare("DIVR") == 0
						|| nmennic.compare("SUBR") == 0
						|| nmennic.compare("RMO") == 0
						|| nmennic.compare("COMPR") == 0
						|| nmennic.compare("TIXR") == 0) {

					vector<string> parsed_oper;
					string tok_for_parsing;
					istringstream ss3(operand);
					while (getline(ss3, tok_for_parsing, ','))
						parsed_oper.push_back(tok_for_parsing);

					string formatter2 = "";
					for (int ii = 0; ii < 16; ii++)
						formatter2 += "0";

					string opcode_ = (*optab.find(nmennic)).second;
					string opcode_binary_ = hex_to_binary(opcode_);

					for (int ii = 0; ii < (int) opcode_binary_.size(); ii++)
						formatter2.at(ii) = opcode_binary_.at(ii);

					int start_loop = 8;

					for (int ii = 0; ii < (int) parsed_oper.size(); ii++) {
						string current = parsed_oper.at(ii);

						string opcode2 = (*optab.find(current)).second;

						string opcode2_binary = hex_to_binary(opcode2);

						for (int jj = start_loop; jj < start_loop + 4; jj++)
							formatter2.at(jj) = opcode2_binary.at(
									jj - start_loop);

						start_loop += 4;

					}

					string ready_object_code = get_hexa_string_binary(
							formatter2);

					object_code.push_back(
							make_pair(ready_object_code, address));
					string lister = ready_object_code + " " + read;
					for_list_file.push_back(lister);
					read = read2;

					//cout << read << " --- > " << ready_object_code << endl;
					continue;

				}

				if (operand.compare("*") == 0) {
					SYMTAB.insert(
							make_pair("*", make_pair(address, "RELOCATABLE")));
				}

				string current_object_code = "-1";
				string formatter = "";

				for (int i = 0; i < 12; i++)
					formatter += "0";

				if (nmennic.at(0) == '+')
					format_type = 4;
				else
					format_type = 3;

				bool immediate = false;
				bool indirect = false;
				bool indexed = false;
				string work_operand = "";

				string opcode = (*optab.find(nmennic)).second;
				string opcode_binary = hex_to_binary(opcode);

				for (int i = 0; i < (int) opcode_binary.size(); i++)
					formatter.at(i) = opcode_binary.at(i);

				if (operand.at(0) == '#') {

					immediate = true;
					formatter.at(7) = '1';
					work_operand = operand.substr(1, (int) operand.size() - 1);

				} else if (operand.at(0) == '@') {
					indirect = true;
					formatter.at(6) = '1';
					work_operand = operand.substr(1, (int) operand.size() - 1);
				} else {
					formatter.at(6) = '1';
					formatter.at(7) = '1';
					work_operand = operand;

				}

				vector<string> parsed_operand;
				string token_for_parsing;
				istringstream ss3(operand);
				while (getline(ss3, token_for_parsing, ','))
					parsed_operand.push_back(token_for_parsing);

				if ((int) parsed_operand.size() >= 2
						&& (parsed_operand.at(1).at(0) == 'X'
								|| parsed_operand.at(1).at(0) == 'x')) {
					formatter.at(8) = '1';
					indexed = true;

				}

				/*
				 * The above check was for the indexed addressing case
				 *
				 * */

				string next_address = "";

				vector<string> parsing_head;
				stringstream ss4(read2);
				string bufferer = "";
				while (ss4 >> bufferer) {
					parsing_head.push_back(bufferer);

				}
				next_address = parsing_head.at(1);

				/*
				 * assuming that we have this address
				 * */

				if ((indexed && immediate) || (indexed && indirect)) {

					cout << "*** error invalid operand " << endl;
					continue;

				}

				bool error_format_4 = false;
				string operand_address = "";

				bool is_digit_for_immediate = false;
				if (immediate) {
					if (is_Digit_string(work_operand)) {
						is_digit_for_immediate = true;
						operand_address = work_operand;
					} else
						operand_address =
								(*SYMTAB.find(work_operand)).second.first;

				} else if (indirect) {

					if (is_Digit_string(work_operand)) {
						is_digit_for_immediate = true;
						operand_address = work_operand;
					} else
						operand_address =
								(*SYMTAB.find(work_operand)).second.first;

				}

				else if (parsed_operand.size() == 1) {
					operand_address = (*SYMTAB.find(operand)).second.first;

				} else {
					string temp_string = parsed_operand.at(0);
					operand_address = (*SYMTAB.find(temp_string)).second.first;
				}

				string displacement = toHexa(
						toInt(operand_address) - toInt(next_address));

				int displacement_int = toInt(displacement);

				int address_format_4 = toInt(operand_address);

				string handle = "";
				bool error = false;

				if (format_type == 4) {
					handle = dispToString(address_format_4, format_type);
					formatter.at(11) = '1';
					if (handle.compare("") == 0)
						error_format_4 = true;

				} else if (immediate && is_digit_for_immediate) {

					string word_operand_hexa = toHexa(
							atoi(work_operand.c_str()));
					int len = (int) word_operand_hexa.size();
					string temp_ = "";
					int len_imp = 3 - len;
					for (int ii = 0; ii < len_imp; ii++) {
						temp_ += "0";
					}
					string special_case = temp_ + word_operand_hexa;
					handle = hex_to_binary(special_case);

				} else if (indirect && is_digit_for_immediate) {
					string word_operand_hexa = toHexa(
							atoi(work_operand.c_str()));
					int len = (int) word_operand_hexa.size();
					string temp_ = "";
					int len_imp = 3 - len;
					for (int ii = 0; ii < len_imp; ii++) {
						temp_ += "0";
					}
					string special_case = temp_ + word_operand_hexa;
					handle = hex_to_binary(special_case);

				}

				else if (displacement_int >= -2048
						&& displacement_int <= 2047) {

					handle = dispToString(displacement_int, format_type);
					formatter.at(10) = '1';
					//cout << "me" << error << endl;

				} else if (displacement_int >= 0 && displacement_int <= 4095) {

					handle = dispToString(displacement_int, format_type);
					formatter.at(9) = '1';

				} else {
					error = true;

				}
				if (error) {

					cout << "*** error could not generate object code" << endl;
				} else if (error_format_4) {
					//cout << "pp" << endl;
					cout << "*** error could not generate object code " << endl;
				} else {

					formatter += handle;
					//cout << formatter << endl;
					string to_insert_object_code = get_hexa_string_binary(
							formatter);
					//cout << read << " -- >  " << to_insert_object_code << endl;
					object_code.push_back(
							make_pair(to_insert_object_code, address));
					string lister = to_insert_object_code + " " + read + "  ";
					string for6 = "" + formatter.at(6);
					string for7 = "" + formatter.at(7);
					string for8 = "" + formatter.at(8);
					string for9 = "" + formatter.at(9);
					string for10 = "" + formatter.at(10);
					string for11 = "" + formatter.at(11);

					string lister2 = "n=" + get(formatter, 6) + "  i="
							+ get(formatter, 7) + "  x=" + get(formatter, 8)
							+ "  b=" + get(formatter, 9) + "  p="
							+ get(formatter, 10) + "  e=" + get(formatter, 11);
					string lister3 = lister + lister2;
					for_list_file.push_back(lister3);

				}

				read = read2;

			}/* end of reading lines  */

		}/* end of test if ready to assemble  */

	} else {
		cout << " Error : Could not open file to start pass2 ! " << endl;
	}

	cout << "*******************************************************" << endl;

//	for (int i = 0; i < (int) object_code.size(); i++) {
//		cout << object_code.at(i).first << " " << object_code.at(i).second
//				<< endl;
//	}

	printObjectCode(object_code, program_name);

	cout << "*******************************************************" << endl;

	cout << "el 7amd le allah " << endl;
	print_for_list_file();
	return 0;
}
