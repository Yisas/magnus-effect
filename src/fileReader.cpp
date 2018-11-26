#include "fileReader.h"
#include <iostream>
#include <fstream>

fileReader::fileReader()
{
}


fileReader::~fileReader()
{
}

void fileReader::nextPosition(string line, std::size_t &previous, std::size_t &current)
{
	current = line.find(",", previous);
	previous = current + 1;
	current = line.find(",", previous);
}

bool fileReader::loadFile()
{
	ifstream file(fileAddress);
	if (file.is_open()) {
		//int numOfReadKeyframes = 0;
		//std::vector<Quaternion> readQuaternions;	// Store local quaternions for this set of joints
		string line;

		while (getline(file, line))
		{
			DataEntry dataEntry;

			std::size_t current = 0, previous = 0;

			// Skip the first entry since it's just the title
			nextPosition(line, previous, current);

			dataEntry.initialHeight = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			dataEntry.initialAngle = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			dataEntry.initialVelocity = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			dataEntry.rotVelocity = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);

			if (line.substr(previous, current - previous) == "Back Spin")
				dataEntry.spinType = SpinType::BackSpin;
			else if (line.substr(previous, current - previous) == "Forward Spin")
				dataEntry.spinType = SpinType::Topspin;
			else if (line.substr(previous, current - previous) == "N/A")
				dataEntry.spinType = SpinType::None;
			nextPosition(line, previous, current);

			dataEntry.peakHeight = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			dataEntry.horizontalDisplacement = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);

			numberOfReadLines++;
		}

		file.close();
	}
	else {
		cout << "Could not open file!\n";
		return false;
	}

	return true;
}
