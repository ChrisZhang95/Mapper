#include "StreetData.h"
StreetData* StreetData::objectPtr = NULL; // Necessary for StreetData to function (https://piazza.com/class/iiatrubidpk507?cid=268)

// Again these are ALL required to be redeclared here to solve this issue: http://forums.devshed.com/programming-42/linker-errors-undefined-reference-static-data-193010.html
std::unordered_map< std::string, std::vector<unsigned> > StreetData::streetIDsByName;
std::unordered_map< std::string, std::vector<unsigned> > StreetData::streetIntersectionsByName;
std::unordered_map< std::string, std::vector<unsigned> > StreetData::POIByName;
std::vector<std::vector<unsigned> > StreetData::streetSegsByIntersectionID;
std::vector<std::vector<std::string> > StreetData::streetNamesBlonIntersection;
std::vector<std::vector<unsigned> > StreetData::streetSegsByStreetID;
std::vector<std::vector<unsigned> > StreetData::streetIntersectionsByStreetID;
std::vector<IntersectionNode> StreetData::intersectionNodeByIntersection;
std::vector<std::vector<std::vector<unsigned> >> StreetData::groupedIntersections;
double StreetData::latIntersectionMin;
double StreetData::latIntersectionMax;
double StreetData::lonIntersectionMin;
double StreetData::lonIntersectionMax;
std::vector<std::vector<std::vector<unsigned> >> StreetData::groupedPOIs;
double StreetData::xPOIMin;
double StreetData::xPOIMax;
double StreetData::yPOIMin;
double StreetData::yPOIMax;
bool StreetData::zoom_fitt = false;//check if intersection is zoomfit already
bool StreetData::clear = false;//check if input is cleared already
double StreetData::zoomLevel;
bool StreetData::loadPressed;
std::vector<unsigned> StreetData::highlightedStreetIDs;
    
// For the global lookup variables they need to be sized correctly before any data is put into them
void StreetData::initializeVectors() {
    streetSegsByIntersectionID.resize(getNumberOfIntersections());
    streetNamesBlonIntersection.resize(getNumberOfIntersections());
    streetSegsByStreetID.resize(getNumberOfStreets());
    streetIntersectionsByStreetID.resize(getNumberOfStreets());
    intersectionNodeByIntersection.resize(getNumberOfIntersections());
    for (unsigned i=0;i<getNumberOfIntersections();i++) {
        intersectionNodeByIntersection[i].intersectionID = i;
    }
    groupedIntersections.resize(CELL_DIVISION_AMOUNT+1);
    for (unsigned i=0;i<=CELL_DIVISION_AMOUNT;i++) {
        groupedIntersections[i].resize(CELL_DIVISION_AMOUNT+1);
    }
    groupedPOIs.resize(CELL_DIVISION_AMOUNT+1);
    for (unsigned i=0;i<=CELL_DIVISION_AMOUNT;i++) {
        groupedPOIs[i].resize(CELL_DIVISION_AMOUNT+1);
    }
    
    for (unsigned i=0;i<getNumberOfPointsOfInterest();i++) {
        POIByName[getPointOfInterestName(i)].push_back(i);
    }
}

// Fill in the streetIDsByName so that we can map street names to IDs
void StreetData::initializeStreetIDsByName() {
    for (unsigned i = 0; i < getNumberOfStreets(); i++) {
        streetIDsByName[getStreetName(i)].push_back(i);
    }
}

// Fill in the streetSegsByIntersectionIDs so that we can map street segments to intersection IDs
// Fill in the streetIntersectionsByStreetID so that we can map street IDs to intersection IDs
void StreetData::initializeStreetSegmentData() {
    for (unsigned i=0;i<getNumberOfStreetSegments();i++) {
        streetSegsByIntersectionID[getStreetSegmentInfo(i).from].push_back(i);
        streetSegsByIntersectionID[getStreetSegmentInfo(i).to].push_back(i);
        streetSegsByStreetID[getStreetSegmentInfo(i).streetID].push_back(i);
        streetIntersectionsByStreetID[getStreetSegmentInfo(i).streetID].push_back(getStreetSegmentInfo(i).to);
        streetIntersectionsByStreetID[getStreetSegmentInfo(i).streetID].push_back(getStreetSegmentInfo(i).from);
    }
}

// Make streetIntersectionsByStreetID contain unique data
void StreetData::initializeStreetsData() {
    for (unsigned i=0;i<getNumberOfStreets();i++) {
        // This function relies on initializeStreetSegmentData() being called first
        std::sort(streetIntersectionsByStreetID[i].begin(),streetIntersectionsByStreetID[i].end());
        std::vector<unsigned>::iterator it = std::unique(streetIntersectionsByStreetID[i].begin(),streetIntersectionsByStreetID[i].end());
        streetIntersectionsByStreetID[i].resize(std::distance(streetIntersectionsByStreetID[i].begin(),it));
    }
}

