#include "easygl/graphics.h"
#include "StreetsDatabaseAPI.h"
#include "draw_feature.h"
#define DEG_TO_RAD 0.017453292519943295769236907684886

void draw_river(unsigned featureID, double scalingFactor) {
    setcolor(112, 176, 255);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_building(unsigned featureID, double scalingFactor) {
    setlinestyle(SOLID);
    setlinewidth(1);
    setcolor(LIGHTGREY);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_stream(unsigned featureID, double scalingFactor) {
    setcolor(112, 176, 255);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID) - 1; j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;

        double beginX = getFeaturePoint(featureID, j).lon*scalingFactor;
        double beginY = getFeaturePoint(featureID, j).lat;
        double endX = getFeaturePoint(featureID, j + 1).lon*scalingFactor;
        double endY = getFeaturePoint(featureID, j + 1).lat;

        drawline(beginX, beginY, endX, endY);
    }
}

void draw_park(unsigned featureID, double scalingFactor) {
    setcolor(150, 195, 150);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_greenspace(unsigned featureID, double scalingFactor) {
    setcolor(100, 198, 100);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_golfcourse(unsigned featureID, double scalingFactor) {
    setcolor(FIREBRICK);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_island(unsigned featureID, double scalingFactor) {
    setcolor(150, 195, 150);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_beach(unsigned featureID, double scalingFactor) {
    setlinestyle(SOLID);
    setlinewidth(2);
    setcolor(255, 220, 177);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_shoreline(unsigned featureID, double scalingFactor) {
    setlinestyle(SOLID);
    setlinewidth(2);
    setcolor(255, 220, 177);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;


        if (j != getFeaturePointCount(featureID) - 1) {
            double beginX = getFeaturePoint(featureID, j).lon*scalingFactor;
            double beginY = getFeaturePoint(featureID, j).lat;
            double endX = getFeaturePoint(featureID, j + 1).lon*scalingFactor;
            double endY = getFeaturePoint(featureID, j + 1).lat;
            drawline(beginX, beginY, endX, endY);
        } else {
            double beginX = getFeaturePoint(featureID, j).lon*scalingFactor;
            double beginY = getFeaturePoint(featureID, j).lat;
            double endX = getFeaturePoint(featureID, 0).lon*scalingFactor;
            double endY = getFeaturePoint(featureID, 0).lat;
            drawline(beginX, beginY, endX, endY);
        }

    }
}

void draw_lake(unsigned featureID, double scalingFactor) {
    setlinestyle(SOLID);
    setlinewidth(3);
    setcolor(112, 176, 255);
    t_point points[getFeaturePointCount(featureID)];

    for (unsigned j = 0; j < getFeaturePointCount(featureID); j++) {
        points[j].x = getFeaturePoint(featureID, j).lon*scalingFactor;
        points[j].y = getFeaturePoint(featureID, j).lat;
    }

    fillpoly(points, getFeaturePointCount(featureID));
}

void draw_slanted_text(std::string text, double startX,double startY,double endX,double endY) {
    double angle = atan2(endY-startY,endX-startX);

    double angleDeg = (angle/DEG_TO_RAD)+180;
    if (angleDeg > 90 && angleDeg < 270) {
        if (angleDeg > 180) {
            angleDeg = angleDeg - 180;
        }
        else {
            angleDeg = angleDeg + 180;
        }
    }
    double savedAng = angleDeg*DEG_TO_RAD;
    settextrotation(angleDeg);
    if (angleDeg >= 270) {
        angleDeg = 360 - angleDeg;
    }

    drawtext(-sin(savedAng)*(xscrn_to_world(6)-xscrn_to_world(1))+(startX+endX)/2, -cos(savedAng)*(yscrn_to_world(6)-yscrn_to_world(1))+(startY+endY)/2, text, (1-(angleDeg/90)*(angleDeg/90))*sqrt((endX-startX)*(endX-startX) + (endY-startY)*(endY-startY)), 9999);
}
