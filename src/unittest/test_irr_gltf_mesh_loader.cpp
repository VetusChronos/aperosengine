// Minetest
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "CSceneManager.h"
#include "content/subgames.h"
#include "filesys.h"

#include "CReadFile.h"
#include "irr_v3d.h"
#include "irr_v2d.h"

#include <irrlicht.h>

#include "catch.h"

TEST_CASE("gltf") {

const auto gamespec = findSubgame("devtest");

if (!gamespec.isValid())
	SKIP();

irr::scene::CSceneManager smgr(nullptr, nullptr, nullptr);
const auto loadMesh = [&smgr](const irr::io::path& filepath) {
	irr::io::CReadFile file(filepath);
	return smgr.getMesh(&file);
};

const static auto model_stem = gamespec.gamemods_path +
		DIR_DELIM + "gltf" + DIR_DELIM + "models" + DIR_DELIM + "gltf_";

SECTION("error cases") {
	const static auto invalid_model_path = gamespec.gamemods_path + DIR_DELIM + "gltf" + DIR_DELIM + "invalid" + DIR_DELIM;

	SECTION("empty gltf file") {
		CHECK(loadMesh(invalid_model_path + "empty.gltf") == nullptr);
	}

	SECTION("null file pointer") {
		CHECK(smgr.getMesh(nullptr) == nullptr);
	}

	SECTION("invalid JSON") {
		CHECK(loadMesh(invalid_model_path + "json_missing_brace.gltf") == nullptr);
	}

	// This is an example of something that should be validated by tiniergltf.
	SECTION("invalid bufferview bounds")
	{
		CHECK(loadMesh(invalid_model_path + "invalid_bufferview_bounds.gltf") == nullptr);
	}
}

SECTION("minimal triangle") {
	const auto path = GENERATE(
			model_stem + "minimal_triangle.gltf",
			model_stem + "triangle_with_vertex_stride.gltf",
			// Test non-indexed geometry.
			model_stem + "triangle_without_indices.gltf");
	INFO(path);
	const auto mesh = loadMesh(path);
	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 1);

	SECTION("vertex coordinates are correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 3);
		auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(0)->getVertices());
		CHECK(vertices[0].Pos == v3f {0.0f, 0.0f, 0.0f});
		CHECK(vertices[1].Pos == v3f {1.0f, 0.0f, 0.0f});
		CHECK(vertices[2].Pos == v3f {0.0f, 1.0f, 0.0f});
	}

	SECTION("vertex indices are correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getIndexCount() == 3);
		auto indices = static_cast<const irr::u16 *>(
				mesh->getMeshBuffer(0)->getIndices());
		CHECK(indices[0] == 2);
		CHECK(indices[1] == 1);
		CHECK(indices[2] == 0);
	}
}

SECTION("blender cube") {
	const auto mesh = loadMesh(model_stem + "blender_cube.gltf");
	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 1);
	SECTION("vertex coordinates are correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
		auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(0)->getVertices());
		CHECK(vertices[0].Pos == v3f{-10.0f, -10.0f, -10.0f});
		CHECK(vertices[3].Pos == v3f{-10.0f, 10.0f, -10.0f});
		CHECK(vertices[6].Pos == v3f{-10.0f, -10.0f, 10.0f});
		CHECK(vertices[9].Pos == v3f{-10.0f, 10.0f, 10.0f});
		CHECK(vertices[12].Pos == v3f{10.0f, -10.0f, -10.0f});
		CHECK(vertices[15].Pos == v3f{10.0f, 10.0f, -10.0f});
		CHECK(vertices[18].Pos == v3f{10.0f, -10.0f, 10.0f});
		CHECK(vertices[21].Pos == v3f{10.0f, 10.0f, 10.0f});
	}

	SECTION("vertex indices are correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getIndexCount() == 36);
		auto indices = static_cast<const irr::u16 *>(
				mesh->getMeshBuffer(0)->getIndices());
		CHECK(indices[0] == 16);
		CHECK(indices[1] == 5);
		CHECK(indices[2] == 22);
		CHECK(indices[35] == 0);
	}

	SECTION("vertex normals are correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
		auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(0)->getVertices());
		CHECK(vertices[0].Normal == v3f{-1.0f, 0.0f, 0.0f});
		CHECK(vertices[1].Normal == v3f{0.0f, -1.0f, 0.0f});
		CHECK(vertices[2].Normal == v3f{0.0f, 0.0f, -1.0f});
		CHECK(vertices[3].Normal == v3f{-1.0f, 0.0f, 0.0f});
		CHECK(vertices[6].Normal == v3f{-1.0f, 0.0f, 0.0f});
		CHECK(vertices[23].Normal == v3f{1.0f, 0.0f, 0.0f});

	}

	SECTION("texture coords are correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
		auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(0)->getVertices());
		CHECK(vertices[0].TCoords == v2f{0.375f, 1.0f});
		CHECK(vertices[1].TCoords == v2f{0.125f, 0.25f});
		CHECK(vertices[2].TCoords == v2f{0.375f, 0.0f});
		CHECK(vertices[3].TCoords == v2f{0.6250f, 1.0f});
		CHECK(vertices[6].TCoords == v2f{0.375f, 0.75f});
	}
}

