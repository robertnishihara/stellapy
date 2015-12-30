#include <boost/numpy.hpp>
#include <cmath>
#include "macros.h"
#include <iostream>

// LOLOLOLOL just need _SetCtrl to access paddle state (not even sure if its necessary)
#define private public
#define protected public
#include <ale_interface.hpp>
#undef private
#undef protected

#include "emucore/m6502/src/System.hxx"
#include "emucore/FSNode.hxx"
#include "emucore/OSystem.hxx"
#include "os_dependent/SettingsWin32.hxx"
#include "os_dependent/OSystemWin32.hxx"
#include "os_dependent/SettingsUNIX.hxx"
#include "os_dependent/OSystemUNIX.hxx"
#include "games/Roms.hpp"
#include "common/Defaults.hpp"
#include "common/display_screen.h"
#include "environment/stella_environment.hpp"

namespace bp = boost::python;
namespace bn = boost::numpy;

#define UINT8_DTYPE bn::dtype::get_builtin<uint8_t>()
#define CHECK_UINT8_ARRAY(x,ndim) FAIL_IF_FALSE((x).get_dtype() == UINT8_DTYPE && (x).get_nd() == (ndim) && (x).get_flags() & bn::ndarray::C_CONTIGUOUS)


template<typename T>
bn::ndarray toNdarray1(const T* data, long dim0) {
  long dims[1] = {dim0};
  bn::ndarray out = bn::empty(1, dims, bn::dtype::get_builtin<T>());
  memcpy(out.get_data(), data, dim0*sizeof(T));
  return out;
}
template<typename T>
bn::ndarray toNdarray2(const T* data, long dim0, long dim1) {
  long dims[2] = {dim0,dim1};
  bn::ndarray out = bn::empty(2, dims, bn::dtype::get_builtin<T>());
  memcpy(out.get_data(), data, dim0*dim1*sizeof(T));
  return out;
}
template<typename T>
bn::ndarray toNdarray3(const T* data, long dim0, long dim1, long dim2) {
  long dims[3] = {dim0,dim1,dim2};
  bn::ndarray out = bn::empty(3, dims, bn::dtype::get_builtin<T>());
  memcpy(out.get_data(), data, dim0*dim1*dim2*sizeof(T));
  return out;
}


enum ObsType {
  OBS_RAM,
  OBS_LAST_IMAGE,
  OBS_ALL_IMAGES
};


class AtariWorld {

public:
  AtariWorld(const std::string&);
  bp::tuple Step(const bn::ndarray& x, const bn::ndarray& u);
  void Plot(const bn::ndarray& x);
  bn::ndarray GetRAM(const bn::ndarray& x);
  bn::ndarray GetLegalActions();
  bn::ndarray GetMinimalActions();
  bn::ndarray GetInitialState(int noopSteps);
  bool TrialDone(const bn::ndarray& x);
  bn::ndarray GetImage();
  bn::ndarray GetRAM0();
  void SetNumSteps(int n) {m_numSteps = n;}
  void SetObsType(ObsType ot) {m_obsType = ot;}
  ~AtariWorld();


private:
    void _SetState(const bn::ndarray& x);
    void _SetAction(const bn::ndarray& u);
    bn::ndarray _GetState();
    ALEInterface m_ale;

    int m_numSteps;
    int m_obsType;
    Action m_action;
    std::string m_md5sum;
};

AtariWorld::AtariWorld(const std::string& binfile)
  : m_ale(false),
    m_numSteps(4),
    m_obsType(OBS_ALL_IMAGES),
    m_action(PLAYER_A_NOOP)
{
  m_ale.loadROM(binfile);

    // Load the ROM file

	// Get the vector of legal actions
	m_md5sum = m_ale.theOSystem->console().properties().get(Cartridge_MD5);
}

AtariWorld::~AtariWorld() {
}


void AtariWorld::_SetState(const bn::ndarray& x) {
  const char* data = reinterpret_cast<const char*>(x.get_data());
  int nbytes = x.get_dtype().get_itemsize() * x.shape(0);
  std::string state(data, nbytes);
  Deserializer deser(state);
  m_ale.theOSystem->console().system().loadState(m_md5sum, deser);
  m_ale.settings->loadState(deser);
  m_ale.environment->m_state.m_left_paddle = deser.getInt();
  m_ale.environment->m_state.m_right_paddle = deser.getInt();
}

bn::ndarray AtariWorld::_GetState() {
	Serializer ser;
  m_ale.theOSystem->console().system().saveState(m_md5sum,ser);
  m_ale.settings->saveState(ser);
  ser.putInt(m_ale.environment->m_state.m_left_paddle);
  ser.putInt(m_ale.environment->m_state.m_right_paddle);
  std::string s = ser.get_str();
  return toNdarray1<uint8_t>((uint8_t*)s.data(), s.size());
}

