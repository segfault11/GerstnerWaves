#include <CGK/App/Application.h>
#include "Ocean.h"

int main(int argc, const char *argv[])
{
    CGKAppInit("Ocean", 10, 10, 800, 600);
	Ocean ocean(CGKVector2f(0.0, 0.0), CGKVector2f(1.0f, 2.0f), 0.05f);

	CGKAppRegisterDrawable(ocean);
    CGKAppRun();
    CGKAppDestroy();

    return 0;
}

