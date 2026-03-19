# FULL PUGGELISTE – 100% DEKNING (MCQ + korte svar)


## 🧠 KJERNE MODELL

- Applikasjon → Transport → Nettverk → Lenke → Fysisk
- Encapsulation: Data → TCP → IP → MAC
- Decapsulation: motsatt vei

---

## 🔴 LAG + ROLLE

| Lag | Rolle |
| --- | --- |
| Applikasjon | tjenester (HTTP, DNS, SMTP) |
| Transport | ende-til-ende (TCP/UDP) |
| Nettverk | routing (IP) |
| Lenke | lokal levering (MAC) |
| Fysisk | signal |

---

## 🔴 PROTOKOLLER (lag + hva)

### HTTP (HyperText Transfer Protocol)

- Lag: Applikasjon
- Web request/response

### HTTPS (HTTP Secure)

- Lag: Applikasjon
- HTTP + TLS

### WebSocket

- Lag: Applikasjon
- Full-duplex (begge veier samtidig)

### DNS (Domain Name System)

- Lag: Applikasjon
- Navn → IP
- Bruker TYPE (ikke port; for eksempel A, AAAA, MX, NS)
- Cache + rekursiv (sjekk cache; ellers → root → TLD → autoritativ)

#### Reverse DNS

- IP → navn
- Bruker PTR record
- Motsatt av vanlig DNS-oppslag

#### DNS-søk når det må gå "hele veien"

1. Klient spør lokal DNS-resolver om et navn
2. Resolver sjekker cache
3. Hvis ikke cachet: spør root-server
4. Root svarer med hvem som kjenner TLD-en, for eksempel `.com`
5. Resolver spør TLD-server
6. TLD svarer med autoritativ navneserver for domenet
7. Resolver spør autoritativ navneserver
8. Autoritativ server svarer med riktig record, for eksempel A/AAAA
9. Resolver cacher svaret
10. Resolver sender IP tilbake til klient

*Eksempel: Vg.no → 195.88.0.0*

1. Klient spør lokal DNS-resolver: "Hva er IP for vg.no?"
2. Resolver sjekker cache → ingen treff
3. Resolver spør root-server: "Hvem kjenner .no?"
4. Root svarer: "Spør .no TLD-server"
5. Resolver spør .no TLD-server: "Hvem kjenner vg.no?"
6. TLD-server svarer: "Spør autoritativ navneserver for vg.no"
7. Resolver spør autoritativ navneserver: "Hva er IP for vg.no?"
8. Autoritativ navneserver svarer: "195.88.0.0"
9. Resolver cacher svaret
10. Resolver sender IP tilbake til klient

*Eksempel reverse DNS: 195.88.0.0 → vg.no*

1. Klient spør lokal DNS-resolver: "Hva er navn for 195.88.0.0?"
2. Resolver sjekker cache → ingen treff
3. Resolver gjør IP-en om til reverse-navn: `0.0.88.195.in-addr.arpa`
4. Resolver spør root-server: "Hvem kjenner `in-addr.arpa`?"
5. Root svarer med hvem som kjenner `arpa`
6. Resolver spør riktig `in-addr.arpa`-server
7. Resolver får svar om autoritativ navneserver for `0.0.88.195.in-addr.arpa`
8. Resolver spør autoritativ navneserver etter PTR record
9. Autoritativ navneserver svarer: `vg.no`
10. Resolver cacher svaret
11. Resolver sender navnet tilbake til klient

### DHCP (Dynamic Host Configuration Protocol)

- Lag: Applikasjon
- Gir IP automatisk
- Porter: 67 (server), 68 (client)
- Bruker broadcast + MAC

#### DORA (Discover → Offer → Request → Ack)

*DORA blir brukt når en klient kobler seg til et nettverk og trenger en IP-adresse og annen nettverksinformasjon.*

**Discover → Offer → Request → Ack**

1. Discover: klient sender broadcast for å finne DHCP-server
2. Offer: server tilbyr IP og nettverksinfo
3. Request: klient ber om å få bruke tilbudet
4. Ack: server bekrefter og deler ut adressen

### SMTP (Simple Mail Transfer Protocol)

- Lag: Applikasjon
- Sender epost

### MIME (Multipurpose Internet Mail Extensions)

