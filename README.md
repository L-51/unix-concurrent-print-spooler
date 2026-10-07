# Unix Concurrent Print Spooler

## Overview
A concurrent, multi-process print spooler built in C for POSIX/Linux environments. This project demonstrates advanced Unix system programming, focusing on Inter-Process Communication (IPC), process synchronization, and signal handling. 

The system simulates a print server that safely handles concurrent print requests from multiple clients without data corruption or interleaving.

## Architecture & IPC Mechanisms
* **Client-Server Communication:** Utilizes named pipes (FIFOs) for the initial handshake.
* **Process Spawning:** The main server uses `fork()` and `exec()` to spawn dedicated `proxy` processes for each incoming client connection.
* **I/O Redirection:** Uses `dup()`/`dup2()` to redirect standard input streams between the client FIFOs and the proxies.
* **File Locking:** Implements `fcntl()` record locking (`F_WRLCK`) to ensure that only one proxy can write to the shared "printer" (standard output) at a time, preventing race conditions.
* **Signal Handling:** Safely traps `SIGCHLD` to reap child processes and prevent zombies, and handles `SIGPIPE` for broken pipe gracefully.

## Components
1. `servidor.c`: The main daemon listening for client requests and spawning proxies.
2. `clientes.c`: A multiprocess load generator that spawns concurrent clients to stress-test the spooler.
3. `proxy.c`: The dedicated worker process that buffers client data into a `tmpfile()` and flushes it to stdout once an exclusive `fcntl()` lock is acquired.

## How to Run
```bash
# Compile the components
gcc -o servidor servidor.c
gcc -o clientes clientes.c
gcc -o proxy proxy.c

# Terminal 1: Start the print server with a base FIFO name
./servidor my_print_queue

# Terminal 2: Spawn 5 concurrent clients to send jobs to the server
./clientes my_print_queue 5
