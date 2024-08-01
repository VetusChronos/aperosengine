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

#include "mesh.h"
#include "debug.h"
#include "log.h"
#include <cmath>
#include <iostream>
#include <IAnimatedMesh.h>
#include <SAnimatedMesh.h>
#include <IAnimatedMeshSceneNode.h>

inline static void applyShadeFactor(video::SColor& color, float factor) {
	color.setRed(core::clamp(static_cast<int>(color.getRed() * factor), 0, 255));
	color.setGreen(core::clamp(static_cast<int>(color.getGreen() * factor), 0, 255));
	color.setBlue(core::clamp(static_cast<int>(color.getBlue() * factor), 0, 255));
}

void applyFacesShading(video::SColor &color, const v3f &normal) {
	constexpr float sqrt1_0 = 1.0f;
	constexpr float sqrt0_2 = 0.447213f;
	constexpr float sqrt0_45 = 0.670820f;
	constexpr float sqrt0_7 = 0.836660f;

	float x2 = normal.X * normal.X;
	float y2 = normal.Y * normal.Y;
	float z2 = normal.Z * normal.Z;

	if (normal.Y < 0) {
		applyShadeFactor(color, sqrt0_45 * x2 + sqrt0_2 * y2 + sqrt0_7 * z2);
	} else if ((x2 > 1e-3) || (z2 > 1e-3)) {
		applyShadeFactor(color, sqrt0_45 * x2 + sqrt1_0 * y2 + sqrt0_7 * z2);
	}
}

