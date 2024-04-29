# Chip8-Interpreter

Developed by Patrick Henegan

## What's a CHIP-8?

This project is an interpreter which executes instructions for the CHIP-8 microprocessor.
For those unaware, the CHIP-8 microprocessor was a platform on computers from the 1970's and
1980's, which was intended to be a standard instruction set to develop games for.

## Motivation

It's a relatively simple instruction set, and even recently there were still games developed
for it. This seemed like a good way to reinforce the low-level concepts I was learning in my
Computer Systems class and Digital Design class (the latter is about the electronic design of a RISC-V CPU).

This is also a good starting point before moving on to other retro interpreters/emulators,
and seemed interesting to learn about.

## Setup

This interpreter is written in C using GCC. The only requirements for the emulator are a
glibc implementation and (once I get graphics working) SDL2.

This project was developed and tested on linux_x86, and depends on  `time.h`, `fcntl.h`,
and `unistd.h`. In theory, this code should work on any POSIX-compliant system, but I don't
have access to a MacOS system. From what I can tell, none of the above libraries work
in Windows either, though it would probably work with something like WSL 2 if you're dedicated enough.

In order to build the interpreter, start by opening a terminal window in the project directory. 

You can run `make chip8`. Then, to run the program, simply type `./chip8`.

## Credit

[This guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) was a great resource in describing the specifications of the CHIP-8 instructions and expected behaviors.
