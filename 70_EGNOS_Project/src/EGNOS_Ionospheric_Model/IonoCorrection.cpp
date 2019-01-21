#include "IGPMap.hpp"
#include "IonoCorrection.hpp"

using namespace std;

namespace EGNOS {

	const double SlantIonoDelay::Re = 6378136.3;
	const double SlantIonoDelay::hI = 350000;

	VerticalIonoDelayInterpolator::VerticalIonoDelayInterpolator(IGPMap * const linkedMap) {
		try
		{
			this->registerIGPMap(linkedMap);
		}
		catch (std::exception& e)
		{
			throw e;
		}
		
	}

	VerticalIonoDelayInterpolator::~VerticalIonoDelayInterpolator(void) {
		Map->deleteInterPolElementLink(this);
	}

	void SlantIonoDelay::setRoverPosition(double lat, double lon, double height) {
	
		this->rlat = lat;
		this->rlon = lon;
		this->rheight = height;

		validRoverPosotion = true;
	}

	void SlantIonoDelay::setazimuthOfSatId(double az, double el) {

		this->azimuthOfSatId = az;
		this->elevationOfSatId = el;
	}

	void SlantIonoDelay::calculatePP(void) {
	
		double centralAngle;
		
		centralAngle = M_PI - this->elevationOfSatId - asin(((Re + this->rheight) / (Re + hI)) * cos(this->elevationOfSatId));

		this->ppLat = asin(sin(this->rlat) * cos(centralAngle) + cos(this->rlat) * sin(centralAngle) * cos(this->azimuthOfSatId));


		if (
			(this->rlat > 70 * M_PI/360 && tan(centralAngle) * cos(this->azimuthOfSatId) > tan(M_PI/2 - this->rlat)) ||
			(this->rlat > -70 * M_PI / 360 && -tan(centralAngle) * cos(this->azimuthOfSatId) > tan(M_PI / 2 - this->rlat))
			) {
			ppLon = this->rlon + M_PI - asin(sin(centralAngle) * sin(this->azimuthOfSatId) / cos(this->ppLat));
		}
		else {
			ppLon = this->rlon + asin(sin(centralAngle) * sin(this->azimuthOfSatId) / cos(this->ppLat));
		}
	}

	double VerticalIonoDelayInterpolator::interpolation4point(	double xpp, double ypp,
																double ionoDelay1,
																double ionoDelay2,
																double ionoDelay3,
																double ionoDelay4) {
		return			xpp			* ypp		* ionoDelay1 +
						(1 - xpp)	* ypp		* ionoDelay2 +
						(1 - xpp)	* (1 - ypp)	* ionoDelay3 +
						xpp			* (1 - ypp)	* ionoDelay4;
	}

	double VerticalIonoDelayInterpolator::interpolation3point(	double xpp, double ypp,
																double ionoDelay1, 
																double ionoDelay2, 
																double ionoDelay3) {
		return			ypp				* ionoDelay1 +
						(1 - xpp - ypp)	* ionoDelay2 +
						xpp				* ionoDelay3;
	}

	void VerticalIonoDelayInterpolator::calculate_xpp_and_ypp(	double &xpp,	double &ypp, 
																double &lat1,	double &lat2, 
																double &lon1,	double &lon2) {

		if (ionoPP.lat >= 85.0 || ionoPP.lat <= -85.0) {
			ypp = (abs(ionoPP.lat) - 85.0) / 10.0;
			xpp = ( (ionoPP.lon - lon1) / 90.0 ) * (1 - 2*ypp) + ypp;
		}
		else {
			xpp = ionoPP.lon - lon1 / (lon2 - lon1);
			ypp = ionoPP.lat - lat1 / (lat2 - lat1);
		}
	}

	void VerticalIonoDelayInterpolator::setPP(IonosphericGridPoint newPP) {
		this->ionoPP = newPP;
	}

	double VerticalIonoDelayInterpolator::gridPointSelectionCriteria(void) {

		double rtv;
		if (abs(this->ionoPP.lat) <= 55.0 ) {
			try
			{
				rtv = this->grid5x5Interpolator();
			}
			catch (exception &e)
			{
				//throw std::domain_error("Interppolation in 5x5 grid is not possible");
				rtv = this->grid10x10Interpolator();
				try
				{
					rtv = this->grid5x5Interpolator();
				}
				catch (exception &e)
				{
					throw std::domain_error("Interppolation in 10x10 grid is not possible");
					
				}
			}
			
		}
		else {
			throw std::domain_error("Unfinished code");
		}

		return rtv;
	}
	
