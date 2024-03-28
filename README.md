# OS_Simulation

This program simulates the behavior of a simple operating system with various processes and commands. It provides an interface for users to interact with the simulated operating system through a command-line interface.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Commands](#Commands)
- [Contributing](#contributing)

## Introduction

This operating system simulation program mimics the behavior of a basic operating system, allowing users to create processes, perform process management operations such as forking and killing processes, manage semaphores, and more.

## Features

- Process creation and management: Users can create new processes with different priorities, fork existing processes, kill processes, and exit processes.
- Message passing: Processes can send messages to each other and reply to received messages.
- Semaphore management: Users can create and manage semaphores, perform semaphore P (wait) and V (signal) operations.
- Information retrieval: The program provides commands to retrieve detailed information about processes and display the status of process queues.

## Getting Started
To get started with OS_Simulation, follow these steps:

1. Clone the repository:
    ```bash
    git clone https://github.com/erfanshafagh/OS_Simulation.git
    ```

2. Compile the program:
    ```bash
    make
    ```

3. Run the program :
    ```bash
    ./OS-Simulation
    ```

## Usage

Once the program is compiled and ready to run, follow these steps to use the operating system simulation:

1. Open a terminal or command prompt.
2. Navigate to the directory where the compiled executable is located.
3. Run the executable.
4. Follow the on-screen instructions to interact with the simulated operating system. Enter command characters corresponding to the desired actions (e.g., create a process, send a message).

## Commands

1. C/c: Create a new process.
2. F/f: Fork the current process.
3. K/k: Kill a process.
4. E/e: Exit the current process.
5. Q/q: Execute a quantum for the current process using round robin.
6. S/s: Send a message to another process.
7. R/r: Receive a message.
8. Y/y: Reply to a message.
9. N/n: Create a new semaphore.
10. P/p: Perform a semaphore P (wait) operation.
11. V/v: Perform a semaphore V (signal) operation.
12. I/i: Display process information for a specific process.
13. T/t: Display total process information.


## Contributing

If you find any issues or have suggestions for improvement, feel free to open an [issue](https://github.com/erfanshafagh/OS_Simulation/issues) or create a [pull request](https://github.com/erfanshafagh/OS_Simulation/pulls).

