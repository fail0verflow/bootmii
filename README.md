# BootMii loader

This repository contains the public release of the source code for
the BootMii stub loader.

Included portions:

* ELF loader stub
* BootMii SD loader proper
* Reset stub
* mini (submodule)

Not included:

* boot2-style-wad packaging stuff
* PyWii (see hbc)
* UI/PPC side (CE1LING_CAT)
* Installer

Note that the code in this repository may differ from the source code used to
build the official version of BootMii.

This code is released with no warranty, and has only been build tested.
If you release this to anyone but yourself without extensive testing, you are an
irresponsible person and we will forever hate you.

## Build instructions

You need armeb-eabi cross compilers. Same build setup as
[mini](https://github.com/fail0verflow/mini). See
[bootmii-utils](https://github.com/fail0verflow/bootmii-utils) for some outdated
toolchain build scripts. Type `make` to compile. Good luck.

Output is at `bootmii.bin`, which is the main `armboot.bin` style executable
for BootMii-as-IOS, with a dummy reset stub as payload. Installation as boot2
would replace the ELF payload with the original boot2 instead.

## Seriously

Do NOT release this to users unless you've read
[this](https://marcan.st/2011/01/safe-hacking/) and spent months testing
hardware variations and building an installer that cross checks every step
including boot1 simulation prior to actually committing to NAND. There's a
reason we didn't brick any Wiis with this stuff. We're releasing this because
it's been over a decade and some people are asking for it to experiment with
devkits and other weird stuff, not because anyone should try using this on their
Wii.

## License

Unless otherwise noted in an individual file header, all source code in this
repository is released under the terms of the GNU General Public License,
version 2 or later. The full text of the license can be found in the COPYING
file.
