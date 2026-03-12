# [Datakom for ingeniører - Eksamensøving](https://sites.google.com/view/tdat2004-datakom/)

# **Lagmodellen**

- Basert på forenklede 5-lags OSI-modellen

```
Netverksprogram

<->

Applikasjonslaget
Transportlaget
Nettverkslaget
Lenkelaget
Fysisk lag
```

![Modell](image.png)

---

## Om Lagmodellen
- Utviklet for å dele opp en kompleks oppgave i separate deloppgaver, organisert i "lag".
- Modellen er utviklet av `ISO` og kalles Open Systems Interconnection (`OSI`) modellen.
- Andre standardiseringsorganisasjoner er `IETF`, `IEEE` og `W3C`.

*Det er flere utgaver av modellen, bla. 7-lags OSI-modellen og en TCO/IP-modell. Men vi benytter den forenklede modellen*

### Beskrivelse

1. #### Hovedprinsipper.
Systemutviklere har ansvar for å utvikle  i henhold til grensesnitt mot protokoller på applikasjonslaget. Prinsipper er at apps og protokoller kommuniserer med hverandre på likestilt nivå, men benytter tjenerter fra laget under for å overføre data mellom partene. Ved en slik approach kan man bytte ut en protokoll (eller blokk/modul) uten å påvirke andre deler av systemet. Hver blokk/modul er en tjeneste.

![Osi-5](image-1.png)

2. #### Oppagene for lagene.
- Applikasjonslaget: Grensesnitt mot distrubuerte applikasjoner. *Eksempler er HTTP, SMTP (e-post), FTP (filoverføring)*
- Transportlaget: Ende-ende overføring av meldinger. *Eksempler er TCP (Transport Control Protocol) og UDP (User Datagram Protocol)*
- Nettverkslaget: Sørger for at hver pakke rutes gjennom nettet. *Eksempler er IP (Internet Protocol) og ICMP (Internet Control Message Protocol)*
- Lenkelaget: Sørger for at pakkene overføres mellom to tilstøtende noder (mellom to nettverkskort). *Eksempler er Ethernet og Wi-Fi*
- Fysisk lag: Sender signaler over et transmisjonsmedium (luft, kopper, fiber). *Eksempler er elektriske signaler, optiske signaler og radiobølger.*

3. #### Pakkeenheter protocol data unit (`PDU`) og adressering.
PDU er en enkel **enhet** med informasjon som sendes mellom likestilte enheter i et datanettverk. Den består av protokoll-spesifikk informasjon (*pakkehodet*) og brukserdata (*nyttelast fra laget over*).

![tabell lag](image-2.png)

---

## Innpakkingsprinsippet
Innpakkingsprinsippet går ut på at når en pakke skal sendes så legges det til en ny pakkeheader på toppen av pakken for hvert lag pakken går igjennoom, som en pyramide. Pakken blir støyye for hvert lag, og når den kommer frem til mottaker så blir den avpakket igjen i motsatt rekkefølge.

### Beskrivelse
Netverk er delt inn i forskjellige lag, disse er helt separat fra hverandre, og er ikke interessert i hva laget under inneholder av informasjon, kun i hvordan data skal bli sendt videre til rett mottaker.

![OSI-modellen eksempel](image-3.png)

## Lagdelt Kryptering
Vil si at innholdet (nyttelast; dataen til laget over) på hvert enkel lag i OSI kan krypteres uavhengi av hva som skjer i andre lag. Da blkir feil av en enkeltstående beskyttelsesmekanisme ikke føre til  at det helgetlige systemet bliur stående uten forsvarsverker (uten beskyttelse).

De tre sikkerhetsmekanismekle som bruker i lagdelt struktur er `WPA2`, `TLS` og `VPN`, på forskjellige lag i OSI-modellen.
- `TLS` - Applikasjonslaget: HTTP, SMTP, FTP
- `VPN` - Nettverkslaget: IP
- `WPA2` - Lenkelaget: Ethernet og Wi-Fi

![lagdelt kryptering](image-4.png)

### Beskrivelse
Ved å bruke forskjellige krypteringsmetoder vil sensitiv informasjon sikres godt, noe som er viktig i dagens samfunn. Alt blir kryptert uavheingige med hverandre i henhold til lag, dersom et lag feilereller noen klarer å bryte det, er det fortsatt flere lag med kryptering igenn.

#### 1. TLS (Transport Layer Security):
HTTP***S*** er kryptert versjon av HTTP som benytter TLS for sikker transport. Og ligger i lagene **Applikasjonslaget og Transportlaget**. TLS sørger for at data som sendes mellom klient og server er kryptert og beskyttet mot avlytting og manipulering. Er etterfølgeren til SSL (Secure Sockets Layer) og er mye brukt for å sikre nettkommunikasjon. TLS brukes også i andre protokoller som SMTP (for e-post) og FTP (for filoverføring) for å sikre dataoverføringen.

