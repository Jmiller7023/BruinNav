#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment> m_segments;
};

MapLoaderImpl::MapLoaderImpl()
{
	//we have nothing to initialize.
}

MapLoaderImpl::~MapLoaderImpl()
{
	//we have no dynamic allocations, so do nothing.
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);    // infile is a name of our choosing
	if (!infile)		        // Did opening the file fail?
	{
		cerr << "Error: Cannot open mapFile!" << endl;
		return false;
	}

	StreetSegment tempstreet;
	Attraction tempattract;
	string line;
	int num;
	string latitude, longitude, name, coords;
	while (getline(infile, line))
	{
		//assign streetname 
		tempstreet.streetName = line;

		//get a string for starting and ending coordinates.
		getline(infile, line);

		//get the latitude then remove starting latitude info from the string.
		latitude = line.substr(0, line.find_first_of(','));
		coords = line.substr(line.find_first_of(',') + 1);
		
		if (coords[0] == ' ')
			coords = coords.substr(1); //remove the space.

		//get the longitude, assign Geocoord, remove starting longitude info from string
		longitude = coords.substr(0, coords.find_first_of(' ')); 
		GeoCoord startcoord(latitude, longitude);
		coords = coords.substr(coords.find_first_of(' ') + 1);
		
		//get the latitude then remove ending latitude info from the string.
		latitude = coords.substr(0, line.find_first_of(','));
		coords = coords.substr(line.find_first_of(',') + 1);

		if (coords[0] == ' ')
			coords = coords.substr(1); //remove the space.

		//all thats left is ending longitude.
		longitude = coords;
		GeoCoord endcoord(latitude, longitude);
		
		//assign Geosegment
		GeoSegment tempseg(startcoord, endcoord);
		tempstreet.segment = tempseg;

		//get and pushback number of attractions.
		getline(infile, line);
		num = stoi(line);
		
		vector<Attraction> attractionvector;
		for (int i = 0; i < num; i++) {
			//get this attraction
			getline(infile, line);

			//assign the name
			tempattract.name = line.substr(0, line.find('|'));

			//find and set its Geocoord
			coords = line.substr(line.find('|') + 1); //create a substring with all characters after the |.
			latitude = coords.substr(0, coords.find(',')); 
			coords = coords.substr(coords.find(',') + 1);
			if (coords[0] == ' ')
				coords = coords.substr(1); //remove the space.
			longitude = coords;
			GeoCoord coord(latitude, longitude);
			tempattract.geocoordinates = coord;
			
			//pushback this attraction in attractions vector
			attractionvector.push_back(tempattract);
		}

		//set the vector of attractions.
		tempstreet.attractions = attractionvector;

		//pushback this streetsegment
		m_segments.push_back(tempstreet);
	}

	return true;  
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_segments.size();   
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if(segNum < 0 || segNum > m_segments.size() - 1) // must be within bounds
		return false;  
	seg = m_segments[segNum];
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
