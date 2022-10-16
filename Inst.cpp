#include"Inst.h"
Inst::Inst() {
	rd = 0;
	rs = 0;
	rt = 0;
	constant = 0;
	operation = 0;
	issue = 0;
}

Inst::Inst(int RD, int RS, int RT, int Const, int OP) {
	rd = RD;
	rs = RS;
	rt = RT;
	constant = Const;
	operation = OP;
	issue = 0;
}