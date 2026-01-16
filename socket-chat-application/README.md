# Python Socket-Based Chat Application

This project implements a **client–server chat application** using Python sockets and multithreading.  
The system allows multiple clients to connect to a central server, register with a unique username, and exchange messages either **directly** or via **broadcast**.

The application demonstrates fundamental networking concepts such as TCP socket communication, concurrent client handling, message routing, and thread-safe I/O operations.

---

## Project Info

- **Project Name:** Python Socket-Based Chat Application  
- **Language:** Python  
- **Author:** Betul Aslan  

---

## Academic Info

- **Course:** Distributed Systems and Applications
- **Institution:** Galatasaray University  
- **Department:** Computer Engineering  
- **Academic Year:** 2025–2026 Fall  
- **Assignment:** Coursework

---

## System Overview

The application follows a **client–server architecture**:

- The **server** listens for incoming TCP connections and manages connected clients.
- Each **client** connects to the server, registers with a username, and sends or receives messages.
- Communication is text-based and line-oriented over TCP.

---

## Features

### Server (`server.py`)
- Accepts multiple concurrent client connections
- Enforces **unique username registration**
- Supports:
  - **Direct messaging** between clients
  - **Broadcast messaging** using the keyword `ALL`
- Thread-safe message delivery using per-connection locks
- Graceful handling of client disconnections

### Client (`client.py`)
- Real-time, interactive console interface
- Separate threads for input handling and message listening
- Non-blocking user input while receiving messages
- Graceful exit using `q`, `exit`, or `Ctrl+C`

---

## Message Protocol

### User Registration
After establishing a connection, each client must register by sending a message where the sender and receiver fields are identical.  
This step assigns a unique username to the client and is mandatory before any messaging is allowed.

---

### Direct Messaging
Once registered, a client can send a private message by specifying the recipient’s username and the message content.  
The server routes the message only to the intended recipient.

---

### Broadcast Messaging
Clients can send a broadcast message by using a reserved keyword as the receiver field.  
The server forwards the message to all connected clients except the sender.
