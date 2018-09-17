#include <stdio.h>

#include <iostream>
#include <chrono>

#include "prettyprint.h"

#include "rad.h"

int main(int argc, char** argv)
{
	CTimer timer("Main");
	CRAD* rad = new CRAD(argv[2]);

	rad->ReadLights(argv[1]);
	rad->LoadBSP();
	rad->Setup();
	rad->ParseEntities();
	//rad->ExtractBrushEntityShadowCasters();
	rad->AddBrushesForRayTrace();
	delete rad;

	timer.End();
	getchar();

	return 0;
}