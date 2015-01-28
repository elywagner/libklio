/**
 * This file is part of libklio.
 *
 * (c) Fraunhofer ITWM - Mathias Dalheimer <dalheimer@itwm.fhg.de>,    2010
 *                       Ely de Oliveira   <ely.oliveira@itwm.fhg.de>, 2013
 *
 * libklio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libklio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libklio. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBKLIO_SENSOR_HPP
#define LIBKLIO_SENSOR_HPP 1

#include <map>
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include <libklio/common.hpp>
#include <libklio/device-type.hpp>

using namespace std;

namespace klio {
    const static string DEFAULT_SENSOR_DESCRIPTION = "Unknown";

    class Sensor {
    public:
        typedef boost::shared_ptr<Sensor> Ptr;
        typedef boost::uuids::uuid uuid_t;

        Sensor(const uuid_t uuid,
                const string& external_id,
                const string& name,
                const string& description,
                const string& unit,
                const string& timezone,
                const klio::DeviceType::Ptr device_type) :
        _uuid(uuid),
        _external_id(external_id),
        _name(name),
        _description(description),
        _unit(unit),
        _timezone(timezone),
        _device_type(device_type) {
        };

        virtual ~Sensor() {
        };

        // standard sensor methods

        const uuid_t uuid() const {
            return _uuid;
        };

        const string external_id() const {
            return _external_id;
        };

        const string name() const {
            return _name;
        };

        const string description() const {
            return _description;
        };

        const string unit() const {
            return _unit;
        };

        const string timezone() const {
            return _timezone;
        };

        const klio::DeviceType::Ptr device_type() const {
            return _device_type;
        };

        void external_id(const string& external_id) {
            _external_id = external_id;
        }

        void name(const string& name) {
            _name = name;
        }

        void description(const string& description) {
            _description = description;
        }

        void unit(const string& unit) {
            _unit = unit;
        }

        void timezone(const string& timezone) {
            _timezone = timezone;
        }

        void device_type(const klio::DeviceType::Ptr device_type) {
            _device_type = device_type;
        }

        const string str();
        const string uuid_string() const;
        const string uuid_short() const;

        bool operator ==(const Sensor& rhs);
        bool operator !=(const Sensor& rhs);

    private:
        Sensor(const Sensor& original);
        Sensor& operator =(const Sensor& rhs);

        uuid_t _uuid;
        string _external_id;
        string _name;
        string _description;
        string _unit;
        string _timezone;
        klio::DeviceType::Ptr _device_type;
    };
};

#endif /* LIBKLIO_SENSOR_HPP */