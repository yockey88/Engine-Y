#ifndef ENGINEY_UTILS_HPP
#define ENGINEY_UTILS_HPP

#include <string>
#include <string_view>
#include <vector>
#include <array>

namespace EngineY {

namespace utils {

    // std::vector<std::string> SplitString(const std::string_view string , const std::string_view& delimiters);
    // std::vector<std::string> SplitString(const std::string_view string, const char delimiter);    

    template <size_t N>
    constexpr std::array<std::string_view , N> SplitString(std::string_view src , std::string_view delim) {
        std::array<std::string_view , N> result;

        auto start = src.begin();
        auto pos = start;

        size_t i = 0;

        while (pos != src.end()) {
            if (std::string_view(&*pos , delim.size()) == delim) {
                result[i] = std::string_view(&*start , (pos - start));
                start = pos += delim.size();
                ++i;
            } else {
                ++pos;
            }
        }

        if (pos != src.begin()) {
            result[N - 1] = std::string_view(&*start , (pos - start));
        }

        return result;
    }

    constexpr std::string_view RemoveNamespace(std::string_view name) {
    	const auto pos = name.find_last_of(':');
    	if (pos == std::string_view::npos) {
    	    return name;
        }
    	
    	return name.substr(name.find_last_of(':') + 1);
    }

    template<size_t N>
    constexpr std::array<std::string_view , N> RemoveNamespace(std::array<std::string_view , N> member_list) {
        std::array<std::string_view , N> result;

        uint32_t i = 0;
	for (std::string_view& full_name : member_list) {
    	    const auto pos = full_name.find_last_of(':');
    	    if (pos == std::string_view::npos) {
    	        result[i] = full_name;
            } else {
                result[i] = full_name.substr(full_name.find_last_of(':') + 1);
            }
            ++i;
        }

	return result;
    }

} // namespace utils

} // namespace EngineY

#endif // !ENGINEY_UTILS_HPP
