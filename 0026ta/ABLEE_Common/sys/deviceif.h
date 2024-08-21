#ifndef CDEVICEIF_HPP
#define CDEVICEIF_HPP

#include "mainapp.hpp"

extern CPcie   g_pcie;
extern CBib    g_bib;
extern CDbm    g_dbm;
extern CDfm    g_dfm;
extern CDle    g_dle;
extern CLog    g_log;
extern CHw     g_hw;
extern CPg     g_pg;
extern CPps    g_pps;
extern CTg     g_tg;
extern CUbm    g_ubm;
extern CSys    g_sys;
extern CVector g_vector;
extern CDevSocket    g_devsock;

#define START_DEVICE(a)          														\
    class DeviceIF : public CApi	  										            \
    {																					\
    public:																				\
		DeviceIF() : CApi() { SetDeviceProgram(#a); }                                   \
        ~DeviceIF()														                \
        {																				\
        }																				\

#define END_DEVICE()    																\
    };																					\
	DeviceIF* pDevPtr;                                                                  \
    int main( int argc, char* argv[] )                                                  \
    {                                                                                   \
		CMainApp app;                                                                   \
		static DeviceIF pgm;                                                            \
		pDevPtr  = &pgm;                                                                \
		app.SetDeviceIf( argv[0], &pgm );                                               \
		return app.parseArguments( argc, argv );                                        \
    }                                                                                   \

#endif // DEVICEIF_HPP
