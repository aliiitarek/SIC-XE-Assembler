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

string comment = "";
string valid_command = " ";
string invalid_command = ".";

int line_counter = 0;
map<string, int> mnemonics;
map<string, string> SYMTAB;
queue<string> q;

bool isEqual(string a, string b) {
	if (a.length() != b.length())
		return false;

	for (int i = 0; i < (int) a.length(); i++) {
		if (a[i] != b[i])
			return false;
	}
	return true;
}

string toHexa(int in) {
	string str;
	char current;

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
	return ans;
}

string split(string str, string arr[]) {

	string a[6];
	for (int j = 0; j < 6; j++)
		a[j] = "";
	for (int j = 0; j < 4; j++)
		arr[j] = "";

	int i = 0;
	while (str[i] == ' ') {
		i++;
	}

	if ((int) str.length() == 0 || i == (int) str.length())
		return ".";

	if (str[i] == '.')
		return "";

	int p = 0;
	bool space = false;

	for (; i < (int) str.length(); i++) {

		if (str[i] != ' ') {
			if (p > 5) {
				p = 5;
			}
			a[p] += str[i];
			space = false;
		} else {
			if (!space) {
				space = true;
				p++;
			}
		}
	}
	if (isEqual(a[0], "RMO") || isEqual(a[0], "ADDR") || isEqual(a[0], "SUBR")
			|| isEqual(a[0], "MULR") || isEqual(a[0], "DIVR")
			|| isEqual(a[0], "COMPR")) {
		arr[0] = a[0];
		arr[1] = a[1];
		if (arr[1].length() < 3) {
			arr[1] += a[2];
			if (arr[1].length() < 3) {
				arr[1] += a[3];
				arr[2] = a[4];
			} else {
				arr[2] = a[3];
			}
		} else {
			arr[2] = a[2];
		}
	} else if (isEqual(a[1], "RMO") || isEqual(a[1], "ADDR")
			|| isEqual(a[1], "SUBR") || isEqual(a[1], "MULR")
			|| isEqual(a[1], "DIVR") || isEqual(a[1], "COMPR")) {
		arr[0] = a[0];
		arr[1] = a[1];
		arr[2] = a[2];
		if (arr[2].length() < 3) {
			arr[2] += a[3];
			if (arr[2].length() < 3) {
				arr[2] += a[4];
				arr[3] = a[5];
			} else {
				arr[3] = a[4];
			}
		} else {
			arr[3] = a[3];
		}
	} else {
		for (int i = 0; i < 4; i++) {
			arr[i] = a[i];
		}
	}
	return " ";
}

bool fillMnemonic() {
	ifstream file("mnemonics.txt");
	string line_read;
	if (file.is_open()) {

		while (getline(file, line_read)) {
			stringstream ss(line_read);
			string buffer;
			vector<string> tokens;

			while (ss >> buffer)
				tokens.push_back(buffer);

			mnemonics.insert(
					make_pair(tokens.at(0), atoi(tokens.at(1).c_str())));

		}
		return true;

	} else {
		return false;
	}

}

bool is_integer(string str) {

	if ((int) str.length() == 0)
		return false;

	for (int i = 0; i < (int) str.length(); i++) {
		if (!isdigit(str.at(i)))
			return false;
	}
	return true;

}

void printInTheFile(string loc, vector<string> v) {

	cout << line_counter++ << " " << loc;
	for (int i = 0; i < (int) v.size(); i++) {
		cout << " " << v.at(i);
	}
	cout << endl;

}

