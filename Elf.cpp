// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2023 Koko Software. All rights reserved.
//
// Author: Adrian Warecki <embedded@kokosoftware.pl>

#include "types.hpp"
#include "Elf.hpp"

Elf::Elf(std::filesystem::path path)
	: file(path, std::ios::binary)
{
	if (!file.is_open())
		throw String(_T("File open error."));

	file.seekg(0, std::ios_base::end);
	file_size = file.tellg();

	read_header();

	read_section(strings, file_header.shstrndx);

	read_sections();
}

void Elf::read(void* buf, std::streamsize size) {
	file.read(static_cast<char*>(buf), size);

	if (file.fail())
		throw String(_T("File read error."));
}

void Elf::read_header() {
	// ELFMAG ELFCLASS32 ELFDATA2LSB EV_CURRENT
	const char supported_header[] = "\177ELF\x01\x01\x01";

	file.seekg(0, std::ios_base::beg);

	// Read file header
	read(&file_header, sizeof(file_header));
	if (std::strncmp(reinterpret_cast<const char*>(file_header.ident), supported_header, sizeof(supported_header) - 1))
		throw String(_T("Unsupported elf file."));

	// TODO: Validate file header
}

void Elf::read_programs() {

}

void Elf::read_sections() {
	file.seekg(file_header.shoff, std::ios_base::beg);
#if 1
	Elf32_Shdr sect;

	for (unsigned int idx = 0; idx < file_header.shnum; idx++) {
		read(&sect, sizeof(sect));
		printf("Section %u (%s)\n", idx, strings.buffer.get() + sect.name);
		printf("\tSection name index: 0x%04x\n", sect.name);
		printf("\tSection type: 0x%04x\n", sect.type);
		printf("\tSection flags: 0x%04x\n", sect.flags);
		printf("\tAddress in memory image: 0x%04x\n", sect.vaddr);
		printf("\tOffset in file: 0x%04x\n", sect.off);
		printf("\tSize in bytes: 0x%04x\n", sect.size);
		printf("\tIndex of a related section: 0x%04x\n", sect.link);
		printf("\tDepends on section type: 0x%04x\n", sect.info);
		printf("\tAlignment in bytes: 0x%04x\n", sect.addralign);
		printf("\tSize of each entry in section: 0x%04x\n", sect.entsize);

	}
#endif
}

void Elf::print() {
	printf("File type: 0x%04x\n", file_header.type);
	printf("Machine architecture: 0x%04x\n", file_header.machine);
	printf("ELF format version: 0x%08x\n", file_header.version);
	printf("Entry point: 0x%08x\n", file_header.entry);
	printf("Program header file offset: 0x%08x\n", file_header.phoff);
	printf("Section header file offset: 0x%08x\n", file_header.shoff);
	printf("Architecture-specific flags: 0x%08x\n", file_header.flags);
	printf("Size of ELF header in bytes: 0x%04x\n", file_header.ehsize);
	printf("Size of program header entry: 0x%04x\n", file_header.phentsize);
	printf("Number of program header entries: 0x%04x\n", file_header.phnum); //off + count*size
	printf("Size of section header entry: 0x%04x\n", file_header.shentsize);// sizeof(Elf32_Shdr)
	printf("Number of section header entries: 0x%04x\n", file_header.shnum);
	printf("Section name strings section: 0x%04x\n", file_header.shstrndx);
}

void Elf::read_section(Section& section, unsigned int index) {
	if (index >= file_header.shnum)
		throw String(_T("Invalid section index."));

	file.seekg(file_header.shoff + index * file_header.shentsize, std::ios_base::beg);
	read(reinterpret_cast<char*>(&section.header), sizeof(section.header));

	// TODO: Validate size
	// TODO: Validate offset

	section.buffer = std::shared_ptr<unsigned char[]>(new unsigned char[section.header.size]);
	file.seekg(section.header.off, std::ios_base::beg);
	read(reinterpret_cast<char*>(section.buffer.get()), section.header.size);
}

std::string StringSection::get(unsigned int index) {
	// TODO: Check index, return string from buffer
	return std::string(reinterpret_cast<const char*>(buffer.get() + index));
}