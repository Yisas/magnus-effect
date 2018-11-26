#pragma once
#include <string>
using namespace std;

/**
* Parser for the csv model files
**/
class fileReader
{
	enum SpinType { BackSpin, Topspin, None };

	struct DataEntry
	{
		float initialHeight;
		float initialAngle;
		float initialVelocity;
		float rotVelocity;
		SpinType spinType;
		float peakHeight;
		float horizontalDisplacement;
	};

public:
	fileReader();
	~fileReader();
	
private:
	const string fileAddress = "Data.txt";
	int numberOfReadLines = 0;

	bool loadFile();
	void nextPosition(string line, std::size_t &previous, std::size_t &current);
};

