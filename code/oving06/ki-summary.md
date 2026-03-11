Context for the task:

The provided code runs a Node.js program that starts TWO servers using raw TCP sockets.

1) HTTP server (port 3000)
- Serves a simple HTML test page.
- When you open http://localhost:3000 in the browser, it returns an HTML page.
- That page contains JavaScript.

2) Browser JavaScript
- The HTML page runs JS that creates a WebSocket connection.
- The test page provides an input + Send button and a log area.
- When the client receives a message, it appends it to the on-page log.

3) WebSocket server (port 3001)
- This is the part we must implement.
- It currently only accepts TCP connections and prints incoming data.

Assignment goal:
Implement a WebSocket server library using raw socket programming (no WebSocket libraries) according to RFC6455.

The server must:
• Perform the WebSocket HTTP upgrade handshake
• Read short WebSocket messages from clients
• Send short messages to clients
• Broadcast received messages to all connected clients

Flow of the program:

Browser
   │
   │ GET http://localhost:3000
   ▼
HTTP server (serves test page)

Browser executes JS
   │
   │ WebSocket connection
   ▼
ws://localhost:3001
   │
   ▼
Your WebSocket server implementation

Important:
Port 3000 only serves the webpage.
Port 3001 is where the WebSocket protocol must be implemented.

Implementation started in this workspace:

- [ws-lib.js](ws-lib.js) implements a minimal RFC6455 WebSocket server “library” on top of a raw TCP socket.
   - Parses the HTTP Upgrade request, validates required headers, and replies with `101 Switching Protocols`.
   - Decodes incoming WebSocket frames from browser clients (masked frames), and supports short text messages.
   - Encodes and sends server-to-client frames (unmasked) and provides a `broadcast()` helper.
- [server.js](server.js) wires it up with the provided HTTP test page (port 3000) + WebSocket endpoint (port 3001).
   - The served test page connects to `ws://127.0.0.1:3001` (avoids potential IPv6/`localhost` resolution issues).
   - The served test page disables caching (`Cache-Control: no-store`) to reduce stale-page issues while debugging.

RFC6455 notes used:

1) Handshake (HTTP Upgrade)
- Browser sends an HTTP request containing at least:
   - `Upgrade: websocket`
   - `Connection: Upgrade`
   - `Sec-WebSocket-Version: 13`
   - `Sec-WebSocket-Key: <random base64>`
- Server computes:
   - `Sec-WebSocket-Accept = base64( SHA1( Sec-WebSocket-Key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" ) )`
- Server responds:
   - `HTTP/1.1 101 Switching Protocols`
   - `Upgrade: websocket`
   - `Connection: Upgrade`
   - `Sec-WebSocket-Accept: ...`

2) Frames (data messages)
- Client → server frames are **masked** (must XOR payload using the 4-byte mask key).
- Server → client frames are **not masked**.
- We handle:
   - Text frames (`opcode = 0x1`) and Close/Ping/Pong.
   - “Short” payloads (<= 125 bytes) plus 16-bit extended length (126) for robustness.
- For simplicity (fits the assignment’s “korte meldinger”), fragmented frames (`FIN=0`) are rejected with a close.

How to run/test:

1) Start the servers:
    - `node server.js`
2) Open:
    - `http://localhost:3000`
3) Wait for status to become “Connected”, then type a message and press Send.
4) Open the page in two tabs to verify broadcast (both tabs should log incoming messages).

Quick verification checklist (what to demonstrate when delivering):

- Handshake: server responds with `101 Switching Protocols` and `Sec-WebSocket-Accept`.
- Masking: server correctly unmasks client → server frames (required by RFC6455).
- Messaging: server reads a short text message and can send a short text message back.
- Broadcast: open two browser tabs on `http://localhost:3000` and verify both receive the message.

Limitations / scope choices:

- Only text messages are supported (`opcode=0x1`).
- No fragmentation support: frames with `FIN=0` are rejected (fits “korte meldinger”).
- No extensions (e.g. permessage-deflate) and no TLS (`wss://`).
- Payload lengths > 65535 are not a target for this assignment (but basic handling is present).

Debugging notes from implementation/testing:

- If the page is stuck on “Connecting…” and the Send button never enables, the page’s JavaScript is likely not running.
   - In Brave, Shields or an extension can block scripts even on `localhost`.
   - Use a hard refresh (`Ctrl+Shift+R`) after changes.
- Be careful when embedding JavaScript inside a Node template literal: writing `"\n"` inside the server-side string can become an actual newline in the browser source and break parsing.
   - Use `"\\n"` so the browser receives a valid `\n` escape sequence.