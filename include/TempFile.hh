#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <filesystem>

#ifndef INCLUDE_SHIMIYUU_TEMPFILE_HH_
#define INCLUDE_SHIMIYUU_TEMPFILE_HH_

namespace shimiyuu {

/**
 * Thread safe single-process temporary file handle.
 *
 * Uses std::tmpnam, which may generate the same name multiple times
 * when used simultaneously from different processes / executables.
 * In this case, the file is still deleted safely, but may receive data
 * from multiple processes or be deleted unexpectedly.
 *
 * The temporary file is deleted from the filesystem when all copies of *this are deleted.
 * Thus, ensure *this (or its copy / moved-to instance) outlives any file stream of this->name().
 */
class TempFile {

	static const std::default_delete<std::string> default_string_deleter;
	static constexpr auto temp_file_deleter = [](std::string* filename) {
		std::filesystem::remove(*filename);
		default_string_deleter(filename);
	};

	static std::mutex m_mtx;
	static std::string create_temp_file() {
		std::scoped_lock lock { m_mtx };

		char tmpname_cstr[L_tmpnam];
		if (std::tmpnam(tmpname_cstr) == nullptr)
			throw std::runtime_error { "no temporary file available" };
		std::ofstream { tmpname_cstr }; // keep mtx locked, otherwise tmpnam may return same name on a different thread!
		return tmpname_cstr;
	}

	std::shared_ptr<std::string> m_file_handle;

public:

	TempFile() : m_file_handle { new std::string { create_temp_file() }, temp_file_deleter } {
	}

	std::string name() const {
		return *m_file_handle;
	}

	operator std::string() const {
		return name();
	}
};

inline std::mutex TempFile::m_mtx;

}

#endif
