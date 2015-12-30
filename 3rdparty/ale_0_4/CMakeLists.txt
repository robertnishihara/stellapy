
include_directories(src/games src/emucore src/emucore/m6502/src src/emucore/m6502/src/bspf/src src/common src/controllers src/agents src)

option(USE_SDL OFF)


set (ALE_SRCS
	 src/emucore/AtariVox.cxx
	 src/emucore/Booster.cxx
	 src/emucore/Cart2K.cxx
	 src/emucore/Cart3F.cxx
	 src/emucore/Cart3E.cxx
	 src/emucore/Cart4A50.cxx
	 src/emucore/Cart4K.cxx
	 src/emucore/CartAR.cxx
	 src/emucore/CartCV.cxx
	 src/emucore/Cart.cxx
	 src/emucore/CartDPC.cxx
	 src/emucore/CartE0.cxx
	 src/emucore/CartE7.cxx
	 src/emucore/CartF4.cxx
	 src/emucore/CartF4SC.cxx
	 src/emucore/CartF6.cxx
	 src/emucore/CartF6SC.cxx
	 src/emucore/CartF8.cxx
	 src/emucore/CartF8SC.cxx
	 src/emucore/CartFASC.cxx
	 src/emucore/CartFE.cxx
	 src/emucore/CartMB.cxx
	 src/emucore/CartMC.cxx
	 src/emucore/CartUA.cxx
	 src/emucore/Cart0840.cxx
	 src/emucore/Console.cxx
	 src/emucore/Control.cxx
	 src/emucore/Deserializer.cxx
	 src/emucore/Driving.cxx
	 src/emucore/Event.cxx
	 src/emucore/FSNode.cxx
	 src/emucore/Joystick.cxx
	 src/emucore/Keyboard.cxx
	 src/emucore/M6532.cxx
	 src/emucore/MD5.cxx
	 src/emucore/MediaSrc.cxx
	 src/emucore/OSystem.cxx
	 src/emucore/Paddles.cxx
	 src/emucore/Props.cxx
	 src/emucore/PropsSet.cxx
	 src/emucore/Random.cxx
	 src/emucore/Serializer.cxx
	 src/emucore/Settings.cxx
	 src/emucore/SpeakJet.cxx
	 src/emucore/Switches.cxx
	 src/emucore/TIA.cxx
	 src/emucore/TIASnd.cxx
	 src/emucore/unzip.c
	 src/emucore/m6502/src/Device.cxx
	 src/emucore/m6502/src/M6502.cxx
	 src/emucore/m6502/src/M6502Low.cxx
	 src/emucore/m6502/src/M6502Hi.cxx
	 src/emucore/m6502/src/NullDev.cxx
	 src/emucore/m6502/src/System.cxx
	 src/common/SoundNull.cxx
	 src/common/export_screen.cpp
	 src/common/display_screen.cpp
	 src/common/Constants.cpp
	 src/common/Defaults.cpp
	 src/controllers/ale_controller.cpp
	 src/controllers/fifo_controller.cpp
	 src/controllers/internal_controller.cpp
	 src/controllers/rlglue_controller.cpp
	 src/os_dependent/FSNodePOSIX.cxx
	 src/os_dependent/OSystemUNIX.cxx
	 src/os_dependent/SettingsUNIX.cxx
	 src/agents/PlayerAgent.cpp
	 src/agents/RandomAgent.cpp
	 src/agents/SingleActionAgent.cpp
	 src/agents/SDLKeyboardAgent.cpp
	 src/games/Roms.cpp
	 src/games/RomSettings.cpp
	 src/games/RomUtils.cpp
	 src/games/supported/AirRaid.cpp
	 src/games/supported/Alien.cpp
	 src/games/supported/Amidar.cpp
	 src/games/supported/Assault.cpp
	 src/games/supported/Asterix.cpp
	 src/games/supported/Asteroids.cpp
	 src/games/supported/Atlantis.cpp
	 src/games/supported/BankHeist.cpp
	 src/games/supported/BattleZone.cpp
	 src/games/supported/BeamRider.cpp
	 src/games/supported/Berzerk.cpp
	 src/games/supported/Bowling.cpp
	 src/games/supported/Boxing.cpp
	 src/games/supported/Breakout.cpp
	 src/games/supported/Carnival.cpp
	 src/games/supported/Centipede.cpp
	 src/games/supported/ChopperCommand.cpp
	 src/games/supported/CrazyClimber.cpp
	 src/games/supported/Defender.cpp
	 src/games/supported/DemonAttack.cpp
	 src/games/supported/DoubleDunk.cpp
	 src/games/supported/ElevatorAction.cpp
	 src/games/supported/Enduro.cpp
	 src/games/supported/FishingDerby.cpp
	 src/games/supported/Freeway.cpp
	 src/games/supported/Frostbite.cpp
	 src/games/supported/Gopher.cpp
	 src/games/supported/Gravitar.cpp
	 src/games/supported/Hero.cpp
	 src/games/supported/IceHockey.cpp
	 src/games/supported/JamesBond.cpp
	 src/games/supported/JourneyEscape.cpp
	 src/games/supported/Kangaroo.cpp
	 src/games/supported/Krull.cpp
	 src/games/supported/KungFuMaster.cpp
	 src/games/supported/MontezumaRevenge.cpp
	 src/games/supported/MsPacman.cpp
	 src/games/supported/NameThisGame.cpp
	 src/games/supported/Phoenix.cpp
	 src/games/supported/Pitfall.cpp
	 src/games/supported/Pong.cpp
	 src/games/supported/Pooyan.cpp
	 src/games/supported/PrivateEye.cpp
	 src/games/supported/QBert.cpp
	 src/games/supported/RiverRaid.cpp
	 src/games/supported/RoadRunner.cpp
	 src/games/supported/RoboTank.cpp
	 src/games/supported/Seaquest.cpp
	 src/games/supported/Skiing.cpp
	 src/games/supported/Solaris.cpp
	 src/games/supported/SpaceInvaders.cpp
	 src/games/supported/StarGunner.cpp
	 src/games/supported/Tennis.cpp
	 src/games/supported/TimePilot.cpp
	 src/games/supported/Tutankham.cpp
	 src/games/supported/UpNDown.cpp
	 src/games/supported/Venture.cpp
	 src/games/supported/VideoPinball.cpp
	 src/games/supported/WizardOfWor.cpp
	 src/games/supported/YarsRevenge.cpp
	 src/games/supported/Zaxxon.cpp
	 src/environment/ale_state.cpp
	 src/environment/stella_environment.cpp
	 src/environment/phosphor_blend.cpp
)

if (USE_SDL)
	 set(MAYBE_SDLGFX_SRCS
		  sdlgfx/SDL/SDL_framerate.c
		  sdlgfx/SDL/SDL_gfxPrimitives.c
		  sdlgfx/SDL/SDL_imageFilter.c
		  sdlgfx/SDL/SDL_rotozoom.c
		  sdlgfx/SDL/SDL_gfxBlitFunc.c
	 )
endif(USE_SDL)


add_library(ale SHARED 
	${ALE_SRCS}
	${MAYBE_SDLGFX_SRCS}
)

if (USE_SDL)
	add_definitions("-D__USE_SDL")
	find_package(SDL REQUIRED)
	find_package(SDL_image REQUIRED)
	include_directories(${SDL_INCLUDE_DIR} sdlgfx)
	message("SDL : ${SDLmain_LIBRARY}")
	target_link_libraries(ale ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY})	
endif(USE_SDL)
	

target_link_libraries(ale z)

add_executable(runale src/main.cpp)
target_link_libraries(runale ale ${SDLmain_LIBRARY})