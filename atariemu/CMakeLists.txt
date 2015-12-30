# cmake_minimum_required(VERSION 2.6)
# project(atari_emu)
# set(CMAKE_BUILD_TYPE DEBUG)

set(EMU_SRC_DIR "${CMAKE_SOURCE_DIR}/3rdparty/ale_0_4/src/emucore")
include_directories(
	"${EMU_SRC_DIR}"
	"${EMU_SRC_DIR}/m6502/src"
	"${EMU_SRC_DIR}/m6502/src/bspf/src"
	"${EMU_SRC_DIR}/../common"
)

set (EMU_SRCS
	 # "${EMU_SRC_DIR}/AtariVox.cxx"
	 "${EMU_SRC_DIR}/Booster.cxx"
	 "${EMU_SRC_DIR}/Cart2K.cxx"
	 "${EMU_SRC_DIR}/Cart3F.cxx"
	 "${EMU_SRC_DIR}/Cart3E.cxx"
	 "${EMU_SRC_DIR}/Cart4A50.cxx"
	 "${EMU_SRC_DIR}/Cart4K.cxx"
	 "${EMU_SRC_DIR}/CartAR.cxx"
	 "${EMU_SRC_DIR}/CartCV.cxx"
	 "${EMU_SRC_DIR}/Cart.cxx"
	 "${EMU_SRC_DIR}/CartDPC.cxx"
	 "${EMU_SRC_DIR}/CartE0.cxx"
	 "${EMU_SRC_DIR}/CartE7.cxx"
	 "${EMU_SRC_DIR}/CartF4.cxx"
	 "${EMU_SRC_DIR}/CartF4SC.cxx"
	 "${EMU_SRC_DIR}/CartF6.cxx"
	 "${EMU_SRC_DIR}/CartF6SC.cxx"
	 "${EMU_SRC_DIR}/CartF8.cxx"
	 "${EMU_SRC_DIR}/CartF8SC.cxx"
	 "${EMU_SRC_DIR}/CartFASC.cxx"
	 "${EMU_SRC_DIR}/CartFE.cxx"
	 "${EMU_SRC_DIR}/CartMB.cxx"
	 "${EMU_SRC_DIR}/CartMC.cxx"
	 "${EMU_SRC_DIR}/CartUA.cxx"
	 "${EMU_SRC_DIR}/Cart0840.cxx"
	 "${EMU_SRC_DIR}/Console.cxx"
	 "${EMU_SRC_DIR}/Control.cxx"
	 "${EMU_SRC_DIR}/Deserializer.cxx"
	 "${EMU_SRC_DIR}/Driving.cxx"
	 "${EMU_SRC_DIR}/Event.cxx"
	 "${EMU_SRC_DIR}/FSNode.cxx"
	 "${EMU_SRC_DIR}/Joystick.cxx"
	 "${EMU_SRC_DIR}/Keyboard.cxx"
	 "${EMU_SRC_DIR}/M6532.cxx"
	 "${EMU_SRC_DIR}/MD5.cxx"
	 "${EMU_SRC_DIR}/MediaSrc.cxx"
	 "${EMU_SRC_DIR}/OSystem.cxx"
	 "${EMU_SRC_DIR}/Paddles.cxx"
	 "${EMU_SRC_DIR}/Props.cxx"
	 "${EMU_SRC_DIR}/PropsSet.cxx"
	 "${EMU_SRC_DIR}/Random.cxx"
	 "${EMU_SRC_DIR}/Serializer.cxx"
	 "${EMU_SRC_DIR}/Settings.cxx"
	 "${EMU_SRC_DIR}/SpeakJet.cxx"
	 "${EMU_SRC_DIR}/Switches.cxx"
	 "${EMU_SRC_DIR}/TIA.cxx"
	 "${EMU_SRC_DIR}/TIASnd.cxx"
	 "${EMU_SRC_DIR}/unzip.c"
	 "${EMU_SRC_DIR}/m6502/src/Device.cxx"
	 "${EMU_SRC_DIR}/m6502/src/M6502.cxx"
	 "${EMU_SRC_DIR}/m6502/src/M6502Low.cxx"
	 "${EMU_SRC_DIR}/m6502/src/M6502Hi.cxx"
	 "${EMU_SRC_DIR}/m6502/src/NullDev.cxx"
	 "${EMU_SRC_DIR}/m6502/src/System.cxx"
	 "${EMU_SRC_DIR}/../common/SoundNull.cxx"
	 "${EMU_SRC_DIR}/../os_dependent/FSNodePOSIX.cxx"
)

include_directories($EMU_SRC_DIR)

set(MY_SRCS
	atariemu.cpp
)
add_library(atariemu SHARED ${MY_SRCS} ${EMU_SRCS})
set_target_properties(atariemu PROPERTIES SUFFIX ".so")
target_link_libraries(atariemu z)

add_executable(testatariemu testatariemu.cpp)
target_link_libraries(testatariemu atariemu)
