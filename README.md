# InkyFrame image viewer
This projects has two parts: the InkyFrame 7.3 inch client and the image server.
The client wakes up every so often and downloads an image from the image server.
The image server hosts a few different types of images: a rain radar for the south east of the uk; moon phases for the norhtern hemisphere; and staic images of my choice.

Features:
- Rain radar
- Moon phase display
- Server dictated client refresh times


### notes:

- https://shop.pimoroni.com/products/inky-frame-7-3?variant=40541882089555
- https://github.com/pimoroni/inky-frame

To get bootloader mode, hold BOOTSEL on the rpi and tap reset on the frame.
I followed this to get started https://learn.pimoroni.com/article/pico-development-using-wsl.
There is a docker file that can be used for local development and by github actions.


TODO: move this to firmware_c readme:
The `Serial Monitor` extension from microsoft works even in the container and I can see the printfs from the board.
Getting the serial monitor to work reliable is a little tricky:
Often after reprogramming, the serila monitor will hang `Waiting for reconnection`.
Instead of changing the settings, a press of the reset button should work reestablish connection.

