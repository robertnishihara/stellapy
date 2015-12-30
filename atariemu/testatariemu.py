import ctypes as ct
import os,sys
assert len(sys.argv)==2
rom_dir = "/Users/joschu/Proj/control/domain_data/atari_roms"
lib=ct.cdll.LoadLibrary("/Users/joschu/build/control/lib/libatariemu.so")
emuNewState=lib.emuNewState
emuNewState.restype = ct.c_void_p
emuDeleteState=lib.emuDeleteState
emuGetImage=lib.emuGetImage
emuReset=lib.emuReset
emuStep=lib.emuStep
p = emuNewState(ct.c_char_p(sys.argv[1]), ct.c_char_p(rom_dir))
if p is None:
    sys.exit(1)
e = ct.c_void_p(p)
emuReset(e)
reward = ct.c_int()
gameOver = ct.c_bool()
roundOver = ct.c_bool()
import numpy as np
imgArr = np.zeros((210,160,3),'uint8')
imgBuf = imgArr.ctypes.data_as(ct.POINTER(ct.c_char))
ramBuf = ct.c_char_p()

class Action(ct.Structure):
    _fields_ = [("horiz", ct.c_int),
                ("vert", ct.c_int),
                ("button", ct.c_int),
                ]
a = Action(0,0,0)
emuStep(e,ct.byref(a),ct.byref(reward),ct.byref(gameOver),ct.byref(roundOver),imgBuf,ramBuf)
a.horiz=1
a.button=1

import cv2
for i in xrange(100000):
    a.horiz=np.random.randint(-1,2)
    a.button=np.random.randint(0,2)
    emuStep(e,ct.byref(a),ct.byref(reward),ct.byref(gameOver),ct.byref(roundOver))
    emuGetImage(e,imgBuf)
    cv2.imshow('hi',imgArr)
    cv2.waitKey(5)
    if gameOver.value:
        emuReset(e)

