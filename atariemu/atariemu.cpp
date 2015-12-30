#include <sys/stat.h>
#include "m6502/src/System.hxx"
#include "OSystem.hxx"
#include "atariemu.h"
#include <string>
#include <cstring>

namespace {

int fileExists(const char *filename)
	{
	  struct stat   buffer;   
	  return (stat (filename, &buffer) == 0);
	}

const int RAM_SIZE = 128;
const int PADDLE_DELTA=23000, PADDLE_MIN=27450,PADDLE_MAX=790196,PADDLE_DEFAULT_VALUE=(PADDLE_MAX+PADDLE_MIN)/2;

emuGame gameStr2Enum(const char* gamename) {
	#define ADDGAME(name) if (strcmp(gamename,#name)==0) return emu_##name;
	ADDGAME(beam_rider)
	ADDGAME(breakout)
	ADDGAME(enduro)
	ADDGAME(pong)
	ADDGAME(qbert)
	ADDGAME(seaquest)
	ADDGAME(space_invaders)	
	#undef ADDGAME
	printf("couldn't find game of name %s",gamename);
	abort();
}

/* reads a byte at a memory location between 0 and 128 */
int readRam(const System* system, int offset) {
    // peek modifies data-bus state, but is logically const from
    // the point of view of the RL interface
    // JDS: Huh?
    System* sys = const_cast<System*>(system);
    return sys->peek((offset & 0x7F) + 0x80);
}

/* extracts a decimal value from a byte */
int getDecimalScore(int index, const System* system) {
    
    int score = 0;
    int digits_val = readRam(system, index);
    int right_digit = digits_val & 15;
    int left_digit = digits_val >> 4;
    score += ((10 * left_digit) + right_digit);    

    return score;
}

/* extracts a decimal value from 2 bytes */
int getDecimalScore(int lower_index, int higher_index, const System* system) {

    int score = 0;
    int lower_digits_val = readRam(system, lower_index);
    int lower_right_digit = lower_digits_val & 15;
    int lower_left_digit = (lower_digits_val - lower_right_digit) >> 4;
    score += ((10 * lower_left_digit) + lower_right_digit);
    if (higher_index < 0) {
        return score;
    }
    int higher_digits_val = readRam(system, higher_index);
    int higher_right_digit = higher_digits_val & 15;
    int higher_left_digit = (higher_digits_val - higher_right_digit) >> 4;
    score += ((1000 * higher_left_digit) + 100 * higher_right_digit);
    return score;
}

/* extracts a decimal value from 3 bytes */
int getDecimalScore(int lower_index, int middle_index, int higher_index, const System* system) {

    int score = getDecimalScore(lower_index, middle_index, system);
    int higher_digits_val = readRam(system, higher_index);
    int higher_right_digit = higher_digits_val & 15;
    int higher_left_digit = (higher_digits_val - higher_right_digit) >> 4;
    score += ((100000 * higher_left_digit) + 10000 * higher_right_digit);
    return score;
}

// clip integer
int clipi(int x, int lo, int hi) {
	return (x < lo) ? lo : (x > hi ? hi : x);
}

void setAction(emuState* e, const emuAction& a) {
	Event* event = e->o->event();

	// paddles
	int prevPaddlePos = event->get(Event::PaddleZeroResistance);
	event->set(Event::PaddleZeroResistance, clipi(prevPaddlePos + a.horiz*PADDLE_DELTA, PADDLE_MIN, PADDLE_MAX));
	event->set(Event::PaddleZeroFire,		a.fire);

	// joystick
	event->set(Event::JoystickZeroFire,		a.fire);
	event->set(Event::JoystickZeroDown,		a.vert==-1);
	event->set(Event::JoystickZeroUp,		a.vert==1);	
	event->set(Event::JoystickZeroLeft,		a.horiz==-1);
	event->set(Event::JoystickZeroRight,	a.horiz==1);

}


}


