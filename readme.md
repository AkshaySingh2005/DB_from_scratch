# MyDB - A Database Engine from Scratch

## Welcome to MyDB! This is my journey into the deep, dark world of database internals. Instead of just using a library, I decided to build one. Heavily inspired by the "Small but Mighty" SQLite. 

## 👉 What's Cooking? (Current Progress)

- \[1\] **Tokenization:** Turning raw strings into "meaningful" words.
- \[2\] **Query Parser:** Understanding what the user actually wants to do.
- \[3\] **The Storage Engine:** Pager, Slotted Pages, and Heap Tables are alive and kicking.
- \[4\] **TUI App:** I'm planning a terminal interface so you can *see* the data moving in real-time.
- \[5\] **B+ Trees:** Because scanning every page to find one user is for amateurs.

## 👉 How to Play with It

If you have `cmake` and a C++ compiler, you're good to go:
Bash
```
mkdir build && cd build
cmake ..
make
./shell
```