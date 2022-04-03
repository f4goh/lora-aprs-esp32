# Tracker

Tracker V2 : modified version from Peter OE5BPA

## Source Code [tracker](https://github.com/lora-aprs/LoRa_APRS_Tracker) 

[Reset GPS](https://github.com/lora-aprs/TTGO-T-Beam_GPS-reset) 1st on TTGO before programming 

Complementary files

- AXP192 (power management)
- LilyGo_TBeam_V1.1 schematics

***
## TTGO BOARD

![TTGO](TTGO_upper_lower.png  "TTGO BOARD")

***

## TTGO PINOUT

![TTGO](TTGO_pinout.png  "TTGO PINOUT")

## TTGO mechanical dimension

![TTGO](TTGO_board.png  "mechanical dimension")

```console
m key to enter config menu
help command for info
>help
Configuation exemple
Set new callsign : call f4goh-6
Transmit every minutes at (0-59) second : second 10
Set frequency : freq 433775000
Set car symbol : symbol >
Set new comment : comment hello
Set Battery measurement in comment (0 or 1) : setbat 1
Set altitude feild in pdu (0 or 1) : setalt 1
Set compression position (0 or 1) : setcomp 1
Get pdu (to see APRS data) : getpdu
Show configuration : shconfig
Reset default configuration : raz
Exit menu : exit
>shconfig
Call is : f4goh-5
Symbol is : O
Frequency is : 433775000
Transmit at second : 20
Battery measurement is : Enable
Altitude is : Disable
Compression is : Disable
Comment is : hello
>symbol >
symbol is >
>getpdu
```