emuState* emuNewState(const char* game, const char* rom_dir) {
	std::string sromfile = std::string(rom_dir) + std::string("/") + std::string(game) + std::string(".bin");
	const char* romfile=sromfile.c_str();
	if (!fileExists(romfile)) {
		fprintf( stderr, "romfile %s doesn't exist!\n" , romfile);
		return NULL;
	}

	// allocations
	emuState* e = new emuState();
	e->o = new OSystem();
	e->s = new Settings(e->o);

	OSystem* o=e->o; // for brevity

	// Settings
	e->s->setBool("display_screen", true);	
	// o->settings().loadConfig();
	o->settings().validate();
	o->create();
	o->createConsole(romfile);
	o->settings().setString("rom_file", romfile);
	// o->settings().setString("cpu","low");
	o->console().setPalette("standard");

	MediaSource& ms = o->console().mediaSource();
	e->screenWidth = ms.width();
	e->screenHeight = ms.height();
	e->game = gameStr2Enum(game);
	e->score = 0;
	e->lives = 0;

	return e;
}

void resetKeys(Event* event) {
    event->set(Event::ConsoleReset, 0);
    event->set(Event::JoystickZeroFire, 0);
    event->set(Event::JoystickZeroUp, 0);
    event->set(Event::JoystickZeroDown, 0);
    event->set(Event::JoystickZeroRight, 0);
    event->set(Event::JoystickZeroLeft, 0);
    event->set(Event::JoystickOneFire, 0);
    event->set(Event::JoystickOneUp, 0);
    event->set(Event::JoystickOneDown, 0);
    event->set(Event::JoystickOneRight, 0);
    event->set(Event::JoystickOneLeft, 0);
    
    // also reset paddle fire
    event->set(Event::PaddleZeroFire, 0);
    event->set(Event::PaddleOneFire, 0);
}

void emuReset(emuState* e) {
	// reset all keys
	// reset paddles
	Event* event = e->o->event();
	resetKeys(event);
    event->set(Event::PaddleZeroResistance,PADDLE_DEFAULT_VALUE);
    // reset variables
	e->score=0;
	e->lives=0;
	// whatever this does
	e->o->console().system().reset();
	// press the reset button
	emuAction a = {};
	setAction(e,a);

	MediaSource& ms = e->o->console().mediaSource();

	// press the reset button
	event->set(Event::ConsoleReset, true);	
	for (int i=0;i < 5; ++i) {
		ms.update();
	}	
	// noop steps
	event->set(Event::ConsoleReset, false);	
	for (int i=0;i < 40; ++i) {
		ms.update();
	}
}

void emuDeleteState(emuState* e) {
	// delete e->s;
	delete e->o;
	delete e;
}

void emuScreenShape(emuState* e, int& nrows, int& ncols, int& nchan) {
	nrows = e->screenHeight;
	ncols = e->screenWidth;
	nchan = 3;
}

void emuGetImage(emuState* e, char* imgBuf) {
	const uInt8* inputData = e->o->console().mediaSource().currentFrameBuffer();
	const uInt32* palette = e->o->console().getPalette();

	int npix = e->screenHeight*e->screenWidth;
	for (int i=0; i < npix; ++i) {
		uInt32 pv = palette[inputData[i]];
	    *imgBuf++ = pv & 0xff;
	    *imgBuf++ = (pv >> 8) & 0xff;
	    *imgBuf++ = (pv >> 16) & 0xff;
	}
}

int emuRamSize() {
	return RAM_SIZE;
}

void emuGetRam(emuState* e, char* ramBuf) {
	for (int i=0; i < RAM_SIZE; ++i) {
		ramBuf[i] = e->o->console().system().peek(i + 0x80); 
	}
}

