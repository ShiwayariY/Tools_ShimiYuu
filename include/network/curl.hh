#include <string>
#include <vector>
#include <ostream>
#include <map>

#include <curl/curl.h>

#ifndef INCLUDE_SHIMIYUU_NETWORK_CURL_HH_
#define INCLUDE_SHIMIYUU_NETWORK_CURL_HH_

namespace shimiyuu::network {

class Curl {

public:
	Curl();
	~Curl();

	Curl(const Curl&) = delete;
	Curl& operator=(const Curl&) = delete;

	void set_cookies(const std::string& cookies) const;
	void set_referer(const std::string& referer) const;

	std::string get_string(const std::string& url) const;
	std::string post(const std::string& url, const std::map<std::string, std::string>& data) const;
	void write(const std::string& url, std::ostream& os) const;

	std::vector<std::string> get_cookies() const;

private:
	class Global {
	public:
		static void init();

	private:
		Global();
		~Global();
	};

	static std::size_t write_string(void* chunk, size_t size, size_t nmemb, void* buf);
	static std::size_t write_to_stream(void* chunk, size_t size, size_t nmemb, void* buf);

	CURL* m_handle;
};

}

#endif
