# ULPLoRa Project

Ultra Low Power LoRa Board repository contains software, hardware sources and documentation
## Getting Started

* Make sure that you have a ULP LoRa Board.
* Install arduino-lmic from [here](https://gitlab.com/icfoss/OpenIoT/ulplora/tree/master/Software/Arduino/Libraries/) . Copy the library to ~/Arduino/libraries/
* Select board : Arduino Pro Mini 3.3v 8Mhz
* Try Example : [ULPLoRa_ABP](https://gitlab.com/icfoss/OpenIoT/ulplora/tree/master/Software/Arduino/Examples)
   
### Prerequisites

Arduino IDE - 1.8.5 [Tested]

## Contributing

Instructions coming up soon.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* LMIC by [matthijskooijman](https://github.com/matthijskooijman/arduino-lmic)

## Changelog

* V2.1 : Removed Onboard Li-ion charging circuit. Added reverse polarity protection for RP-604. Updated battery voltage sensing circuit. Removed analog interfaces which were found to be of limited use.