- Lag: Applikasjon
- Binær → ASCII (Base64 ~33% større)

### TCP (Transmission Control Protocol)

- Lag: Transport
- Pålitelig (ACK, seq, retransmit)
- Oppdager tap via:
  - timeout
  - duplicate ACK
- Har sender- og mottakerbuffer

### UDP (User Datagram Protocol)

- Lag: Transport
- Rask, ingen garanti
- HAR checksum

### IP (Internet Protocol)

- Lag: Nettverk
- Best effort routing

### ICMP (Internet Control Message Protocol)

- Lag: Nettverk
- Feil (ping, traceroute)

### ARP (Address Resolution Protocol)

- Lag: mellom L2/L3
- IP → MAC

### TLS (Transport Layer Security)

- Kryptering over TCP

### WPA2 (Wi-Fi Protected Access 2)

- Lag: Lenke
- Kryptert WiFi

### VPN (Virtual Private Network)

- Lag: Nettverk
- Kryptert tunnel

---

## 🔴 ADRESSER

| Type | Betydning |
| --- | --- |
| MAC | lenkelag (fysisk adresse) |
| IP | nettverk (logisk adresse) |
| Port | transport (app) |

**Socket = IP + port**

---

## 🔴 PORTNUMMER (pugg)

| Port | Tjeneste |
| --- | --- |
| 80 | HTTP |
| 443 | HTTPS |
| 53 | DNS |
| 25 | SMTP |
| 67/68 | DHCP |

**Port-kategorier:**

- 0–1023 → well-known
- ephemeral → klient

---

## 🔴 TCP DETALJER

### 3-way handshake

SYN → SYN-ACK → ACK

### ACK regel (kvittering)

ACK = siste byte + 1

*Eksempel:*

- Sender sender byte 1-1000
- Mottaker har fått alt til og med byte 1000
- Neste byte mottaker forventer er 1001
- Derfor sender mottaker: ACK = 1001

### Sliding window

- flere pakker samtidig

### Seq number

- byte-posisjon

### Connection close

- starter med FIN

---

## 🔴 DNS TYPES

| Type | Betydning |
| --- | --- |
| A | IPv4 |
| AAAA | IPv6 |
| MX | mail |
| NS | nameserver |

---

## 🔴 DHCP

**Discover → Offer → Request → Ack**

**Gir:**

- IP
- subnet
- gateway
- DNS

---

## 🔴 HTTP HEADERS

| Header | Betydning |
| --- | --- |
| Host | server |
| Cookie | state |
| Keep-alive | holder TCP |
| If-modified-since | cache |

**Keep-alive konkret:**

- Client: Connection: keep-alive
- Server: Keep-Alive: timeout, max

**HTTP = stateless**

**Cookies = gir state**

---

## 🔴 TRACEROUTE

- TTL = 1,2,3...
- Router svarer med ICMP "TTL exceeded"

**TTL (Time To Live) = maks hopp**

---

## 🔴 ROUTING

- Routing table → hvor sende
- Default gateway → ut av nett

**Protocol field (IP header) → velger TCP/UDP**

---

## 🔴 SUBNETTING

- Nettverksadresse = første
- Broadcast = siste
- Host = mellom

**CIDR (/24 osv) = nett-bits**

---

## 🔴 PRIVATE IP

- 192.168.x.x
- 10.x.x.x
- 172.16–31.x.x

**Ikke routable på internett**

---

## 🔴 NAT

- Oversetter privat IP ↔ offentlig IP
- Bruker port som nøkkel

---

## 🔴 CHECKSUM (feller)

- TCP feil → kast
- IPv4 feil → kast + ICMP
- Ethernet → kast
- UDP → HAR checksum

---

## 🔴 FELT → PROTOKOLL

| Felt | Protokoll |
| --- | --- |
| TTL | IPv4 |
| Next header | IPv6 |
| Source port | TCP/UDP |
| Seq number | TCP |
| Host | HTTP |
| MAC | Ethernet |

---

## 🔴 ETHERNET / WIFI

| Teknologi | Verdi |
| --- | --- |
| Ethernet | 802.3 |
| WiFi | 802.11 |

