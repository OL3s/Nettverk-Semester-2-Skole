# DKO5 Innlevering

## ARP

**Hva er ARP**?

ARP står for ***Address Resolution Protocol***. Det dette gjør er å oversette IP-adresse -> MAC-adresse i lokalnett.

* IP - lag 3
* MAC - lag 2

Med andre ord, *hvilken MAC har denne IP-en på LAN*

**Hvilken enhet er mottakere av ARP-pakker. Hvordan kan du verifisere dette?**

...

**Finn en forventet IPv4-adresse med ipconfig og noter**:

...

## Rutingtabell på PC

**Hvilken adresse er listet som din Gateway?**

``` bash
ip route
default via 10.22.100.1 dev wlp0s20f3 proto dhcp src 10.22.100.243 metric 600 
10.22.100.0/22 dev wlp0s20f3 proto kernel scope link src 10.22.100.243 metric 600 
```

`default gateway` blir det samme som *default via* som er `10.22.100.1`

Om man skal få en 
