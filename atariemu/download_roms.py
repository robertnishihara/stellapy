#!/usr/bin/env python
from control4.misc.console_utils import mkdirp,call_and_print,yes_or_no
from control4.config import user_cfg_dir
import os,os.path as osp,sys

rom_dir = osp.join(user_cfg_dir(),"roms")
if yes_or_no("downloading to %s. OK?"%rom_dir):
    pass
else:
    print "Alright then. Exiting"
    sys.exit(0)

mkdirp(rom_dir)
os.chdir(rom_dir)

for substr in ["A-E","F-J","K-P","Q-S","T-Z"]:    
    call_and_print("wget http://www.atariage.com/2600/emulation/RomPacks/Atari2600_%s.zip"%substr)
    call_and_print("unzip -o Atari2600_%s.zip"%substr)
