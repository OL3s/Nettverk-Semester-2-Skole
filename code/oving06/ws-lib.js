const crypto = require('crypto');

const WS_GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';

function computeAcceptValue(secWebSocketKey) {
  return crypto
    .createHash('sha1')
    .update(secWebSocketKey + WS_GUID, 'binary')
    .digest('base64');
}

function tryParseHttpRequest(buffer) {
  const headerEnd = buffer.indexOf('\r\n\r\n');
  if (headerEnd === -1) return null;

  const rawHeaders = buffer.slice(0, headerEnd).toString('utf8');
  const rest = buffer.slice(headerEnd + 4);

  const lines = rawHeaders.split('\r\n');
  const requestLine = lines.shift() || '';
  const [method, path, httpVersion] = requestLine.split(' ');

  const headers = {};
  for (const line of lines) {
    const idx = line.indexOf(':');
    if (idx === -1) continue;
    const name = line.slice(0, idx).trim().toLowerCase();
    const value = line.slice(idx + 1).trim();
    headers[name] = value;
  }

  return { method, path, httpVersion, headers, rest };
}

function encodeFrame({ opcode, payload }) {
  const finAndOpcode = 0x80 | (opcode & 0x0f);
  const payloadLength = payload.length;

  if (payloadLength < 126) {
    return Buffer.concat([Buffer.from([finAndOpcode, payloadLength]), payload]);
  }

  if (payloadLength <= 0xffff) {
    const ext = Buffer.alloc(2);
    ext.writeUInt16BE(payloadLength, 0);
    return Buffer.concat([Buffer.from([finAndOpcode, 126]), ext, payload]);
  }

  // Not needed for the assignment (short messages), but implemented for completeness.
  const ext = Buffer.alloc(8);
  ext.writeBigUInt64BE(BigInt(payloadLength), 0);
  return Buffer.concat([Buffer.from([finAndOpcode, 127]), ext, payload]);
}

function tryDecodeFrame(buffer) {
  if (buffer.length < 2) return null;

  const b1 = buffer[0];
  const b2 = buffer[1];

  const fin = (b1 & 0x80) !== 0;
  const opcode = b1 & 0x0f;
  const masked = (b2 & 0x80) !== 0;
  let length = b2 & 0x7f;

  let offset = 2;

  if (length === 126) {
    if (buffer.length < offset + 2) return null;
    length = buffer.readUInt16BE(offset);
    offset += 2;
  } else if (length === 127) {
    if (buffer.length < offset + 8) return null;
    const bigLen = buffer.readBigUInt64BE(offset);
    if (bigLen > BigInt(Number.MAX_SAFE_INTEGER)) {
      throw new Error('Frame too large');
    }
    length = Number(bigLen);
    offset += 8;
  }

  let maskingKey = null;
  if (masked) {
    if (buffer.length < offset + 4) return null;
    maskingKey = buffer.subarray(offset, offset + 4);
    offset += 4;
  }

  if (buffer.length < offset + length) return null;

  let payload = buffer.subarray(offset, offset + length);
  const remaining = buffer.subarray(offset + length);

  if (masked && maskingKey) {
    const unmasked = Buffer.allocUnsafe(payload.length);
    for (let i = 0; i < payload.length; i++) {
      unmasked[i] = payload[i] ^ maskingKey[i % 4];
    }
    payload = unmasked;
  }

  return { frame: { fin, opcode, masked, payload }, remaining };
}

class WebSocketServer {
  constructor({ server, onMessage } = {}) {
    this.server = server;
    this.onMessage = onMessage;
    this.clients = new Set();
  }

  handleConnection(connection) {
    const state = {
      isWebSocket: false,
      buffer: Buffer.alloc(0),
    };

    const cleanup = () => {
      this.clients.delete(connection);
    };

    connection.on('data', (chunk) => {
      state.buffer = Buffer.concat([state.buffer, chunk]);

      if (!state.isWebSocket) {
        const req = tryParseHttpRequest(state.buffer);
        if (!req) return;

        state.buffer = req.rest;

        const upgrade = (req.headers['upgrade'] || '').toLowerCase();
        const connectionHdr = (req.headers['connection'] || '').toLowerCase();
        const version = (req.headers['sec-websocket-version'] || '').trim();
        const key = (req.headers['sec-websocket-key'] || '').trim();

        const isUpgrade =
          req.method === 'GET' &&
          upgrade === 'websocket' &&
          connectionHdr.includes('upgrade') &&
          version === '13' &&
          key.length > 0;

        if (!isUpgrade) {
          connection.write('HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n');
          connection.end();
          return;
        }

        const accept = computeAcceptValue(key);
        const response =
          'HTTP/1.1 101 Switching Protocols\r\n' +
          'Upgrade: websocket\r\n' +
          'Connection: Upgrade\r\n' +
          `Sec-WebSocket-Accept: ${accept}\r\n` +
          '\r\n';

        connection.write(response);
        state.isWebSocket = true;
        this.clients.add(connection);
      }

      while (state.isWebSocket) {
        const decoded = tryDecodeFrame(state.buffer);
        if (!decoded) return;

        state.buffer = decoded.remaining;
        const { opcode, payload, fin } = decoded.frame;

        // Assignment scope: short, unfragmented frames
        if (!fin) {
          // For simplicity, close on fragmented messages.
          this.close(connection, 1003, 'Fragmented frames not supported');
          return;
        }

        if (opcode === 0x1) {
          // Text
          const text = payload.toString('utf8');
          if (typeof this.onMessage === 'function') {
            this.onMessage(text, connection);
          }
        } else if (opcode === 0x8) {
          // Close
          this.close(connection);
          return;
        } else if (opcode === 0x9) {
          // Ping -> Pong
          this.sendPong(connection, payload);
        } else if (opcode === 0xa) {
          // Pong (ignore)
        } else {
          // Unsupported (binary/continuation/etc)
          this.close(connection, 1003, 'Unsupported opcode');
          return;
        }
      }
    });

    connection.on('end', cleanup);
    connection.on('close', cleanup);
    connection.on('error', cleanup);
  }

  broadcast(text) {
    for (const client of this.clients) {
      this.sendText(client, text);
    }
  }

  sendText(connection, text) {
    const payload = Buffer.from(String(text), 'utf8');
    const frame = encodeFrame({ opcode: 0x1, payload });
    connection.write(frame);
  }

  sendPong(connection, payload) {
    const frame = encodeFrame({ opcode: 0x0a, payload: payload || Buffer.alloc(0) });
    connection.write(frame);
  }

  close(connection, code = 1000, reason = '') {
    try {
      const reasonBuf = Buffer.from(reason, 'utf8');
      const payload = Buffer.alloc(2 + reasonBuf.length);
      payload.writeUInt16BE(code, 0);
      reasonBuf.copy(payload, 2);
      connection.write(encodeFrame({ opcode: 0x8, payload }));
    } catch {
      // ignore
    } finally {
      connection.end();
      this.clients.delete(connection);
    }
  }
}

module.exports = {
  WebSocketServer,
};