SECTION("blender cube scaled") {
	const auto mesh = loadMesh(model_stem + "blender_cube_scaled.gltf");
	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 1);

	SECTION("Scaling is correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
		auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(0)->getVertices());

		CHECK(vertices[0].Pos == v3f{-150.0f, -1.0f, -21.5f});
		CHECK(vertices[3].Pos == v3f{-150.0f, 1.0f, -21.5f});
		CHECK(vertices[6].Pos == v3f{-150.0f, -1.0f, 21.5f});
		CHECK(vertices[9].Pos == v3f{-150.0f, 1.0f, 21.5f});
		CHECK(vertices[12].Pos == v3f{150.0f, -1.0f, -21.5f});
		CHECK(vertices[15].Pos == v3f{150.0f, 1.0f, -21.5f});
		CHECK(vertices[18].Pos == v3f{150.0f, -1.0f, 21.5f});
		CHECK(vertices[21].Pos == v3f{150.0f, 1.0f, 21.5f});
	}
}

SECTION("blender cube matrix transform") {
	const auto mesh = loadMesh(model_stem + "blender_cube_matrix_transform.gltf");
	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 1);

	SECTION("Transformation is correct") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
		auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(0)->getVertices());
		const auto checkVertex = [&](const std::size_t i, v3f vec) {
			// The transform scales by (1, 2, 3) and translates by (4, 5, 6).
			CHECK(vertices[i].Pos == vec * v3f{1, 2, 3}
					// The -6 is due to the coordinate system conversion.
					+ v3f{4, 5, -6});
		};
		checkVertex(0, v3f{-1, -1, -1});
		checkVertex(3, v3f{-1, 1, -1});
		checkVertex(6, v3f{-1, -1, 1});
		checkVertex(9, v3f{-1, 1, 1});
		checkVertex(12, v3f{1, -1, -1});
		checkVertex(15, v3f{1, 1, -1});
		checkVertex(18, v3f{1, -1, 1});
		checkVertex(21, v3f{1, 1, 1});
	}
}

