#pragma once

#include "StreetsDatabaseAPI.h"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "IntersectionNode.h"

#define CELL_DIVISION_AMOUNT 1


class StreetData {
private:
    static StreetData *objectPtr;
    static void initializeVectors();
    static void initializeStreetIDsByName();
    static void initializeStreetSegmentData();
    static void initializeStreetsData();
    static void initializeIntersectionMinMax();
    static void initializeIntersectionGroups();
    static void initializePOIMinMax();
    static void initializePOIGroups();
public:
    // The global variables that we can look up in the rest of the code
    static std::unordered_map< std::string, std::vector<unsigned> > streetIDsByName; // Contains the streetIDs mapped by street name
    static std::unordered_map< std::string, std::vector<unsigned> > streetIntersectionsByName; // Contains the streetIDs mapped by street name
    static std::unordered_map< std::string, std::vector<unsigned> > POIByName;
    static std::vector<std::vector<unsigned> > streetSegsByIntersectionID; // Contains the street segments mapped by intersection ID
    static std::vector<std::vector<std::string> > streetNamesBlonIntersection; // Contains the street segments mapped by intersection ID
    static std::vector<std::vector<unsigned> > streetSegsByStreetID; // Contains the street segments mapped by street ID
    static std::vector<std::vector<unsigned> >streetIntersectionsByStreetID; // Contains the street intersections mapped by street ID
    static std::vector<IntersectionNode> intersectionNodeByIntersection;
    static std::vector<std::vector<std::vector<unsigned> >>groupedIntersections; // Contains a 2D vector containing a vector of intersection IDs that are closest to this "fake" intersection
    static double latIntersectionMin; // The lowest latitude value of an intersection for a given map
    static double latIntersectionMax; // The biggest latitude value of an intersection for a given map
    static double lonIntersectionMin; // The lowest longitude value of an intersection for a given map
    static double lonIntersectionMax; // The biggest longitude value of an intersection for a given map
    static std::vector<std::vector<std::vector<unsigned> >>groupedPOIs; // Contains a 2D vector containing a vector of POI IDs that are closest to this "fake" POI
    static double xPOIMin; // The lowest latitude value of a POI for a given map
    static double xPOIMax; // The biggest latitude value of a POI for a given map
    static double yPOIMin; // The lowest longitude value of a POI for a given map
    static double yPOIMax; // The biggest longitude value of a POI for a given map
    static double zoomLevel; // The zoom level of the map
    static bool loadPressed; // If the load button was pressed
    static std::vector<unsigned> highlightedStreetIDs;
    static bool zoom_fitt;//check if intersection is zoomfit already
    static bool clear;//check if input is cleared already
    // Construct the StreetData object

    StreetData() {
        if (objectPtr == NULL) {
            // Loop through all the vectors
            initializeVectors();
            // Loop through all the street IDs
            initializeStreetIDsByName();
            // Loop through all street segments
            initializeStreetSegmentData();          
            // Loop through all the streets
            initializeStreetsData();           
            // Loop through all the intersection IDs and get the min/max
            initializeIntersectionMinMax();
            // Group intersections for easier searching after
            initializeIntersectionGroups();
            // Loop through all the points of interest and get the min/max
            initializePOIMinMax();
            // Group POI for easier searching after
            initializePOIGroups();
            
            
            objectPtr = this;
        } else {
            std::cerr << "You're trying to create multiple StreetData objects.";
        }
    }
    
    // Destroy the StreetData object
    ~StreetData() {
        objectPtr = NULL;
        streetIDsByName.clear();
        streetIntersectionsByName.clear();
        streetSegsByIntersectionID.clear();
        streetNamesBlonIntersection.clear();
        streetSegsByStreetID.clear();
        streetIntersectionsByStreetID.clear();
        intersectionNodeByIntersection.clear();
        groupedIntersections.clear();
        groupedPOIs.clear();
        highlightedStreetIDs.clear();
    }

    static StreetData *getObjectPtr() {
        if (objectPtr == NULL) {
            StreetData(); // Load the data if not loaded yet
        }
        return objectPtr;
    }
    
    static bool exists() {
        return objectPtr != NULL;
    }
};