bool check_literal(string literal) {

	if ((int) literal.size() < 4) {
		return false;
	}

	if (literal.at(0) == '='
			&& (literal.at(1) == 'X' || literal.at(1) == 'x')) {

		if (literal.at(2) == '\''
				&& literal.at((int) literal.size() - 1) == '\'') {

			for (int ii = 3; ii < (int) literal.size() - 1; ii++) {
				if (literal.at(ii) == '\'')
					return false;
			}

			string check_hexa = "";
			for (int ii = 3; ii < (int) literal.size() - 1; ii++) {
				check_hexa += literal.at(ii);
			}

			if (isHexa(check_hexa))
				return true;
			else
				return false;

		}

	} else if (literal.at(0) == '='
			&& (literal.at(1) == 'c' || literal.at(1) == 'C')) {

		if (literal.at(2) == '\''
				&& literal.at((int) literal.size() - 1) == '\'') {

			for (int ii = 3; ii < (int) literal.size() - 1; ii++) {
				if (literal.at(ii) == '\'')
					return false;
			}

			return true;

		}

	}

	return false;

}

void validate(vector<string> v, bool &status, int &position,
		bool &label_exist) {

	/*
	 if ((int) v.size() > 4) {
	 status = false;
	 cout << "*** Too many arguments ***" << endl;

	 }
	 */
	for (int i = 0; i < (int) v.size(); i++) {
		if (mnemonics.count(v.at(i)) != 0) {
			position = i;
		}

	}

	if (position == -1) {
		cout << "*** No mnemonic is found " << endl;
		status = false;
	}

	if (position != 0 && position != 1 && status) {
		cout << "*** Mnemonic is in wrong position" << endl;
		status = false;
	}

	string mnemonic_current = "-1";
	if (position >= 0 && position < (int) v.size())
		mnemonic_current = v.at(position);
	if (position == (int) v.size() - 1 && status
			&& !isEqual(mnemonic_current, "END")
			&& !isEqual(mnemonic_current, "RSUB")
			&& !isEqual(mnemonic_current, "NOBASE")
			&& !isEqual(mnemonic_current, "LTORG")) {
		cout << "*** No operands are found ***" << endl;
		status = false;
	}

	if (position == 0 && (int) v.size() >= 4) {
		cout << "*** Too much operands after the mnemonic" << endl;
		status = false;
	}

	if (position == 1) {
		// there is a label
		label_exist = true;

	}

	if (label_exist) {

		if (!isalpha(v.at(0).at(0))) {
			cout << "*** Invalid Label Name" << endl;
			status = false;
		}

		if (mnemonics.count(v.at(0)) != 0) {
			cout << "*** Name of Label is reserved as a mnemonic" << endl;
			status = false;
		}

		if (SYMTAB.count(v.at(0)) != 0) {
			cout << "*** Duplicate Label Name" << endl;
			status = false;
		}

	}

	if (status) {
		string current_mnemonic = v.at(position);
		if ((current_mnemonic.compare("BYTE") == 0
				|| current_mnemonic.compare("WORD") == 0
				|| current_mnemonic.compare("RESW") == 0
				|| current_mnemonic.compare("RESB") == 0) && (!label_exist)) {
			cout << "***There should be a Label when reserving storage" << endl;
			status = false;

		}

		if (current_mnemonic.compare("WORD") == 0
				|| current_mnemonic.compare("RESW") == 0
				|| current_mnemonic.compare("RESB") == 0) {

			string operand = v.at(position + 1);
			if (!is_integer(operand)) {
				status = false;
				cout << "*** Invalid operand for reserving storage " << endl;
			}

		}

		if (current_mnemonic.compare("BYTE") == 0 && status) {

			//cout << "here" << endl;
			string operand = v.at(position + 1);

			if ((int) operand.size() < 3) {
				status = false;
				cout << "*** Invalid operand for BYTE" << endl;
			}

			if (status) {

				if (operand.at(0) == 'C' || operand.at(0) == 'c') {
					if ((int) operand.size() < 3) {
						status = false;
						cout << "*** Invalid operand for BYTE directive "
								<< endl;
					}
//
//					if (status
//							&& (operand.at(1) != '\''
//									|| operand.at((int) operand.size() - 1)
//											!= '\'')) {
//						status = false;
//						cout << "*** Invalid operand for BYTE directive"
//								<< endl;
//
//					}

//					if (status) {
//
//						for (int ii = 2; ii < (int) operand.size() - 1; ii++) {
//							if (operand.at(ii) == '\'') {
//								status = false;
//								cout << "*** Invalid operand for BYTE directive"
//										<< endl;
//							}
//
//						}
//
//					}

				} else if (operand.at(0) == 'X' || operand.at(0) == 'x') {

					if ((int) operand.size() < 3) {
						status = false;
						cout << "*** Invalid operand for BYTE directive "
								<< endl;
					}

					if (status
							&& (operand.at(1) != '\''
									|| operand.at((int) operand.size() - 1)
											!= '\'')) {
						status = false;
						cout << "*** Invalid operand for BYTE directive"
								<< endl;

					}

					if (status) {

						for (int ii = 2; ii < (int) operand.size() - 1; ii++) {
							if (operand.at(ii) == '\'') {
								status = false;
								cout << "*** Invalid operand for BYTE directive"
										<< endl;
							}

						}

					}

					if (status) {

						string check_hexa = "";
						for (int ii = 2; ii < (int) operand.size() - 1; ii++) {
							check_hexa += operand.at(ii);
						}

						if (!isHexa(check_hexa)) {
							status = false;
							cout
									<< "*** Invalid hexadecimal operand for BYTE directive"
									<< endl;
						}

					}

				} else {
					status = false;
					cout << "*** Invalid operand for BYTE directive" << endl;

				}

			}

		}

		// end of checks for the BYTE directive

	}

}

