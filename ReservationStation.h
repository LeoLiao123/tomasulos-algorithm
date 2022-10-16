#ifndef _RS_H
#define _RS_H

class ReservationStation {
public:
	bool busy;
	int Qj;
	int Qk;
	int Vj;
	int Vk;
	int latency;
	int operation;
	int result;
	bool complete;
	int InstNum;
	int IssueLatency;
	int WriteBackLatency;
	int unCompleteResultJ;
	int unCompleteResultK;
	ReservationStation();
	ReservationStation(int, int);

};
#endif //_RS_H