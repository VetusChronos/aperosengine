/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once

#include <chrono>
#include <string>
#include <mutex>
#include <stdexcept>

#include "log.h"

/**
 * @brief Retrieves the current local time.
 *
 * @return std::tm structure containing the local time.
 * @throws errorstream if the local time cannot be determined.
 */
inline std::tm mt_localtime() {
	// Initialize the time zone on first invocation
	static std::once_flag tz_init;
	std::call_once(tz_init, [] {
#ifdef _WIN32
		_tzset();
#else
				tzset();
#endif
	});

	std::tm ret{};
	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	// Use the appropriate localtime function with error checking
#ifdef _WIN32
	if (localtime_s(&ret, &now) != 0) {
		throw errorstream << "Failed to get local time.";
	}
#else
	if (localtime_r(&now, &ret) == nullptr) {
		throw errorstream << "Failed to get local time.";
	}
#endif
	return ret;
}

/**
 * @brief Generates a timestamp string in the format YYYY-MM-DD HH:MM:SS.
 *
 * @return A string representing the current timestamp.
 * @throws errorstream if the time cannot be formatted.
 */
inline std::string get_timestamp() {
	constexpr size_t TIMESTAMP_SIZE = 20; // YYYY-MM-DD HH:MM:SS + '\0'
	const std::tm tm = mt_localtime();
	char cs[TIMESTAMP_SIZE];
	if (std::strftime(cs, sizeof(cs), "%Y-%m-%d %H:%M:%S", &tm) == 0) {
		throw errorstream << "Failed to format time.";
	}
	return std::string(cs);
}
