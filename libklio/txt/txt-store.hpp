/**
 * This class represents a local store, implemented as a CSV file.
 *
 * (c) Fraunhofer ITWM - Ely de Oliveira   <ely.oliveira@itwm.fhg.de>, 2014
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

#ifndef LIBKLIO_TXT_TXTSTORE_HPP
#define LIBKLIO_TXT_TXTSTORE_HPP 1

#include <boost/filesystem.hpp>
#include <libklio/store.hpp>

namespace bfs = boost::filesystem;

namespace klio {

    class TXTStore : public Store {
    public:
        typedef boost::shared_ptr<TXTStore> Ptr;

        TXTStore(const bfs::path& path, const string& separator) :
        Store(true, true, 0, 10, 10000),
        _path(path),
        _field_separator(separator),
        _token_separator(separator.c_str()) {
        };

        virtual ~TXTStore() {
            close();
        };

        void open();
        void close();
        void check_integrity();
        void initialize();
        void dispose();
        const string str();

        const static string DEFAULT_FIELD_SEPARATOR;

    protected:
        void add_sensor_record(const Sensor::Ptr sensor);
        void remove_sensor_record(const Sensor::Ptr sensor);
        void update_sensor_record(const Sensor::Ptr sensor);
        void add_single_reading_record(const Sensor::Ptr sensor, const timestamp_t timestamp, const double value, const bool ignore_errors);
        void add_bulk_reading_records(const Sensor::Ptr sensor, const readings_t& readings, const bool ignore_errors);
        void update_reading_records(const Sensor::Ptr sensor, const readings_t& readings, const bool ignore_errors);

        vector<Sensor::Ptr> get_sensor_records();
        readings_t_Ptr get_all_reading_records(const Sensor::Ptr sensor);
        readings_t_Ptr get_timeframe_reading_records(const Sensor::Ptr sensor, const timestamp_t begin, const timestamp_t end);
        unsigned long int get_num_readings_value(const Sensor::Ptr sensor);
        reading_t get_last_reading_record(const Sensor::Ptr sensor);
        reading_t get_reading_record(const Sensor::Ptr sensor, const timestamp_t timestamp);

    private:
        TXTStore(const TXTStore& original);

        const static string ENABLED;
        const static string DISABLED;
        const static string NOT_A_NUMBER;

        void save_sensor(ofstream& file, const Sensor::Ptr sensor);
        void save_reading(ofstream& file, const timestamp_t& timestamp, const double value);
        vector<vector<string>> read_records(const string& path);

        void check_sensor(const Sensor::Ptr sensor, const bool exists);
        const string compose_db_path();
        const string compose_sensors_path();
        const string compose_sensor_path(const string& uuid);
        const string compose_sensor_properties_path(const string& uuid);
        const string compose_sensor_readings_path(const string& uuid);
        void create_directory(const string& dir);

        bfs::path _path;
        string _field_separator;
        const boost::char_separator<char> _token_separator;
    };
};

#endif /* LIBKLIO_TXT_TXTSTORE_HPP */
