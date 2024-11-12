#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<string>
#include<cmath>
#include"Inst.h"
#include"RegisterStatus.h"
#include"ReservationStation.h"

using namespace std;

const int ADD_Nums = 3, MULandDIV_Nums = 2;
const int ADDcode = 0, ADDIcode = 1, SUBcode = 2, MULcode = 3, DIVcode = 4;
const int ADDlatency = 2, MULlatency = 6, DIVlatency = 8;
const int ISSUElatency = 1, WRITEBACKlatency = 1;
const int REGzero = 10000, RATempty = 10001, QperandAva = 10002, OperandInit = 10003;

int cycleNum = 0, WRnums = 0, InstIssue = 0;
int RF[6] = { 0,0,2,4,6,8 };  // Register values
int buffer[2] = { 50000, 50000 };

bool isChange;
vector<Inst> Instructions;
vector<ReservationStation> ResStations;
vector<RegisterStatus> RATarr;
vector<int> Register = { REGzero, 0, 2, 4, 6, 8 };

int digits(int num) {
    return (num == 0) ? 1 : static_cast<int>(log10(num)) + 1;
}

// Print Register File (RF) status
void printRF() {
    cout << "     _ RF __" << endl;
    for (int i = 1; i < 6; i++) {
        cout << "  F" << i << " | ";
        for (int j = 0; j < 3 - digits(RF[i]); j++) cout << " ";
        cout << RF[i] << " |" << endl;
    }
    cout << "     -------\n" << endl;
}

// Print Register Alias Table (RAT) status
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

// Print Reservation Station (RS) status
void printRS() {
    cout << "    _ RS _________________" << endl;
    for (int i = 0; i < ADD_Nums + MULandDIV_Nums; i++) {
        cout << "RS" << i + 1 << " | ";
        if (ResStations[i].busy) {
            // Print operation type
            cout << (ResStations[i].operation == ADDcode || ResStations[i].operation == ADDIcode ? "+ |" :
                     ResStations[i].operation == SUBcode ? "- |" : 
                     ResStations[i].operation == MULcode ? "* |" : "/ |");
            // Print values or dependency on other RS
            cout << (ResStations[i].unCompleteResultJ != -1 ? "  RS" + to_string(ResStations[i].unCompleteResultJ + 1) :
                     to_string(ResStations[i].Vj)) << " |";
            cout << (ResStations[i].unCompleteResultK != -1 ? "  RS" + to_string(ResStations[i].unCompleteResultK + 1) :
                     to_string(ResStations[i].Vk)) << " |";
        } else {
            cout << "  |      |      |";
        }
        cout << endl;
    }
    cout << "    ----------------------\n" << endl;
}

// Issue the instruction if a free RS is available
void issue() {
    if (InstIssue >= Instructions.size()) return;
    int order = Instructions[InstIssue].operation, assignedRS = -1;
    bool RSfree = false;
    
    // Select appropriate RS based on operation type
    if (order == ADDcode || order == SUBcode || order == ADDIcode) {
        for (int i = 0; i < ADD_Nums && !RSfree; i++) {
            if (!ResStations[i].busy) {
                RSfree = true;
                assignedRS = i;
            }
        }
    } else {
        for (int i = ADD_Nums; i < ADD_Nums + MULandDIV_Nums && !RSfree; i++) {
            if (!ResStations[i].busy) {
                RSfree = true;
                assignedRS = i;
            }
        }
    }
    if (!RSfree) return;

    isChange = true;
    ResStations[assignedRS].operation = order;
    ResStations[assignedRS].busy = true;
    ResStations[assignedRS].IssueLatency = 0;
    ResStations[assignedRS].InstNum = InstIssue;

    // Update dependencies and RAT entries
    int rs = Instructions[InstIssue].rs, rt = Instructions[InstIssue].rt, rd = Instructions[InstIssue].rd;
    if (RATarr[rs].Qi == RATempty) {
        ResStations[assignedRS].Vj = Register[rs];
        ResStations[assignedRS].Qj = QperandAva;
    } else {
        ResStations[assignedRS].Qj = RATarr[rs].Qi;
    }
    if (order == ADDIcode) {
        ResStations[assignedRS].Vk = Instructions[InstIssue].constant;
        ResStations[assignedRS].Qk = QperandAva;
    } else if (RATarr[rt].Qi == RATempty) {
        ResStations[assignedRS].Vk = Register[rt];
        ResStations[assignedRS].Qk = QperandAva;
    } else {
        ResStations[assignedRS].Qk = RATarr[rt].Qi;
    }
    RATarr[rd].Qi = assignedRS;
    InstIssue++;
}

