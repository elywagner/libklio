#include <sstream>
#include <boost/uuid/uuid_io.hpp>
#include <libklio/local-time.hpp>
#include <libklio/error.hpp>
#include <libklio/sensor-factory.hpp>


using namespace klio;

boost::uuids::random_generator SensorFactory::_gen_uuid;

Sensor::Ptr SensorFactory::createSensor(
        const string& external_id,
        const string& name,
        const string& unit,
        const string& timezone
        ) {
    return createSensor(external_id, name, unit, timezone, DeviceType::UNKNOWN_DEVICE);
}

Sensor::Ptr SensorFactory::createSensor(
        const string& external_id,
        const string& name,
        const string& unit,
        const string& timezone,
        const DeviceType::Ptr device_type
        ) {
    return createSensor(external_id, name, DEFAULT_SENSOR_DESCRIPTION, unit, timezone, device_type);
}

Sensor::Ptr SensorFactory::createSensor(
        const string& external_id,
        const string& name,
        const string& description,
        const string& unit,
        const string& timezone,
        const DeviceType::Ptr device_type
        ) {
    return createSensor(_gen_uuid(), external_id, name, description, unit, timezone, device_type);
}

Sensor::Ptr SensorFactory::createSensor(
        const string& uuid_string,
        const string& external_id,
        const string& name,
        const string& unit,
        const string& timezone
        ) {
    return createSensor(uuid_string, external_id, name, DEFAULT_SENSOR_DESCRIPTION, unit, timezone);
}

Sensor::Ptr SensorFactory::createSensor(
        const string& uuid_string,
        const string& external_id,
        const string& name,
        const string& description,
        const string& unit,
        const string& timezone
        ) {

    return createSensor(uuid_string, external_id, name, description, unit, timezone, DeviceType::UNKNOWN_DEVICE);
}

Sensor::Ptr SensorFactory::createSensor(
        const string& uuid_string,
        const string& external_id,
        const string& name,
        const string& description,
        const string& unit,
        const string& timezone,
        const DeviceType::Ptr device_type
        ) {

    // type conversion: uuid_string to real uuid type
    boost::uuids::uuid u;
    stringstream ss;
    ss << uuid_string;
    ss >> u;

    return createSensor(u, external_id, name, description, unit, timezone, device_type);
}

Sensor::Ptr SensorFactory::createSensor(
        const Sensor::uuid_t& uuid,
        const string& external_id,
        const string& name,
        const string& description,
        const string& unit,
        const string& timezone
        ) {

    return createSensor(uuid, external_id, name, description, unit, timezone, DeviceType::UNKNOWN_DEVICE);
}

Sensor::Ptr SensorFactory::createSensor(
        const Sensor::uuid_t& uuid,
        const string& external_id,
        const string& name,
        const string& description,
        const string& unit,
        const string& timezone,
        const DeviceType::Ptr device_type
        ) {

    const LocalTime::Ptr local_time(new LocalTime("../.."));

    if (!local_time->is_valid_timezone(timezone)) {

        const vector<string> valid_regions(local_time->get_valid_timezones());

        ostringstream oss;
        oss << "Invalid timezone " << timezone << ". Valid timezones are: " << endl;
        copy(valid_regions.begin(), valid_regions.end(), ostream_iterator<string> (oss, ", "));

        throw DataFormatException(oss.str());
    }

    return Sensor::Ptr(new Sensor(uuid, external_id, name, description, unit, timezone, device_type));
}