SECTION("snow man") {
	const auto mesh = loadMesh(model_stem + "snow_man.gltf");
	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 3);

	SECTION("vertex coordinates are correct for all buffers") {
		REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
		{
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(0)->getVertices());
			CHECK(vertices[0].Pos == v3f{3.0f, 24.0f, -3.0f});
			CHECK(vertices[3].Pos == v3f{3.0f, 18.0f, 3.0f});
			CHECK(vertices[6].Pos == v3f{-3.0f, 18.0f, -3.0f});
			CHECK(vertices[9].Pos == v3f{3.0f, 24.0f, 3.0f});
			CHECK(vertices[12].Pos == v3f{3.0f, 18.0f, -3.0f});
			CHECK(vertices[15].Pos == v3f{-3.0f, 18.0f, 3.0f});
			CHECK(vertices[18].Pos == v3f{3.0f, 18.0f, -3.0f});
			CHECK(vertices[21].Pos == v3f{3.0f, 18.0f, 3.0f});
		}
		{
			REQUIRE(mesh->getMeshBuffer(1)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(1)->getVertices());
			CHECK(vertices[2].Pos == v3f{5.0f, 10.0f, 5.0f});
			CHECK(vertices[3].Pos == v3f{5.0f, 0.0f, 5.0f});
			CHECK(vertices[7].Pos == v3f{-5.0f, 0.0f, 5.0f});
			CHECK(vertices[8].Pos == v3f{5.0f, 10.0f, -5.0f});
			CHECK(vertices[14].Pos == v3f{5.0f, 0.0f, 5.0f});
			CHECK(vertices[16].Pos == v3f{5.0f, 10.0f, -5.0f});
			CHECK(vertices[22].Pos == v3f{-5.0f, 10.0f, 5.0f});
			CHECK(vertices[23].Pos == v3f{-5.0f, 0.0f, 5.0f});
		}
		{
			REQUIRE(mesh->getMeshBuffer(2)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(2)->getVertices());
			CHECK(vertices[1].Pos == v3f{4.0f, 10.0f, -4.0f});
			CHECK(vertices[2].Pos == v3f{4.0f, 18.0f, 4.0f});
			CHECK(vertices[3].Pos == v3f{4.0f, 10.0f, 4.0f});
			CHECK(vertices[10].Pos == v3f{-4.0f, 18.0f, -4.0f});
			CHECK(vertices[11].Pos == v3f{-4.0f, 18.0f, 4.0f});
			CHECK(vertices[12].Pos == v3f{4.0f, 10.0f, -4.0f});
			CHECK(vertices[17].Pos == v3f{-4.0f, 18.0f, -4.0f});
			CHECK(vertices[18].Pos == v3f{4.0f, 10.0f, -4.0f});
		}
	}

	SECTION("vertex indices are correct for all buffers") {
		{
			REQUIRE(mesh->getMeshBuffer(0)->getIndexCount() == 36);
			auto indices = static_cast<const irr::u16 *>(
					mesh->getMeshBuffer(0)->getIndices());
			CHECK(indices[0] == 23);
			CHECK(indices[1] == 21);
			CHECK(indices[2] == 22);
			CHECK(indices[35] == 2);
		}
		{
			REQUIRE(mesh->getMeshBuffer(1)->getIndexCount() == 36);
			auto indices = static_cast<const irr::u16 *>(
					mesh->getMeshBuffer(1)->getIndices());
			CHECK(indices[10] == 16);
			CHECK(indices[11] == 18);
			CHECK(indices[15] == 13);
			CHECK(indices[27] == 5);
		}
		{
			REQUIRE(mesh->getMeshBuffer(2)->getIndexCount() == 36);
			auto indices = static_cast<const irr::u16 *>(
					mesh->getMeshBuffer(2)->getIndices());
			CHECK(indices[26] == 6);
			CHECK(indices[27] == 5);
			CHECK(indices[29] == 6);
			CHECK(indices[32] == 2);
		}
	}


	SECTION("vertex normals are correct for all buffers") {
		{
			REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(0)->getVertices());
			CHECK(vertices[0].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[1].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[2].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[3].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[6].Normal == v3f{-1.0f, 0.0f, -0.0f});
			CHECK(vertices[23].Normal == v3f{0.0f, 0.0f, 1.0f});
		}
		{
			REQUIRE(mesh->getMeshBuffer(1)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(1)->getVertices());
			CHECK(vertices[0].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[1].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[3].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[6].Normal == v3f{-1.0f, 0.0f, -0.0f});
			CHECK(vertices[7].Normal == v3f{-1.0f, 0.0f, -0.0f});
			CHECK(vertices[22].Normal == v3f{0.0f, 0.0f, 1.0f});
		}
		{
			REQUIRE(mesh->getMeshBuffer(2)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
				mesh->getMeshBuffer(2)->getVertices());
			CHECK(vertices[3].Normal == v3f{1.0f, 0.0f, -0.0f});
			CHECK(vertices[4].Normal == v3f{-1.0f, 0.0f, -0.0f});
			CHECK(vertices[5].Normal == v3f{-1.0f, 0.0f, -0.0f});
			CHECK(vertices[10].Normal == v3f{0.0f, 1.0f, -0.0f});
			CHECK(vertices[11].Normal == v3f{0.0f, 1.0f, -0.0f});
			CHECK(vertices[19].Normal == v3f{0.0f, 0.0f, -1.0f});
		}
	}


	SECTION("texture coords are correct for all buffers") {
		{
			REQUIRE(mesh->getMeshBuffer(0)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(0)->getVertices());
			CHECK(vertices[0].TCoords == v2f{0.583333313f, 0.791666686f});
			CHECK(vertices[1].TCoords == v2f{0.583333313f, 0.666666686f});
			CHECK(vertices[2].TCoords == v2f{0.708333313f, 0.791666686f});
			CHECK(vertices[5].TCoords == v2f{0.375f, 0.416666657f});
			CHECK(vertices[6].TCoords == v2f{0.5f, 0.291666657f});
			CHECK(vertices[19].TCoords == v2f{0.708333313f, 0.75f});
		}
		{
			REQUIRE(mesh->getMeshBuffer(1)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(1)->getVertices());

			CHECK(vertices[1].TCoords == v2f{0.0f, 0.791666686f});
			CHECK(vertices[4].TCoords == v2f{0.208333328f, 0.791666686f});
			CHECK(vertices[5].TCoords == v2f{0.0f, 0.791666686f});
			CHECK(vertices[6].TCoords == v2f{0.208333328f, 0.583333313f});
			CHECK(vertices[12].TCoords == v2f{0.416666657f, 0.791666686f});
			CHECK(vertices[15].TCoords == v2f{0.208333328f, 0.583333313f});
		}
		{
			REQUIRE(mesh->getMeshBuffer(2)->getVertexCount() == 24);
			auto vertices = static_cast<const irr::video::S3DVertex *>(
					mesh->getMeshBuffer(2)->getVertices());
			CHECK(vertices[10].TCoords == v2f{0.375f, 0.416666657f});
			CHECK(vertices[11].TCoords == v2f{0.375f, 0.583333313f});
			CHECK(vertices[12].TCoords == v2f{0.708333313f, 0.625f});
			CHECK(vertices[17].TCoords == v2f{0.541666687f, 0.458333343f});
			CHECK(vertices[20].TCoords == v2f{0.208333328f, 0.416666657f});
			CHECK(vertices[22].TCoords == v2f{0.375f, 0.416666657f});
		}
	}
}

// https://github.com/KhronosGroup/glTF-Sample-Models/tree/main/2.0/SimpleSparseAccessor
SECTION("simple sparse accessor")
{
	const auto mesh = loadMesh(model_stem + "simple_sparse_accessor.gltf");
	REQUIRE(mesh != nullptr);
	const auto *vertices = reinterpret_cast<irr::video::S3DVertex *>(
			mesh->getMeshBuffer(0)->getVertices());
	const std::array<v3f, 14> expectedPositions = {
			// Lower
			v3f(0, 0, 0),
			v3f(1, 0, 0),
			v3f(2, 0, 0),
			v3f(3, 0, 0),
			v3f(4, 0, 0),
			v3f(5, 0, 0),
			v3f(6, 0, 0),
			// Upper
			v3f(0, 1, 0),
			v3f(1, 2, 0), // overridden
			v3f(2, 1, 0),
			v3f(3, 3, 0), // overridden
			v3f(4, 1, 0),
			v3f(5, 4, 0), // overridden
			v3f(6, 1, 0),
	};
	for (std::size_t i = 0; i < expectedPositions.size(); ++i)
		CHECK(vertices[i].Pos == expectedPositions[i]);
}

}
