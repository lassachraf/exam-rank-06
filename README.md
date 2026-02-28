# 42 Cursus — Exam Rank 06  
## 🖥️ TCP Chat Server (select-based)

This repository contains my implementation of the **Exam Rank 06** project from the 42 Common Core curriculum.

The project consists of building a minimal TCP chat server capable of handling multiple clients simultaneously using `select()` for I/O multiplexing.

---

## 🧠 Project Overview

The program implements a simple TCP server that:

- Listens on `127.0.0.1`
- Accepts multiple client connections
- Assigns a unique ID to each client
- Broadcasts messages to all connected clients
- Notifies clients when someone joins or leaves
- Handles partial messages and line buffering
- Manages multiple file descriptors using `select()`

The server runs in an infinite loop and handles all communication without threads or forks.

---

## ⚙️ Technical Concepts Used

This project demonstrates mastery of:

- `socket()`
- `bind()`
- `listen()`
- `accept()`
- `select()`
- `recv()`
- `send()`
- `FD_SET`, `FD_CLR`, `FD_ISSET`
- File descriptor management
- Network byte order (`htonl`, `htons`)
- Basic TCP/IP networking
- Buffered message reconstruction

---

## 🏗️ Architecture

### Global Structures

- `clients[1024]`  
  Stores:
  - Unique client ID
  - Buffered message until newline

- `fd_set fds`  
  Master file descriptor set

- `read_fds` / `write_fds`  
  Copies used by `select()`

---

### Main Server Flow

1. Initialize socket
2. Bind to `127.0.0.1:<port>`
3. Listen for connections
4. Add server socket to `fds`
5. Enter infinite `select()` loop
6. Handle:
   - New connections
   - Client messages
   - Disconnections

---

## 🔄 Message Handling Logic

Messages are buffered per client until a newline (`\n`) is received.

When a full line is detected:
- It is formatted as:
```bash
client <id>: <message>
```
- Broadcasted to all other connected clients
- Client buffer is cleared

This ensures:
- Proper handling of partial TCP packets
- No message interleaving
- Clean separation of client messages

---

## 🚀 Compilation

```bash
gcc -Wall -Wextra -Werror mini_serv.c -o server
```

## ▶️ Usage

```bash
./server <port>
```

## 📌 Behavior

When a client connects:

```bash
server: client <id> just arrived
```

When a client disconnects:

```bash
server: client <id> just left
```


When a client sends a message:

```bash
client <id>: <message>
```

## 🛡️ Error Handling

- Fatal errors immediately terminate the program
- Disconnections are properly handled
- File descriptors are cleaned up
- Buffers are cleared after use

## 🎯 What This Exam Demonstrates

- Understanding of low-level networking
- Correct usage of select() for multiplexing
- Efficient FD management
- Real-time multi-client communication
- Robust handling of TCP stream behavior
- Writing clean C under time constraints

## 🏆 Status

✔ Exam Rank 06 — Passed
📅 Completed: February, 25th 2026

## 👨‍💻 Author

alassiqu
42 Network Student