// Find the largest and smallest latitude/longitude values for a map
void StreetData::initializeIntersectionMinMax() {
    latIntersectionMin = getIntersectionPosition(0).lat;
    latIntersectionMax = getIntersectionPosition(0).lat;
    lonIntersectionMin = getIntersectionPosition(0).lon;
    lonIntersectionMax = getIntersectionPosition(0).lon;
    for (unsigned i=0;i<getNumberOfIntersections();i++) {
        std::vector<std::string> streetNames;
        for (unsigned j=0;j<getIntersectionStreetSegmentCount(i);j++) {
            std::string name = getStreetName(getStreetSegmentInfo(streetSegsByIntersectionID[i][j]).streetID);
            streetNames.push_back(name);
            streetIntersectionsByName[name].push_back(i);
        }
        streetNamesBlonIntersection[i] = streetNames;
        if (getIntersectionPosition(i).lat < latIntersectionMin) {
            latIntersectionMin = getIntersectionPosition(i).lat;
        }
        if (getIntersectionPosition(i).lat > latIntersectionMax) {
            latIntersectionMax = getIntersectionPosition(i).lat;
        }
        if (getIntersectionPosition(i).lon < lonIntersectionMin) {
            lonIntersectionMin = getIntersectionPosition(i).lon;
        }
        if (getIntersectionPosition(i).lon > lonIntersectionMax) {
            lonIntersectionMax = getIntersectionPosition(i).lon;
        }
    }
}

// Go through all the intersections and send each "real" intersection to its
// closest "fake" intersection. The point of these "fake" intersections is so that
// when searching for the closest intersection you only need to search the 
// intersections in the closest "fake" intersection.
void StreetData::initializeIntersectionGroups() {
    for (unsigned i=0;i<getNumberOfIntersections();i++) {
        double xPos = getIntersectionPosition(i).lat;
        double yPos = getIntersectionPosition(i).lon;
        int groupedIntersectionsXIndex = (int)round(CELL_DIVISION_AMOUNT*(xPos-latIntersectionMin)/(latIntersectionMax-latIntersectionMin));
        int groupedIntersectionsYIndex = (int)round(CELL_DIVISION_AMOUNT*(yPos-lonIntersectionMin)/(lonIntersectionMax-lonIntersectionMin));
        groupedIntersections[groupedIntersectionsXIndex][groupedIntersectionsYIndex].push_back(i);
    }
}

// Find the largest and smallest latitude/longitude values for a map
void StreetData::initializePOIMinMax() {
    xPOIMin = getPointOfInterestPosition(0).lat;
    xPOIMax = getPointOfInterestPosition(0).lat;
    yPOIMin = getPointOfInterestPosition(0).lon;
    yPOIMax = getPointOfInterestPosition(0).lon;
    for (unsigned i=0;i<getNumberOfPointsOfInterest();i++) {
        if (getPointOfInterestPosition(i).lat < xPOIMin) {
            xPOIMin = getPointOfInterestPosition(i).lat;
        }
        if (getPointOfInterestPosition(i).lat > xPOIMax) {
            xPOIMax = getPointOfInterestPosition(i).lat;
        }
        if (getPointOfInterestPosition(i).lon < yPOIMin) {
            yPOIMin = getPointOfInterestPosition(i).lon;
        }
        if (getPointOfInterestPosition(i).lon > yPOIMax) {
            yPOIMax = getPointOfInterestPosition(i).lon;
        }
    }
}

// Go through all the POIs and send each "real" POI to its
// closest "fake" POI. The point of these "fake" POIs is so that
// when searching for the closest POI you only need to search the 
// POIs in the closest "fake" POI.
void StreetData::initializePOIGroups() {
    for (unsigned i=0;i<getNumberOfPointsOfInterest();i++) {
        double xPos = getPointOfInterestPosition(i).lat;
        double yPos = getPointOfInterestPosition(i).lon;
        int groupedPOIXIndex = (int)round(CELL_DIVISION_AMOUNT*(xPos-xPOIMin)/(xPOIMax-xPOIMin));
        int groupedPOIYIndex = (int)round(CELL_DIVISION_AMOUNT*(yPos-yPOIMin)/(yPOIMax-yPOIMin));
        groupedPOIs[groupedPOIXIndex][groupedPOIYIndex].push_back(i);
    }
}
