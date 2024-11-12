# Tomasulo's Algorithm Simulation

This project implements a simulation of Tomasulo's algorithm in C++, designed to manage instructions with latency and ensure out-of-order execution for improved CPU efficiency. The algorithm leverages Reservation Stations, Register Status tables, and buffers to handle ADD, SUB, MUL, and DIV operations.

## Key Features
- **Instruction Parsing**: Reads and parses instructions for ADD, ADDI, SUB, MUL, and DIV operations from `input.txt`.
- **Functional Units**: 
  - Reservation Stations for managing operation dispatch.
  - Register Status (RAT) for tracking register allocations.
  - Buffers for intermediate results and operand availability.
- **Execution Phases**:
  - Instruction Issue: Allocates functional units if available.
  - Execute: Processes instructions based on latency and buffer availability.
  - Writeback: Writes results back to registers and releases Reservation Stations.

## Usage
1. Place instructions in `input.txt` in the format: `ADD F1, F2, F3`.
2. Run the simulation to observe cycle-by-cycle register file (RF), Reservation Station (RS), and RAT status updates.

## Example Cycle Output
```
Cycle 1:

     _ RF __
  F1 |   0 |
  F2 |   2 |
  F3 |   4 |
  F4 |   6 |
  F5 |   8 |
     -------

     _ RAT __
  F1 |  RS1 |
  F2 |      |
  F3 |      |
  F4 |      |
  F5 |      |
     --------

    _ RS _________________
RS1 |    + |    2 |    1 |
RS2 |      |      |      |
RS3 |      |      |      |
    ----------------------

BUFFER: empty

    _ RS _________________
RS4 |      |      |      |
RS5 |      |      |      |
    ----------------------

BUFFER: empty


Cycle 2:

     _ RF __
  F1 |   0 |
  F2 |   2 |
  F3 |   4 |
  F4 |   6 |
  F5 |   8 |
     -------

     _ RAT __
  F1 |  RS2 |
  F2 |      |
  F3 |      |
  F4 |      |
  F5 |      |
     --------

    _ RS _________________
RS1 |    + |    2 |    1 |
RS2 |    - |    4 |    6 |
RS3 |      |      |      |
    ----------------------

BUFFER: (RS1) 2 + 1

    _ RS _________________
RS4 |      |      |      |
RS5 |      |      |      |
    ----------------------

BUFFER: empty


Cycle 3:

     _ RF __
  F1 |   0 |
  F2 |   2 |
  F3 |   4 |
  F4 |   6 |
  F5 |   8 |
     -------

     _ RAT __
  F1 |  RS4 |
  F2 |      |
  F3 |      |
  F4 |      |
  F5 |      |
     --------

    _ RS _________________
RS1 |    + |    2 |    1 |
RS2 |    - |    4 |    6 |
RS3 |      |      |      |
    ----------------------

BUFFER: (RS2) 4 - 6

    _ RS _________________
RS4 |    / |    2 |    4 |
RS5 |      |      |      |
    ----------------------

BUFFER: empty


Cycle 4:

     _ RF __
  F1 |   0 |
  F2 |   2 |
  F3 |   4 |
  F4 |   6 |
  F5 |   8 |
     -------

     _ RAT __
  F1 |  RS4 |
  F2 |  RS5 |
  F3 |      |
  F4 |      |
  F5 |      |
     --------

    _ RS _________________
RS1 |      |      |      |
RS2 |    - |    4 |    6 |
RS3 |      |      |      |
    ----------------------

BUFFER: empty

    _ RS _________________
RS4 |    / |    2 |    4 |
RS5 |    * |    4 |    6 |
    ----------------------

BUFFER: (RS4) 2 / 4

...(See output.txt)
```