#### 2. VPN (Virtual Private Network):
Er som regel i flere lag i OSI-modellen, men store deler av arbeidet skjer i **nettverkslaget (IP, ICMP)**. Her er ofte nyttelasten (dataen laget over) fra transportlaget allerede kryptert (TLS).

VPN må autorisere seg , der kommunikasjon skjer via en kryptert nøkkel, der ingen uautoriserte brukere får tilgang til det som skjer tråløst på nettverket. Dataen er unyttig da den også er kryptert.

![vpn](image-5.png)

#### 3. WPA2 (Wi-Fi Protected Access 2):
Benyttes av **lenkelaget (Ethernet & Wifi)**, det brukes etter både TLS og VPN når en datamaskin sender ut data. Brukes for å la enheter komble seg til et tråløst nett, uten at inntrengere kan lure seg inn, og er **påbudt for alle nye wifi-enheter** Søttter bruk av temporary key integrity protocol (`TKIP`) og Counter Mode with Cipher Block Chaining Message Authentication Code Protocol (`CCMP`) for å sikre tråløs kommunikasjon.

CCMP er sikrere enn TKIP og skal teoretisk være uknekkelig, men har vist seg å ha svakheter som kunne benyttes.

Etter å ha vært gjennom lenkelaget, går nyttelasten videre til det fysiske laget for å overføres til en annen datamaskin, da har det kanskje blitt brukt kryptering med TLS, VPN og WPA2 for en sikker overføring.

## Hva er IP-nett?
samling av datamaskiner med en felles nettadresse. Datamaskiner på samme IP-nett kan sande pakker uten å gå via en ruter. Hvis den skal sendes til en annen IP-nett, sendes den gjennom ruteren. Dette blir adressert ved hjelp av en IP-adresse, som er en unik identifikator.

IP-adressen består av:
- Nett-id
- Host-id

Disse kan ikke leses direkte fra EP-en uten at man bruker en logisk **OG operasjon** på nettmasken og IP-adressen for å finne den. En adresse kan også deles inn i flere mindre nett (subnett), som gir firdeler som bla. reduserer broadkast trafikk fra mange enheter som forstyrrer hverandre og øker sikkerhet.

### Beskrivelse 
1. #### IP-Nett:
- **Nettverkslaget (IP):** Sørger for at hver pakke som skal sendes via rutes gjennom nettet.
- **Ruter:** Videresender pakker mellom ulike IP-nett gjennom ulike datanettverkene den er koblet opp med.

IP-nett er noe av det mest sentrale innen datakommunikasjon, alt av datatrafikk og sending av datapakker skjer over IP-nett og rutere. Ved at det er en samling av maskiner med lik nettadresse har de dermet lik nettmasker. Dette betyr også at de har samme kringkasningsdomene siden de befinner seg i samme nett-lag

 ![ip-nett](image-6.png)


2. #### Hvordan fungerer IP-nettet:
- Når en datamaskin skal sende en pakke til en annen datamaskin, sjekker den først om mottakerens IP-adresse er i samme IP-nett ved å bruke nettmasken.
- Hvis mottakerens IP-adresse er i samme IP-nett, sender datamaskinen pakken direkte til mottakeren uten å gå gjennom en ruter.
- Hvis mottakerens IP-adresse er i et annet IP-nett, sender datamaskinen pakken til en ruter, som deretter videresender pakken til riktig IP-nettverk.

![ip-nett med ruter](image-7.png)

3. #### IP-adresse
- En IP-adresse er en **logisk adresse** (nettverkslaget) som identifiserer en enhet/et nettverkskort i et IP-nett, slik at pakker kan rutes til riktig mottaker.
- En enhet kan ha **flere IP-adresser samtidig** (f.eks. IPv4 + IPv6, og én per nettverksinterface som Wi‑Fi/Ethernet).
- IPv4 og IPv6 er to versjoner av IP: **IPv4 er 32-bit**, mens **IPv6 er 128-bit** (mye større adressrom).

| Type IP-adresse | Eksempel | Hva identifiserer den? | Hvor gjelder den? |
|---|---|---|---|
| Privat IPv4 (lokal) | 10.22.102.101 | Enheten din i et lokalt IP-nett (LAN) | Kun i private nett (ikke rutet på internett) |
| Offentlig IPv4 | 129.241.236.17 | Tilkoblingen ut mot internett (ofte ruteren/brannmuren – kan deles via NAT/CGNAT) | Globalt på internett |
| Loopback IPv4 | 127.0.0.1 | “Denne maskinen” (seg selv) | Kun lokalt på maskinen |
| Global IPv6 | 2001:700:300:4019:31eb:d741:f652:90bc | Enheten din med globalt routbar adresse | Globalt på internett |
| Link-local IPv6 | fe80::7bae:7a6d:1f49:f5dc | Enheten din på samme lokale lenke | Kun på samme lenke (samme Wi‑Fi/Ethernet), ikke rutet videre |
| Loopback IPv6 | ::1 | “Denne maskinen” (seg selv) | Kun lokalt på maskinen |

---

