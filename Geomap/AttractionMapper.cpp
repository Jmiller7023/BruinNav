#include "provided.h"
#include "MyMap.h"
#include <cctype>
#include <string>
using namespace std;



class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> m_map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
	//we have nothing to initialize.
}

AttractionMapperImpl::~AttractionMapperImpl()
{
	//we have no dynamic allocations, so do nothing.
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	string name;
	StreetSegment seg;
	for (size_t i = 0; i < ml.getNumSegments(); i++) 
		if(ml.getSegment(i, seg))	{
			//convert to lowercase to meet case insensitivity
			for (size_t k = 0; k < seg.attractions.size(); k++){
				name = seg.attractions[k].name;
				for (size_t j = 0; j < name.size(); j++)
					name[j] = tolower(name[j]);
				m_map.associate(name, seg.attractions[k].geocoordinates);
			}
		}
	
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	//convert to lowercase to meet case insensitivity
	for (int j = 0; j < attraction.size(); j++)
		attraction[j] = tolower(attraction[j]);

	const GeoCoord* p = m_map.find(attraction);
	if (p != nullptr) { //if it was found
		gc = *p;
		return true;
	}

	return false;  // wasn't found
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
