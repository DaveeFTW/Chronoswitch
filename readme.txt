Chronoswitch Downgrader
http://lolhax.org

[VERSION 5]
	-> Added support for 09g units to 6.20

[What does it do?]
Allows users who are incontent with their firmware to downgrade to 6.20 using the Sony Updater.

[How do I use it?]
Copy the "PSP" folder to your PSP. The downgrader is "signed" and can be launched from OFW XMB. You need to copy the 6.20 update to PSP/GAME/UPDATE/EBOOT.PBP and for PSPgo it must be the eflash it is placed on. After setup run the downgrader and follow the onscreen instructions.

[What does it technically do?]
6.20-6.35: It uses the utility/power exploit to gain kernel access and reboots into the updater with a special PRX running. This PRX uses the pspdecrypt functionality to decrypt the updater PRX when needed. This allows the updater to boot in the newer firmwares.
6.38/6.39: It uses my http_storage exploit to gain kernel access.  Basically, http_storage has a vulnerability in it, where I can write -1 to anywhere in memory.
6.60: Uses sceNetMPulldown to gain kernel access.

[I'm a dev, sup?]
Source code included in package.

[Credits]
Davee - legend. Originally made 6.35/6.31 downgrader
some1 - legend. 6.38/6.39/6.60 support.
bbtgp - legend. Continued updates to psardumper (pspdecrypt) and the signing application "prxEncrypter".
coyotebean - legend. Coninuted updates to psardumper (pspdecrypt) and large influence and research regarding cryptographics.
kgsws - legend. First application signed and method released.
Silverspring - legend. Lots and lots of info on KIRK where we'd still be in the dark without.
Bubbletune - BTCNF injection code and bits and bobs here ;D

[READ]
You run the application at your OWN risk. I am not help accountable if you PSP fucks up.