	IonosphericGridPoint VerticalIonoDelayInterpolator::getIGP(double lat, double lon) {

		try
		{
			return this->Map->getIGP(lat, lon);
		}
		catch (std::exception& e)
		{
			throw std::domain_error("Interppolation in 5x5 grid is not possible");
		}
		
	}

	void VerticalIonoDelayInterpolator::registerIGPMap(IGPMap * link2Map) {
		if (link2Map == NULL) {
			throw std::bad_alloc();
		}
		link2Map->interPolList.push_back(this);
		
		this->Map = link2Map;
	}

	int VerticalIonoDelayInterpolator::closestNumberFromLow(int n, int m)
	{
		// find the quotient 
		int q = n / m;

		// 1st possible closest number 
		int n1 = m * q;

		// 2nd possible closest number 
		int n2 = (n * m) > 0 ? (m * (q + 1)) : (m * (q - 1));

		// if true, then n1 is the required closest number 
		if ((n - n1) > (n - n2))
			return n1;

		// else n2 is the required closest number     
		return n2;
	}

	int VerticalIonoDelayInterpolator::closestNumberFromHigh(int n, int m) {
		// find the quotient 
		int q = n / m;

		// 1st possible closest number 
		int n1 = m * q;

		// 2nd possible closest number 
		int n2 = (n * m) > 0 ? (m * (q + 1)) : (m * (q - 1));

		// if true, then n1 is the required closest number 
		if ((n - n1) < (n - n2))
			return n1;

		// else n2 is the required closest number     
		return n2;
	}

	int VerticalIonoDelayInterpolator::secondClosestNumberFromLow(int n, int m)
	{
		// find the quotient 
		int q = n / m;

		// 1st possible closest number 
		int n1 = m * (q - 1);

		// 2nd possible closest number 
		int n2 = (n * m) > 0 ? (m * (q + 2)) : (m * (q - 2));

		// if true, then n1 is the required closest number 
		if ((n - n1) > (n - n2))
			return n1;

		// else n2 is the required closest number     
		return n2;
	
	}

	int VerticalIonoDelayInterpolator::secondClosestNumberFromHigh(int n, int m)
	{
		// find the quotient 
		int q = n / m;

		// 1st possible closest number 
		//int n1 = m * (q - 1);
		int n1 = (n * m) < 0 ? (m * (q + 1)) : (m * (q - 1));

		// 2nd possible closest number 
		int n2 = (n * m) > 0 ? (m * (q + 2)) : (m * (q - 2));

		// if true, then n1 is the required closest number 
		if ((n - n1) < (n - n2))
			return n1;

		// else n2 is the required closest number     
		return n2;

	}

	double VerticalIonoDelayInterpolator::absDistanceOfLongitude(double lon1, double lon2) {

		double result = abs(lon2 - lon1);

		if (result > 180) {
			return 360 - result;
		}
		else {
			return result;
		}

	}

	double VerticalIonoDelayInterpolator::grid5x5Interpolator(void) {
	
		if (this->Map == NULL) {
			throw std::domain_error("Interppolation in 5x5 grid is not possible");
		}

		double corr;
		double gridDistance = 5;
		VerticesOfSquare table;
		getVerticesOf5x5Square(table);

		try
		{
			corr = symmetricInterpolator(gridDistance, table);
		}
		catch (const std::exception&)
		{
			throw std::domain_error("Interppolation in 5x5 grid is not possible");
		}
	}

	double VerticalIonoDelayInterpolator::grid10x10Interpolator(void) {

		if (this->Map == NULL) {
			throw std::domain_error("Interppolation in 10x10 grid is not possible");
		}

		double corr;
		double gridDistance = 10;
		VerticesOfSquare table;

		getVerticesOf10x10Square(table);

		try
		{
			corr = symmetricInterpolator(gridDistance, table);
		}
		catch (const std::exception&)
		{
			throw std::domain_error("Interppolation in 10x10 grid is not possible");
		}



	}

