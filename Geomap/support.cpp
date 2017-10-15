#include "provided.h"
#include "support.h"

bool operator<(const GeoCoord& lhs, const GeoCoord& rhs) {
	//compare latitudes first to get more balance in tree.
	if (lhs.latitude != rhs.latitude)
		return lhs.latitude < rhs.latitude;

	//if latitudes were the same compare based on longitude.
	return  lhs.longitude < rhs.longitude;
}

bool operator==(const GeoCoord& lhs, const GeoCoord& rhs) {
	//no need to check string versions since the double versions are calculated by those.
	return lhs.latitude == rhs.latitude && lhs.longitude == rhs.longitude; 
}

bool operator!=(const GeoCoord& lhs, const GeoCoord& rhs) {
	return !(lhs == rhs);
}
 
