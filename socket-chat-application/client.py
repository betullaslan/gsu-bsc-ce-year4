import socket
import threading
import sys
import msvcrt  
import select

ENC = "utf-8"  # Encoding for all socket traffic
INPUT_SEP = "//"  # Separator used in user input
WIRE_SEP = "|"  # Separator used on the wire
EOL = "\n"  # Line terminator sent over the wire

SERVER_IP = "10.244.68.54"  # Target server IP
PORT = 5050  # Target server port

PROMPT_TEXT = (
    "Type messages like:  RECEIVER // MESSAGE\n"
    "For introduction:    NAME // NAME\n"
    "Type q to exit.\n"
)
PROMPT_PREFIX = "You: "  # Prefix shown before the local input buffer

stdout_lock = threading.Lock()  # Prevents interleaved console output

def _erase_input_line(prefix: str, buf: str) -> None:
    """Clear the current input line from the console."""
    with stdout_lock:
        sys.stdout.write("\r" + (" " * (len(prefix) + len(buf))) + "\r")
        sys.stdout.flush()

def _show_prompt(prefix: str, buf: str) -> None:
    """Re-render the input prompt and buffer."""
    with stdout_lock:
        sys.stdout.write(prefix + buf)
        sys.stdout.flush()

def _print_line(text: str) -> None:
    """Thread-safe print helper."""
    with stdout_lock:
        print(text)

def send_line(conn: socket.socket, text: str) -> None:
    conn.sendall((text + EOL).encode(ENC))

class State:
    def __init__(self):
        self.input_buf = ""
        self.running = True
        self.prompt_prefix = PROMPT_PREFIX
        self.my_name: str | None = None
        self.lock = threading.Lock()

state = State()

def listener_thread(sock: socket.socket, st: State):
    recv_buf = bytearray()
    sock.setblocking(False)
    while st.running:
        try:
            # Poll for incoming data without blocking typing
            rlist, _, _ = select.select([sock], [], [], 0.2)
            if sock not in rlist:
                continue
            data = sock.recv(4096)
            if not data:
                _erase_input_line(st.prompt_prefix, st.input_buf)
                _print_line("[Connection lost]")
                st.running = False
                break
            recv_buf.extend(data)
            while True:
                try:
                    i = recv_buf.index(b"\n")
                except ValueError:
                    break
                line = recv_buf[:i]
                del recv_buf[: i + 1]
                raw = line.decode(ENC, errors="replace").rstrip("\r\n")
                if WIRE_SEP in raw:
                    f1, f2 = raw.split(WIRE_SEP, 1)
                    f1, f2 = f1.strip(), f2.strip()
                    if f1 == "SERVER":
                        msg = f"[SERVER] {f2}"
                    else:
                        msg = f"{f1}: {f2}"
                else:
                    msg = raw
                _erase_input_line(st.prompt_prefix, st.input_buf)
                _print_line(msg)
                _show_prompt(st.prompt_prefix, st.input_buf)
        except Exception as e:
            _erase_input_line(st.prompt_prefix, st.input_buf)
            _print_line(f"[Listener error] {e}")
            st.running = False
            break

def input_thread(sock: socket.socket, st: State):
    _print_line(f"[CONNECTED] Server: {SERVER_IP}:{PORT}\n")
    _print_line(PROMPT_TEXT)
    _show_prompt(st.prompt_prefix, st.input_buf)
    try:
        while st.running:
            if not msvcrt.kbhit():
                continue
            ch = msvcrt.getwch()
            if ch in ("\r", "\n"):
                line = st.input_buf.strip()
                _erase_input_line(st.prompt_prefix, st.input_buf)
                st.input_buf = ""
                if not line:
                    _show_prompt(st.prompt_prefix, st.input_buf)
                    continue
                if line.lower() in {"q", "quit", "exit"}:
                    _print_line("[Exit]")
                    st.running = False
                    try:
                        sock.shutdown(socket.SHUT_RDWR)
                    except Exception:
                        pass
                    break
                if INPUT_SEP not in line:
                    _print_line(" Format: RECEIVER // MESSAGE  (intro: NAME // NAME)")
                    _show_prompt(st.prompt_prefix, st.input_buf)
                    continue
                f1, f2 = line.split(INPUT_SEP, 1)
                f1, f2 = f1.strip(), f2.strip()
                if f1 == f2 and f1:
                    st.my_name = f1
                if not f1 or not f2:
                    _print_line("Fields cannot be empty. Example: ipek // ipek  or  zeynep // hello")
                    _show_prompt(st.prompt_prefix, st.input_buf)
                    continue
                try:
                    send_line(sock, f"{f1}{WIRE_SEP}{f2}")
                    if f1 == f2:
                        label = st.my_name or "You"
                        _print_line(f"[You] Intro: {label}")
                    else:
                        label = st.my_name or "You"
                        _print_line(f"{label}: {f2}")
                except OSError:
                    _print_line("[Error] Message could not be sent (connection closed)")
                    st.running = False
                    break
                _show_prompt(st.prompt_prefix, st.input_buf)
            elif ch == "\x08":  # Backspace key
                if st.input_buf:
                    st.input_buf = st.input_buf[:-1]
                    with stdout_lock:
                        sys.stdout.write("\b \b")
                        sys.stdout.flush()
            elif ch == "\x03":  # Ctrl+C exit
                _erase_input_line(st.prompt_prefix, st.input_buf)
                _print_line("[Ctrl+C]\n[Exit]")
                st.running = False
                try:
                    sock.shutdown(socket.SHUT_RDWR)
                except Exception:
                    pass
                break
            else:
                st.input_buf += ch
                with stdout_lock:
                    sys.stdout.write(ch)
                    sys.stdout.flush()
    finally:
        try:
            sock.close()
        except Exception:
            pass

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_IP, PORT))
    t1 = threading.Thread(target=listener_thread, args=(s, state), daemon=True)
    t2 = threading.Thread(target=input_thread, args=(s, state), daemon=False)
    t1.start()
    t2.start()
    t2.join()

if __name__ == "__main__":
    main()