	double VerticalIonoDelayInterpolator::symmetricInterpolator(double gridDistance, VerticesOfSquare table) {

		double numberOfValidIGP = int(table.first.valid) + int(table.second.valid) + int(table.third.valid) + int(table.fourth.valid);

		if (numberOfValidIGP < 3) {
			throw std::domain_error("Interppolation is not possible");
		}

		if (table.first.valid && table.second.valid && table.third.valid && table.fourth.valid) {

			double xpp = absDistanceOfLongitude(ionoPP.lon, table.third.lon) / gridDistance;
			double ypp = abs(ionoPP.lat - table.third.lat) / gridDistance;

			double corr = interpolation4point(xpp, ypp, table.first.getIonoCorr(), table.second.getIonoCorr(), table.third.getIonoCorr(), table.fourth.getIonoCorr());
			return corr;
		}


		if (table.first.valid == false) {
			if (abs(ionoPP.lat - table.third.lat) <= gridDistance - absDistanceOfLongitude(table.third.lon, this->ionoPP.lon)) {

				double xpp = absDistanceOfLongitude(ionoPP.lon, table.third.lon) / gridDistance;
				double ypp = abs(ionoPP.lat - table.third.lat) / gridDistance;

				double corr = interpolation3point(xpp, ypp, table.second.getIonoCorr(), table.third.getIonoCorr(), table.fourth.getIonoCorr());
				return corr;
			}
			else {
				throw std::domain_error("Interppolation in 5x5 grid is not possible");
			}
		}
		else if (table.second.valid == false) {
			if (abs(ionoPP.lat - table.fourth.lat) <= 5.0 - absDistanceOfLongitude(table.fourth.lon, this->ionoPP.lon)) {

				double xpp = absDistanceOfLongitude(ionoPP.lon, table.fourth.lon) / gridDistance;
				double ypp = abs(ionoPP.lat - table.fourth.lat) / gridDistance;

				double corr = interpolation3point(xpp, ypp, table.first.getIonoCorr(), table.fourth.getIonoCorr(), table.third.getIonoCorr());
				return corr;
			}
			else {
				throw std::domain_error("Interppolation in 5x5 grid is not possible");
			}
		}
		else if (table.third.valid == false) {
			if (abs(ionoPP.lat - table.fourth.lat) >= gridDistance - absDistanceOfLongitude(table.second.lon, this->ionoPP.lon)) {

				double xpp = absDistanceOfLongitude(ionoPP.lon, table.first.lon) / gridDistance;
				double ypp = abs(ionoPP.lat - table.first.lat) / gridDistance;

				double corr = interpolation3point(xpp, ypp, table.fourth.getIonoCorr(), table.first.getIonoCorr(), table.second.getIonoCorr());
				return corr;
			}
			else {
				throw std::domain_error("Interppolation in 5x5 grid is not possible");
			}
		}
		else if (table.fourth.valid == false) {
			if (abs(ionoPP.lat - table.third.lat) >= absDistanceOfLongitude(table.third.lon, this->ionoPP.lon)) {

				double xpp = absDistanceOfLongitude(ionoPP.lon, table.second.lon) / gridDistance;
				double ypp = abs(ionoPP.lat - table.second.lat) / gridDistance;

				double corr = interpolation3point(xpp, ypp, table.third.getIonoCorr(), table.second.getIonoCorr(), table.first.getIonoCorr());
				return corr;
			}
			else {
				throw std::domain_error("Interppolation in 5x5 grid is not possible");
			}
		}

		throw std::domain_error("Interppolation is not possible");
		return 0;
	}

	void VerticalIonoDelayInterpolator::getVerticesOf5x5Square(VerticesOfSquare& table) {
	
		double lat1, lat2, lon1, lon2;
		getNearestLatLot(lat1, lat2, lon1, lon2);
		
		IonosphericGridPoint igp1, igp2, igp3, igp4;

		int numberOfValidIGP = 0;

		try
		{
			igp1 = Map->getIGP(lat2, lon2);
			numberOfValidIGP++;
		}
		catch (const std::exception&)
		{
			igp1.valid = false;
		}

		try
		{
			igp2 = Map->getIGP(lat2, lon1);
			numberOfValidIGP++;
		}
		catch (const std::exception&)
		{
			igp2.valid = false;
		}

		try
		{
			igp3 = Map->getIGP(lat1, lon1);
			numberOfValidIGP++;
		}
		catch (const std::exception&)
		{
			igp3.valid = false;
		}
		table.first = igp1;
		try
		{
			igp4 = Map->getIGP(lat1, lon2);
			numberOfValidIGP++;
		}
		catch (const std::exception&)
		{
			igp4.valid = false;
		}


		table.first = igp1;
		table.second = igp2;
		table.third = igp3;
		table.fourth = igp4;
	}