void AtariWorld::_SetAction(const bn::ndarray& u) {
  FAIL_IF_FALSE(u.get_dtype() == UINT8_DTYPE);
  const uint8_t* data = reinterpret_cast<const uint8_t*>(u.get_data());
  m_action = (Action)data[0];


}


bp::tuple AtariWorld::Step(const bn::ndarray& x, const bn::ndarray& u) {
  CHECK_UINT8_ARRAY(x,1);
  CHECK_UINT8_ARRAY(u,1);
	_SetState(x);
	_SetAction(u);

  bp::list imgs;

  int reward=0;
  Event* event = m_ale.theOSystem->event();
  for (int t = 0; t < m_numSteps; t++) {
    if (m_ale.environment->m_use_paddles) {
      m_ale.environment->m_state.applyActionPaddles(event, m_action, PLAYER_B_NOOP);
    }
    else {
      m_ale.environment->m_state.setActionJoysticks(event, m_action, PLAYER_B_NOOP);
    }
    m_ale.environment->m_osystem->console().mediaSource().update();
    m_ale.settings->step(m_ale.theOSystem->console().system()); // just reads out ram to get reward
    reward += m_ale.settings->getReward();
    if (m_obsType == OBS_ALL_IMAGES) {
      imgs.append(GetImage());
    }
  }
  switch (m_obsType) {
    case OBS_RAM:
      return bp::make_tuple(_GetState(), reward, GetRAM0(), m_ale.environment->isTerminal());
    case OBS_LAST_IMAGE:
      return bp::make_tuple(_GetState(), reward, GetImage(), m_ale.environment->isTerminal());
    case OBS_ALL_IMAGES:
      return bp::make_tuple(_GetState(), reward, imgs, m_ale.environment->isTerminal());
  }
  // unreachable
  return bp::make_tuple();
}

bn::ndarray AtariWorld::GetImage() {
  int width = m_ale.getScreen().width();
  int height = m_ale.getScreen().height();
  int n_pix = m_ale.getScreen().arraySize();
  bn::ndarray img = bn::empty(bp::make_tuple(height,width,3), UINT8_DTYPE);
  uint8_t* imgdata = m_ale.theOSystem->console().mediaSource().currentFrameBuffer();
  uint8_t* outimgdata = (uint8_t*)img.get_data();

  ExportScreen* es = m_ale.theOSystem->p_export_screen;
  for (int i=0; i < n_pix; ++i) {
    int r,g,b;
    es->get_rgb_from_palette(imgdata[i], r, g, b);
    outimgdata[3*i] = b;
    outimgdata[3*i+1] = g;
    outimgdata[3*i+2] = r;
  }
  return img;
}

bn::ndarray AtariWorld::GetRAM0() {
  m_ale.environment->processRAM();
  const ALERAM& ram = m_ale.environment->getRAM();
  return toNdarray1<uint8_t>(ram.array(), ram.size());
}

bn::ndarray AtariWorld::GetRAM(const bn::ndarray& x) {
  CHECK_UINT8_ARRAY(x,1);
  _SetState(x);
  return GetRAM0();
}


void AtariWorld::Plot(const bn::ndarray& x) {
  CHECK_UINT8_ARRAY(x,1);
  _SetState(x);
	m_ale.theOSystem->p_display_screen->display_screen(m_ale.theOSystem->console().mediaSource());
}

bn::ndarray AtariWorld::GetLegalActions() {
  ActionVect vec = m_ale.getLegalActionSet();
  return toNdarray1<int>((const int*)vec.data(), vec.size());
}
bn::ndarray AtariWorld::GetMinimalActions() {
  ActionVect vec = m_ale.getMinimalActionSet();
  return toNdarray1<int>((const int*)vec.data(), vec.size());
}

bn::ndarray AtariWorld::GetInitialState(int noopSteps) {
	m_ale.environment->reset(noopSteps+30);
	return _GetState();
}

bool AtariWorld::TrialDone(const bn::ndarray& x) {
  _SetState(x);
  return m_ale.environment->isTerminal();
}

