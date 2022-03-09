# Igate

## Source Code [igate](https://github.com/lora-aprs/LoRa_APRS_iGate) 

Complementary files

- AXP192 (power management)
- LilyGo_T3_V1.6 schematics

***
## TTGO BOARD

![TTGO](ttgo.jpg  "TTGO BOARD")

***

## TTGO TEST with esp32 and RA02 (SX1278)

Igate software recognize TTGO lora32 V1

change i2c lines in BoardFinder.cpp file to use ssd1306 display

```c++
BoardConfig TTGO_LORA32_V1        ("TTGO_LORA32_V1",         eTTGO_LORA32_V1,          21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26);
```


![TTGO](cablage_igate.png  "TTGO TEST")

