#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GPSWeekSecond.hpp"
#include "BasicFramework.hpp"
#include "PRSolution.hpp"

#include "EGNOS_EMS_Data.hpp"
#include "EGNOS_EMS_Stream.hpp"

#include "IGPMap.hpp"
#include "IonosphericMaskBand.hpp"
#include "IonosphericGridPoint.hpp"

#include "GINAConfig.h"

#include "IonexStore.hpp"

#include "IGPMap.hpp"
#include "IonoCorrection.hpp"
#include "EGNOS_Ionex_Converter.hpp"

using namespace std;
namespace EGNOS 
{
	namespace EGNOS_RUNNABLE 
	{
		string createStrFileIndex(int index);

		void processEMS(std::string EDAS_FileNamewPath,
						std::string EDAS_FileNamewPath_LastMap,
						std::string Output_IonexFileNamewPath,
						std::string Output_IonexFileNamewPath_Detailed = "",
						EGNOSMapType mapType = EGNOS::europe5x5_tec,
						bool interPolationOn = true);
	};
	
};