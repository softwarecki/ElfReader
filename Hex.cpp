// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2023 Koko Software. All rights reserved.
//
// Author: Adrian Warecki <embedded@kokosoftware.pl>

#include <iostream>
#include <vector>

#include "types.hpp"

#include "Hex.hpp"

Hex::Hex(std::filesystem::path path)
	: file(path)
{
	if (!file.is_open())
		throw String(_T("File open error."));

	read_file();
}

void Hex::print() {

}
// TODO: Use string view

void Hex::convert_hex(std::vector<uint8_t>& payload, const std::string line) {
	uint8_t checksum = 0;
	uint8_t value = 0;

	size_t length = line.length() / 2;

	for (size_t i = 0; i < length; i++) {
		std::from_chars(line.data() + i * 2, line.data() + i * 2 + 1, value, 16);
		payload.push_back(value);
		checksum += value;
	}

	if (checksum)
		throw String(_T("Corrupted hex file."));
}

void Hex::read_file() {
	size_t start;
	std::string line;
	std::vector<uint8_t> payload;

	while (std::getline(file, line)) {
		std::cout << line;

		// 1. Find colon ':'
		start = line.find(':');
		if ((start == std::string::npos) || ((line.length() - start) & 1))
			throw String(_T("Invalid hex file format."));

		// Convert hex to bytes
		convert_hex(payload, line.substr(start));

		std::cout << std::endl;
	}
}
