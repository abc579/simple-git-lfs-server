#include <cstddef>
#include <cstdint>
#include <ios>
#include <iterator>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>
#include <algorithm>

#include "json11.hpp"
#include "simple_git_lfs.h"
#include "config.h"
#include "server.h"
#include "base64.h"

void sgls::batch_request_handler(const request_t& request, response_t& response)
{
    try {
        auto req = json(request.body);
        create_batch_response(req, response);
    } catch (const json_parse_error& jpe) {
        throw jpe;
    }
}

// In this case, the OID comes in the URL, so we have to extract that and
// check if the file exists in the directory. If it does, store the contents
// in a string, put that into the response and return it.
void sgls::download_handler(const request_t& request, response_t& response)
{
    // @FIXME: when the file is too large, git lfs client does it in chunks using range,
    // so we have to do adapt to that situation.
    const std::string oid = get_oid_from_url(request.path);

    std::ifstream file (get_filesystem_path(oid), std::ios::in | std::ios::binary);
    std::noskipws(file);
    std::vector<unsigned char> binary_file (std::istream_iterator<char>(file), std::istream_iterator<char>{});
    file.close();

    response.set_header("Accept", accept_lfs);
    response.set_content(std::string(binary_file.begin(), binary_file.end()), "application/octet-stream");
    response.status = (int) http_response_codes::ok;
}

// We got a client request, which contains all the objects that the clients
// wants to get. Here we check for each object if it exists and act accordingly,
// that is, creating a response by filling the RESPONSE parameter.
void sgls::create_batch_response(const json& request, response_t& response)
{
    static const error_object error {"Object does not exist.", (int) object_error_codes::not_found};
    
    response.set_header("Content-Type", content_type_lfs);
    response.set_header("Accept", accept_lfs);
    
    batch_response br;

    const auto objects = request.get_array_items("objects");
    const auto operation = request.get_string_value("operation");

    if (operation == "download") {
	for (const auto& object : objects) {
	    const auto oid = object["oid"].string_value();
	    batch_object bo {oid, object["size"].int_value()};
	
	    if (can_open(get_filesystem_path(oid))) {
		Actions ac = {
		    {"actions", link_object {download_object_link + oid}}
		};
		br.objects.push_back({bo, ac, {}});
	    } else {
		br.objects.push_back({bo, {}, error});
	    }
	}
    } else if (operation == "upload") {
	// We do the same thing whether the object exists or not. The API
	// does not offer a lot of light in this case. For now we are doing the
	// same thing as the git lfs server offered as example made in Go.
	for (const auto& object : objects) {
	    const auto oid = object["oid"].string_value();
	    batch_object bo {oid, object["size"].int_value()};

	    Actions ac = {
		{"actions", link_object {upload_object_link + oid}}
	    };

	    br.objects.push_back({bo, ac, {}});
	}
    }
    
    response.set_content(encode_batch_response(br, operation), "application/json");
    response.status = (int) http_response_codes::ok;
}

std::string sgls::encode_batch_response(const batch_response& br, const std::string& operation)
{
    auto j = json11::Json::object {
        { "transfer", br.transfer },
        { "hash_algo", hash_algo }
    };

    auto object_array = json11::Json::array();

    for (const auto& o : br.objects) {
        json11::Json::object jtemp;
        const std::string download_link { download_object_link + o.object.oid };

        if (o.error.message.empty()) {
            jtemp = {
                { "oid", o.object.oid },
                { "size", o.object.size},
                {   "actions", json11::Json::object {
                        {   operation, json11::Json::object {
                                { "href", download_link}
                            }
                        }
                    }
                }
            };
        } else {
            jtemp = {
                { "oid", o.object.oid},
                { "size", o.object.size},
                {   "error", json11::Json::object {
                        { "code", o.error.code},
                        { "message", o.error.message}
                    }
                }
            };
        }

        object_array.push_back(jtemp);
    }

    j["objects"] = object_array;

    return json11::Json(j).dump();
}

bool sgls::can_open(const std::string& path)
{
    std::ifstream ifs (path, std::ios::in | std::ios::binary);

    return ifs.is_open();
}

size_t sgls::get_file_size(const std::string& path)
{
    if (!can_open(path)) {
	std::cerr << "get_file_size(): Could not open file " << path << std::endl;
	return 0;		// @TODO: Do better.
    }

    std::ifstream file (path, std::ios::binary);
    file.unsetf(std::ios::skipws);
    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    
    return file_size;
}

int create_directory(const std::string& path)
{
    return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

// Git format.
std::string sgls::get_filesystem_path(const std::string& oid)
{
    return file_directory + '/' + oid.substr(0, 2) + '/' + oid.substr(2, 2) + '/' + oid.substr(4);
}

// We will get something like this: "Authorization" = "Basic dGVtcDp0ZW1w".
// Notice that the string has Base64 encoding.
void sgls::upload_handler(const request_t& request, response_t& response)
{
    const std::string prefix {"Basic "};
    const auto encoded_auth = request.headers.find("Authorization"); // It is encoded in Base64.

    user_data data = [&encoded_auth, &prefix]() -> user_data {
	if (encoded_auth->second.find(prefix) == std::string::npos) {
	    std::cerr << "Something weird happened: we cannot find the base64 string in the request." << std::endl;
	    return {};
	}
	
	const auto decoded_auth = base64_decode(encoded_auth->second.substr(prefix.length()));
	const auto colon_pos = decoded_auth.find(':');

	return {decoded_auth.substr(0, colon_pos), decoded_auth.substr(colon_pos + 1)};
    } ();

    // All good, continue and save file.
    // @TODO: We should have support for more than one user in the future.
    if (data.user == sv_user && data.passwd == sv_passwd) {
	save_file_in_directory(get_oid_from_url(request.target), request.body);
    }
    
}

std::string get_oid_from_url(const std::string& url)
{
    const auto pos = url.find_last_of('/');

    return url.substr(pos + 1);
}

void save_file_in_directory(const std::string& oid, const std::string& raw)
{
    // TODO: Get directory in chunks and check for dir existance.
    // Then, create directories and finally the file.
    if (create_directory(file_directory + ) == -1) {
	
    }
}
