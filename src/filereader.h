#pragma once
#include <string>
#include <vector>
using namespace std;

/**
* Parser for the csv model files
**/
class FileReader
{
public:
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

	FileReader();
	~FileReader();

	vector<DataEntry> getReadDataEntries() { return readDataEntries; }

private:
	const string fileAddress = "data.csv";
	int numberOfReadLines = 0;
	vector<DataEntry> readDataEntries;

	bool loadFile();
	void nextPosition(string line, std::size_t &previous, std::size_t &current);
};

