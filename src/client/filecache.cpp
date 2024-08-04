/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
Copyright (C) 2013 Jonathan Neuschäfer <j.neuschaefer@gmx.net>

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

#include "filecache.h"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <system_error>

#include "network/networkprotocol.h"
#include "log.h"

namespace fs = std::filesystem;

void FileCache::createDir() {
	std::error_code ec;
	if (!fs::create_directories(m_dir, ec) && ec) {
		errorstream << "Could not create cache directory: " << m_dir << " - " << ec.message() << '\n';
	}
}

bool FileCache::loadByPath(const std::string &path, std::ostream &os) {
	std::ifstream fis(path, std::ios::binary);
	if (!fis) {
		return false;
	}

	os << fis.rdbuf();
	if (fis.bad()) {
		errorstream << "FileCache: Failed to read file from cache: \"" << path << "\"" << '\n';
		return false;
	}

	return true;
}

bool FileCache::updateByPath(const std::string &path, std::string_view data) {
	createDir();

	std::ofstream file(path, std::ios::binary);
	if (!file) {
		return false;
	}

	file.write(data.data(), data.size());
	if (file.fail()) {
		errorstream << "FileCache: Failed to write file to cache: \"" << path << "\"" << '\n';
		return false;
	}

	return true;
}

bool FileCache::update(const std::string &name, std::string_view data) {
	std::string path = m_dir + fs::path::preferred_separator + name;
	return updateByPath(path, data);
}

bool FileCache::load(const std::string &name, std::ostream &os) {
	std::string path = m_dir + fs::path::preferred_separator + name;
	return loadByPath(path, os);
}

bool FileCache::exists(const std::string &name) {
	std::string path = m_dir + fs::path::preferred_separator + name;
	return fs::exists(path);
}

bool FileCache::updateCopyFile(const std::string &name, const std::string &src_path) {
	std::string path = m_dir + fs::path::preferred_separator + name;
	createDir();

	std::error_code ec;
	fs::copy_file(src_path, path, fs::copy_options::overwrite_existing, ec);
	if (ec) {
		errorstream << "FileCache: Failed to copy file from \"" << src_path << "\" to \"" << path << "\" - "
					<< ec.message() << '\n';
		return false;
	}

	return true;
}
