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


(This install setup is outdated because I'm learning how to use CMake, I will update this once I
have a more finalized CMake setup)

### Dependencies

This project depends on SDL2, SDL2_image, and SDL2_sound. These can be installed with a package manager,
or using the included shell.nix setup for systems using the nix package manager.

### Building

(Optional, if using nix)
```
nix-shell ./shell.nix
```
**If not using nix, make sure you have SDL2, SDL2_image, and SDL2_sound installed via a package manager.** These may be bundled
into a single SDL2 package depending on the distribution.

Then execute the following two shell scripts:
```
./configure.sh
./build.sh
```

This will create the executable file `./build/chip8` which can be run as a program

In order to build the interpreter, start by opening a terminal window in the project directory. 

You can run `make chip8`. Then, to run the program, simply type `./chip8`.

## Credit

[This guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) was a great resource in describing the specifications of the CHIP-8 instructions and expected behaviors.

[SDL2](https://wiki.libsdl.org/SDL2/FrontPage) Is the library I used for displaying graphics and getting user inputs.
