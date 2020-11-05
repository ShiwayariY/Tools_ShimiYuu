#include <stdexcept>
#include <map>

#include <network/curl.hh>

namespace shimiyuu::network {

Curl::Curl() {
	Global::init();
	m_handle = curl_easy_init();
	curl_easy_setopt(m_handle, CURLOPT_COOKIEFILE, "");
	curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(m_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:82.0) Gecko/20100101 Firefox/82.0");
}

Curl::~Curl() {
	curl_easy_cleanup(m_handle);
}

void Curl::set_cookies(const std::string& cookies) const {
	curl_easy_setopt(m_handle, CURLOPT_COOKIE, cookies.c_str());
}

void Curl::set_referer(const std::string& referer) const {
	curl_easy_setopt(m_handle, CURLOPT_REFERER, referer.c_str());
}

std::string Curl::get_string(const std::string& url) const {
	std::string buf;
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &buf);
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_string);
	curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());
	if (curl_easy_perform(m_handle)) throw std::runtime_error("failed GET string");
	return buf;
}

std::string Curl::post(const std::string& url, const std::map<std::string, std::string>& data) const {
	std::string post_data, buf;
	for (const auto& [key, val] : data)
		post_data += (post_data.empty() ? "" : "&") + key + "=" + val;
	curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, post_data.c_str());
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &buf);
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_string);
	curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());
	const bool error = curl_easy_perform(m_handle);
	curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
	if (error) throw std::runtime_error("failed POST");
	return buf;
}

void Curl::write(const std::string& url, std::ostream& os) const {
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &os);
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_to_stream);
	curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());
	if (curl_easy_perform(m_handle)) throw std::runtime_error("failed GET data");
}

std::vector<std::string> Curl::get_cookies() const {
	std::vector<std::string> cookies_v;
	struct curl_slist* cookies = NULL;
	if (!curl_easy_getinfo(m_handle, CURLINFO_COOKIELIST, &cookies)) {
		struct curl_slist* remaining_cookies = cookies;
		while (remaining_cookies) {
			cookies_v.push_back(remaining_cookies->data);
			remaining_cookies = remaining_cookies->next;
		}
	}
	return cookies_v;
}

void Curl::Global::init() {
	static Global instance;
}

Curl::Global::Global() {
	curl_global_init(CURL_GLOBAL_ALL);
}

Curl::Global::~Global() {
	curl_global_cleanup();
}

size_t Curl::write_string(void* chunk, size_t size, size_t nmemb, void* buf) {
	const size_t realsize = size * nmemb;
	static_cast<std::string*>(buf)->append(static_cast<char*>(chunk), realsize);
	return realsize;
}

size_t Curl::write_to_stream(void* chunk, size_t size, size_t nmemb, void* buf) {
	const size_t realsize = size * nmemb;
	static_cast<std::ostream*>(buf)->write(static_cast<char*>(chunk), realsize);
	return realsize;
}

}
