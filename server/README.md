# notes
- Install `uv`
- run with `uv run python main.py --deploy`

## Building the rain radar image

### Rain data:
https://doc.rainbow.ai/api-ref/tiles/

See API usage:
https://developer.rainbow.ai/reports

### Background map image
I started out `prettymaps` but found it too slow.
`maptiler.com` is much beter and easier to use for this use case.
maptiler workflow: create new map from some preset, I think I made a tile preset that pops up in your maps.
The web based map creation / style modification works pretty well.
I bought a one month subscription so I could use the API to download map tiles.


## Hosting image server
I'm using tailscale funnel.
I point it to the `public_available/` folder and it simply hosts that folder on a publicly available web server.
Each wifi network index gets its own subfolder allowing different images per wifi network.
There is a cronjob to run `uv` every 10 minutes.

## other images
https://www.singletonmills.com/sydney-first-sheep.html
