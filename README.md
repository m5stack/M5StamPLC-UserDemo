# StamPLC User Demo

User demo source code of [StamPLC](https://docs.m5stack.com/en/products/sku/k141).

## Build

### Fetch Dependencies

```bash
python ./fetch_repos.py
```

## Desktop Build

#### Tool Chains

```bash
sudo apt install build-essential cmake
```

#### Build

```bash
mkdir build && cd build
```

```bash
cmake .. && make -j8
```

#### Run

```bash
cd desktop && ./app_desktop_build
```

## IDF Build

#### Tool Chains

[ESP-IDF v5.1.4](https://docs.espressif.com/projects/esp-idf/en/v5.1.4/esp32s3/index.html)

#### Build

```bash
cd platforms/esp32s3
```

```bash
idf.py build
```

#### Flash

```bash
idf.py -p <YourPort> flash -b 1500000
```

##### Flash AssetPool Bin File

```bash
parttool.py --port <YourPort> write_partition --partition-name=assetpool --input "./AssetPool.bin"
```

## Acknowledgments

This project references the following open-source libraries and resources:

- [https://github.com/m5stack/M5GFX](https://github.com/m5stack/M5GFX)
- [https://github.com/m5stack/M5Unified](https://github.com/m5stack/M5Unified)
- [https://github.com/bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [https://github.com/hoeken/PsychicHttp](https://github.com/hoeken/PsychicHttp)
- [https://github.com/nayuki/QR-Code-generator](https://github.com/nayuki/QR-Code-generator)
- [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)
- [https://github.com/adafruit/Adafruit_AW9523](https://github.com/adafruit/Adafruit_AW9523)
- [https://github.com/jarzebski/Arduino-INA226](https://github.com/jarzebski/Arduino-INA226)
- [https://github.com/ARMmbed/LM75B](https://github.com/ARMmbed/LM75B)
- [https://github.com/alexreinert/piVCCU/blob/master/kernel/rtc-rx8130.c](https://github.com/alexreinert/piVCCU/blob/master/kernel/rtc-rx8130.c)
- [https://github.com/256dpi/arduino-mqtt](https://github.com/256dpi/arduino-mqtt)
