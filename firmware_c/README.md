# Firmware C/C++

## Workflow
- I start the container with `docker compose up --detach`.
- Then I launch vscode to attach to the container.
- In the container I have setup vscode similar to: https://paulbupejr.com/raspberry-pi-pico-windows-development/
- I left the cmake kit as unspecified.
- Make the `rain_radar_app/build` folder.
- `> CMake: Configure` And selected the rain radar CMakeLists.txt.
- Then `> Cmake: Build` command works.
- This should create the uf2 file.
- Put the pico into bootloader and copy over `docker cp 2e933ce5c2cc:/home/appuser/pico/rain_radar_app/build/rain_radar.uf2 /media/my_user/RPI-RP2`

## Single command build:
Look at the github workflow file.

### misc
https://www.raspberrypi.com/documentation/pico-sdk/

Note that the pico wireless example isnt an example of using the rasp pi pico w to connect to the internt.
It's for this board which uses an esp32: https://shop.pimoroni.com/products/pico-wireless-pack
See this:
https://datasheets.raspberrypi.com/picow/connecting-to-the-internet-with-pico-w.pdf


