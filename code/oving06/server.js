const net = require('net');
const { WebSocketServer } = require('./ws-lib');

// Simple HTTP server responds with a simple WebSocket client test
const httpServer = net.createServer((connection) => {
  connection.on('data', () => {
    const content = `<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8" />
</head>
<body>
<div>WebSocket test page</div>

<noscript>
  <div style="margin-top: 12px; color: red;">
    JavaScript is disabled. Enable JavaScript for localhost to use WebSockets.
  </div>
</noscript>

<div style="margin-top: 12px;">
  <div id="status">Connecting...</div>
  <div id="state" style="margin-top: 4px;">(If this never changes, JS may be blocked)</div>
  <div style="margin-top: 8px;">
    <input id="msg" type="text" placeholder="Type a message" />
    <button id="send" type="button" disabled>Send</button>
  </div>
  <pre id="log" style="margin-top: 8px; white-space: pre-wrap;"></pre>
</div>

<script>
// Use 127.0.0.1 to avoid IPv6/localhost resolution issues on some systems.
let ws = new WebSocket('ws://127.0.0.1:3001');
const statusEl = document.getElementById('status');
const stateEl = document.getElementById('state');
const inputEl = document.getElementById('msg');
const sendEl = document.getElementById('send');
const logEl = document.getElementById('log');

function log(line) {
  logEl.textContent += line + "\\n";
}

window.addEventListener('error', (e) => {
  log('JS error: ' + (e.message || e.type));
});

log('Script loaded');

sendEl.disabled = true;

function updateStateLine() {
  const names = ['CONNECTING', 'OPEN', 'CLOSING', 'CLOSED'];
  if (!stateEl) return;
  stateEl.textContent = 'readyState=' + ws.readyState + ' (' + (names[ws.readyState] || 'UNKNOWN') + ')';
}

updateStateLine();
setInterval(updateStateLine, 250);

ws.onopen = () => {
  statusEl.textContent = 'Connected';
  sendEl.disabled = false;
  log('Connected');
  updateStateLine();
};

ws.onclose = () => {
  statusEl.textContent = 'Disconnected';
  sendEl.disabled = true;
  log('Disconnected');
  updateStateLine();
};

ws.onerror = () => {
  statusEl.textContent = 'Error';
  log('WebSocket error');
  updateStateLine();
};

ws.onmessage = event => {
  log('Message from server: ' + event.data);
};

function sendMessage() {
  const text = inputEl.value;
  if (!text) return;
  if (ws.readyState !== WebSocket.OPEN) {
    log('Not connected yet; readyState=' + ws.readyState);
    return;
  }
  ws.send(text);
  log('You: ' + text);
  inputEl.value = '';
}

sendEl.onclick = sendMessage;
inputEl.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') sendMessage();
});
</script>
</body>
</html>`;

    connection.write(
      'HTTP/1.1 200 OK\r\n' +
        'Content-Type: text/html; charset=utf-8\r\n' +
        'Cache-Control: no-store\r\n' +
        'Connection: close\r\n' +
        'Content-Length: ' +
        Buffer.byteLength(content, 'utf8') +
        '\r\n\r\n' +
        content
    );

    connection.end();
  });
});

httpServer.listen(3000, () => {
  console.log('HTTP server listening on port 3000');
});

const wsServerNet = net.createServer();
const wsServer = new WebSocketServer({
  server: wsServerNet,
  onMessage: (text) => {
    console.log('WS message:', text);
    wsServer.broadcast(text);
  },
});

wsServerNet.on('connection', (connection) => {
  console.log('Client connected');
  wsServer.handleConnection(connection);
});

wsServerNet.on('error', (error) => {
  console.error('Error:', error);
});

wsServerNet.listen(3001, () => {
  console.log('WebSocket server listening on port 3001');
});
