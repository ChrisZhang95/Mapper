#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "m1.h"
#include "StreetData.h"
#include <algorithm>
#include <limits>
#include "Dijkstra.h"

StreetData *streetData;
//load the map

bool load_map(std::string mapName) {
    // Move loadStreetsDatabaseBIN into singleton
    bool load_success = loadStreetsDatabaseBIN(mapName);
    // change the filename to be "/blah/blah/CITY.streets.bin" to "/blah/blah/CITY.osm.bin"
    const std::string streetsBin = "streets.bin";
    std::string osmFileName = mapName;
    osmFileName.replace(osmFileName.end()-streetsBin.length(),osmFileName.end(),"osm.bin");
    loadOSMDatabaseBIN(osmFileName);
    //prevent creation of street data for empty map
    if (load_success) {
        streetData = new StreetData();
        initializeNodes(StreetData::getObjectPtr()->streetSegsByIntersectionID);
    }
    // create any data structures here to speed up your API functions
    // ...
    return load_success;
}




//close the map
void close_map() {
    closeStreetDatabase();
    closeOSMDatabase();
    if (StreetData::exists()) {
        delete streetData;
    }
    // destroy/clear any data structures you created in load_map
}



//function to return street id(s) for a street name
//return a 0-length vector if no street with this name exists.
std::vector<unsigned> find_street_ids_from_name(std::string streetName) {
    return StreetData::getObjectPtr()->streetIDsByName[streetName];
}


//function to return the street segments for a given intersection 
std::vector<unsigned> find_intersection_street_segments(unsigned intersectionId) {
    return StreetData::getObjectPtr()->streetSegsByIntersectionID[intersectionId];
}



//function to return street names at an intersection (include duplicate street names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersectionId) {
    return StreetData::getObjectPtr()->streetNamesBlonIntersection[intersectionId];
}




//can you get from intersection1 to intersection2 using a single street segment (check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(unsigned intersectionId1, unsigned intersectionId2) {
    
    //if given same id
    if (intersectionId1 == intersectionId2)
        return true;
    
    //gets street segments for the intersections
    std::vector<unsigned> streetSegmentIDs1 = find_intersection_street_segments(intersectionId1);
    std::vector<unsigned> streetSegmentIDs2 = find_intersection_street_segments(intersectionId2);
    
    std::vector<unsigned> commonStreetSegments;
    
//adds in all the street segments found and put them into the common street segments variable
    std::set_intersection(streetSegmentIDs1.begin(), streetSegmentIDs1.end(),
                          streetSegmentIDs2.begin(), streetSegmentIDs2.end(),
                          std::back_inserter(commonStreetSegments));
    
    //returns true if at least one common street segment
    return commonStreetSegments.size() > 0;
}




//find all intersections reachable by traveling down one street segment 
//the returned vector should NOT contain duplicate intersections
std::vector<unsigned> find_adjacent_intersections(unsigned intersectionId) {
    
    std::vector<unsigned> returnValue;
    std::vector<unsigned> adjacentStreetSegs = find_intersection_street_segments(intersectionId);
    
    //looping through adjacent street segments
    for (unsigned i = 0; i < adjacentStreetSegs.size(); i++) {
        StreetSegmentInfo streetSegmentInfo = getStreetSegmentInfo(adjacentStreetSegs[i]);
       
        
        if (streetSegmentInfo.from == intersectionId) {
            // Check if the value doesn't already exist in the vector for returnValue
            if (std::find(returnValue.begin(), returnValue.end(), streetSegmentInfo.to) == returnValue.end())
                returnValue.push_back(streetSegmentInfo.to);
        } else if (!streetSegmentInfo.oneWay) {
            //if the street segment isn't a one way street, push back the 'from' portion of the street
            
            if (std::find(returnValue.begin(), returnValue.end(), streetSegmentInfo.from) == returnValue.end())
                returnValue.push_back(streetSegmentInfo.from);
        }
    }
    return returnValue;
}




//for a given street, return all the street segments
std::vector<unsigned> find_street_street_segments(unsigned streetId) {
    return StreetData::getObjectPtr()->streetSegsByStreetID[streetId];
}




//for a given street, find all the intersections
std::vector<unsigned> find_all_street_intersections(unsigned streetId) {
    return StreetData::getObjectPtr()->streetIntersectionsByStreetID[streetId];
}




