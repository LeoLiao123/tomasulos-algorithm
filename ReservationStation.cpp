#include"ReservationStation.h"

ReservationStation::ReservationStation() {
	busy = false;
	operation = 0;
	latency = 0;
	result = 0;
	complete = false;
	Qj = 0;
	Qk = 0;
	Vj = 0;
	Vk = 0;
	InstNum = 10000;
	IssueLatency = 0;
	WriteBackLatency = 0;
}

ReservationStation::ReservationStation(int OP, int RegisterOpAva) {
	busy = false;
	operation = OP;
	latency = 0;
	result = 0;
	complete = false;
	Qj = RegisterOpAva;
	Qk = RegisterOpAva;
	Vj = 0;
	Vk = 0;
	InstNum = 100000;
	IssueLatency = 0;
	WriteBackLatency = 0;
	unCompleteResultJ = -1;
	unCompleteResultK = -1;
}