- CSMA/CD → kabel (hub)
- CSMA/CA → WiFi
- CRC → sjekksum
- MAC første halvdel = produsent

---

## 🔴 NETTVERKSENHETER

| Enhet | Lag |
| --- | --- |
| Switch | lag 2 (MAC) |
| Router | lag 3 (IP) |

---

## 🔴 KRYPTON

| Teknologi | Lag/rolle |
| --- | --- |
| TLS | ende-til-ende |
| VPN | nettverk |
| WPA2 | lenkelag |

**Sertifikat:**

- inneholder public key
- verifiseres med CA public key
- dekrypter signatur → sammenlign hash

- RSA → nøkkelutveksling
- AES → symmetrisk kryptering

---

## 🔴 DUPLEX

| Type | Betydning |
| --- | --- |
| Full duplex | begge sender |
| Half duplex | en om gangen |

---

## ⚠️ FELLES FELLER

### ❌ Feil

- DNS bruker port
- Ethernet reparerer feil
- IP er pålitelig
- UDP har ikke checksum

### ✔ Riktig

- DNS bruker TYPE
- TCP er pålitelig
- ARP = IP → MAC

---

## 🔥 SUPER-KORT (før eksamen)

- TCP = pålitelig
- UDP = rask
- DNS = navn → IP
- ARP = IP → MAC
- DHCP = DORA
- Socket = IP + port

**Encapsulation:**

- Data → TCP → IP → MAC

**Ports:**

- 80 HTTP
- 443 HTTPS
- 53 DNS

- Switch = L2
- Router = L3
- ACK = +1
- TTL = hopp
- Private IP ≠ internett
- NAT = oversetter IP

## Begrepsbeskrivelser

