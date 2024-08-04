/*
Minetest
Copyright (C) 2020 sfan5 <sfan5@live.de>

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

#include <type_traits>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "particles.h"

using namespace ParticleParamTypes;

template <typename T>
void RangedParameter<T>::serialize(std::ostream &os) const {
	min.serialize(os);
	max.serialize(os);
	writeF32(os, bias);
}

template <typename T>
void RangedParameter<T>::deSerialize(std::istream &is) {
	min.deSerialize(is);
	max.deSerialize(is);
	bias = readF32(is);
}

template <typename T>
T RangedParameter<T>::pickWithin() const {
	typename T::pickFactors values;
	auto p = std::abs(bias) + 1;
	for (auto &value : values) {
		value = (bias < 0) ? 1.0f - std::pow(myrand_float(), p) : std::pow(myrand_float(), p);
	}
	return T::pick(values, min, max);
}

template <typename T>
T TweenedParameter<T>::blend(float fac) const {
	if (fac > beginning) {
		fac = (fac - beginning) / (1 - beginning) * reps;
		fac = std::fmod(fac, 1.0f);

		switch (style) {
			case TweenStyle::fwd:
				break;
			case TweenStyle::rev:
				fac = 1.0f - fac;
				break;
			case TweenStyle::pulse:
			case TweenStyle::flicker:
				fac = (fac > 0.5f) ? 1.f - (fac * 2.f - 1.f) : fac * 2;
				if (style == TweenStyle::flicker) {
					fac *= myrand_range(0.7f, 1.0f);
				}
				break;
			default:
				break;
		}
		fac = std::clamp(fac, 0.0f, 1.0f);
	} else {
		fac = (style == TweenStyle::rev) ? 1.f : 0.f;
	}
	return start.interpolate(fac, end);
}

template <typename T>
void TweenedParameter<T>::serialize(std::ostream &os) const {
	writeU8(os, static_cast<u8>(style));
	writeU16(os, reps);
	writeF32(os, beginning);
	start.serialize(os);
	end.serialize(os);
}

template <typename T>
void TweenedParameter<T>::deSerialize(std::istream &is) {
	style = static_cast<TweenStyle>(readU8(is));
	if (style >= TweenStyle::TweenStyle_END) {
		style = TweenStyle::fwd;
	}
	reps = readU16(is);
	beginning = readF32(is);
	start.deSerialize(is);
	end.deSerialize(is);
}

namespace ParticleParamTypes {
template struct RangedParameter<v3fParameter>;
template struct RangedParameter<f32Parameter>;

template struct TweenedParameter<v2fParameter>;
template struct TweenedParameter<v3fParameter>;
template struct TweenedParameter<f32Parameter>;
template struct TweenedParameter<v3fRange>;
template struct TweenedParameter<f32Range>;
} //namespace ParticleParamTypes

template <typename T>
static T numericalBlend(float fac, T min, T max) {
	return min + ((max - min) * fac);
}

#define PARAM_PVFN(n) ParticleParamTypes::n##ParameterValue
v2f PARAM_PVFN(pick)(float *f, const v2f a, const v2f b) {
	return v2f(
			numericalBlend(f[0], a.X, b.X),
			numericalBlend(f[1], a.Y, b.Y));
}

v3f PARAM_PVFN(pick)(float *f, const v3f a, const v3f b) {
	return v3f(
			numericalBlend(f[0], a.X, b.X),
			numericalBlend(f[1], a.Y, b.Y),
			numericalBlend(f[2], a.Z, b.Z));
}

v2f PARAM_PVFN(interpolate)(float fac, const v2f a, const v2f b) {
	return b.getInterpolated(a, fac);
}

v3f PARAM_PVFN(interpolate)(float fac, const v3f a, const v3f b) {
	return b.getInterpolated(a, fac);
}

#define PARAM_DEF_SRZR(T, wr, rd)                                     \
	void PARAM_PVFN(serialize)(std::ostream & os, T v) { wr(os, v); } \
	void PARAM_PVFN(deSerialize)(std::istream & is, T & v) { v = rd(is); }

#define PARAM_DEF_NUM(T, wr, rd)                                                                        \
	PARAM_DEF_SRZR(T, wr, rd)                                                                           \
	T PARAM_PVFN(interpolate)(float fac, const T a, const T b) { return numericalBlend<T>(fac, a, b); } \
	T PARAM_PVFN(pick)(float *f, const T a, const T b) { return numericalBlend<T>(f[0], a, b); }

PARAM_DEF_NUM(u8, writeU8, readU8);
PARAM_DEF_NUM(s8, writeS8, readS8);
PARAM_DEF_NUM(u16, writeU16, readU16);
PARAM_DEF_NUM(s16, writeS16, readS16);
PARAM_DEF_NUM(u32, writeU32, readU32);
PARAM_DEF_NUM(s32, writeS32, readS32);
PARAM_DEF_NUM(f32, writeF32, readF32);
PARAM_DEF_SRZR(v2f, writeV2F32, readV2F32);
PARAM_DEF_SRZR(v3f, writeV3F32, readV3F32);

enum class ParticleTextureFlags : u8 {
	animated = 1,
	blend = 0x7 << 1,
};

using FlagT = std::underlying_type_t<ParticleTextureFlags>;

void ServerParticleTexture::serialize(std::ostream &os, u16 protocol_ver, bool newPropertiesOnly, bool skipAnimation) const {
	FlagT flags = 0;
	if (animated) {
		flags |= FlagT(ParticleTextureFlags::animated);
	}
	if (blendmode != BlendMode::alpha) {
		flags |= FlagT(blendmode) << 1;
	}
	serializeParameterValue(os, flags);

	alpha.serialize(os);
	scale.serialize(os);
	if (!newPropertiesOnly) {
		os << serializeString32(string);
	}

	if (!skipAnimation && animated) {
		animation.serialize(os, protocol_ver);
	}
}

void ServerParticleTexture::deSerialize(std::istream &is, u16 protocol_ver, bool newPropertiesOnly, bool skipAnimation) {
	FlagT flags = 0;
	deSerializeParameterValue(is, flags);
	if (is.eof()) {
		return;
	}

	animated = !!(flags & FlagT(ParticleTextureFlags::animated));
	blendmode = BlendMode((flags & FlagT(ParticleTextureFlags::blend)) >> 1);

	alpha.deSerialize(is);
	scale.deSerialize(is);
	if (!newPropertiesOnly) {
		string = deSerializeString32(is);
	}

	if (!skipAnimation && animated) {
		animation.deSerialize(is, protocol_ver);
	}
}

void ParticleParameters::serialize(std::ostream &os, u16 protocol_ver) const {
	writeV3F32(os, pos);
	writeV3F32(os, vel);
	writeV3F32(os, acc);
	writeF32(os, expirationtime);
	writeF32(os, size);
	writeU8(os, collisiondetection);
	os << serializeString32(texture.string);
	writeU8(os, vertical);
	writeU8(os, collision_removal);
	animation.serialize(os, protocol_ver);
	writeU8(os, glow);
	writeU8(os, object_collision);
	writeU16(os, node.param0);
	writeU8(os, node.param2);
	writeU8(os, node_tile);
	writeV3F32(os, drag);
	jitter.serialize(os);
	bounce.serialize(os);
	texture.serialize(os, protocol_ver, true, true);
}

template <typename T, T(reader)(std::istream &is)>
inline bool streamEndsBeforeParam(T &val, std::istream &is) {
	// Check if the next character is EOF before reading
	if (is.peek() == EOF) {
		return true;
	}
	// Read the value using the provided reader
	val = reader(is);

	return false;
}

void ParticleParameters::deSerialize(std::istream &is, u16 protocol_ver) {
	pos = readV3F32(is);
	vel = readV3F32(is);
	acc = readV3F32(is);
	expirationtime = readF32(is);
	size = readF32(is);
	collisiondetection = readU8(is);
	texture.string = deSerializeString32(is);
	vertical = readU8(is);
	collision_removal = readU8(is);
	animation.deSerialize(is, protocol_ver);
	glow = readU8(is);
	object_collision = readU8(is);

	if (streamEndsBeforeParam<u16, readU16>(node.param0, is)) {
		return;
	}
	node.param2 = readU8(is);
	node_tile = readU8(is);

	if (streamEndsBeforeParam<v3f, readV3F32>(drag, is)) {
		return;
	}
	jitter.deSerialize(is);
	bounce.deSerialize(is);
	texture.deSerialize(is, protocol_ver, true, true);
}
