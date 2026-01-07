import socket
import threading
from typing import Dict, Optional, Tuple
from threading import Lock
 
ENC = "utf-8"  # Encoding for all socket traffic
WIRE_SEP = "|"  # Delimiter between sender and message
EOL = "\n"  # Line terminator sent over the wire

HOST = "0.0.0.0"  # Listen on all interfaces
PORT = 5050  # Default port for this chat server

clients_by_name: Dict[str, socket.socket] = {}  # name -> socket
send_locks_by_conn: Dict[socket.socket, Lock] = {}  # conn -> write lock

clients_lock = threading.Lock()  # Guards both registries above
 
def _get_send_lock(conn: socket.socket) -> Optional[Lock]:
    """Return the send lock for the connection, if it exists."""
    with clients_lock:
        return send_locks_by_conn.get(conn)

def _send_with_lock(conn: socket.socket, text: str) -> None:
    """Prevent concurrent writes to the same socket by locking."""
    lock = _get_send_lock(conn)
    if lock is None:
        return
    with lock:
        try:
            conn.sendall((text + EOL).encode(ENC))
        except Exception:
            pass

def send_line(conn: socket.socket, text: str) -> None:
    """Basic send helper without locking."""
    try:
        conn.sendall((text + EOL).encode(ENC))
    except Exception:
        pass

def register(name: str, conn: socket.socket) -> bool:
    """Register a new user; returns False if the name is already taken."""
    with clients_lock:
        if name in clients_by_name and clients_by_name[name] is not conn:
            return False
        clients_by_name[name] = conn
        if conn not in send_locks_by_conn:
            send_locks_by_conn[conn] = threading.Lock()
    print(f"[SERVER] User registered: {name} ({conn.getpeername()[0]}:{conn.getpeername()[1]})")
    return True

def unregister(conn: socket.socket) -> None:
    """Remove the connection from tracking structures."""
    removed_name = None
    with clients_lock:
        for k, v in list(clients_by_name.items()):
            if v is conn:
                removed_name = k
                del clients_by_name[k]
        if conn in send_locks_by_conn:
            del send_locks_by_conn[conn]
    if removed_name:
        print(f"[SERVER] User disconnected: {removed_name}")
    else:
        print(f"[SERVER] Connection closed (no name).")
 
def handle_client(conn: socket.socket, addr: Tuple[str, int]) -> None:
    try:
        print(f"[SERVER] New connection: {addr[0]}:{addr[1]}")
        _send_with_lock(conn, "SERVER|Register with: NAME|NAME (client: NAME // NAME)")
 
        buf = bytearray()
        name: Optional[str] = None
 
        while True:
            data = conn.recv(4096)
            if not data:
                break
            buf.extend(data)
 
            while True:
                try:
                    i = buf.index(b"\n")
                except ValueError:
                    break
 
                line = buf[:i]
                del buf[: i + 1]
                raw = line.decode(ENC, errors="replace").strip()
                if WIRE_SEP not in raw:
                    continue
 
                f1, f2 = (x.strip() for x in raw.split(WIRE_SEP, 1))

                # First message must be the self-introduction: NAME|NAME
                if name is None and f1 and f2 and f1 == f2:
                    candidate = f1
                    if register(candidate, conn):
                        name = candidate
                        _send_with_lock(conn, "SERVER|Registration successful")
                    else:
                        _send_with_lock(conn, f"SERVER|Name in use: {candidate}")
                        print(f"[SERVER] Name conflict: {candidate}")
                        return
                    continue

                if name is None:
                    _send_with_lock(conn, "SERVER|Register first: NAME|NAME")
                    continue

                receiver = f1
                message = f2

                if receiver.upper() == "ALL":
                    # Broadcast to everyone except the sender
                    with clients_lock:
                        targets = [c for _, c in clients_by_name.items() if c is not conn]
                    for c in targets:
                        _send_with_lock(c, f"{name}|{message}")
                else:
                    # Direct message to a single recipient
                    with clients_lock:
                        target = clients_by_name.get(receiver)
                    if target:
                        _send_with_lock(target, f"{name}|{message}")
                    else:
                        _send_with_lock(conn, f"SERVER|User not found: {receiver}")
                        print(f"[SERVER] User not found: {receiver}")
    finally:
        unregister(conn)
        try:
            conn.close()
        except Exception:
            pass
 
def main() -> None:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(100)
        print(f"[SERVER] Listening on {HOST}:{PORT}")
        while True:
            conn, addr = s.accept()
            t = threading.Thread(target=handle_client, args=(conn, addr), daemon=True)
            t.start()
 
if __name__ == "__main__":
    main()
