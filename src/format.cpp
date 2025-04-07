#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;
using std::to_string;


using namespace std;

string Format::ElapsedTime(long seconds) {
    const int seconds_per_minute = 60;
    const int minutes_per_hour = 60;

    int hours = seconds / (minutes_per_hour * seconds_per_minute);
    int minutes = (seconds % (minutes_per_hour * seconds_per_minute)) / seconds_per_minute;
    int remaining_seconds = seconds % seconds_per_minute;

    // Format the string as HH:MM:SS with leading zeros
    ostringstream elapsed_time;
    elapsed_time << setw(2) << setfill('0') << hours << ":"
                 << setw(2) << setfill('0') << minutes << ":"
                 << setw(2) << setfill('0') << remaining_seconds;

    return elapsed_time.str();
}