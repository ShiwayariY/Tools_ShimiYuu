#include <string>
#include <string_view>
#include <map>
#include <stdexcept>
#include <optional>

#ifndef INCLUDE_SHIMIYUU_SUBCOMMAND_PICKER_HH_
#define INCLUDE_SHIMIYUU_SUBCOMMAND_PICKER_HH_

namespace shimiyuu {

template<typename Subcommand>
class SubcommandPicker {

	std::map<std::string, Subcommand> m_subcommand_dict;
	std::optional<Subcommand> m_default;

public:

	constexpr SubcommandPicker(std::map<std::string, Subcommand> subcommands) :
			m_subcommand_dict { std::move(subcommands) } {
	}

	constexpr SubcommandPicker<Subcommand>& set_default(Subcommand defaut_cmnd) noexcept {
		m_default = std::move(defaut_cmnd);
		return *this;
	}

	[[nodiscard]] Subcommand pick(int& argc, char**& argv) const {
		auto default_or_throw = [this](std::string_view msg) {
			if (!m_default) throw std::invalid_argument { msg.data() };
			return m_default.value();
		};

		if (argc < 2) return default_or_throw("missing subcommand");
		std::string subcmd_str = argv[1];

		if (auto subcmd_it = m_subcommand_dict.find(subcmd_str); subcmd_it != m_subcommand_dict.end()) {
			--argc;
			++argv;
			return subcmd_it->second;
		}
		return default_or_throw("invalid subcommand '" + subcmd_str + "'");
	}

};

}

#endif
