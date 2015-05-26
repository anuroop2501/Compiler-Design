#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>

#define CFG "cfg.txt"
#define PREC "precedence.txt"

using namespace std;

stack<string> ops;
map<string, string> grammar;
vector<vector<string> > precedence;
vector<string> tokens;
string input;

string replace(string original, string findStr, string replaceStr) {
	size_t index = 0;

	while (true) {
		index = original.find(findStr, index);
		if (index == string::npos)
			break;

		original.replace(index, findStr.length(), replaceStr);

		index += replaceStr.length();
	}

	return original;
}

void readCfg() {
	ifstream cfg(CFG);
	string line;
	while (getline(cfg, line)) {
		line = replace(line, " ", "");
		size_t index = line.find(":");
		string lhs = line.substr(0, index);
		string rhs = line.substr(index + 1);
		grammar.insert(pair<string, string>(rhs, lhs));
	}
	cfg.close();
	cout << "Context Free Grammar read." << endl;
}

void readPrecedence() {
	ifstream prec(PREC);
	string line, token;
	getline(prec, line);
	istringstream iss(line);
	do {
		iss >> token;
		tokens.push_back(token);
	} while(iss);

	while (getline(prec, line)) {
		vector<string> row;
		istringstream iss(line);
		iss >> token;
		do {
			iss >> token;
			row.push_back(token);
		} while(iss);
		precedence.push_back(row);
	}
	prec.close();
	cout << "Precedence table read." << endl;
}

void printStack() {
	stack<string> tmp;

	while (!ops.empty()) {
		cout << ops.top() << " ";
		tmp.push(ops.top());
		ops.pop();
	}

	cout << endl;

	while (!tmp.empty()) {
		ops.push(tmp.top());
		tmp.pop();
	}
}

void reduce() {
	string sequence;
	stack<string> tmp;
	bool reduced = false;
	while (ops.top() != "$") {
		sequence = ops.top() + sequence;
		tmp.push(ops.top());
		ops.pop();
		if (grammar.find(sequence) != grammar.end()) {
			cout << "Production Rule: " << grammar[sequence] << ": " << sequence << endl;
			ops.push(grammar[sequence]);
			reduced = true;
			break;
		}
	}

	if (!reduced) {
		while (!tmp.empty()) {
			ops.push(tmp.top());
			tmp.pop();
		}
	}
}

string nextToken() {
	string input_copy = input;
	bool found = false;
	string token = "";
	while (!found) {
		for (int i = 0; i < tokens.size(); i++) {
			if (tokens[i] == token) {
				found = true;
				break;
			}
		}
		if (!found) {
			token += input_copy[0];
			input_copy = input_copy.substr(1);
		}
	}
	return token;
}

int getIndexOfToken(string token) {
	for (int i = 0; i < precedence.size(); i++) {
		if (tokens[i] == token)
			return i;
	}
}

bool isReducible() {
	string sequence;
	stack<string> tmp;
	bool reducible = false;
	bool preceed = false;

	while(ops.top() != "$") {
		sequence = ops.top() + sequence;
		tmp.push(ops.top());
		ops.pop();
		if (grammar.find(sequence) != grammar.end()) {
			// process the precedence table
			string incoming = nextToken();
			int incomingIndex = getIndexOfToken(incoming);

			while (!tmp.empty()) {
				string parsed = tmp.top();
				int parsedIndex = getIndexOfToken(parsed);
				if (precedence[incomingIndex][parsedIndex] == ">") {
					cout << incoming << " has higher precedence than " << parsed << endl;
					preceed = true;
					break;
				}
				ops.push(parsed);
				tmp.pop();
			}

			if (preceed)
				break;
			reducible = true;
			break;
		}
	}

	while (!tmp.empty()) {
		ops.push(tmp.top());
		tmp.pop();
	}

	return reducible;
}

void parseOp() {
	while (input != "$") {
		bool found = false;
		string token = "";
		while (!found) {
			for (int i = 0; i < tokens.size(); i++) {
				if (tokens[i] == token) {
					found = true;
					break;
				}
			}
			if (!found) {
				token += input[0];
				input = input.substr(1);
			}
		}
		// shift operation
		ops.push(token);
		cout << "Shift: ";
		printStack();

		// reduce operation
		while (isReducible()) {
			reduce();
			cout << "Reduce: ";
			printStack();
		}
	}

	// check if the entered expression is valid
	if (ops.top() == "E") {
		ops.pop();
		if (ops.top() == "$") {
			cout << "The expression is valid." << endl;
			return;
		}
	}

	cout << "The expression is invalid." << endl;
}

int main() {
	readCfg();
	readPrecedence();

	cin >> input;
	input += "$";
	ops.push("$");

	cout << endl << "Parsing expression ..." << endl;
	parseOp();
	return 0;
}
