# Connect4
Connect4 engine, written in C.

## Protocol
Input is a sequence of numbers representing subsequent move columns.
Output is:
- position (ascii art)
- info lines for root moves
- info line for best move
- bestmove

## Limitations

Cannot really handle first couple moves.
It needs to start from ply 5 or so.

## Features

- Negamax with PVS
- Transposition Table with Zobrist hashing and vertical symmetry
- Move Ordering with Killer, History, and File Bonuses
- Bitboards Representation

## Building
To build run:
```bash
git clone https://github.com/Rentib/connect4
cd connect4/src
make
```
