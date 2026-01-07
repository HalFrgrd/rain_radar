# Firmware C/C++

## Workflow
- I start the container with `docker compose up --detach`.
- Then I launch vscode to attach to the container.
- In the container I have setup vscode similar to: https://paulbupejr.com/raspberry-pi-pico-windows-development/
- I left the cmake kit as unspecified.
- Set the `Cmake: Build Directory` to e.g. `${workspaceFolder}/rain_radar_app/build_pico2_w`
- `> CMake: Configure` And selected the rain radar CMakeLists.txt.
- Then `> Cmake: Build` command works.
- This should create the uf2 file.
- Put the pico into bootloader and copy over the uf2 file from the build dir.

## Single command build:
Look at the github workflow file.

### misc
https://www.raspberrypi.com/documentation/pico-sdk/

Note that the pico wireless example isnt an example of using the rasp pi pico w to connect to the internt.
It's for this board which uses an esp32: https://shop.pimoroni.com/products/pico-wireless-pack
See this:
https://datasheets.raspberrypi.com/picow/connecting-to-the-internet-with-pico-w.pdf


