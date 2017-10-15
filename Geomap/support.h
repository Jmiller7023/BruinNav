#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED
#include "provided.h"
//allow comparisons for Geocoords
bool operator<(const GeoCoord& lhs, const GeoCoord& rhs);
bool operator==(const GeoCoord& lhs, const GeoCoord& rhs);
bool operator!=(const GeoCoord& lhs, const GeoCoord& rhs);
#endif //SUPPORT_INLCUDED
