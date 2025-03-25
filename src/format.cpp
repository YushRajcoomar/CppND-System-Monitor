#include <string>

#include "format.h"

using std::string;
using std::to_string;
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    const int seconds_per_minute = 60;
    const int minutes_per_hour = 60;
    const int hours_per_day = 24;
    const int seconds_per_day = seconds_per_minute * minutes_per_hour * hours_per_day;

    int days = seconds/seconds_per_day;

    long hours_left_per_day = seconds - (days * seconds_per_day);
    int hours = hours_left_per_day / (minutes_per_hour * seconds_per_minute); 

    long minutes_left_per_day =  hours_left_per_day - (hours * minutes_per_hour * seconds_per_minute);
    int minutes = minutes_left_per_day/seconds_per_minute ;
    
    long seconds_left_per_day = minutes_left_per_day - (minutes * seconds_per_minute);

    string elapsed_time = to_string(days) + " Days, " + to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds_left_per_day);
    return elapsed_time;
}