| Begrep | Fullt navn / type | Hva det gjør | Lag |
| --- | --- | --- | --- |
| HTTP | HyperText Transfer Protocol | Henter og sender nettsider med request/response | Applikasjon |
| HTTPS | HTTP Secure | HTTP med TLS for kryptert webtrafikk | Applikasjon |
| WebSocket | Protokoll | Gir toveis kommunikasjon i samme forbindelse | Applikasjon |
| DNS | Domain Name System | Oversetter domenenavn til IP-adresser | Applikasjon |
| DHCP | Dynamic Host Configuration Protocol | Deler ut IP, subnett, gateway og DNS automatisk | Applikasjon |
| SMTP | Simple Mail Transfer Protocol | Sender e-post mellom klienter og servere | Applikasjon |
| MIME | Multipurpose Internet Mail Extensions | Gjør at e-post kan inneholde binærdata og vedlegg | Applikasjon |
| TCP | Transmission Control Protocol | Pålitelig transport med ACK, retransmisjon og rekkefølge | Transport |
| UDP | User Datagram Protocol | Rask transport uten garanti for levering | Transport |
| IP | Internet Protocol | Sender pakker mellom nett via routing | Nettverk |
| ICMP | Internet Control Message Protocol | Sender feilmeldinger og brukes av ping/traceroute | Nettverk |
| ARP | Address Resolution Protocol | Finner MAC-adresse fra IP-adresse | Mellom lenke/nettverk |
| TLS | Transport Layer Security | Krypterer data over TCP-forbindelser | Over transport / brukes av applikasjon |
| WPA2 | Wi-Fi Protected Access 2 | Krypterer trådløst nett | Lenke |
| VPN | Virtual Private Network | Lager kryptert tunnel gjennom et nettverk | Nettverk |
| MAC | Media Access Control address | Fysisk adresse brukt lokalt på lenkelaget | Lenke |
| IP-adresse | Logisk adresse | Identifiserer en enhet i et nettverk | Nettverk |
| Port | Transportidentifikator | Skiller mellom tjenester og applikasjoner på samme IP | Transport |
| Socket | IP + port | Identifiserer en konkret ende i kommunikasjon | Transport / applikasjon |
| A | DNS record type | Gir IPv4-adresse for et navn | Applikasjon |
| AAAA | DNS record type | Gir IPv6-adresse for et navn | Applikasjon |
| MX | DNS record type | Angir hvilken mailserver som håndterer domenet | Applikasjon |
| NS | DNS record type | Angir navneserver for et domene | Applikasjon |
| Root-server | DNS-servertype | Vet hvor TLD-serverne er | Applikasjon |
| TLD-server | Top Level Domain server | Vet hvilke autoritative servere som kjenner et domene | Applikasjon |
| Autoritativ navneserver | DNS-servertype | Har det endelige svaret for domenet | Applikasjon |
| Cache | Mellomlagring | Lagrer tidligere svar for raskere oppslag | Flere |
| DORA | Discover, Offer, Request, Ack | De fire stegene i DHCP-prosessen | Applikasjon |
| SYN | TCP-flagg | Starter en TCP-forbindelse | Transport |
| ACK | Acknowledgment | Bekrefter mottatte byte i TCP | Transport |
| FIN | TCP-flagg | Avslutter en TCP-forbindelse | Transport |
| Seq number | Sequence number | Viser byte-posisjon i TCP-strømmen | Transport |
| Sliding window | TCP-mekanisme | Lar flere pakker være ute samtidig | Transport |
| TTL | Time To Live | Begrenser hvor mange hopp en pakke kan ta | Nettverk |
| Routing table | Tabell i router/host | Bestemmer hvor pakker skal sendes videre | Nettverk |
| Default gateway | Standard rute ut av nett | Sender trafikk ut av lokalnettet når ingen mer spesifikk rute finnes | Nettverk |
| Protocol field | Felt i IP-header | Forteller om nyttelasten er TCP, UDP eller annet | Nettverk |
| Next header | Felt i IPv6-header | Forteller hva som kommer etter IPv6-headeren | Nettverk |
| Source port | Felt i TCP/UDP-header | Viser hvilken avsenderport som brukes | Transport |
| Host | HTTP-header | Forteller hvilken server eller hvilket domene klienten vil nå | Applikasjon |
| Cookie | HTTP-header/data | Lagrer tilstand mellom ellers stateless HTTP-requests | Applikasjon |
| Keep-alive | HTTP/TCP-mekanisme | Holder forbindelsen åpen for flere requests | Applikasjon / transport |
| If-Modified-Since | HTTP-header | Brukes til caching og betinget henting | Applikasjon |
| Traceroute | Verktøy/metode | Finner veien pakker tar ved å øke TTL trinnvis | Nettverk |
| CIDR | Classless Inter-Domain Routing | Beskriver hvor mange bits som er nettverksdel, for eksempel /24 | Nettverk |
| Nettverksadresse | Adresse-type | Første adresse i subnettet | Nettverk |
| Broadcast | Adresse-type | Siste adresse i subnettet, brukes til alle i lokalnettet i IPv4 | Nettverk |
| NAT | Network Address Translation | Oversetter privat IP til offentlig IP og tilbake | Nettverk |
| Ethernet | 802.3 | Kablet teknologi for lokal levering av rammer | Lenke |
| WiFi | 802.11 | Trådløs teknologi for lokal levering av rammer | Lenke |
| CSMA/CD | Carrier Sense Multiple Access with Collision Detection | Kollisjonshåndtering i delt kablet medium | Lenke |
| CSMA/CA | Carrier Sense Multiple Access with Collision Avoidance | Unngår kollisjoner i WiFi | Lenke |
| CRC | Cyclic Redundancy Check | Oppdager bitfeil i en Ethernet-ramme | Lenke |
| Switch | Nettverksenhet | Sender rammer videre basert på MAC-adresser | Lenke |
| Router | Nettverksenhet | Sender pakker videre basert på IP-adresser | Nettverk |
| RSA | Krypteringsalgoritme | Brukes til nøkkelutveksling/asymmetrisk kryptografi | Flere |
| AES | Advanced Encryption Standard | Symmetrisk kryptering av data | Flere |
| Sertifikat | Digital identitet | Binder public key til en identitet og kan verifiseres av CA | Flere |
| CA | Certificate Authority | Verifiserer og signerer sertifikater | Flere |
| Full duplex | Overføringsmodus | Begge parter kan sende samtidig | Lenke / fysisk |
| Half duplex | Overføringsmodus | Bare én part sender om gangen | Lenke / fysisk |
| Encapsulation | Prosess | Data pakkes inn med headere nedover lagene | Flere |
| Decapsulation | Prosess | Mottaker pakker ut headerne oppover lagene | Flere |
