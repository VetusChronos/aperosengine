/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>

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

#include <array>

inline u32 time_to_daynight_ratio(float time_of_day, bool smooth) {
	// Normalize time_of_day to be within [0, 24000)
	time_of_day = fmod(time_of_day + 24000.0f, 24000.0f);
	if (time_of_day > 12000.0f) {
		time_of_day = 24000.0f - time_of_day;
	}

	constexpr std::array<std::array<float, 2>, 9> values = {{
		{ 4375.0f,  175.0f },
		{ 4625.0f,  175.0f },
		{ 4875.0f,  250.0f },
		{ 5125.0f,  350.0f },
		{ 5375.0f,  500.0f },
		{ 5625.0f,  675.0f },
		{ 5875.0f,  875.0f },
		{ 6125.0f, 1000.0f },
		{ 6375.0f, 1000.0f },
	}};

	if (!smooth) {
		float lastt = values[0][0];
		for (size_t i = 1; i < values.size(); i++) {
			float t0 = values[i][0];
			float switch_t = (t0 + lastt) / 2.0f;
			lastt = t0;
			if (switch_t <= time_of_day) 
				continue;

			return values[i][1];
		}

		return 1000;
	}

	if (time_of_day <= 4625.0f) { // 4500 + 125
		return values[0][1];
	} else if (time_of_day >= 6125.0f) { // 6000 + 125
		return 1000;
	}

	// Smooth interpolation for time values between known points
	for (size_t i = 1; i < values.size(); i++) {
		if (values[i][0] > time_of_day) {
			float td0 = values[i][0] - values[i - 1][0];
			float f = (time_of_day - values[i - 1][0]) / td0;
			return static_cast<u32>(f * values[i][1] + (1.0f - f) * values[i - 1][1]);
		}
	}

	return 1000;
}