//function to return all intersection ids for two intersecting streets
//but duplicate street names are allowed, so more than 1 intersection id may exist for 2 street names
std::vector<unsigned> find_intersection_ids_from_street_names(std::string streetName1, std::string streetName2) {
    
    std::vector<unsigned> commonIntersecs;
    //getting street intersections from the street name
    std::vector<unsigned> streetIntersecs1 = StreetData::getObjectPtr()->streetIntersectionsByName[streetName1];
    std::vector<unsigned> streetIntersecs2 = StreetData::getObjectPtr()->streetIntersectionsByName[streetName2];

//sorts intersections so that set_intersection can find the common intersections and puts them in commonIntersecs
    std::sort(streetIntersecs1.begin(),streetIntersecs1.end());
    std::sort(streetIntersecs2.begin(),streetIntersecs2.end());
    std::set_intersection(streetIntersecs1.begin(), streetIntersecs1.end(),
                          streetIntersecs2.begin(), streetIntersecs2.end(),
                          std::back_inserter(commonIntersecs));
    
    // Resize commonIntersecs so that only the unique common intersections remain
    std::vector<unsigned>::iterator it = std::unique(commonIntersecs.begin(), commonIntersecs.end());
    commonIntersecs.resize(std::distance(commonIntersecs.begin(),it));
    
    return commonIntersecs;
}





//find distance between two coordinates
double find_distance_between_two_points(LatLon point1, LatLon point2) {

    //get the latitude point formula
    double y = (DEG_TO_RAD) * (point1.lat - point2.lat);
    double lat_avg = (point1.lat + point2.lat) / 2 * DEG_TO_RAD;

    //get longitude point formula
    double x1 = (point1.lon * cos(lat_avg));
    double x2 = (point2.lon * cos(lat_avg));
    double x = (DEG_TO_RAD) * (x2 - x1);

    return (EARTH_RADIUS_IN_METERS * (sqrt(y * y + x * x)));
}





//find the length of a given street segment
double find_street_segment_length(unsigned streetSegmentId) {
   
    double segmentLength = 0;

    // Get street segment info from the helper function
    StreetSegmentInfo streetSegInfo = getStreetSegmentInfo(streetSegmentId);
    // Use that info to get the "from" and "to" intersections
    unsigned fromIntersection = streetSegInfo.from;
    unsigned toIntersection = streetSegInfo.to;

    // Get the locations of those intersection IDs
    LatLon point1 = getIntersectionPosition(fromIntersection);
    LatLon point2 = getIntersectionPosition(toIntersection);

    
    // Loop for curved path segments (if there's no curve points this loop doesn't run)
    for (unsigned i = 0; i < streetSegInfo.curvePointCount; i++) {
        if (i == 0) { // Special case for first - segment length between beginning point and first curve point
            segmentLength += find_distance_between_two_points(point1, getStreetSegmentCurvePoint(streetSegmentId, i));
        } else { // Adds the length between the curve point at i and the curve point at i-1 over all curve points in the street segment
            segmentLength += find_distance_between_two_points(getStreetSegmentCurvePoint(streetSegmentId, (i-1)), getStreetSegmentCurvePoint(streetSegmentId, i));
        }
    }

    //if there are no curve points just get the straight line distance between two points
    if (streetSegInfo.curvePointCount == 0) {
        return find_distance_between_two_points(point1, point2);
    } else { // Add the very last distance between point2 and the last curve point
        segmentLength += find_distance_between_two_points(getStreetSegmentCurvePoint(streetSegmentId, (streetSegInfo.curvePointCount-1)), point2);
        return segmentLength;
    }
}





//find the length of a whole street
double find_street_length(unsigned streetId) {
    double length = 0;
    
    std::vector<unsigned> streetStreetSegments = find_street_street_segments(streetId);
    
    for (unsigned i = 0; i < streetStreetSegments.size(); i++) {
        //for each segment of the street add its length to the length variable
        length += find_street_segment_length(streetStreetSegments[i]);
    }
    
    return length;
}




//find the travel time to drive a street segment (time(minutes) = distance(km)/speed_limit(km/hr) * 60
double find_street_segment_travel_time(unsigned streetSegmentId) {

    //convert to km to use in formula above
    double distanceStreetSegment = (find_street_segment_length(streetSegmentId)) / 1000;

    //access the speed limit of a given segment
    StreetSegmentInfo streetSegInfo = getStreetSegmentInfo(streetSegmentId);
    double intersectionSpeed = streetSegInfo.speedLimit;

    //in minutes, distance in km, speed limit in km/h
    double time = distanceStreetSegment / intersectionSpeed * 60;
    return time;
}




