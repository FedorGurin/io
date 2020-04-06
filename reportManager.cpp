#include "reportManager.h"
#include "platform.h"
#include <iostream>

#ifdef WINDOWS_PLATFORM
#include "Windows.h"
#elif LINUX_PLATFORM
#include <dlfcn.h>
#endif


ReportManager::ReportManager(uint32_t idClass):ICalculateElement(idClass)
{
    instDll     = 0;
    initDll     = 0;
    eventDll    = 0;
    bindDll     = 0;
    calcDll     = 0;

    setFreq(ICalculateElement::Hz100);
    setStop();
    //! загрузка библиотеки для Win
#ifdef WINDOWS_PLATFORM
    std::string nameLib = "libReportQt.dll";
#ifdef SUB_DEBUG
    nameLib = "libReportQtd.dll";
#endif
    HINSTANCE hModule = NULL;
    hModule = ::LoadLibraryA(nameLib.c_str());

    if(hModule!=NULL)
    {
        instDll = (Inst_)::GetProcAddress((HMODULE)hModule,"Inst");
        initDll = (Init_)::GetProcAddress((HMODULE)hModule,"Init");
        eventDll= (Event_)::GetProcAddress((HMODULE)hModule,"eEvent");
        bindDll = (Bind_)::GetProcAddress((HMODULE)hModule,"Bind");
        calcDll = (Calculate_)::GetProcAddress((HMODULE)hModule,"Calculate");
        setStart();
    }else
    {
        std::cout<<"Submodel: working without report\n";
        setStop();
    }
#else
    std::string nameLib = "libReportQtd.so";
#ifdef SUB_DEBUG
    nameLib = "libReportQtd.so";
#endif
    void *lib = NULL;
#ifdef LINUX_PLATFORM
    lib = dlopen(nameLib.c_str(), RTLD_LAZY); // RTLD_NOW
    if (lib != NULL)
    {
        instDll  = (Inst_)      dlsym(lib,"Inst");
        initDll  = (Init_)      dlsym(lib,"Init");
        eventDll = (Event_)     dlsym(lib,"eEvent");
        bindDll  = (Bind_)      dlsym(lib,"Bind");
        calcDll  = (Calculate_) dlsym(lib,"Calculate");
        setStart();
    }else
    {
        std::cout<<"Submodel: work without report\n";
        setStop();
    }
#endif
#endif
}
bool ReportManager::bind()
{
    if(bindDll!=0)
        bindDll();
    return ICalculateElement::bind();
}

void ReportManager::init()
{
    if(initDll!=0)
        initDll();
    ICalculateElement::init();
}
void ReportManager::calculate()
{
    if(calcDll!=0)
    {
        calcDll((float)timeStep());
    }

}
void ReportManager::finite()
{

}
void ReportManager::eEvent(TEventModel *event)
{
    if(eventDll!=0)
        eventDll(event);
    ICalculateElement::eEvent(event);
}