bool validate_(vector<string> v) {

	//return true ;
	if ((int) v.size() == 0) {
		return false;
	}

	if (v.at(0).compare("START") == 0) {

		if ((int) v.size() > 1) {

			if (isHexa(v.at(1))) {
				return true;
			} else
				return false;

		} else
			return true;

	} else {

		if ((mnemonics.count(v.at(0)) != 0) || (int) v.size() == 1) {

			return false;
		} else {

			if (v.at(1).compare("START") == 0)
				return true;
			else
				return false;

		}

	}

}

void print_literal(string LOC, string str) {

	cout << line_counter++ << " " << LOC << " BYTE " << str << endl;

}

void print_sym_Tab() {

	freopen("SYMTAB.txt", "w", stdout);
	//cout << "********************************************************" << endl;
	//cout << "START OF SYMTAB" << endl;

	//cout << "          NAME          VALUE          ABS/REL" << endl;

	map<string, string>::iterator it;

	for (it = SYMTAB.begin(); it != SYMTAB.end(); ++it) {
		cout << "          " << (*it).first << "          " << (*it).second
				<< "          RELOCATABLE" << endl;

	}

	//cout << "END OF SYMTAB" << endl;
	//cout << "********************************************************" << endl;

}

int main() {

	bool mnemonics_ready = fillMnemonic();
	//freopen("out.txt", "w", stdout);

	if (!mnemonics_ready) {
		cout << "***The program could not load the mnemonics " << endl;
		cout << "***The program is terminating now " << endl;
	}
	ifstream file("read.txt");
	if (file.is_open() && mnemonics_ready) {

		cout << "********************************************************"
				<< endl;
		cout << "START OF PASS 1" << endl;
		string line_read;
		string LOCCTR;
		string starting_address;
		string program_name;

		bool start_program = false;
		bool end_specified = false;
		//getline(file, line_read);
		vector<string> parsed;

		while (true) {

			parsed.clear();

			getline(file, line_read);

			stringstream ss(line_read);
			string buffer;
			while (ss >> buffer)
				parsed.push_back(buffer);

			if (parsed.at(0).at(0) != '.') {
				break;

			}

			ss.clear();
		}

		//cout << (int) parsed.size() << endl;
		bool validator = validate_(parsed);

		if (validator) {
			if (mnemonics.count(parsed[0]) != 0 && isEqual(parsed[0], "START")
					&& isHexa(parsed[1])) {

				starting_address = parsed[1];
				LOCCTR = parsed[1];
				printInTheFile(LOCCTR, parsed);
				toHexa(toInt(LOCCTR) + 3);
				start_program = true;
				getline(file, line_read);

			} else if (isEqual(parsed[1], "START") && isHexa(parsed[2])) {

				starting_address = parsed[2];
				program_name = parsed[0];
				SYMTAB.insert(make_pair(program_name, parsed[2]));
				LOCCTR = parsed[2];
				printInTheFile(LOCCTR, parsed);
				toHexa(toInt(LOCCTR) + 3);
				start_program = true;
				getline(file, line_read);

			} else {

				starting_address = "0";
				LOCCTR = "0";
				start_program = true;
				printInTheFile(LOCCTR, parsed);

			}

		}

		if (start_program) {

			do {
				string buffer;
				stringstream ss(line_read);
				vector<string> parsed;
				if (line_read.compare(".") == 0 || line_read.compare("") == 0)
					continue;
				while (ss >> buffer)
					parsed.push_back(buffer);

				if ((int) parsed.size() == 0) {
					continue;
				}

				bool label_exist = false;
				int position_mnemonic = -1;
				bool status = true;

				validate(parsed, status, position_mnemonic, label_exist);

				if (status && parsed[position_mnemonic].compare("START") == 0) {
					status = false;
					cout << "***Duplicate Start directive" << endl;
				}

				if (status && parsed[position_mnemonic].compare("END") == 0
						&& !end_specified) {

					end_specified = true;

					while (!q.empty()) {
						string front = q.front();
						q.pop();
						front = front.substr(1, (int) front.size());
						int length = (int) front.size();
						length -= 3;
						print_literal(LOCCTR, front);
						LOCCTR = toHexa(toInt(LOCCTR) + length);

					}
					printInTheFile(LOCCTR, parsed);
					status = false;

				}

				if (status && end_specified) {
					status = false;
					cout
							<< "*** Assembler error . statements exists after END directive"
							<< endl;
				}

				if (status && parsed[position_mnemonic].compare("LTORG") == 0) {

					while (!q.empty()) {
						string front = q.front();
						q.pop();
						front = front.substr(1, (int) front.size());
						int length = (int) front.size();
						length -= 3;
						//cout <<"** "<<length<<endl;
						print_literal(LOCCTR, front);
						LOCCTR = toHexa(toInt(LOCCTR) + length);

					}
					status = false;

				}

				if (status && parsed[position_mnemonic].compare("ORG") == 0) {

					int position_operand = position_mnemonic + 1;
					string operand = parsed[position_operand];
					if (SYMTAB.count(operand) != 0) {

						printInTheFile(LOCCTR, parsed);
						LOCCTR = (*SYMTAB.find(operand)).second;
						status = false;

					} else {

						if (isHexa(operand)) {
							printInTheFile(LOCCTR, parsed);
							status = false;
							LOCCTR = operand;
						} else {

							cout << "*** Invalid operand for ORG directive"
									<< endl;
							status = false;

						}

					}

				}

				if (status && parsed[position_mnemonic].compare("EQU") == 0) {

					if (position_mnemonic == 0) {
						cout << "***EQU requires Label" << endl;
						status = false;
					} else if (position_mnemonic == (int) parsed.size() - 1) {
						cout << "***EQU requires operand" << endl;
						status = false;
					} else {
						int position_operand = position_mnemonic + 1;
						string operand = parsed.at(position_operand);

						if (isHexa(operand)) {

							if (SYMTAB.count(parsed.at(0)) != 0) {
								SYMTAB.erase(parsed.at(0));
								SYMTAB.insert(make_pair(parsed.at(0), operand));
							} else
								SYMTAB.insert(make_pair(parsed.at(0), operand));
						} else {
							cout << "CASE NOT HANDLED BUT WE KNOW IT :)"
									<< endl;
						}

					}

				}

				if (status && !label_exist) {

					int size_mnemonic = (*mnemonics.find(
							parsed[position_mnemonic])).second;

					printInTheFile(LOCCTR, parsed);

					string mnemonic_itself = parsed[position_mnemonic];
					if (position_mnemonic != (int) parsed.size() - 1) {
						int position_operand = position_mnemonic + 1;
						string operand = parsed[position_operand];

						if (check_literal(operand)) {

							q.push(operand);

						}
					}

					LOCCTR = toHexa(toInt(LOCCTR) + size_mnemonic);

				} else if (status && label_exist) {
					//cout << "prob rsub -- " << endl;

					string current_mnemonic = parsed[position_mnemonic];
					string label = parsed[position_mnemonic - 1];

					//cout << "me here" << endl;
					if (current_mnemonic.compare("RESW") == 0) {

						SYMTAB.insert(make_pair(label, LOCCTR));
						printInTheFile(LOCCTR, parsed);
						LOCCTR = toHexa(
								toInt(LOCCTR)
										+ 3
												* atoi(
														parsed[position_mnemonic
																+ 1].c_str()));

					} else if (current_mnemonic.compare("RESB") == 0) {
						SYMTAB.insert(make_pair(label, LOCCTR));
						printInTheFile(LOCCTR, parsed);
						LOCCTR =
								toHexa(
										toInt(LOCCTR)
												+ atoi(
														parsed[position_mnemonic
																+ 1].c_str()));

					} else if (current_mnemonic.compare("BYTE") == 0) {
						SYMTAB.insert(make_pair(label, LOCCTR));
						int position_operand = position_mnemonic + 1;
						string operand = parsed.at(position_operand);

						int get_length = 0;

						if (operand.at(0) == 'X' || operand.at(0) == 'x') {
							get_length = (int) operand.length() - 3;
							get_length = (int) ceil(get_length / 2);
							printInTheFile(LOCCTR, parsed);
							LOCCTR = toHexa(toInt(LOCCTR) + get_length);

						} else {

							/* Note that this will not work if the label starts with C'*/
							bool found_end_comma = false;
							int position_c = -1;
							for (int ii = 0; ii < (int) line_read.size() - 1;
									ii++) {

								if ((line_read.at(ii) == 'C'
										|| line_read.at(ii) == 'c')
										&& line_read.at(ii + 1) == '\'') {
									position_c = ii;

								}

							}

							int for_lOLY = -1;
							for (int ii = position_c + 2;
									ii < (int) line_read.size(); ii++) {

								if (line_read.at(ii) == '\'') {

									for_lOLY = ii;
									break;

								}

							}

							if (for_lOLY != (int) line_read.size() - 1
									&& line_read.at(for_lOLY + 1) != ' ') {
								cout << "*** Invalid format for BYTE directive"
										<< endl;

							} else {

								get_length = for_lOLY - position_c - 2;
								printInTheFile(LOCCTR, parsed);
								LOCCTR = toHexa(toInt(LOCCTR) + get_length);

							}

						}

					} else if (current_mnemonic.compare("WORD") == 0) {
						SYMTAB.insert(make_pair(label, LOCCTR));
						printInTheFile(LOCCTR, parsed);
						LOCCTR = toHexa(toInt(LOCCTR) + 3);

					} else {
						//cout << "go here\nadfgads" << endl;

						SYMTAB.insert(make_pair(label, LOCCTR));
						int size_mnemonic = (*mnemonics.find(
								parsed[position_mnemonic])).second;
						printInTheFile(LOCCTR, parsed);

						//int position_operand = position_mnemonic + 1;

						//

						if (position_mnemonic != (int) parsed.size() - 1) {
							int position_operand = position_mnemonic + 1;
							string operand = parsed[position_operand];
							if (check_literal(operand)) {

								q.push(operand);

							}
							LOCCTR = toHexa(toInt(LOCCTR) + size_mnemonic);

						} else {

							//

							LOCCTR = toHexa(toInt(LOCCTR) + size_mnemonic);

						}
					}

				}

			} while (getline(file, line_read));
			cout << "END OF PASS 1" << endl;
			cout << "********************************************************"
					<< endl;
			print_sym_Tab();

		} else {
			cout << "***Unable to start assembling the program  !" << endl;
		}

	} else {
		cout << "***Unable to open File  ! " << endl;
	}

	return 0;
}

