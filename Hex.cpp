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
	: file(path), start_address(0), segment_address(0)
{
	if (!file.is_open())
		throw Exception("File open error.");

	read_file();
}

void Hex::print() {

}

enum {
	RT_DATA,				// Data
	RT_EOF,					// End Of File
	RT_EXT_SEG_ADDR,		// Extended Segment Address
	RT_START_SEG_ADDR,		// Start Segment Address
	RT_EXT_ADDR,			// Extended Linear Address
	RT_START_ADDR,			// Start Linear Address
};

enum {
	POS_BYTE_CNT,		// Byte count
	POS_ADDR_H,			// Address
	POS_ADDR_L,
	POS_TYPE,			// Record type
	POS_PAYLOAD,		// Start of a payload
	POS_CHECKSUM = -1,	// Checksum
};

constexpr int MIN_LENGTH = 5;

// TODO: Use string view
void Hex::parse_line(const std::string line) {
	uint8_t checksum = 0;
	uint8_t value;

	size_t length = line.length() / 2;

	if ((line.length() & 1) || (length < MIN_LENGTH))
		throw Exception("Invalid line length.");

	payload.reserve(length);
	payload.clear();

	for (size_t i = 0; i < length; i++) {
		auto ret = std::from_chars(line.data() + i * 2, line.data() + i * 2 + 2, value, 16);
		
		if (ret.ec != std::errc())
			throw Exception("Invalid character in record.");

		payload.push_back(value);
		checksum += value;
	}

	if (checksum)
		throw Exception("Invalid line checksum.");

	if (payload.size() != (payload[POS_BYTE_CNT] + MIN_LENGTH))
		throw Exception("Invalid record size.");
}

bool Hex::process_line() {
#define CHECK_SIZE(x) if (payload[POS_BYTE_CNT] != (x)) throw Exception("Invalid record byte count.")

	switch (payload[POS_TYPE]) {
		case RT_DATA:
		{
			uint32_t address = (payload[POS_ADDR_H] << 8) | payload[POS_ADDR_L];
			sections.process(segment_address + address, std::span<uint8_t>(&payload[POS_PAYLOAD], payload[POS_BYTE_CNT]));
			std::cout << " RT_DATA";
			break;
		}

		case RT_EOF:
			CHECK_SIZE(0);
			std::cout << " RT_EOF";
			return true;

		case RT_EXT_SEG_ADDR:
			std::cout << " RT_EXT_SEG_ADDR";
			CHECK_SIZE(2);
			segment_address = ((payload[POS_PAYLOAD] << 8) | payload[POS_PAYLOAD + 1]) * 16;
			break;

		case RT_START_SEG_ADDR:
			std::cout << " RT_START_SEG_ADDR";
			CHECK_SIZE(4);
			start_address = ((payload[POS_PAYLOAD + 0] << 8) | payload[POS_PAYLOAD + 1]) * 16 +
							((payload[POS_PAYLOAD + 2] << 8) | payload[POS_PAYLOAD + 3]);
			break;

		case RT_EXT_ADDR:
			std::cout << " RT_EXT_ADDR";
			CHECK_SIZE(2);
			segment_address = (payload[POS_PAYLOAD] << 24) | (payload[POS_PAYLOAD + 1] << 16);
			break;

		case RT_START_ADDR:
			CHECK_SIZE(4);
			start_address = (payload[POS_PAYLOAD + 0] << 24) | (payload[POS_PAYLOAD + 1] << 16) |
							(payload[POS_PAYLOAD + 2] << 8) | payload[POS_PAYLOAD + 3];
			std::cout << " RT_START_ADDR";
			break;

		default:
			throw Exception("Unknown recod type.");
	}

	return false;
}


void Hex::read_file() {
	size_t start;
	std::string line;
	bool done = false;

	while (!done && std::getline(file, line)) {
		std::cout << line;

		// 1. Find colon ':'
		start = line.find(':');
		if (start == std::string::npos)
			continue;

		parse_line(line.substr(start + 1));
		done = process_line();

		std::cout << std::endl;
	}

	if (!done)
		throw Exception("Unexcepted end of file.");
}




void Sections::process(uint32_t address, std::span<uint8_t> data) {
	std::cout << " 0x" << std::hex << address << " 0x" << data.size_bytes();

}