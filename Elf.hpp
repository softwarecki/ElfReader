/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2023 Koko Software. All rights reserved.
 *
 * Author: Adrian Warecki <embedded@kokosoftware.pl>
 */

#ifndef __ELF_HPP__
#define __ELF_HPP__

#include <fstream>
#include <filesystem>
#include <vector>

#include "elf.h"


namespace elf {
	class StringsTable;

	class SectionHeader : public Elf32_Shdr {
		public:
			SectionHeader();
			SectionHeader(std::istream* stream, std::streamsize file_size = 0);

			void update_name(const StringsTable &str);
			
			std::string name_str;
	};

	class Section {
		public:
			void read(std::istream* stream, const SectionHeader* header,
				  std::streamsize file_size = 0);

		protected:
			SectionHeader header;
			std::shared_ptr<unsigned char[]> buffer;
	};

	class StringsTable: public Section {
		public:
			std::string get(unsigned int index) const;
			void print();
	};

	class SymbolTable: public Section {
		public:
			uint32_t get(std::string name);
			void print(const StringsTable* str = nullptr);
	};

	class Elf {
		public:
			Elf(std::filesystem::path path);
			void print();
			void read_section(Section &section, unsigned int index);
			void read_section(Section &section, std::string name);
			int find_section(const std::string name);

		protected:
			std::ifstream file;
			std::streamsize file_size;
			std::vector<SectionHeader> sections;
			std::vector<Elf32_Phdr> programs;

		private:
			Elf32_Ehdr file_header;

			void read(void *buf, std::streamsize size);
			void read_header();
			void read_programs();
			void read_sections();
			void update_section_names();
	};

};

#endif /* __ELF_HPP__ */
