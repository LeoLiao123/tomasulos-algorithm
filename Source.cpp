#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<string>
#include"Inst.h"
#include"RegisterStatus.h"
#include"ReservationStation.h"
using namespace std;
const int ADD_Nums = 3;
const int MULandDIV_Nums = 2;

const int ADDcode = 0;
const int ADDIcode = 1;
const int SUBcode = 2;
const int MULcode = 3;
const int DIVcode = 4;

const int ADDlatency = 2;
const int MULlatency = 6;
const int DIVlatency = 8;

const int ISSUElatency = 1;
const int WRITEBACKlatency = 1;

int cycleNum = 0;
int WRnums = 0;
int InstIssue = 0;

const int REGzero = 10000;
const int RATempty = 10001;
const int QperandAva = 10002;
const int OperandInit = 10003;

bool isChange;
int RF[6] = { 0,0,2,4,6,8 };
int buffer[2] = { 50000,50000 };
vector<Inst> Instructions;
vector<ReservationStation>ResStations;
vector<RegisterStatus>RATarr;
vector<int>Register = { REGzero,0,2,4,6,8 };
int digits(int num) {
	int value = 0;
	while (num > 9)
		num /= 10, value++;
	return value;
}
void printRF() {
	cout << "     _ RF __" << endl;
	for (int i = 1; i < 6; i++) {
		cout << "  F" << i << " |";
		for (int j = 0; j < 3 - digits(RF[i]); j++) cout << " ";
		cout << RF[i] << " |" << endl;
	}
	cout << "     -------\n" << endl;
}
void printRAT() {
	cout << "     _ RAT __" << endl;
	for (int i = 1; i < 6; i++) {
		cout << "  F" << i << " |  ";
		if (RATarr[i].Qi != RATempty)
			cout << "RS" << RATarr[i].Qi + 1;
		else
			cout << "   ";
		cout << " |" << endl;
	}
	cout << "     --------\n" << endl;
}
void printRS() {
	cout << "    _ RS _________________" << endl;
	for (int i = 1; i < 4; i++) {
		cout << "RS" << i << " |    ";
		if (ResStations[i - 1].busy) {
			switch (ResStations[i - 1].operation)
			{
			case ADDcode:
				cout << "+ |";
				break;
			case ADDIcode:
				cout << "+ |";
				break;
			case SUBcode:
				cout << "- |";
				break;
			default:
				cout << "  |";
				break;
			}
			if (ResStations[i - 1].unCompleteResultJ != -1)
				cout << "  RS" << ResStations[i - 1].unCompleteResultJ + 1 << " |";
			else {
				for (int j = 0; j < 4 - digits(ResStations[i - 1].Vj); j++) cout << " ";
				cout << ResStations[i - 1].Vj << " |";
			}
			if (ResStations[i - 1].unCompleteResultK != -1)
				cout << "  RS" << ResStations[i - 1].unCompleteResultK + 1 << " |";
			else {
				for (int j = 0; j < 4 - digits(ResStations[i - 1].Vk); j++) cout << " ";
				cout << ResStations[i - 1].Vk << " |";
			}
		}
		else
			cout << "  |      |      |";
		cout << endl;
	}
	cout << "    ----------------------\n" << endl;
	cout << "BUFFER:";
	if (buffer[0] == 50000)
		cout << " empty\n" << endl;
	else {
		cout << " (RS" << buffer[0] + 1 << ") " << ResStations[buffer[0]].Vj;
		switch (ResStations[buffer[0]].operation)
		{
		case ADDcode:
			cout << " + ";
			break;
		case ADDIcode:
			cout << " + ";
			break;
		case SUBcode:
			cout << " - ";
			break;
		}
		cout << ResStations[buffer[0]].Vk << endl;
		cout << endl;
	}
	//#############################################################################
	cout << "    _ RS _________________" << endl;
	for (int i = 4; i < 6; i++) {
		cout << "RS" << i << " |    ";
		if (ResStations[i - 1].busy) {
			switch (ResStations[i - 1].operation)
			{
			case MULcode:
				cout << "* |";
				break;
			case DIVcode:
				cout << "/ |";
				break;
			default:
				cout << "  |";
				break;
			}
			if (ResStations[i - 1].unCompleteResultJ != -1)
				cout << "  RS" << ResStations[i - 1].unCompleteResultJ + 1 << " |";
			else {
				for (int j = 0; j < 4 - digits(ResStations[i - 1].Vj); j++) cout << " ";
				cout << ResStations[i - 1].Vj << " |";
			}
			if (ResStations[i - 1].unCompleteResultK != -1)
				cout << "  RS" << ResStations[i - 1].unCompleteResultK + 1 << " |";
			else {
				for (int j = 0; j < 4 - digits(ResStations[i - 1].Vk); j++) cout << " ";
				cout << ResStations[i - 1].Vk << " |";
			}
		}
		else
			cout << "  |      |      |";
		cout << endl;
	}
	cout << "    ----------------------\n" << endl;
	cout << "BUFFER:";
	if (buffer[1] == 50000)
		cout << " empty\n" << endl;
	else {
		cout << " (RS" << buffer[1] + 1 << ") " << ResStations[buffer[1]].Vj;
		switch (ResStations[buffer[1]].operation)
		{
		case MULcode:
			cout << " * ";
			break;
		case DIVcode:
			cout << " / ";
			break;
		}
		cout << ResStations[buffer[1]].Vk << endl;
		cout << endl;
	}
}
void issue() {
	bool RSfree = false;
	
	if (InstIssue >= Instructions.size())
		return;
	int order = Instructions[InstIssue].operation;
	if (order == ADDcode || order == SUBcode || order == ADDIcode) {
		for (int i = 0; i < ADD_Nums; i++) {
			if (!ResStations[i].busy) {
				isChange = true;
				InstIssue++;
				RSfree = 1;
				if (order == ADDcode)
					ResStations[i].operation = ADDcode;
				else if (order == SUBcode)
					ResStations[i].operation = SUBcode;
				else if (order == ADDIcode)
					ResStations[i].operation = ADDIcode;
				order = i;
				break;
			}
		}
	}
	else {
		for (int i = ADD_Nums; i < ADD_Nums + MULandDIV_Nums; i++) {
			if (!ResStations[i].busy) {
				isChange = true;
				InstIssue++;
				if(order==MULcode)
					ResStations[i].operation = MULcode;
				else
					ResStations[i].operation = DIVcode;
				RSfree = 1;
				order = i;
				break;
			}
		}
	}
	if (!RSfree)
		return;

	if (RATarr[Instructions[InstIssue - 1].rs].Qi == RATempty) {
		ResStations[order].Vj = Register[Instructions[InstIssue - 1].rs];
		ResStations[order].Qj = QperandAva;
		ResStations[order].unCompleteResultJ = -1;
	}
	else {
		ResStations[order].Qk = RATarr[Instructions[InstIssue - 1].rs].Qi;
		ResStations[order].unCompleteResultJ = RATarr[Instructions[InstIssue - 1].rs].Qi;
	}
	if (Instructions[InstIssue-1].operation == ADDIcode) {
		ResStations[order].Vk = Instructions[InstIssue - 1].constant;
		ResStations[order].Qk = QperandAva;
	}
	else if (RATarr[Instructions[InstIssue - 1].rt].Qi == RATempty) {
		ResStations[order].Vk = Register[Instructions[InstIssue - 1].rt];
		ResStations[order].Qk = QperandAva;
		ResStations[order].unCompleteResultK = -1;
	}
	else {
		ResStations[order].Qk = RATarr[Instructions[InstIssue - 1].rt].Qi;
		ResStations[order].unCompleteResultK = RATarr[Instructions[InstIssue - 1].rt].Qi;
	}
	ResStations[order].busy = true;
	ResStations[order].IssueLatency = 0;
	ResStations[order].InstNum = InstIssue-1;
	RATarr[Instructions[InstIssue - 1].rd].Qi = order;
}
void execute() {
	for (int i = 0; i < ResStations.size(); i++) {
		if (ResStations[i].busy) {
			if (ResStations[i].IssueLatency >= ISSUElatency) {
				if (ResStations[i].Qj == QperandAva && ResStations[i].Qk == QperandAva) {
					bool exe = true;
					if (ResStations[i].operation < 3) {
						if (buffer[0] == 50000)
							buffer[0] = i;
						else if (buffer[0] != i)
							exe = false;
					}
					else {
						if (buffer[1] == 50000)
							buffer[1] = i;
						else if (buffer[1] != i)
							exe = false;
					}
					if (exe) {
						ResStations[i].latency++;
						switch (ResStations[i].operation) {
						case ADDcode:
							if (ResStations[i].latency == ADDlatency) {
								isChange = true;
								ResStations[i].result = ResStations[i].Vj + ResStations[i].Vk;
								ResStations[i].complete = true;
								ResStations[i].latency = 0;
								ResStations[i].IssueLatency = 0;
								buffer[0] = 50000;
								break;
							}
						case ADDIcode:
							if (ResStations[i].latency == ADDlatency) {
								isChange = true;
								ResStations[i].result = ResStations[i].Vj + ResStations[i].Vk;
								ResStations[i].complete = true;
								ResStations[i].latency = 0;
								ResStations[i].IssueLatency = 0;
								buffer[0] = 50000;
								break;
							}
						case SUBcode:
							if (ResStations[i].latency == ADDlatency) {
								isChange = true;
								ResStations[i].result = ResStations[i].Vj - ResStations[i].Vk;
								ResStations[i].complete = true;
								ResStations[i].latency = 0;
								ResStations[i].IssueLatency = 0;
								buffer[0] = 50000;
								break;
							}
						case MULcode:
							if (ResStations[i].latency == MULlatency) {
								isChange = true;
								ResStations[i].result = ResStations[i].Vj * ResStations[i].Vk;
								ResStations[i].complete = true;
								ResStations[i].latency = 0;
								ResStations[i].IssueLatency = 0;
								buffer[1] = 50000;
								break;
							}
						case DIVcode:
							if (ResStations[i].latency == DIVlatency) {
								isChange = true;
								ResStations[i].result = ResStations[i].Vj / ResStations[i].Vk;
								ResStations[i].complete = true;
								ResStations[i].latency = 0;
								ResStations[i].IssueLatency = 0;
								buffer[1] = 50000;
								break;
							}
						default:
							break;
						}
					}
				}
			}
			else
				ResStations[i].IssueLatency++;
		}
	}
}
void writeback() {
	for (int i = 0; i < ResStations.size(); i++) {
		if (ResStations[i].complete) {
			if (ResStations[i].WriteBackLatency == WRITEBACKlatency) {
				isChange = true;
				for (int j = 0; j < Register.size(); j++) {
					if (RATarr[j].Qi == i)
						Register[j] = ResStations[i].result, RATarr[j].Qi = RATempty;
				}
				for (int j = 0; j < ResStations.size(); j++) {
					if (ResStations[j].Qj == i)
						ResStations[j].Vj = ResStations[i].result, ResStations[j].Qj = QperandAva;
					if (ResStations[j].Qk == i)
						ResStations[j].Vk = ResStations[i].result, ResStations[j].Qk = QperandAva;
				}
				ResStations[i].busy = false;
				ResStations[i].complete = false;
				ResStations[i].Qj = QperandAva;
				ResStations[i].Qk = QperandAva;
				ResStations[i].Vj = 0;
				ResStations[i].Vk = 0;
				ResStations[i].latency = 0;
				WRnums++;
			}
			else
				ResStations[i].WriteBackLatency++;
		}
	}
}
int main() {
	ifstream file;
	file.open("input.txt");
	string s;
	while (getline(file, s)) {
		stringstream ss(s);
		string word;
		vector<string>instruction;
		while (ss >> word) {
			if (word[word.size() - 1] == ',') word.pop_back();
			if (word[0] == 'F') word.erase(word.begin());
			instruction.push_back(word);
		}
		int rd = atoi(instruction[1].c_str()), rs = atoi(instruction[2].c_str()), rt = atoi(instruction[3].c_str());
		if (instruction[0] == "ADD") {
			Inst I(rd, rs, rt, 0, ADDcode);
			Instructions.push_back(I);
		}
		else if (instruction[0] == "ADDI") {
			Inst I(rd, rs, 0, rt, ADDIcode);
			Instructions.push_back(I);
		}
		else if (instruction[0] == "SUB") {
			Inst I(rd, rs, rt, 0, SUBcode);
			Instructions.push_back(I);

		}
		else if (instruction[0] == "MUL") {
			Inst I(rd, rs, rt, 0, MULcode);
			Instructions.push_back(I);

		}
		else if (instruction[0] == "DIV") {
			Inst I(rd, rs, rt, 0, DIVcode);
			Instructions.push_back(I);
		}
	}
	ReservationStation ADD(ADDcode, OperandInit), MUL(MULcode, OperandInit);
	for (int i = 0; i < 3; i++) ResStations.push_back(ADD);
	for (int i = 0; i < 2; i++)ResStations.push_back(MUL);
	RegisterStatus F(RATempty);
	for (int i = 0; i < 6; i++)RATarr.push_back(F);
	while (1) {
		isChange = false;
		cycleNum++;
		issue();
		execute();
		writeback();
		if (WRnums == Instructions.size())
			break;
		if (isChange) {
			cout << "Cycle " << cycleNum << ":\n" << endl;
			printRF();
			printRAT();
			printRS();
			cout << endl;
		}
	}
	for (int i = 0; i < 6; i++)
		RF[i] = Register[i];
	cout << "Cycle " << cycleNum << ":\n" << endl;
	printRF();
	printRAT();
	printRS();
}