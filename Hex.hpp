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

class Hex {
	public:
		Hex(std::filesystem::path path);
		void print();

	protected:
		std::ifstream file;

	private:
		void read_file();
		void convert_hex(std::vector<uint8_t>& payload, const std::string line);

};

#endif /* __HEX_HPP__ */