#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "local-time.hpp"


namespace bfs = boost::filesystem;

using namespace klio;
using namespace boost::gregorian;
using namespace boost::local_time;
using namespace boost::posix_time;


LocalTime::LocalTime(const char* cmd)
: _tz_db() {
    try {
        bfs::path zonespec_filename("date_time_zonespec.csv");
        std::ifstream zs_stream;
        if (bfs::exists(bfs::path(cmd) / zonespec_filename)) {
            _tz_db.load_from_file(
                    (bfs::path(cmd) / zonespec_filename).c_str()
                    );
        }
        if (bfs::exists(bfs::path(cmd) / ".." / "share" / "libklio" / zonespec_filename)) {
            _tz_db.load_from_file(
                    (bfs::path(cmd) / ".." / "share" / "libklio" / zonespec_filename).c_str()
                    );
        } else if (bfs::exists(bfs::path("./share/libklio") / zonespec_filename)) {
            _tz_db.load_from_file(
                    (bfs::path("./share/libklio") / zonespec_filename).c_str()
                    );
        } else if (bfs::exists(bfs::path(INSTALL_PREFIX) / bfs::path("share/libklio") / zonespec_filename)) {
            _tz_db.load_from_file(
                    (bfs::path(INSTALL_PREFIX) / bfs::path("share/libklio") / zonespec_filename).c_str()
                    );
        } else if (bfs::exists(bfs::path("/usr") / bfs::path("share/libklio") / zonespec_filename)) {
            _tz_db.load_from_file(
                    (bfs::path("/usr") / bfs::path("share/libklio") / zonespec_filename).c_str()
                    );
        } else {
            std::cerr << "Cannot open timezone database " << zonespec_filename << ", aborting." << std::endl;
            std::cerr << "Tried " <<
                    (bfs::path(cmd) / zonespec_filename).c_str() << std::endl;
            std::cerr << "Tried " <<
                    (bfs::path(cmd) / ".." / zonespec_filename).c_str() << std::endl;
            std::cerr << "Tried " <<
                    (bfs::path("./share/libklio") / zonespec_filename).c_str() << std::endl;
            std::cerr << "Tried " <<
                    (bfs::path(INSTALL_PREFIX) / bfs::path("share/libklio") / zonespec_filename).c_str() << std::endl;
            std::cerr << "Tried " <<
                    (bfs::path("/usr") / bfs::path("share/libklio") / zonespec_filename).c_str() << std::endl;
            exit(-1);
        }
    } catch (data_not_accessible dna) {
        std::cerr << "Error with time zone data file: " << dna.what() << std::endl;
        exit(EXIT_FAILURE);
    } catch (bad_field_count bfc) {
        std::cerr << "Error with time zone data file: " << bfc.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::vector<std::string> LocalTime::get_valid_timezones() {
    return _tz_db.region_list();
}

bool LocalTime::is_valid_timezone(const std::string& zone) {
    std::vector<std::string> regions;
    std::vector<std::string>::iterator it;
    regions = _tz_db.region_list();
    bool valid = false;
    for (it = regions.begin(); it < regions.end(); ++it) {
        if (boost::iequals((*it), zone)) {
            valid = true;
        }
    }
    return valid;
}

time_zone_ptr LocalTime::get_timezone_ptr(klio::Sensor::Ptr sensor) {
    return _tz_db.time_zone_from_region(sensor->timezone());
}

boost::local_time::local_date_time
LocalTime::get_local_time(klio::Sensor::Ptr sensor, klio::timestamp_t time) {
    time_zone_ptr tz(get_timezone_ptr(sensor));
    ptime pt = boost::posix_time::from_time_t(time);
    return local_date_time(pt, tz);
}

boost::posix_time::ptime
LocalTime::get_utc_time(klio::Sensor::Ptr sensor, klio::timestamp_t time) {
    return boost::posix_time::from_time_t(time);
}

boost::gregorian::date
LocalTime::get_local_date(klio::Sensor::Ptr sensor, klio::timestamp_t time) {
    return date(get_local_time(sensor, time).date());
}

uint16_t
LocalTime::get_local_day_of_year(klio::Sensor::Ptr sensor, klio::timestamp_t time) {
    static partial_date new_years_day(1, Jan);
    local_date_time localtime = get_local_time(sensor, time);
    ptime local_ptime(localtime.local_time());
    date local_date(local_ptime.date());
    return (1 + (local_date - new_years_day.get_date(local_date.year())).days());
}

uint16_t LocalTime::get_local_hour(
        klio::Sensor::Ptr sensor, klio::timestamp_t time) {
    local_date_time localtime = get_local_time(sensor, time);
    ptime local_ptime(localtime.local_time());
    time_duration time_of_day(local_ptime.time_of_day());
    return time_of_day.hours();
}

klio::timestamp_t LocalTime::get_timestamp(
        boost::posix_time::ptime ptime) {
    static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    time_duration diff(ptime - epoch);
    return (diff.ticks() / diff.ticks_per_second());
}

klio::timestamp_t LocalTime::get_timestamp(
        klio::Sensor::Ptr sensor, boost::local_time::local_date_time time) {
    static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));

    time_duration diff = time.utc_time() - epoch;
    return diff.total_seconds();
}