	void VerticalIonoDelayInterpolator::getVerticesOf10x10Square(VerticesOfSquare& table) {

		double gridDistance = 10;
		double numberOfValidIGP = 0;

		double lat1, lat2, lon1, lon2;
		getNearestLatLot(lat1, lat2, lon1, lon2);

		numberOfValidIGP = 0;
		{
			IonosphericGridPoint igp1, igp21, igp33, igp42;

			try
			{
				igp1 = Map->getIGP(lat2, lon2);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp1.valid = false;
			}

			try
			{
				igp21 = Map->getIGP(lat2, restrictLong(lon1 - 5));
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp21.valid = false;
			}

			try
			{
				igp33 = Map->getIGP(lat1 - 5, restrictLong(lon1 - 5));
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp33.valid = false;
			}

			try
			{
				igp42 = Map->getIGP(lat1 - 5, lon2);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp42.valid = false;
			}

			table.first = igp1;
			table.second = igp21;
			table.third = igp33;
			table.fourth = igp42;

			if (numberOfValidIGP == 4) {
				return; 
			}

			if (numberOfValidIGP == 3) {
				if (igp1.valid == false) {
					if (abs(ionoPP.lat - igp33.lat) <= gridDistance - absDistanceOfLongitude(igp33.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp21.valid == false) {
					if (abs(ionoPP.lat - igp42.lat) <= gridDistance - absDistanceOfLongitude(igp42.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp33.valid == false) {
					if (abs(ionoPP.lat - igp42.lat) >= gridDistance - absDistanceOfLongitude(igp21.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp42.valid == false) {
					if (abs(ionoPP.lat - igp33.lat) >= absDistanceOfLongitude(igp33.lon, this->ionoPP.lon)) {
						return;
					}
				}
			}

		}

		numberOfValidIGP = 0;
		{
			IonosphericGridPoint igp11, igp2, igp32, igp43;
		
			try
			{
				igp11 = Map->getIGP(lat2, restrictLong(lon2 + 5));
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp11.valid = false;
			}

			try
			{
				igp2 = Map->getIGP(lat2, lon1);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp2.valid = false;
			}

			try
			{
				igp32 = Map->getIGP(lat1 - 5, lon1);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp32.valid = false;
			}

			try
			{
				igp43 = Map->getIGP(lat1 - 5, restrictLong(lon2 + 5));
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp43.valid = false;
			}

			table.first = igp11;
			table.second = igp2;
			table.third = igp32;
			table.fourth = igp43;

			if (numberOfValidIGP == 4) {
				return;
			}

			if (numberOfValidIGP == 3) {
				if (igp11.valid == false) {
					if (abs(ionoPP.lat - igp32.lat) <= gridDistance - absDistanceOfLongitude(igp32.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp2.valid == false) {
					if (abs(ionoPP.lat - igp43.lat) <= gridDistance - absDistanceOfLongitude(igp43.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp32.valid == false) {
					if (abs(ionoPP.lat - igp43.lat) >= gridDistance - absDistanceOfLongitude(igp2.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp43.valid == false) {
					if (abs(ionoPP.lat - igp32.lat) >= absDistanceOfLongitude(igp32.lon, this->ionoPP.lon)) {
						return;
					}
				}
			}
		}

		numberOfValidIGP = 0;
		{
			IonosphericGridPoint igp13, igp22, igp3, igp41;

			try
			{
				igp13 = Map->getIGP(lat2 + 5, restrictLong(lon2 + 5));
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp13.valid = false;
			}

			try
			{
				igp22 = Map->getIGP(lat2 + 5, lon1);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp22.valid = false;
			}

			try
			{
				igp3 = Map->getIGP(lat1, lon1);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp3.valid = false;
			}

			try
			{
				igp41 = Map->getIGP(lat1, restrictLong(lon2 + 5));
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp41.valid = false;
			}

			table.first = igp13;
			table.second = igp22;
			table.third = igp3;
			table.fourth = igp41;

			if (numberOfValidIGP == 4) {
				return;
			}

			if (numberOfValidIGP == 3) {
				if (igp13.valid == false) {
					if (abs(ionoPP.lat - igp3.lat) <= gridDistance - absDistanceOfLongitude(igp3.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp22.valid == false) {
					if (abs(ionoPP.lat - igp41.lat) <= gridDistance - absDistanceOfLongitude(igp41.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp3.valid == false) {
					if (abs(ionoPP.lat - igp41.lat) >= gridDistance - absDistanceOfLongitude(igp22.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp41.valid == false) {
					if (abs(ionoPP.lat - igp3.lat) >= absDistanceOfLongitude(igp3.lon, this->ionoPP.lon)) {
						return;
					}
				}
			}
		}

		numberOfValidIGP = 0;
		{
			IonosphericGridPoint igp12, igp23, igp31, igp4;

			try
			{
				igp12 = Map->getIGP(lat2 + 5, lon2);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp12.valid = false;
			}

			try
			{
				igp23 = Map->getIGP(lat2 + 5, lon1 + 5 );
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp23.valid = false;
			}

			try
			{
				igp31 = Map->getIGP(lat1, lon1 - 5);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp31.valid = false;
			}

			try
			{
				igp4 = Map->getIGP(lat1, lon2);
				numberOfValidIGP++;
			}
			catch (const std::exception&)
			{
				igp4.valid = false;
			}

			table.first = igp12;
			table.second = igp23;
			table.third = igp31;
			table.fourth = igp4;

			if (numberOfValidIGP == 4) {
				return;
			}

			if (numberOfValidIGP == 3) {
				if (igp12.valid == false) {
					if (abs(ionoPP.lat - igp31.lat) <= gridDistance - absDistanceOfLongitude(igp31.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp23.valid == false) {
					if (abs(ionoPP.lat - igp4.lat) <= gridDistance - absDistanceOfLongitude(igp4.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp31.valid == false) {
					if (abs(ionoPP.lat - igp4.lat) >= gridDistance - absDistanceOfLongitude(igp23.lon, this->ionoPP.lon)) {
						return;
					}
				}
				else if (igp4.valid == false) {
					if (abs(ionoPP.lat - igp31.lat) >= absDistanceOfLongitude(igp31.lon, this->ionoPP.lon)) {
						return;
					}
				}
			}
		}

	}
	
	void VerticalIonoDelayInterpolator::getNearestLatLot(double &lat1, double &lat2, double &lon1, double &lon2) {

		if (ionoPP.lat >= 0.0) {
			lat1 = closestNumberFromLow(ionoPP.lat, 5);
			lat2 = closestNumberFromHigh(ionoPP.lat, 5);
		}
		else {
			lat2 = closestNumberFromLow(ionoPP.lat, 5);
			lat1 = closestNumberFromHigh(ionoPP.lat, 5);
		}

		if (ionoPP.lon >= 0.0) {
			lon1 = closestNumberFromLow(ionoPP.lon, 5);
			lon2 = closestNumberFromHigh(ionoPP.lon, 5);
		}
		else {
			lon2 = closestNumberFromLow(ionoPP.lon, 5);
			lon1 = closestNumberFromHigh(ionoPP.lon, 5);
		}

		restrictLong(&lon1);
		restrictLong(&lon2);
	}

	void VerticalIonoDelayInterpolator::restrictLong(double *indegree) {

		restrictLonginDegree(*indegree);
	}

	double VerticalIonoDelayInterpolator::restrictLong(double indegree) {

		restrictLonginDegree(indegree);
		return indegree;
	}

	void VerticalIonoDelayInterpolator::restrictLonginDegree(double &indegree) {

		if (indegree >= 180) {
			indegree = fmod(indegree, 180);
			indegree -= 180;
			return;
		}

		if (indegree < -180) {
			indegree = fmod(indegree, 180);
			return;
		}
	}

};