BOOST_PYTHON_MODULE(alepy) {
    bn::initialize();

    bp::enum_<ObsType>("ObsType")
    .value("RAM",OBS_RAM)
    .value("LAST_IMAGE",OBS_LAST_IMAGE)
    .value("ALL_IMAGES",OBS_ALL_IMAGES);

    bp::enum_<Action>("Action")
    .value("PLAYER_A_NOOP",PLAYER_A_NOOP)
    .value("PLAYER_A_FIRE",PLAYER_A_FIRE)
    .value("PLAYER_A_UP",PLAYER_A_UP)
    .value("PLAYER_A_RIGHT",PLAYER_A_RIGHT)
    .value("PLAYER_A_LEFT",PLAYER_A_LEFT)
    .value("PLAYER_A_DOWN",PLAYER_A_DOWN)
    .value("PLAYER_A_UPRIGHT",PLAYER_A_UPRIGHT)
    .value("PLAYER_A_UPLEFT",PLAYER_A_UPLEFT)
    .value("PLAYER_A_DOWNRIGHT",PLAYER_A_DOWNRIGHT)
    .value("PLAYER_A_DOWNLEFT",PLAYER_A_DOWNLEFT)
    .value("PLAYER_A_UPFIRE",PLAYER_A_UPFIRE)
    .value("PLAYER_A_RIGHTFIRE",PLAYER_A_RIGHTFIRE)
    .value("PLAYER_A_LEFTFIRE",PLAYER_A_LEFTFIRE)
    .value("PLAYER_A_DOWNFIRE",PLAYER_A_DOWNFIRE)
    .value("PLAYER_A_UPRIGHTFIRE",PLAYER_A_UPRIGHTFIRE)
    .value("PLAYER_A_UPLEFTFIRE",PLAYER_A_UPLEFTFIRE)
    .value("PLAYER_A_DOWNRIGHTFIRE",PLAYER_A_DOWNRIGHTFIRE)
    .value("PLAYER_A_DOWNLEFTFIRE",PLAYER_A_DOWNLEFTFIRE)
    .value("PLAYER_B_NOOP",PLAYER_B_NOOP)
    .value("PLAYER_B_FIRE",PLAYER_B_FIRE)
    .value("PLAYER_B_UP",PLAYER_B_UP)
    .value("PLAYER_B_RIGHT",PLAYER_B_RIGHT)
    .value("PLAYER_B_LEFT",PLAYER_B_LEFT)
    .value("PLAYER_B_DOWN",PLAYER_B_DOWN)
    .value("PLAYER_B_UPRIGHT",PLAYER_B_UPRIGHT)
    .value("PLAYER_B_UPLEFT",PLAYER_B_UPLEFT)
    .value("PLAYER_B_DOWNRIGHT",PLAYER_B_DOWNRIGHT)
    .value("PLAYER_B_DOWNLEFT",PLAYER_B_DOWNLEFT)
    .value("PLAYER_B_UPFIRE",PLAYER_B_UPFIRE)
    .value("PLAYER_B_RIGHTFIRE",PLAYER_B_RIGHTFIRE)
    .value("PLAYER_B_LEFTFIRE",PLAYER_B_LEFTFIRE)
    .value("PLAYER_B_DOWNFIRE",PLAYER_B_DOWNFIRE)
    .value("PLAYER_B_UPRIGHTFIRE",PLAYER_B_UPRIGHTFIRE)
    .value("PLAYER_B_UPLEFTFIRE",PLAYER_B_UPLEFTFIRE)
    .value("PLAYER_B_DOWNRIGHTFIRE",PLAYER_B_DOWNRIGHTFIRE)
    .value("PLAYER_B_DOWNLEFTFIRE",PLAYER_B_DOWNLEFTFIRE)
    .value("RESET",RESET)
    .value("UNDEFINED",UNDEFINED)
    .value("RANDOM",RANDOM)
    .value("SAVE_STATE",SAVE_STATE)
    .value("LOAD_STATE",LOAD_STATE)
    .value("SYSTEM_RESET",SYSTEM_RESET)
    .value("LAST_ACTION_INDEX",LAST_ACTION_INDEX);

    bp::class_<AtariWorld,boost::noncopyable>("AtariWorld","docstring here", bp::init<const std::string&>())
        .def("Step",&AtariWorld::Step)
        // .def("StepMulti",&AtariWorld::StepMulti)
        .def("Plot",&AtariWorld::Plot)
        .def("GetRAM",&AtariWorld::GetRAM)
        .def("GetLegalActions",&AtariWorld::GetLegalActions)
        .def("GetMinimalActions",&AtariWorld::GetMinimalActions)
        .def("GetInitialState",&AtariWorld::GetInitialState)
        .def("TrialDone",&AtariWorld::TrialDone)
        .def("SetNumSteps",&AtariWorld::SetNumSteps)
        .def("GetImage",&AtariWorld::GetImage)
        .def("SetObsType",&AtariWorld::SetObsType)
        ;
}
