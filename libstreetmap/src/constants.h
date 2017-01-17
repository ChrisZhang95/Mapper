/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   constants.h
 * Author: carusom3
 *
 * Created on February 20, 2016, 10:04 AM
 */
#include <string>
#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

std::string POITYPE[] = {
    // Least important poi types
    "car_rental,old_restaurant,parking,parking_closed,charity,vending_machine,internet_cafe,karaoke,fire_station,childcare,future_fast_food,fountain,ambulance_station,employment_agency,community_centre,bureau_de_change,tutoring,lab,hair_loss,yoga,naturopathy,shoe_repair,funeral_directors,marketplace,food_court,psychic,courthouse,police,arts_centre,mortuary,car_wash,telephone,bicycle_rental,embassy,wedding_chapel,mp_office,bicycle_rental,post_box,billboard,nightclub,retirement_home,car_sharing,signal,clothing_donation,bicycle_parking,vacant,social_facility,money_lender,laser,banquet_hall,bakery,photography_club,storage,security,driving_school,social_centre,social_facility,mortgage_broker,music_school,sewing,tax,answering_service,shelter,ice_cream,languge_school,deli,employment,parking_entrance,fitness_center,english_school,bench,driver_testing,mortgage_broker,carts,security_office,parking_entrance,stripclub,laser_eye_surgery,hypnosis,recycling,taxi,reflexology,laundry,equipment_rental,waste_disposal,optician,sauna,convention_centre,waste_transfer_station,private_club,sweets,grave_yard,swimming_pool,health_center,traffic_ticket,government_office,ferry_terminal,cosmetic_surgery,loading_dock,studio,campaign_office,cash_for_gold,tickets,kollel,mpp_office,credit_union,natural_healing,gymnasium,chiropodist,martial_arts,gym,psychologist,music_lessons,web_design,old_car_rental,ems_station,waste_basket;recycling,physiotherapist,laser_tag,drinking_water,hotel,motel,emergency_room,hackerspace,dojo,charging_station,reuse",
    // Second least important poi types
    "doctors,health_specialty:chiropractic,chiropractor,orthodontist,optometrist,dentist,pharmacy,spa,lawyer,veterinary,clinic,hospital",
    // Third least important poi types
    "university,montessori_school,kindergarten,preschool,college,school,public_building,place_of_worship,library",
    // Third most important poi types
    "fast_food,cafe,pub,bar",
    // Second most important poi types
    "bank,casino,bus_station,fuel,post_office,atm",
    // Most important poi types
    "restaurant,cinema,theatre"
};




#endif /* CONSTANTS_H */

