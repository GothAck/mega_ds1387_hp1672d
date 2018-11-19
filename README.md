# Program a Dallas Semiconductor DS1387 RAMified RTC/NVRAM for HP 1670 series logic analysers

Adapted from <https://www.eevblog.com/forum/testgear/hp-167xd-corrupted-lan-board-error-fix/>

## Wut
I bought a shiny new old HP 1672D deep memory logic analyser.
All tests passed besides the "NIC board".
After trying a firmware upgrade and scouring the internet with the help of a friend I discovered the MAC address
was stored in NVRAM that was part of the DS1387 RTC module, whose battery had long given up the ghost.

## What do you do?
Well you need to desolder the sucker, modify it with an external battery
(steps searchable online, will provide some more info om my experiences later), and bit bang the mac address back
into the RAM using an Arduino.

I recommend socketing the new / modified package. You may need to destroy the original package to remove it, I used a new-old-stock replacement, still modified because fuck potted lithium cells.

## How?
The core code was taken from <https://www.eevblog.com/forum/testgear/hp-167xd-corrupted-lan-board-error-fix/>
and given a simple serial interface.

Send "r" to the serial port to verify the contents of the RAM. Send "w" to write the payload.

The first six bytes of the payload are the MAC address. You can find this on a sticker on the CPU board, see the EEVBlog forum for further details.