// Execute instructions that are ready
void execute() {
    for (int i = 0; i < ResStations.size(); i++) {
        if (ResStations[i].busy && ResStations[i].IssueLatency >= ISSUElatency) {
            if (ResStations[i].Qj == QperandAva && ResStations[i].Qk == QperandAva) {
                bool executeCycle = true;
                // Check execution buffer
                if (ResStations[i].operation < 3 && buffer[0] == 50000) buffer[0] = i;
                else if (ResStations[i].operation >= 3 && buffer[1] == 50000) buffer[1] = i;
                else executeCycle = false;

                // Execute and check latency completion
                if (executeCycle) {
                    ResStations[i].latency++;
                    if ((ResStations[i].operation == ADDcode && ResStations[i].latency == ADDlatency) ||
                        (ResStations[i].operation == MULcode && ResStations[i].latency == MULlatency) ||
                        (ResStations[i].operation == DIVcode && ResStations[i].latency == DIVlatency)) {
                        isChange = true;
                        ResStations[i].result = (ResStations[i].operation == ADDcode ? ResStations[i].Vj + ResStations[i].Vk :
                                                 ResStations[i].operation == SUBcode ? ResStations[i].Vj - ResStations[i].Vk :
                                                 ResStations[i].operation == MULcode ? ResStations[i].Vj * ResStations[i].Vk :
                                                 ResStations[i].Vj / ResStations[i].Vk);
                        ResStations[i].complete = true;
                        buffer[ResStations[i].operation >= 3 ? 1 : 0] = 50000;
                    }
                }
            }
        } else {
            ResStations[i].IssueLatency++;
        }
    }
}

// Write results back to register file and update dependent RS
void writeback() {
    for (auto &RS : ResStations) {
        if (RS.complete && RS.WriteBackLatency == WRITEBACKlatency) {
            isChange = true;
            // Update RF and dependent RS
            for (int i = 0; i < Register.size(); i++) {
                if (RATarr[i].Qi == &RS - &ResStations[0]) Register[i] = RS.result, RATarr[i].Qi = RATempty;
            }
            for (auto &RSdep : ResStations) {
                if (RSdep.Qj == &RS - &ResStations[0]) RSdep.Vj = RS.result, RSdep.Qj = QperandAva;
                if (RSdep.Qk == &RS - &ResStations[0]) RSdep.Vk = RS.result, RSdep.Qk = QperandAva;
            }
            RS.reset();
            WRnums++;
        } else {
            RS.WriteBackLatency++;
        }
    }
}

int main() {
    // Load instructions
    ifstream file("input.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string op;
        int rd, rs, rt = 0;
        ss >> op >> rd >> rs;
        if (op != "ADDI") ss >> rt;
        int opcode = (op == "ADD" ? ADDcode : op == "SUB" ? SUBcode : op == "MUL" ? MULcode : DIVcode);
        Instructions.emplace_back(rd, rs, rt, (op == "ADDI" ? atoi(rt.c_str()) : 0), opcode);
    }

    // Initialize RS and RAT
    ResStations.resize(ADD_Nums + MULandDIV_Nums);
    RATarr.resize(6, RegisterStatus(RATempty));

    // Run simulation cycle by cycle
    while (WRnums < Instructions.size()) {
        isChange = false;
        cycleNum++;
        issue();
        execute();
        writeback();

        if (isChange) {
            cout << "Cycle " << cycleNum << ":\n" << endl;
            printRF();
            printRAT();
            printRS();
        }
    }
}
