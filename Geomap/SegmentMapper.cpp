#include "provided.h"
#include "MyMap.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<GeoCoord, vector<StreetSegment>> m_map;
};

SegmentMapperImpl::SegmentMapperImpl()
{
	//we have nothing to initialize.
}

SegmentMapperImpl::~SegmentMapperImpl()
{
	//we have no dynamic allocations, so do nothing.
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	StreetSegment seg;
	vector<StreetSegment> ssv;
	for (int i = 0; i < ml.getNumSegments(); i++){
		ml.getSegment(i, seg);
		//get start coords associated
		ssv = getSegments(seg.segment.start);
		ssv.push_back(seg);
		m_map.associate(seg.segment.start, ssv);

		//get end coords associated
		ssv = getSegments(seg.segment.end);
		ssv.push_back(seg);
		m_map.associate(seg.segment.end, ssv);

		//associate on each of the attractions
		for (size_t k = 0; k < seg.attractions.size(); k++) {
			ssv = getSegments(seg.attractions[k].geocoordinates);
			ssv.push_back(seg);
			m_map.associate(seg.attractions[k].geocoordinates, ssv);
		}	
	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	vector<StreetSegment> segments;
	const vector<StreetSegment>* p = m_map.find(gc);
	if (p != nullptr) { //if it was found
		segments = *p;
	}
	return segments;  
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
