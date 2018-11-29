#include "fileReader.h"
#include <iostream>
#include <fstream>

fileReader::fileReader()
{
	loadFile();
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
		string line;

		// Ignore first 2 lines
		getline(file, line);
		getline(file, line);

		while (getline(file, line))
		{
			DataEntry dataEntry;

			std::size_t current = 0, previous = 0;

			// Skip the first entry since it's just the title
			nextPosition(line, previous, current);

			if (line.substr(previous, current - previous) == "")
				continue;

			dataEntry.initialHeight = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			dataEntry.initialAngle = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			dataEntry.initialVelocity = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);

			// Use rev/sec angular velocity, skip rest
			dataEntry.rotVelocity = stof(line.substr(previous, current - previous));
			nextPosition(line, previous, current);
			nextPosition(line, previous, current);
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

			readDataEntries.push_back(dataEntry);
		}

		file.close();
	}
	else {
		cout << "Could not open file!\n";
		return false;
	}

	return true;
}
