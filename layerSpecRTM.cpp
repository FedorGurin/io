#include "layerSpecRTM.h"

#ifdef VXWORKS_PLATFORM
uint32_t LayerArincVx::valChIn[];
uint32_t LayerArincVx::valChOut[];
#else
uint32_t LayerArincDefault::valChIn[];
uint32_t LayerArincDefault::valChOut[];
#endif


#if defined(VXWORKS_PLATFORM)
/*template<>*/ bool LayerConfAr::initOk = false;
/*template<>*/ uint8_t LayerConfAr::numCh = 0;

#else
template<> bool LayerConfAr::initOk = false;
template<> uint8_t LayerConfAr::numCh = 0;
#endif