void emuStep(emuState* e, const emuAction& a, int& m_reward, bool& m_terminal, bool& roundOver) {
	System& system = e->o->console().system();
	setAction(e,a);
	e->o->console().mediaSource().update();	
	int& m_score=e->score;
	int& m_lives=e->lives;
	int livesBeforeStep = m_lives;
	// bool m_started=1; 
	switch (e->game) {
		case emu_beam_rider : {
		    // update the reward
		    int score = getDecimalScore(9, 10, 11, &system);
		    m_reward = score - m_score;
		    m_score = score;
		    int new_lives = readRam(&system, 0x85) + 1;

		    // Decrease lives *after* the death animation; this is necessary as the lives counter
		    // blinks during death
		    if (new_lives == m_lives - 1) {
		        if (readRam(&system, 0x8C) == 0x01)
		            m_lives = new_lives;
		    }
		    else
		        m_lives = new_lives;

		    // update terminal status
		    int byte_val = readRam(&system, 5);
		    m_terminal = byte_val == 255;
		    byte_val = byte_val & 15;
		    m_terminal = m_terminal || byte_val < 0;
		    break;
		}
		case emu_breakout : {
		    // update the reward
		    int x = readRam(&system, 77);
		    int y = readRam(&system, 76);
		    reward_t score = 1 * (x & 0x000F) + 10 * ((x & 0x00F0) >> 4) + 100 * (y & 0x000F);
		    m_reward = score - m_score;
		    m_score = score;

		    // update terminal status
		    int byte_val = readRam(&system, 57);
		    m_terminal = byte_val == 0;
		    m_lives = byte_val;
		    break;
		}
		case emu_enduro : {
		    // update the reward
		    int score = 0;
		    int level = readRam(&system, 0xAD);
		    if (level != 0) {
		        int cars_passed = getDecimalScore(0xAB, 0xAC, &system);
		        if (level == 1) cars_passed = 200 - cars_passed;
		        else if (level >= 2) cars_passed = 300 - cars_passed;
		        else assert(false);

		        // First level has 200 cars
		        if (level >= 2) {
		            score = 200;
		            // For every level after the first, 300 cars
		            score += (level - 2) * 300;
		        }
		        score += cars_passed;
		    }

		    m_reward = score - m_score;
		    m_score = score;

		    // update terminal status
		    //int timeLeft = readRam(&system, 0xB1);
		    int deathFlag = readRam(&system, 0xAF);
		    m_terminal = deathFlag == 0xFF;
		    break;
	    }
		case emu_pong : {
		    // update the reward
		    int x = readRam(&system, 13); // cpu score
		    int y = readRam(&system, 14); // player score
		    reward_t score = y - x;
		    m_reward = score - m_score;
		    m_score = score;

		    // update terminal status
		    // game is over when a player reaches 21,
		    // or too many steps have elapsed since the last reset
		    m_terminal = x == 21 || y == 21;
		    break;
		}
		case emu_qbert : {
			 // xxx
		    int lives_value = readRam(&system, 0x88);		
		    m_terminal = (lives_value == 0xFE) ||
		      (lives_value == 0x02 && m_lives == 0xFF);
		    
		    m_lives = lives_value;
		    // update the reward
		    // Ignore reward if reset the game via the fire button; otherwise the agent 
		    //  gets a big negative reward on its last step 
		    if (!m_terminal) {
		      int score = getDecimalScore(0xDB, 0xDA, 0xD9, &system);
		      m_reward = score - m_score;
		      m_score = score;
		    }
		    else {
		      m_reward = 0;
		    }
		    break;


		}
		case emu_seaquest : {
		    // update the reward
		    reward_t score = getDecimalScore(0xBA, 0xB9, 0xB8, &system);
		    m_reward = score - m_score;
		    m_score = score;

		    // update terminal status
		    m_terminal = readRam(&system, 0xA3) != 0;
		    m_lives = readRam(&system, 0xBB) + 1;
		    break;
		}
		case emu_space_invaders : {
		    // update the reward
		    int score = getDecimalScore(0xE8, 0xE6, &system);
		    m_reward = score - m_score;
		    m_score = score;
		    m_lives = readRam(&system, 0xC9);

		    // update terminal status
		    // If bit 0x80 is on, then game is over 
		    int some_byte = readRam(&system, 0x98); 
		    m_terminal = (some_byte & 0x80) || m_lives == 0;
		    break;
		}
		default: abort();

	}
	roundOver = (m_lives < livesBeforeStep);
}

