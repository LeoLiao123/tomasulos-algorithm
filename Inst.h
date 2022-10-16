#pragma once
#ifndef _INST_H
#define _INST_H
class Inst {
public:
	int rd;
	int rs;
	int rt;
	int constant;//for immediate inst
	int operation;
	int issue;

	Inst();
	Inst(int, int, int, int, int);
};

#endif //_INST_H
