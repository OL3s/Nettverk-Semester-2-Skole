Ta utgangspunkt i rfc6455 og programmer et WebSocket-server bibliotek som gjennom socket-programmering
(du skal ikke bruke et eksternt WebSocket bibliotek på serversiden):

• Utfører handshake med klienter  
• Leser korte meldinger fra klienter  
• Kan sende korte meldinger til tilknyttede klienter  

Med WebSocket-server biblioteket skal du lage et WebSocket endepunkt som kan motta melding fra en klient,
og deretter sender mottatt melding til alle tilknyttede klienter.

Du velger selv programmeringsspråk, men et alternativ er å starte med følgende serverside JavaScript-kode:
```
const net = require('net');

// Simple HTTP server responds with a simple WebSocket client test
const httpServer = net.createServer((connection) => {
  connection.on('data', () => {
    let content = `<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8" />
</head>
<body>
WebSocket test page
<script>
let ws = new WebSocket('ws://localhost:3001');
ws.onmessage = event => alert('Message from server: ' + event.data);
ws.onopen = () => ws.send('hello');
</script>
</body>
</html>`;

    connection.write('HTTP/1.1 200 OK\r\nContent-Length: ' + content.length + '\r\n\r\n' + content);
  });
});

httpServer.listen(3000, () => {
  console.log('HTTP server listening on port 3000');
});

// Incomplete WebSocket server
const wsServer = net.createServer((connection) => {
  console.log('Client connected');

  connection.on('data', (data) => {
    console.log('Data received from client:', data.toString());
  });

  connection.on('end', () => {
    console.log('Client disconnected');
  });
});

wsServer.on('error', (error) => {
  console.error('Error:', error);
});

wsServer.listen(3001, () => {
  console.log('WebSocket server listening on port 3001');
});
``` 
Du kjører koden med node og når du åpner http://localhost:3000 vil du se utskriften av WebSocket
handshaken fra nettleseren. På port 3000 kjører en enkel HTTP server som responderer med en enkel
WebSocket test side, og WebSocket serveren som dere skal jobbe videre på blir startet på port 3001.

Frivillig: Bruk WebSocket-server biblioteket du har laget til å lage et tegnebrett
(som vist i videoforelesningen) som flere klienter kan tegne på samtidig.

