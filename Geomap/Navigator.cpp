#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <string>
#include <queue>
#include <vector>
#include <list>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	SegmentMapper m_segmap;
	AttractionMapper m_atmap;
	void getdirections(MyMap<GeoCoord, GeoCoord> &waypoints, vector<NavSegment> &directions, GeoCoord endcoords, GeoCoord startcoords) const;
	string facing(double degrees) const;
	vector<NavSegment> reverse(vector<NavSegment> backwards) const;
};

NavigatorImpl::NavigatorImpl()
{
	//we have nothing to initialize.
}

NavigatorImpl::~NavigatorImpl()
{
	//we have no dynamic allocations, so do nothing.
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	MapLoader maploader;
	if (maploader.load(mapFile)) {
		m_segmap.init(maploader);
		m_atmap.init(maploader);
		return true;
	}
	return false;  
}
string NavigatorImpl::facing(double degrees) const {
	if (degrees <= 22.5)
		return "east";
	else if (degrees <= 67.5)
		return "northeast";
	else if (degrees <= 112.5)
		return "north";
	else if (degrees <= 157.5)
		return "northwest";
	else if (degrees <= 202.5)
		return "west";
	else if (degrees <= 247.5)
		return "southwest";
	else if (degrees <= 292.5)
		return "south";
	else if (degrees <= 337.5)
		return "southeast";
	else // <360
		return "east";
}

vector<NavSegment> NavigatorImpl::reverse(vector<NavSegment> backwards) const {
	vector<NavSegment> forwards;
	for (int i = backwards.size() - 1; i > -1; i--)
		forwards.push_back(backwards[i]);
	return forwards;
}

void NavigatorImpl::getdirections(MyMap<GeoCoord, GeoCoord> &waypoints, vector<NavSegment> &directions, GeoCoord endcoords, GeoCoord startcoords) const{
	//start coorda as endcoords so it can be assigned properly in while loop.
	GeoCoord coordb; GeoCoord coorda = endcoords;
	vector<StreetSegment> ss = m_segmap.getSegments(coorda);
	vector<NavSegment> temp;
	NavSegment lastsegment;
	string currentstreetname;
	string laststreetname;
 
	while(coorda != startcoords){
		//set b to the start of the last street and set a to the start of the new street.
		coordb = coorda;
		coorda = *waypoints.find(coordb);
		GeoSegment segment(coorda, coordb);

		//search the street segments for the segment we created. This gives the streetname index.
		ss = m_segmap.getSegments(coorda);

		//check the attractions for startcoords to get streetname
		if (coorda == startcoords) {
			for (size_t i = 0; i < ss.size(); i++) {
				for (size_t j = 0; j < ss[i].attractions.size(); j++)
					if (ss[i].attractions[j].geocoordinates == coorda){
						currentstreetname = ss[i].streetName;
						break;
					}
			}
		}
		//check the attractions for endcoords to get streetname
		else if (coordb == endcoords) {
			for (size_t i = 0; i < ss.size(); i++) {
				for (size_t i = 0; i < ss.size(); i++) {
					for (size_t j = 0; j < ss[i].attractions.size(); j++)
						if (ss[i].attractions[j].geocoordinates == coordb){
							currentstreetname = ss[i].streetName;
							break;
						}
				}
			}
		}
		else{
			//find the street segment that matches the one we made. 
			//note: the street segment we made could be going the opposite direction of the one in m_segmap.
			for (size_t i = 0; i < ss.size(); i++) {
				if ((ss[i].segment.start == segment.start && ss[i].segment.end == segment.end) || (ss[i].segment.start == segment.end && ss[i].segment.end == segment.start)){
					currentstreetname = ss[i].streetName;
					break;
				}
			}
		}

		//check if we turned onto a new street.
		if (currentstreetname != laststreetname && coordb != endcoords) {
			if(angleBetween2Lines(segment, lastsegment.m_geoSegment) < 180)
				temp.push_back(NavSegment("left", laststreetname));
			else
				temp.push_back(NavSegment("right", laststreetname));
		}
 
		lastsegment.m_geoSegment = segment;
		laststreetname = currentstreetname;
		temp.push_back(NavSegment(facing(angleOfLine(segment)), currentstreetname, distanceEarthMiles(coordb,coorda), segment));
	}

	//we made the vector backwards so reverse it.
	directions = reverse(temp);
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{


	//check if the start and ending coordinates exist.
	GeoCoord startcoord;
	if (!m_atmap.getGeoCoord(start, startcoord))
		return NAV_BAD_SOURCE;
		
	GeoCoord endcoord;
	if (!m_atmap.getGeoCoord(end, endcoord))
		return NAV_BAD_DESTINATION;
	
	//do the path searching
	MyMap<GeoCoord, GeoCoord> waypoints;
	GeoCoord currentcoord;
	vector<StreetSegment> segmentat;
	queue<GeoCoord> travelpath;
	travelpath.push(startcoord);

	while (!travelpath.empty()) {
		currentcoord = travelpath.front();
		travelpath.pop();

		segmentat = m_segmap.getSegments(currentcoord);

		for (size_t i = 0; i < segmentat.size(); i++) {
			//see if we can associate on segment start.
			if (waypoints.find(segmentat[i].segment.start) == nullptr) {
					travelpath.push(segmentat[i].segment.start);
					waypoints.associate(segmentat[i].segment.start, currentcoord);
			}

			//see if we can associate on segment end.
			if (waypoints.find(segmentat[i].segment.end) == nullptr) {	
					travelpath.push(segmentat[i].segment.end);
					waypoints.associate(segmentat[i].segment.end, currentcoord);
			}

			//check all attraction geocoords to see if we hit destination.
			for (int j = 0; j != segmentat[i].attractions.size(); j++) {
				if(segmentat[i].attractions[j].geocoordinates == endcoord){
					 waypoints.associate(segmentat[i].attractions[j].geocoordinates, currentcoord);
					 getdirections(waypoints, directions, endcoord, startcoord); //note: only executes once, so add when taking into account big O.
					 return NAV_SUCCESS;
					}
			}	
		}
	} 

	//went through without finding the path.
	return NAV_NO_ROUTE;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