//find the nearest point of interest to a given position
unsigned find_closest_point_of_interest(LatLon myPosition) {
    
    //the length between a min and max of the entire map
    double totalXDiff = StreetData::getObjectPtr()->xPOIMax-StreetData::getObjectPtr()->xPOIMin;
    double totalYDiff = StreetData::getObjectPtr()->yPOIMax-StreetData::getObjectPtr()->yPOIMin;
    //left side of the map to a certain x/y distance - used to get fraction when grouped
    double xDistFromMin = myPosition.lat-StreetData::getObjectPtr()->xPOIMin;
    double yDistFromMin = myPosition.lon-StreetData::getObjectPtr()->yPOIMin;
    
    //all the POIs are grouped to a common "fake" point
    int groupedPOIXIndex = round(CELL_DIVISION_AMOUNT*(xDistFromMin)/(totalXDiff));
    if (groupedPOIXIndex < 0) groupedPOIXIndex = 0;
    if (groupedPOIXIndex > CELL_DIVISION_AMOUNT) groupedPOIXIndex = CELL_DIVISION_AMOUNT;
    
    int groupedPOIYIndex = round(CELL_DIVISION_AMOUNT*(yDistFromMin)/(totalYDiff));
    if (groupedPOIYIndex < 0) groupedPOIYIndex = 0;
    if (groupedPOIYIndex > CELL_DIVISION_AMOUNT) groupedPOIYIndex = CELL_DIVISION_AMOUNT;
    
    // sets distance to largest value a double can hold to ensure the if statement in the for loop runs
    double distance = std::numeric_limits<double>::max();
    unsigned ID = 0;
    
    //finds the lowest distance between my position and all the grouped POI for the specific indices
    for (unsigned i=0;i<StreetData::getObjectPtr()->groupedPOIs[groupedPOIXIndex][groupedPOIYIndex].size();i++) {
        
       if (find_distance_between_two_points(myPosition,getPointOfInterestPosition(StreetData::getObjectPtr()->groupedPOIs[groupedPOIXIndex][groupedPOIYIndex][i])) < distance) {
           distance = find_distance_between_two_points(myPosition,getPointOfInterestPosition(StreetData::getObjectPtr()->groupedPOIs[groupedPOIXIndex][groupedPOIYIndex][i]));
           ID = StreetData::getObjectPtr()->groupedPOIs[groupedPOIXIndex][groupedPOIYIndex][i];
       }
    }
    
    return ID;
}




//find the nearest intersection (by ID) to a given position
unsigned find_closest_intersection(LatLon myPosition) { 
    
    //the length between a min and max of the entire map
    double totalXDiff = StreetData::getObjectPtr()->latIntersectionMax-StreetData::getObjectPtr()->latIntersectionMin;
    double totalYDiff = StreetData::getObjectPtr()->lonIntersectionMax-StreetData::getObjectPtr()->lonIntersectionMin;
    
    //left side of the map to a certain x/y distance - used to get fraction when grouped
    double xDistFromMin = myPosition.lat-StreetData::getObjectPtr()->latIntersectionMin;
    double yDistFromMin = myPosition.lon-StreetData::getObjectPtr()->lonIntersectionMin;
    
    
    //all the intersections are grouped to a common "fake" intersection
    int groupedIntersecsXIndx = round(CELL_DIVISION_AMOUNT*(xDistFromMin)/(totalXDiff));
    if (groupedIntersecsXIndx < 0) groupedIntersecsXIndx = 0;
    if (groupedIntersecsXIndx > CELL_DIVISION_AMOUNT) groupedIntersecsXIndx = CELL_DIVISION_AMOUNT;
    
    int groupedIntersecsYIndx = round(CELL_DIVISION_AMOUNT*(yDistFromMin)/(totalYDiff));
    if (groupedIntersecsYIndx < 0) groupedIntersecsYIndx = 0;
    if (groupedIntersecsYIndx > CELL_DIVISION_AMOUNT) groupedIntersecsYIndx = CELL_DIVISION_AMOUNT;
    
    // sets distance to largest value a double can hold to ensure the if statement in the for loop runs
    double distance = std::numeric_limits<double>::max();
    unsigned ID = 0;
    
    //finds the lowest distance between my position and all the grouped POI for the specific indices
    for (unsigned i = 0; i < StreetData::getObjectPtr()->groupedIntersections[groupedIntersecsXIndx][groupedIntersecsYIndx].size(); i++) {
        
       if (find_distance_between_two_points(myPosition, getIntersectionPosition(StreetData::getObjectPtr()->groupedIntersections[groupedIntersecsXIndx][groupedIntersecsYIndx][i])) < distance) {
           distance = find_distance_between_two_points(myPosition, getIntersectionPosition(StreetData::getObjectPtr()->groupedIntersections[groupedIntersecsXIndx][groupedIntersecsYIndx][i]));
           ID = StreetData::getObjectPtr()->groupedIntersections[groupedIntersecsXIndx][groupedIntersecsYIndx][i];
       }
  
    }
    
    return ID;
}
