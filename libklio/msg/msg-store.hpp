/**
 * This class represents a remote store located at the mySmartGrid server.
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

#ifndef LIBKLIO_MSG_MSGSTORE_HPP
#define LIBKLIO_MSG_MSGSTORE_HPP 1

#include <libklio/config.h>

#ifdef ENABLE_MSG

#include <curl/curl.h>
#include <json/json.h>
#include <libklio/store.hpp>


namespace klio {

    typedef struct {
        char *data;
        size_t size;
    } CURLresponse;

    class MSGStore : public Store {
    public:
        typedef boost::shared_ptr<MSGStore> Ptr;

        MSGStore(const std::string& url,
                const std::string& id,
                const std::string& key,
                const std::string& description,
                const std::string& type) :
        Store(true, 600, true),
        _url(url),
        _id(id),
        _key(key),
        _description(description),
        _type(type),
        _last_heartbeat(0) {
        };

        virtual ~MSGStore() {
            close();
        };

        const std::string url() const {
            return _url;
        };

        const std::string id() const {
            return _id;
        };

        const std::string key() const {
            return _key;
        };

        const std::string description() const {
            return _description;
        };

        const std::string type() const {
            return _type;
        };

        const std::string activation_code() const {
            return _id.substr(0, 10);
        };

        void open();
        void close();
        void check_integrity();
        void initialize();
        void dispose();
        void flush();
        const std::string str();

        static const std::string DEFAULT_MSG_URL;
        static const std::string DEFAULT_MSG_DESCRIPTION;
        static const std::string DEFAULT_MSG_TYPE;

    protected:
        void add_sensor_record(const Sensor::Ptr sensor);
        void remove_sensor_record(const Sensor::Ptr sensor);
        void update_sensor_record(const Sensor::Ptr sensor);
        void add_reading_records(const Sensor::Ptr sensor, const readings_t& readings, const bool ignore_errors);
        void update_reading_records(const Sensor::Ptr sensor, const readings_t& readings, const bool ignore_errors);

        std::vector<Sensor::Ptr> get_sensor_records();
        readings_t_Ptr get_all_reading_records(const Sensor::Ptr sensor);
        readings_t_Ptr get_timeframe_reading_records(const Sensor::Ptr sensor, const timestamp_t begin, const timestamp_t end);
        unsigned long int get_num_readings_value(const Sensor::Ptr sensor);
        reading_t get_last_reading_record(const Sensor::Ptr sensor);
        reading_t get_reading_record(const Sensor::Ptr sensor, const timestamp_t timestamp);

    private:
        MSGStore(const MSGStore& original);
        MSGStore& operator =(const MSGStore& rhs);

        std::string _url;
        std::string _id;
        std::string _key;
        std::string _description;
        std::string _type;
        timestamp_t _last_heartbeat;

        void heartbeat();

        const std::string format_uuid_string(const std::string& meter);
        const std::string compose_device_url();
        const std::string compose_sensor_url(const Sensor::Ptr sensor);
        const std::string compose_sensor_url(const Sensor::Ptr sensor, const std::string& query);
        const std::string compose_url(const std::string& object, const std::string& id);

        struct json_object *perform_http_get(const std::string& url, const std::string& key);
        struct json_object *perform_http_post(const std::string& url, const std::string& key, json_object *jobject);
        void perform_http_delete(const std::string& url, const std::string& key);
        CURL *create_curl_handler(const std::string& url, const curl_slist *headers);
        const std::string digest_message(const std::string& data, const std::string& key);
        struct json_object *perform_http_request(const std::string& method, const std::string& url, const std::string& key, json_object *jbody);

        struct json_object *create_json_object();
        struct json_object *create_json_string(const std::string& str);
        struct json_object *create_json_array();
        struct json_object *create_json_int(const int value);
        struct json_object *create_json_double(const double value);
        struct json_object *get_json_readings(const Sensor::Ptr sensor);
        Sensor::Ptr parse_sensor(const std::string& uuid_str, json_object *jsensor);
        std::pair<timestamp_t, double > create_reading_pair(json_object *jpair);
        void destroy_object(json_object * jobject);
    };
};

#endif /* ENABLE_MSG */

#endif /* LIBKLIO_MSG_MSGSTORE_HPP */
