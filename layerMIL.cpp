#include "layerMIL.h"
#ifdef VXWORKS_PLATFORM
/*template<>*/ bool LayerConfMIL::initOk = false;
/*template<>*/ uint8_t LayerConfMIL::numCh = 0;

#else
template<> bool LayerConfMIL::initOk = false;
template<> uint8_t LayerConfMIL::numCh = 0;

#endif

