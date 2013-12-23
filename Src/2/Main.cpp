#include "CGK/App/Application.h"
#include <CGK/Config/ConfigFile.h>
#include <CGK/MainCamera/MainCamera.h>
#include <iostream>
#include "Wave.h"

int main(int argc, const char *argv[])
{
    CGKAppCreate("Wave 2", 10, 10, 1000, 1000);
	CGKMainCameraSetPerspective(60.0, 1.0, 0.01, 100);
	
	CGKMainCameraSetView(
		CGKVector3f(0.0, 0.3, -0.03),
		CGKVector3f(0.5, 0.0, 0.5),
		CGKVector3f(0.0, 1.0, 0.0)
	);
	
    CGKConfigFile file("Config.txt");
    std::cout << file.GetNumEntries() << std::endl;
    std::cout << file.GetEntryWithName("entity1")->GetValueForKey("waveLength") << std::endl;


	Wave w;
	CGKAppRegisterDrawable(w);
	CGKAppRun();
	CGKAppDestroy();
    return 0;
}