scene::IAnimatedMesh* createCubeMesh(const v3f &scale) {
	video::SColor c(255, 255, 255, 255);
	video::S3DVertex vertices[24] = {
		// Up
		{{-0.5f, +0.5f, -0.5f}, {0, 1, 0}, c, {0, 1}},
		{{-0.5f, +0.5f, +0.5f}, {0, 1, 0}, c, {0, 0}},
		{{+0.5f, +0.5f, +0.5f}, {0, 1, 0}, c, {1, 0}},
		{{+0.5f, +0.5f, -0.5f}, {0, 1, 0}, c, {1, 1}},
		// Down
		{{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, c, {0, 0}},
		{{+0.5f, -0.5f, -0.5f}, {0, -1, 0}, c, {1, 0}},
		{{+0.5f, -0.5f, +0.5f}, {0, -1, 0}, c, {1, 1}},
		{{-0.5f, -0.5f, +0.5f}, {0, -1, 0}, c, {0, 1}},
		// Right
		{{+0.5f, -0.5f, -0.5f}, {1, 0, 0}, c, {0, 1}},
		{{+0.5f, +0.5f, -0.5f}, {1, 0, 0}, c, {0, 0}},
		{{+0.5f, +0.5f, +0.5f}, {1, 0, 0}, c, {1, 0}},
		{{+0.5f, -0.5f, +0.5f}, {1, 0, 0}, c, {1, 1}},
		// Left
		{{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, c, {1, 1}},
		{{-0.5f, -0.5f, +0.5f}, {-1, 0, 0}, c, {0, 1}},
		{{-0.5f, +0.5f, +0.5f}, {-1, 0, 0}, c, {0, 0}},
		{{-0.5f, +0.5f, -0.5f}, {-1, 0, 0}, c, {1, 0}},
		// Back
		{{-0.5f, -0.5f, +0.5f}, {0, 0, 1}, c, {1, 1}},
		{{+0.5f, -0.5f, +0.5f}, {0, 0, 1}, c, {0, 1}},
		{{+0.5f, +0.5f, +0.5f}, {0, 0, 1}, c, {0, 0}},
		{{-0.5f, +0.5f, +0.5f}, {0, 0, 1}, c, {1, 0}},
		// Front
		{{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, c, {0, 1}},
		{{-0.5f, +0.5f, -0.5f}, {0, 0, -1}, c, {0, 0}},
		{{+0.5f, +0.5f, -0.5f}, {0, 0, -1}, c, {1, 0}},
		{{+0.5f, -0.5f, -0.5f}, {0, 0, -1}, c, {1, 1}},
	};

	constexpr std::array<u16, 6> indices = {0, 1, 2, 2, 3, 0};

	auto mesh = std::make_unique<scene::SMesh>();
	for (u32 i = 0; i < 6; ++i) {
		auto buf = std::make_unique<scene::SMeshBuffer>();
		buf->append(vertices + 4 * i, 4, indices.data(), indices.size());
		// Set default material
		buf->getMaterial().Lighting = false;
		buf->getMaterial().MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		buf->getMaterial().forEachTexture([] (auto &tex) {
			tex.MinFilter = video::ETMINF_NEAREST_MIPMAP_NEAREST;
			tex.MagFilter = video::ETMAGF_NEAREST;
		});
		// Add mesh buffer to mesh
		mesh->addMeshBuffer(buf.release());
	}

	auto anim_mesh = std::make_unique<scene::SAnimatedMesh>(mesh.release());
	scaleMesh(anim_mesh.get(), scale);  // also recalculates bounding box
	return anim_mesh.release();
}

void scaleMesh(scene::IMesh *mesh, const v3f &scale) {
	if (!mesh) { return; }

	aabb3f bbox;
	bbox.reset(0, 0, 0);

	u32 mc = mesh->getMeshBufferCount();
	for (u32 j = 0; j < mc; j++) {
		scene::IMeshBuffer *buf = mesh->getMeshBuffer(j);
		const u32 stride = getVertexPitchFromType(buf->getVertexType());
		u32 vertex_count = buf->getVertexCount();
		auto vertices = reinterpret_cast<video::S3DVertex *>(buf->getVertices());
		std::for_each(vertices, vertices + vertex_count, [scale](auto &vertex) {
			vertex.Pos *= scale;
		});

		buf->recalculateBoundingBox();

		// Calculate total bounding box
		if (j == 0) {
			bbox = buf->getBoundingBox();
		} else {
			bbox.addInternalBox(buf->getBoundingBox());
		}
	}
	mesh->setBoundingBox(bbox);
}

void translateMesh(scene::IMesh *mesh, const v3f &vec) {
	if (!mesh) { return; }

	aabb3f bbox;
	bbox.reset(0, 0, 0);

	u32 mc = mesh->getMeshBufferCount();
	for (u32 j = 0; j < mc; j++) {
		scene::IMeshBuffer *buf = mesh->getMeshBuffer(j);
		const u32 stride = getVertexPitchFromType(buf->getVertexType());
		u32 vertex_count = buf->getVertexCount();
		auto vertices = reinterpret_cast<video::S3DVertex *>(buf->getVertices());
		std::for_each(vertices, vertices + vertex_count, [vec](auto &vertex) {
			vertex.Pos += vec;
		});

		buf->recalculateBoundingBox();

		// calculate total bounding box
		if (j == 0) {
			bbox = buf->getBoundingBox();
		} else {
			bbox.addInternalBox(buf->getBoundingBox());
		}
	}
	mesh->setBoundingBox(bbox);
}

void setMeshBufferColor(scene::IMeshBuffer *buf, const video::SColor &color) {
	const u32 stride = getVertexPitchFromType(buf->getVertexType());
	u32 vertex_count = buf->getVertexCount();
	u8 *vertices = (u8 *) buf->getVertices();
	for (u32 i = 0; i < vertex_count; i++) {
		((video::S3DVertex *) (vertices + i * stride))->Color = color;
	}
}

void setAnimatedMeshColor(scene::IAnimatedMeshSceneNode *node, const video::SColor &color) {
	for (u32 i = 0; i < node->getMaterialCount(); ++i) {
		node->getMaterial(i).EmissiveColor = color;
	}
}

void setMeshColor(scene::IMesh *mesh, const video::SColor &color) {
	if (!mesh) { return; }

	u32 mc = mesh->getMeshBufferCount();
	for (u32 j = 0; j < mc; j++) {
		setMeshBufferColor(mesh->getMeshBuffer(j), color);
	}
}

void setMeshBufferTextureCoords(scene::IMeshBuffer *buf, const v2f *uv, u32 count)
{
	const u32 stride = getVertexPitchFromType(buf->getVertexType());
	assert(buf->getVertexCount() >= count);
	u8 *vertices = (u8 *) buf->getVertices();
	for (u32 i = 0; i < count; i++) {
		((video::S3DVertex*) (vertices + i * stride))->TCoords = uv[i];
	}
}

template <typename F>
static void applyToMesh(scene::IMesh *mesh, const F &fn) {
	u16 mc = mesh->getMeshBufferCount();
	for (u16 j = 0; j < mc; j++) {
		scene::IMeshBuffer *buf = mesh->getMeshBuffer(j);
		const u32 stride = getVertexPitchFromType(buf->getVertexType());
		u32 vertex_count = buf->getVertexCount();
		char *vertices = reinterpret_cast<char *>(buf->getVertices());
		for (u32 i = 0; i < vertex_count; i++) {
			fn(reinterpret_cast<video::S3DVertex *>(vertices + i * stride));
		}
	}
}

void colorizeMeshBuffer(scene::IMeshBuffer *buf, const video::SColor *buffercolor) {
	const u32 stride = getVertexPitchFromType(buf->getVertexType());
	u32 vertex_count = buf->getVertexCount();
	u8 *vertices = (u8 *) buf->getVertices();
	for (u32 i = 0; i < vertex_count; i++) {
		video::S3DVertex *vertex = (video::S3DVertex *) (vertices + i * stride);
		video::SColor *vc = &(vertex->Color);
		// Reset color
		*vc = *buffercolor;
		// Apply shading
		applyFacesShading(*vc, vertex->Normal);
	}
}

void setMeshColorByNormalXYZ(scene::IMesh *mesh, const video::SColor &colorX,
		const video::SColor &colorY,
		const video::SColor &colorZ) {
	if (!mesh) { return; }
	auto colorizator = [=] (video::S3DVertex *vertex) {
		f32 x = fabs(vertex->Normal.X);
		f32 y = fabs(vertex->Normal.Y);
		f32 z = fabs(vertex->Normal.Z);
		if (x >= y && x >= z) {
			vertex->Color = colorX;
		} else if (y >= z) {
			vertex->Color = colorY;
		} else {
			vertex->Color = colorZ;
		}
	};
	applyToMesh(mesh, colorizator);
}

void setMeshColorByNormal(scene::IMesh *mesh, const v3f &normal,
		const video::SColor &color) {
	if (!mesh) { return; }
	auto colorizator = [normal, color] (video::S3DVertex *vertex) {
		if (vertex->Normal == normal) {
			vertex->Color = color;
		}
	};
	applyToMesh(mesh, colorizator);
}

template <float v3f::*U, float v3f::*V>
static void rotateMesh(scene::IMesh *mesh, float degrees) {
	degrees *= M_PI / 180.0f;
	float c = std::cos(degrees);
	float s = std::sin(degrees);
	auto rotator = [c, s] (video::S3DVertex *vertex) {
		float u = vertex->Pos.*U;
		float v = vertex->Pos.*V;
		vertex->Pos.*U = c * u - s * v;
		vertex->Pos.*V = s * u + c * v;
	};
	applyToMesh(mesh, rotator);
}

void rotateMeshXYby(scene::IMesh *mesh, f64 degrees) {
	rotateMesh<&v3f::X, &v3f::Y>(mesh, degrees);
}

void rotateMeshXZby(scene::IMesh *mesh, f64 degrees) {
	rotateMesh<&v3f::X, &v3f::Z>(mesh, degrees);
}

void rotateMeshYZby(scene::IMesh *mesh, f64 degrees) {
	rotateMesh<&v3f::Y, &v3f::Z>(mesh, degrees);
}

void rotateMeshBy6dFacedir(scene::IMesh *mesh, int facedir) 
	int axisdir = facedir >> 2;
	facedir &= 0x03;
	switch (facedir) {
		case 1: rotateMeshXZby(mesh, -90); break;
		case 2: rotateMeshXZby(mesh, 180); break;
		case 3: rotateMeshXZby(mesh, 90); break;
	}
	switch (axisdir) {
		case 1: rotateMeshYZby(mesh, 90); break; // z+
		case 2: rotateMeshYZby(mesh, -90); break; // z-
		case 3: rotateMeshXYby(mesh, -90); break; // x+
		case 4: rotateMeshXYby(mesh, 90); break; // x-
		case 5: rotateMeshXYby(mesh, -180); break;
	}
}

void recalculateBoundingBox(scene::IMesh *src_mesh) {
	aabb3f bbox;
	bbox.reset(0,0,0);
	for (u16 j = 0; j < src_mesh->getMeshBufferCount(); j++) {
		scene::IMeshBuffer *buf = src_mesh->getMeshBuffer(j);
		buf->recalculateBoundingBox();
		if (j == 0) {
			bbox = buf->getBoundingBox();
		} else {
			bbox.addInternalBox(buf->getBoundingBox());
		}
	}
	src_mesh->setBoundingBox(bbox);
}

bool checkMeshNormals(scene::IMesh *mesh) {
	u32 buffer_count = mesh->getMeshBufferCount();

	for (u32 i = 0; i < buffer_count; i++) {
		scene::IMeshBuffer *buffer = mesh->getMeshBuffer(i);
		if (!buffer->getVertexCount()) { continue; }

		// Here we intentionally check only first normal, assuming that if buffer
		// has it valid, then most likely all other ones are fine too. We can
		// check all of the normals to have length, but it seems like an overkill
		// hurting the performance and covering only really weird broken models.
		f32 length = buffer->getNormal(0).getLength();

		if (!std::isfinite(length) || length < 1e-10f) { return false; }
	}

	return true;
}

scene::IMeshBuffer* cloneMeshBuffer(scene::IMeshBuffer *mesh_buffer) {
	switch (mesh_buffer->getVertexType()) {
	case video::EVT_STANDARD: {
		video::S3DVertex *v = (video::S3DVertex *) mesh_buffer->getVertices();
		u16 *indices = mesh_buffer->getIndices();
		scene::SMeshBuffer *cloned_buffer = new scene::SMeshBuffer();
		cloned_buffer->append(v, mesh_buffer->getVertexCount(), indices,
			mesh_buffer->getIndexCount());
		return cloned_buffer;
	}
	case video::EVT_2TCOORDS: {
		video::S3DVertex2TCoords *v =
			(video::S3DVertex2TCoords *) mesh_buffer->getVertices();
		u16 *indices = mesh_buffer->getIndices();
		scene::SMeshBufferLightMap *cloned_buffer =
			new scene::SMeshBufferLightMap();
		cloned_buffer->append(v, mesh_buffer->getVertexCount(), indices,
			mesh_buffer->getIndexCount());
		return cloned_buffer;
	}
	case video::EVT_TANGENTS: {
		video::S3DVertexTangents *v =
			(video::S3DVertexTangents *) mesh_buffer->getVertices();
		u16 *indices = mesh_buffer->getIndices();
		scene::SMeshBufferTangents *cloned_buffer =
			new scene::SMeshBufferTangents();
		cloned_buffer->append(v, mesh_buffer->getVertexCount(), indices,
			mesh_buffer->getIndexCount());
		return cloned_buffer;
	}
	}
	// This should not happen.
	sanity_check(false);
	return NULL;
}

scene::SMesh* cloneMesh(scene::IMesh *srcMesh) {
	if (!srcMesh) { return nullptr; }

	auto mesh = std::make_unique<scene::SMesh>();
	u32 mc = srcMesh->getMeshBufferCount();
	for (u32 j = 0; j < mc; j++) {
		scene::IMeshBuffer *srcBuf = srcMesh->getMeshBuffer(j);
		auto buf = std::make_unique<scene::SMeshBuffer>();
		buf->append(srcBuf->getVertices(), srcBuf->getVertexCount(), srcBuf->getIndices(), srcBuf->getIndexCount());
		buf->BoundingBox = srcBuf->getBoundingBox();
		mesh->addMeshBuffer(buf.release());
	}

	mesh->recalculateBoundingBox();
	return mesh.release();
}

scene::IMesh* convertNodeboxesToMesh(const std::vector<aabb3f> &boxes,
		const f32 *uv_coords, float expand) {
	scene::SMesh* dst_mesh = new scene::SMesh();

	for (u16 j = 0; j < 6; j++) {
		scene::IMeshBuffer *buf = new scene::SMeshBuffer();
		buf->getMaterial().Lighting = false;
		buf->getMaterial().forEachTexture([] (auto &tex) {
			tex.MinFilter = video::ETMINF_NEAREST_MIPMAP_NEAREST;
			tex.MagFilter = video::ETMAGF_NEAREST;
		});
		dst_mesh->addMeshBuffer(buf);
		buf->drop();
	}

	video::SColor c(255,255,255,255);

	for (aabb3f box : boxes) {
		box.repair();

		box.MinEdge.X -= expand;
		box.MinEdge.Y -= expand;
		box.MinEdge.Z -= expand;
		box.MaxEdge.X += expand;
		box.MaxEdge.Y += expand;
		box.MaxEdge.Z += expand;

		// Compute texture UV coords
		f32 tx1 = (box.MinEdge.X / BS) + 0.5;
		f32 ty1 = (box.MinEdge.Y / BS) + 0.5;
		f32 tz1 = (box.MinEdge.Z / BS) + 0.5;
		f32 tx2 = (box.MaxEdge.X / BS) + 0.5;
		f32 ty2 = (box.MaxEdge.Y / BS) + 0.5;
		f32 tz2 = (box.MaxEdge.Z / BS) + 0.5;

		f32 txc_default[24] = {
			// up
			tx1, 1 - tz2, tx2, 1 - tz1,
			// down
			tx1, tz1, tx2, tz2,
			// right
			tz1, 1 - ty2, tz2, 1 - ty1,
			// left
			1 - tz2, 1 - ty2, 1 - tz1, 1 - ty1,
			// back
			1 - tx2, 1 - ty2, 1 - tx1, 1 - ty1,
			// front
			tx1, 1 - ty2, tx2, 1 - ty1,
		};

		// use default texture UV mapping if not provided
		const f32 *txc = uv_coords ? uv_coords : txc_default;

		v3f min = box.MinEdge;
		v3f max = box.MaxEdge;

		video::S3DVertex vertices[24] = {
			// up
			video::S3DVertex(min.X,max.Y,max.Z, 0,1,0, c, txc[0],txc[1]),
			video::S3DVertex(max.X,max.Y,max.Z, 0,1,0, c, txc[2],txc[1]),
			video::S3DVertex(max.X,max.Y,min.Z, 0,1,0, c, txc[2],txc[3]),
			video::S3DVertex(min.X,max.Y,min.Z, 0,1,0, c, txc[0],txc[3]),
			// down
			video::S3DVertex(min.X,min.Y,min.Z, 0,-1,0, c, txc[4],txc[5]),
			video::S3DVertex(max.X,min.Y,min.Z, 0,-1,0, c, txc[6],txc[5]),
			video::S3DVertex(max.X,min.Y,max.Z, 0,-1,0, c, txc[6],txc[7]),
			video::S3DVertex(min.X,min.Y,max.Z, 0,-1,0, c, txc[4],txc[7]),
			// right
			video::S3DVertex(max.X,max.Y,min.Z, 1,0,0, c, txc[ 8],txc[9]),
			video::S3DVertex(max.X,max.Y,max.Z, 1,0,0, c, txc[10],txc[9]),
			video::S3DVertex(max.X,min.Y,max.Z, 1,0,0, c, txc[10],txc[11]),
			video::S3DVertex(max.X,min.Y,min.Z, 1,0,0, c, txc[ 8],txc[11]),
			// left
			video::S3DVertex(min.X,max.Y,max.Z, -1,0,0, c, txc[12],txc[13]),
			video::S3DVertex(min.X,max.Y,min.Z, -1,0,0, c, txc[14],txc[13]),
			video::S3DVertex(min.X,min.Y,min.Z, -1,0,0, c, txc[14],txc[15]),
			video::S3DVertex(min.X,min.Y,max.Z, -1,0,0, c, txc[12],txc[15]),
			// back
			video::S3DVertex(max.X,max.Y,max.Z, 0,0,1, c, txc[16],txc[17]),
			video::S3DVertex(min.X,max.Y,max.Z, 0,0,1, c, txc[18],txc[17]),
			video::S3DVertex(min.X,min.Y,max.Z, 0,0,1, c, txc[18],txc[19]),
			video::S3DVertex(max.X,min.Y,max.Z, 0,0,1, c, txc[16],txc[19]),
			// front
			video::S3DVertex(min.X,max.Y,min.Z, 0,0,-1, c, txc[20],txc[21]),
			video::S3DVertex(max.X,max.Y,min.Z, 0,0,-1, c, txc[22],txc[21]),
			video::S3DVertex(max.X,min.Y,min.Z, 0,0,-1, c, txc[22],txc[23]),
			video::S3DVertex(min.X,min.Y,min.Z, 0,0,-1, c, txc[20],txc[23]),
		};

		u16 indices[] = {0,1,2,2,3,0};

		for(u16 j = 0; j < 24; j += 4) {
			scene::IMeshBuffer *buf = dst_mesh->getMeshBuffer(j / 4);
			buf->append(vertices + j, 4, indices, 6);
		}
	}
	return dst_mesh;
}

void setMaterialFilters(video::SMaterialLayer &tex, bool bilinear, bool trilinear, bool anisotropic) {
	if (trilinear) {
		tex.MinFilter = video::ETMINF_LINEAR_MIPMAP_LINEAR;
	} else if (bilinear) {
		tex.MinFilter = video::ETMINF_LINEAR_MIPMAP_NEAREST;
	} else {
		tex.MinFilter = video::ETMINF_NEAREST_MIPMAP_NEAREST;
	}

	tex.MagFilter = (trilinear || bilinear) ? video::ETMAGF_LINEAR : video::ETMAGF_NEAREST;

	tex.AnisotropicFilter = anisotropic ? 0xFF : 0;
}
