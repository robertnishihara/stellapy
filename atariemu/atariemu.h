

extern "C" {

struct OSystem;
struct Settings;


enum emuGame {
	emu_beam_rider,
	emu_breakout,
	emu_enduro,
	emu_pong,
	emu_qbert,
	emu_seaquest,
	emu_space_invaders
};


struct emuState {
	OSystem* o;
	Settings* s;
	int screenWidth;
	int screenHeight;
	emuGame game;
	int score;
	int lives;
};

struct emuAction {
	int horiz;
	int vert;
	int fire;
};

emuState* emuNewState(const char* game, const char* rom_dir);

void emuDeleteState(emuState* e);

void emuReset(emuState* e);

void emuScreenShape(emuState* e, int& nrows, int& ncols, int& nchan);

void emuGetImage(emuState* e, char* img);

int emuRamSize();

void emuGetRam(emuState* e, char* ram);

void emuStep(emuState* e, const emuAction& a, int& reward, bool& gameOver, bool& roundOver);

}
