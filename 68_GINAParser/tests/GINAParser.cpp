
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//Own includes

#include "IMUBase.hpp"
#include "IMUData.hpp"
#include "IMUHeader.hpp"
#include "IMUStream.hpp"
#include "IMUStore.hpp"

#include "TrajectoryBase.hpp"
#include "TrajectoryData.hpp"
#include "TrajectoryHeader.hpp"
#include "TrajectoryStream.hpp"
#include "TrajectoryStore.hpp"

#include "GPSWeekSecond.hpp"
#include "GALWeekSecond.hpp"
#include "BasicFramework.hpp"

//#include "CoordinateFrames.h"
//#include "CoordinateFrameHandler.h"

using namespace std;

int main(int argc, char **argv) {


	// IMU file parser
	GINASimulator::IMUStream imuFileIn("C:\\Users\\LUS2BP\\Source\\Repos\\GINA\\GINAParser\\files\\example_IMU_Input.gina");
	GINASimulator::IMUStream imuFileOut("C:\\Users\\LUS2BP\\Source\\Repos\\GINA\\GINAParser\\files\\example_IMU_Input_out.gina", std::ios::out);
	GINASimulator::IMUHeader imuHeader;
	GINASimulator::IMUData imuData;

	imuFileIn >> imuHeader;
	imuFileOut << imuHeader;
	
	imuFileIn >> imuData;
	imuFileOut << imuData;

	imuFileIn >> imuData;
	imuFileOut << imuData;

	imuFileIn >> imuData;
	imuFileOut << imuData;
	
	imuFileIn.close();
	imuFileOut.close();

	// Trajectory file parser
	
	GINASimulator::TrajectoryStream trajFileIn("C:\\Users\\LUS2BP\\Source\\Repos\\GINA\\GINAParser\\files\\example_trajectory.gina");
	GINASimulator::TrajectoryStream trajFileOut("C:\\Users\\LUS2BP\\Source\\Repos\\GINA\\GINAParser\\files\\example_trajectory_out.gina", std::ios::out);
	GINASimulator::TrajectoryHeader trajHeader;
	GINASimulator::TrajectoryData trajData;

	trajFileIn >> trajHeader;
	trajFileOut << trajHeader;

	trajFileIn >> trajData;
	trajFileOut << trajData;

	trajFileIn >> trajData;
	trajFileOut << trajData;

	trajFileIn >> trajData;
	trajFileOut << trajData;

	trajFileIn.close();
	trajFileOut.close();

getchar();
return 0;

}