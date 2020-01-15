/*
 * This file is part of ClanBomber;
 * you can get it at "http://www.nongnu.org/clanbomber".
 *
 * Copyright (C) 2009, 2010 Rene Lopez <rsl@members.fsf.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "UtilsGetHome.h"

#include <filesystem>

bool RecursiveDirCreation(const std::filesystem::path &path) {
	if (std::filesystem::exists(path)) {
		return true;
	} else {
		if (RecursiveDirCreation(path.parent_path())) {
			try {
				std::filesystem::create_directory(path);
				return true;
			} catch (const std::filesystem::filesystem_error &fee) {
				return false;
			}
		} else {
			return false;
		}
	}
}

//----------POSIX----------
#ifdef __unix__
# include <cstdlib>

std::filesystem::path GetHome() {
	const char* var = std::getenv("HOME");
	if (var == NULL) {
		//TODO throw an error
		return std::filesystem::path("");
	}
	std::filesystem::path Home(var);
	return Home;
}

std::filesystem::path GetDataHome() {
	const char* var = std::getenv("XDG_DATA_HOME");
	std::filesystem::path DataHome;

	if (var == NULL || *var == '\0') {
		DataHome = GetHome() / ".local/share";
	} else {
		DataHome = var;
	}
	return DataHome;
}

std::filesystem::path GetConfigHome() {
	const char* var = std::getenv("XDG_CONFIG_HOME");
	std::filesystem::path ConfigHome;

	if (var == NULL || *var == '\0') {
		ConfigHome = GetHome() / ".config";
	} else {
		ConfigHome = var;
	}
	return ConfigHome;
}

//----------WINDOWS----------
#elif defined(_WIN32)
# include <windows.h>
# include <shlobj.h>

//SHGetFolderPath is deprecated in Vista use SHGetKnownFolderPath instead.
std::filesystem::path GetHome()
{
	char cpath[MAX_PATH];
	//CSIDL_FLAG_CREATE ensures that the folder exists creating it if it doesn't.
	SHGetFolderPath(NULL, CSIDL_PROFILE | CSIDL_FLAG_CREATE, NULL, 0, cpath);
	return std::filesystem::path(cpath);
}

std::filesystem::path GetDataHome()
{
	char cpath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0,
			cpath);
	return std::filesystem::path(cpath);
}

std::filesystem::path GetConfigHome()
{
	char cpath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0,
			cpath);
	return std::filesystem::path(cpath);
}

//----------ERROR----------
#else
# error "Unsupported platform"
#endif
