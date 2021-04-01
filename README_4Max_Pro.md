# Anycubic 4Max Pro

## Beta build - use with caution!

This is the newest version of the [Marlin Firmware](https://github.com/MarlinFirmware/Marlin), customized and optimized for the Anycubic 4Max Pro, based on 

	[Marlin-Ai3M repository](https://github.com/davidramiro/Marlin-Ai3M/).
	[Anycubic 4MAX_PRO repository] (https://github.com/ANYCUBIC-3D/4MAX_PRO) .

While I will try to keep this version updated with the latest changes from the Marlin firmware.

## Why use this?

While the Anycubic 4Max Pro is a great printer for its price and produces fantastic results in stock, there are some improvements and additional features that this firmware provides:

- Many people have issues getting the Ultrabase leveled perfectly, using Manual Mesh Bed Leveling the printer generates a mesh of the flatness of the bed and compensates for it on the Z-axis for perfect prints without having to level with the screws.
- The stock FW only gives out 9V instead of 12V on the parts cooling fan so some fans like Noctua don't run like they should. This is fixed in this firmware.
- Even better print quality by adding Linear Advance, S-Curve Acceleration and some tweaks on jerk and acceleration.
- No need to slice and upload custom bed leveling tests, test it with a single GCode command
- Easily start an auto PID tune or mesh bed leveling via the special menu (insert SD card, select special menu and press the round arrow)
- Filament change feature enabled: Switch colors/material mid print (instructions below) and control it via display.
- The filament runout, pause and stop functionality have been overhauled and improved: The hotend now parks and retracts (on pause or stop) and purges automatically (on resume).

## Known issues:

- Power outage support is not included
- Estimated print times from your slicer might be slightly off.
- Special characters on any file or folders name on the SD card will cause the file menu to freeze. Simply replace or remove every special character (Chinese, Arabic, Russian, accents, German & Scandinavian umlauts, ...) from the name. Symbols like dashes or underscores are no problem.
**Important note: On the SD card that comes with the printer there is a folder with Chinese characters in it by default. Please rename or remove it.**

## How to flash this?

### compile it yourself:

- Download and install [Arduino IDE](https://www.arduino.cc/en/main/software)
- Clone or download this repo
- Browse into the Marlin folder and run `Marlin.ino`
- In the IDE, under `Tools -> Board` select `Genuino Mega 2560` and `ATmega2560`
- Open Marlin.ino in the Marlin directory of this repo
- [Customize if needed](https://github.com/davidramiro/Marlin-AI3M/wiki/Customization-&-Compiling) (e.g. motor directions and type at line `559` to `566` and line `857` to `865` in `Configuration.h`)
- Under `Sketch`, select `Export compiled binary`
- Look for the .hex file in the Marlin directory (only use the `Marlin.ino.hex`, not the `Marlin.ino.with_bootloader.hex`!)

### After obtaining the hex file:

- Flash the hex with Cura, OctoPrint or similar
- Use a tool with a terminal (OctoPrint, Pronterface, Repetier Host, ...) to send commands to your printer.
- **Important** Connect to the printer and send the following commands:
- `M502` - load hard coded default values
- `M500` - save them to EEPROM
- I highly recommend calibrating the extruder.

#### Calibration and other instructions [Wiki](https://github.com/davidramiro/Marlin-AI3M/wiki/Calibration).

## Manual Mesh Bed Leveling

If you have issues with an uneven bed, this is a great feature.

- Insert an SD card, enter the print menu.
- Enter the special menu by selecting it and pressing the round arrow:

![Special Menu][menu]

- In this menu, the round arrow is used to execute the command you selected.
- Preheat the bed to 60°C with this entry: (if you usually print with a hotter bed, use the Anycubic menu)

![Preheat bed][preheat]

- Level your preheated bed as well as you can with the four screws.
- Start the mesh leveling:

![Start MMBL][start]

- Your nozzle will now move to the first calibration position.
- Don't adjust the bed itself with screws, only use software from here on!
- Use a paper - I recommend using thermopaper like a receipt or baking paper
- Use the onscreen controls to lower or raise your nozzle until you feel a light resistance: (**If you want to send the same command multiple times, select the item again, even though it is still marked red.**)

![Z axis controls][control]

- Once finished , move to the next point:

![Next mesh point][next]

- Repeat the last two steps until all 25 points are done.
- Your printer will beep, wait 20 seconds and then save (the printer will beep once more to confirm):


![Save to EEPROM][save]

- Reboot your printer.


[menu]: https://kore.cc/i3mega/mmbl/menu.jpg "Special Menu"
[preheat]: https://kore.cc/i3mega/mmbl/preheat.jpg "Preheat 60C"
[start]: https://kore.cc/i3mega/mmbl/start.jpg "Start Mesh leveling"
[next]: https://kore.cc/i3mega/mmbl/next.jpg "Next Mesh point"
[control]: https://kore.cc/i3mega/mmbl/control.jpg "Z axis control"
[save]: https://kore.cc/i3mega/mmbl/save.jpg "Save to EEPROM"

### After leveling:

- To ensure your mesh gets used on every print from now on, go into your slicer settings and look for the start GCode
- Look for the Z-homing (either just `G28` or `G28 Z0`) command and insert these two right underneath it:
```
M501
M420 S1
```
- Your printer should now correctly print first layers even on a warped bed.
- When working on the printer, installing a new hotend or nozzle or the bed warping over time, a new Mesh Leveling procedure is recommended.

#### Manual commands for use with OctoPrint etc.:

- `G29 S1` - Start MMBL
- `G29 S2` - Next Mesh point
- Raising Z: `G91`, `G1 Z0.02`, `G90` (one after another, not in one line)
- Lowering Z: `G91`, `G1 Z-0.02`, `G90`
- After seeing `ok` in the console, send `M500` to save.


### Testing your bed leveling

- No need to download or create a bed leveling test, simply send those commands to your printer:
```
G28
G26 C H200 P5 R25 Q4.2 Z4
```
- To adjust your filament's needed temperature, change the number of the `H` parameter
- Default bed temperature is 60°C, if you need another temperature, add e.g. `B80`
- `Q` parameter sets retraction length in mm, `Z` sets unretraction.
- If your leveling is good, you will have a complete pattern of your mesh on your bed that you can peel off in one piece
- Don't worry if the test looks a bit messy, the important thing is just that the line width is the same all over the mesh
- Optional: Hang it up on a wall to display it as a trophy of how great your leveling skills are.


## M600 Filament Change

![M600 Demo][m600 demo]

[m600 demo]: https://kore.cc/i3mega/img/m600demo.jpg "M600 demo"

**BETA: This now also works without USB printing, via SD & display.**

#### Configuration (only needed once):
- Send `M603 L0 U0` to use manual loading & unloading.
- Send `M603 L538 U555` to use automatic loading & unloading
  - The `L` and `U` paramters define the load and unload length in mm. The values above work well on a stock setup, if you modded your extruder, bowden tube or hotend, you might need to adjust those.
- Save with `M500`

#### Filament change process (manual loading):
- For printing via SD:
  - Place `M600` in your GCode at the desired layer
- For printing via USB:
  - Place `M600` in your GCode at the desired layer or send it via terminal
  - Alternatively: Use `FilamentChange Pause` in the Special Menu
- The nozzle will park and your printer will beep
  - For safety reasons, the printer will turn off the hotend after 10 minutes. If you see the temperature being under the target:
    - SD printing: Click `CONTINUE` **(only once!)** on the screen and wait for the hotend to heat up again.
    - USB printing: Send `M108` and wait for the hotend to heat up again.
- Remove the filament from the bowden tube
- Insert the new filament right up to the nozzle, just until a bit of plastic oozes out
- Remove the excess filament from the nozzle with tweezers
- For printing via SD:
  - Click `CONTINUE` on the screen
- For printing via USB:
  - Send `M108` via your USB host or use `FilamentChange Resume` in the Special Menu
  - Note for OctoPrint users: After sending `M108`, enable the advanced options at the bottom of the terminal and press `Fake Acknowledgement`

#### Filament change process (automatic loading):
- For printing via SD:
  - Place `M600` in your GCode at the desired layer
- For printing via USB:
  - Place `M600` in your GCode at the desired layer or send it via terminal
  - Alternatively: Use `FilamentChange Pause` in the Special Menu
- The nozzle will park
- The printer will remove the filament right up to the extruder and beep when finished
  - For safety reasons, the printer will turn off the hotend after 10 minutes. If you see the temperature being under the target:
    - SD printing: Click `CONTINUE` **(only once!)** on the screen and wait for the hotend to heat up again.
    - USB printing: Send `M108` and wait for the hotend to heat up again.
- Insert the new filament just up to the end of the bowden fitting, as shown here:

![Load Filament][m600 load]

[m600 load]: https://kore.cc/i3mega/img/load.jpg "M600 Load"

- For printing via SD:
  - Click `CONTINUE` on the screen
- For printing via USB:
  - Send `M108` via your USB host or use `FilamentChange Resume` in the Special Menu
  - Note for OctoPrint users: After sending `M108`, enable the advanced options at the bottom of the terminal and press `Fake Acknowledgement`
- The printer will now pull in the new filament, watch out since it might ooze quite a bit from the nozzle
- Remove the excess filament from the nozzle with tweezers


## Updating

### Back up & restore your settings

Some updates require the storage to be cleared (`M502`), if mentioned in the update log. In those cases, before updating, send `M503` and make a backup of all the lines starting with:

```
M92
G29
M301
M304
```

After flashing the new version, issue a `M502` and `M500`. After that, enter every line you saved before and finish by saving with `M500`.

## Something went wrong?
No worries. You can easily go back to the default firmware and restore the default settings.
- Flash the hex file from the [manufacturer's website](http://www.anycubic3d.com/support/show/594016.html) (in case it's offline, I have uploaded the stock firmwares [here](https://kore.cc/i3mega/download/stockFW/) as well).
- After flashing, send `M502` and `M500`. Now your machine is exactly as it came out of the box.


## Detailed changes:

- Thermal runaway protection thresholds tweaked
- Manual mesh bed leveling enabled ([check this link](https://github.com/MarlinFirmware/Marlin/wiki/Manual-Mesh-Bed-Leveling) to learn more about it)
- Heatbed PID mode enabled
- TMC2208 configured in standalone mode
- Stepper orientation flipped (you don't have to flip the connectors on the board anymore)
- Linear advance unlocked (Off by default. [Research, calibrate](http://marlinfw.org/docs/features/lin_advance.html) and then enable with `M900 Kx`)
- S-Curve Acceleration enabled
- G26 Mesh Validation enabled
- Some redundant code removed to save memory
- Minor tweaks on default jerk and acceleration
- Print statistics enabled (send `M78` to read them)
- `M600` filament change feature enabled
  - Implemented easy resume via display
- Filament runout behaviour tweaked
  - Added purge and retract
  - Move nozzle to park position on runout
  - Prevent false positives by adding a small delay to the sensor
- Pause and stop behaviour tweaked
- Added `M888` cooldown routine for the Anycubic Ultrabase

## Submitting Patches

Proposed patches should be submitted as a Pull Request against the ([bugfix-2.0.x](https://github.com/MarlinFirmware/Marlin/tree/bugfix-2.0.x)) branch.

- This branch is for fixing bugs and integrating any new features for the duration of the Marlin 2.0.x life-cycle.
- Follow the [Coding Standards](http://marlinfw.org/docs/development/coding_standards.html) to gain points with the maintainers.
- Please submit your questions and concerns to the [Issue Queue](https://github.com/MarlinFirmware/Marlin/issues).

### [RepRap.org Wiki Page](http://reprap.org/wiki/Marlin)

## Credits

The current Marlin dev team consists of:

 - Scott Lahteine [[@thinkyhead](https://github.com/thinkyhead)] - USA &nbsp; [![Flattr Scott](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=thinkyhead&url=https://github.com/MarlinFirmware/Marlin&title=Marlin&language=&tags=github&category=software)
 - Roxanne Neufeld [[@Roxy-3D](https://github.com/Roxy-3D)] - USA
 - Bob Kuhn [[@Bob-the-Kuhn](https://github.com/Bob-the-Kuhn)] - USA
 - Chris Pepper [[@p3p](https://github.com/p3p)] - UK
 - João Brazio [[@jbrazio](https://github.com/jbrazio)] - Portugal
 - Erik van der Zalm [[@ErikZalm](https://github.com/ErikZalm)] - Netherlands &nbsp; [![Flattr Erik](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=ErikZalm&url=https://github.com/MarlinFirmware/Marlin&title=Marlin&language=&tags=github&category=software)

## License

Marlin is published under the [GPL license](/LICENSE) because we believe in open development. The GPL comes with both rights and obligations. Whether you use Marlin firmware as the driver for your open or closed-source product, you must keep Marlin open, and you must provide your compatible Marlin source code to end users upon request. The most straightforward way to comply with the Marlin license is to make a fork of Marlin on Github, perform your modifications, and direct users to your modified fork.

While we can't prevent the use of this code in products (3D printers, CNC, etc.) that are closed source or crippled by a patent, we would prefer that you choose another firmware or, better yet, make your own.
