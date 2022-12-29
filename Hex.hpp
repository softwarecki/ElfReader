/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2023 Koko Software. All rights reserved.
 *
 * Author: Adrian Warecki <embedded@kokosoftware.pl>
 */

#ifndef __HEX_HPP__
#define __HEX_HPP__

#include <filesystem>
#include <fstream>
#include <span>

class Sections {
	public:
		virtual void process(uint32_t address, std::span<uint8_t> data);
};

class Hex {
	public:
		Hex(std::filesystem::path path);
		void print();

	protected:
		std::ifstream file;


	private:
		uint32_t start_address;
		uint32_t segment_address;
		std::vector<uint8_t> payload;
		Sections sections;

		void read_file();
		void parse_line(const std::string line);
		bool process_line();

};

#endif /* __HEX_HPP__ */