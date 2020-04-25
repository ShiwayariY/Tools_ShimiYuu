#include <stdexcept>

#include <network/curl.hh>

namespace shimiyuu::network {

Curl::Curl() {
	Global::init();
	m_handle = curl_easy_init();
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
	if (curl_easy_perform(m_handle))
		throw std::runtime_error("failed GET");
	return buf;
}

void Curl::write(const std::string& url, std::ostream& os) const {
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &os);
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_to_stream);
	curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());

	if (curl_easy_perform(m_handle))
		throw std::runtime_error("failed GET");
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