| Felt | Eksempel | Betyr |
|---|---|---|
| IPv4 adresse (host) | 10.22.102.101 | Adressen til enheten din i subnettet |
| Prefix (nettmaske) | /22 | Hvor mange bit som er nett-id (resten er host-id) |
| Nettadresse (network ID) | 10.22.100.0/22 | Adressen som representerer hele subnettet (nett-id) |
| Broadcast (IPv4) | 10.22.103.255 | “Send til alle” i subnettet (ofte filtrert/blokkert i moderne nett) |
| Gateway (ruter i subnettet) | 10.22.100.1 | Neste hopp ut av subnettet når du skal til andre IP-nett |

---

4. #### Nettmaske
...

5. #### Subnetting
...


| Del av IP-pakken | Hva det er | Eksempel / forklaring |
|---|---|---|
| Versjon | Hvilken IP-versjon som brukes | IPv4 eller IPv6 |
| Header-lengde | Hvor stor IP-headeren er | Forteller hvor nyttelasten begynner |
| Type of Service / DSCP | Prioritering av trafikk | Kan brukes for QoS |
| Total lengde | Hele størrelsen på IP-pakken | Header + nyttelast |
| Identifikasjon | Brukes ved fragmentering | Hjelper med å sette sammen pakker igjen |
| Flagg | Kontrollfelt for fragmentering | F.eks. om pakken kan deles opp |
| Fragment offset | Viser plassering av fragment | Brukes hvis pakken er delt opp |
| TTL (Time To Live) | Hvor mange rutere pakken kan passere | Reduseres med 1 for hvert hopp |
| Protokoll | Hvilken protokoll som ligger inni IP-pakken | TCP, UDP, ICMP |
| Header checksum | Feilsjekk av headeren | Sjekker at headeren ikke er ødelagt |
| Kilde-IP | Avsenderens IP-adresse | F.eks. 10.22.102.101 |
| Destinasjons-IP | Mottakerens IP-adresse | F.eks. 8.8.8.8 |
| Options | Ekstra valgfrie felt | Brukes sjelden |
| Nyttelast (payload) | Selve dataen som sendes videre | F.eks. en TCP-pakke med HTTP-data |

## Pakkesvitjing
Pakkesvitsjing er en metode for å sende data i små biter (pakker) over et nettverk. Hver pakke inneholder både data og informasjon om hvor den skal, og kan rutes uavhengig av andre pakker.

For å sikre at alt er blitt sendt, bruker man **Sjekksum** for å sjekke at dataen ikke har blitt ødelagt underveis. Hvis en pakke blir ødelagt, kan den bli sendt på nytt.

# Applikasjonslaget

## Web

### HTTP
Protokoll for utveksling av meldinger mellom klient og tjener. Programmet (nettleser) sender forespørsel om tilgang til webressurs.

En HTTP-forespørsel består av:
- Forespørselslinje: Metode (GET, POST, etc.) + URL + HTTP-versjon
- Header: Nøkkel-verdi par med metadata (f.eks. User-Agent, Accept)
- Tom linje
- Valgfri kropp (for POST/PUT)

#### 1. Hva er HTTP?
HTTP (Hypertext Transfer Protocol) er en protokoll som spesifiserer hvordan meldinger skal bli utvekslet mellom klient og tjener i en webtjeneste. Kan også deles opp i forespørsler og svar som har hver sitt meldingsformat.

#### 2. Flyten
For det meste sender flient forespøssel til tjener, og tjener svarer tilbake. Kommunikasjon består av 3 hoveddeler:
- TCP-kobling etableres.
- HTTP-forespørsel sendes (GET, POST, etc.)
- Svaret blir sendt fra tjener.

#### 3. Forespørsel
En forespørsel esstår av tre hoveddeler:
- En forespørselslinje
- Diverse headerlinjer
- En kropp med data (valgfri)

![forespørsel eksempel](image-8.png)

#### 4. Svar
Svar består av 3 hoveddeler:
- En statuslinje
- Diverse headerlinjer
- En kropp med data (valgfri)

![svar eksempel](image-9.png)

### HTTP-versjoner

#### 1. HTTP/0.9 - Enlinjeprotokoll
- En svært enkel versjon av HTTP som kun støttet GET-metoden og returnerte ren HTML uten header-informasjon.

![eksempel](image-10.png)

#### 2. HTTP/1.0 - Fleksiebelog utvidbar
- Støttet flere metoder (GET, POST, HEAD) og inkluderte header-informasjon i både forespørsler og svar. Hver forespørsel krevde en ny TCP-kobling, noe som førte til ineffektivitet.

#### 3. HTTP/1.1 - Den standariserte protokollen
- Introducerte vedvarende tilkoblinger, slik at flere forespørsler kunne sendes over samme TCP-kobling, noe som forbedret effektiviteten.

![eksempel](image-11.png)

#### 4. HTTP/2
For å kunne benytte HTTP/2 må SSL (etterfølgeren til TLS) bli obligatorisk, det betyr at det må bli en del endringer iettverksarkutekturen hvis man skal flytte over til dertte for å utnytte ytelsesfordelene.



