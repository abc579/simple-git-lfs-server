#include <cstddef>
#include <cstdint>
#include <ios>
#include <iterator>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>
#include <algorithm>

#include "simple_git_lfs.h"
#include "server.h"

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
    const std::string oid = [&request] () {
	const auto pos = request.path.find_last_of('/');
	return request.path.substr(pos + 1); // @TODO: Check for errors if someone puts a / at the end.
    }();

    std::ifstream file (get_filesystem_path(oid), std::ios::in | std::ios::binary);
    std::noskipws(file);

    std::vector<unsigned char> binary_file (std::istream_iterator<char>(file), std::istream_iterator<char>{});
    
    file.close();

    std::ofstream ofs {"files/temp", std::ios::out | std::ios::binary};

    ofs.write(reinterpret_cast<const char *>(&binary_file.front()), sizeof (unsigned char) * binary_file.size());

    ofs.close();

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
    }
    
    response.set_content(encode_batch_response(br), "application/json");
    response.status = (int) http_response_codes::ok;
}

std::string sgls::encode_batch_response(const batch_response& br)
{
    auto j = json11::Json::object {
        { "transfer", br.transfer },
        { "hash_algo", hash_algo }
    };

    auto object_array = json11::Json::array();

    for (const auto& o : br.objects) {
        json11::Json::object jtemp;
        const std::string download_link { download_object_link + o.object.oid };

        if (can_open(get_filesystem_path(o.object.oid))) {
            jtemp = {
                { "oid", o.object.oid },
                { "size", o.object.size},
                {   "actions", json11::Json::object {
                        {   "download", json11::Json::object {
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

std::string sgls::get_filesystem_path(const std::string& oid)
{
    return file_directory + '/' + oid.substr(0, 2) + '/' + oid.substr(2, 2) + '/' + oid.substr(4);
}
