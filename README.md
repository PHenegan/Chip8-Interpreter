# Chip8-Interpreter

Developed by Patrick Henegan

## What's a CHIP-8?

This project is an interpreter which executes instructions for the CHIP-8 system.
The CHIP-8 was a virtual platform (which sometimes had its own physical hardware)
on some computers from the 1970's and 1980's. 
It was intended to make game development easier by providing a standard instruction set.

## Motivation

It's a relatively simple instruction set, and even recently there were still games developed
for it. This seemed like a good way to reinforce the low-level concepts I was learning in my
Computer Systems class and Digital Design class (the latter is about the electronic design
of a RISC-V CPU).

This is also a good starting point before moving on to other retro interpreters/emulators,
and seemed interesting to learn about.

## What works? What's left?
- the chip-8 sound timer works but currently doesn't actually play any sound. i want to see if i can
  play a tone or something directly through sdl but i may have to find an mp3 or wav file for it.

(These ones are lower priority because they would require a decent amount of work and I want to
move onto other projects)
- right now the controls are set in a header file which is relatively easy to configure, but
  adding support for something like a json or yaml config file would allow more flexibility by
  offering configuration at runtime instead of compile-time. This is low priority because it
  would likely involve bringing in another library for JSON parsing
- I may eventually add CHIP-48 or SUPER-CHIP support. This is also low priority since I want to
  move onto other projects.

## Setup

This interpreter is written in C using GCC. The only requirements are a
glibc implementation and (once I get graphics working) SDL2.

This project was developed and tested on linux_x86, and depends on  `time.h`, `fcntl.h`,
and `unistd.h`. In theory, this code should work on any POSIX-compliant system, but I don't
have access to a MacOS system. From what I can tell, none of the above libraries work
in Windows either, though it would probably work with something like WSL 2
if you're dedicated enough.

### Dependencies

This project depends on SDL2, SDL2_image, and SDL2_sound. These can be installed with 
a package manager or by using the included shell.nix setup (if you have nix-shell).

As mentioned above, this project depends on some POSIX libraries. In addition to those, I use 
the C standard library throughout the codebase.

### Building

(Optional, if using nix)
```
nix-shell ./shell.nix
```
**If not using nix, make sure you have SDL2, SDL2_image, and SDL2_sound installed via a package
manager.** These may be bundled into a single SDL2 package depending on the distribution.

Then execute the following two shell scripts:
```
./configure.sh
./build.sh
```

This will create the executable file `./build/chip8` which can be run as a program

In order to build the interpreter, start by opening a terminal window in the project directory. 

You can run `make chip8`. Then, to run the program, simply type `./chip8 [program-filepath-here]`.

To run the program in debug mode, you can execute the program with the `--debug` command line
argument. This will allow you to step through the program manually (press `N`), 
and pause/unpause the manual-mode with `Enter`.

In either debug or regular mode, holding `Ctrl + C` will stop the program from running, 
which is useful because many programs end by infinitely looping in a finished state.

## Credit

[This guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
was a great resource in describing the specifications of the CHIP-8 instructions and expected
behaviors.

[SDL2](https://wiki.libsdl.org/SDL2/FrontPage) Is the library I used for displaying graphics 
and getting user inputs.
