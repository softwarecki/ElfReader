// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2023 Koko Software. All rights reserved.
//
// Author: Adrian Warecki <embedded@kokosoftware.pl>

#include "types.hpp"
#include "Elf.hpp"

using namespace elf;

SectionHeader::SectionHeader() {
	std::memset(dynamic_cast<Elf32_Shdr*>(this), 0, sizeof(Elf32_Shdr));
}

SectionHeader::SectionHeader(std::istream *stream, std::streamsize file_size) {
	stream->read(reinterpret_cast<char*>(
		dynamic_cast<Elf32_Shdr*>(this)), sizeof(Elf32_Shdr));

	if (file_size && (type != SHT_NOBITS && (off + size > file_size)))
		throw Exception("Invalid section header.");
}

void SectionHeader::update_name(const StringsTable &str) {
	name_str = str.get(name);
}

void Section::read(std::istream* stream, const SectionHeader* header, std::streamsize file_size) {
	if (header->type == SHT_NOBITS)
		throw Exception("Cannot read SHT_NOBITS section.");

	this->header = *header;

	if (file_size && (header->off + header->size > file_size))
		throw Exception("Invalid section position in file.");

	buffer = std::shared_ptr<unsigned char[]>(new unsigned char[header->size]);
	stream->seekg(header->off, std::ios_base::beg);
	stream->read(reinterpret_cast<char*>(buffer.get()), header->size);
}



std::string StringsTable::get(unsigned int index) const {
	// TODO: Check index, return string from buffer
	return std::string(reinterpret_cast<const char*>(buffer.get() + index));
}

void StringsTable::print()
{
	const char *str = reinterpret_cast<const char *>(buffer.get());
	const char *end = str + header.size;
	while (str < end) {
		int len = strlen(str);
		printf("%s\n", str);
		str += len + 1;
	}
}

#if 0
typedef struct {
	Elf32_Word	name;	/* String table index of name. */
	Elf32_Addr	value;	/* Symbol value. */
	Elf32_Word	size;	/* Size of associated object. */
	unsigned char	info;	/* Type and binding information. */
	unsigned char	other;	/* Reserved (not used). */
	Elf32_Half	shndx;	/* Section index of symbol. */
} Elf32_Sym;
#endif

uint32_t SymbolTable::get(std::string name) {
	return 0;
}

void SymbolTable::print(const StringsTable* str) {
	const Elf32_Sym *sym = reinterpret_cast<Elf32_Sym *>(buffer.get());
	const Elf32_Sym *end = sym + header.size / sizeof(*sym);

	printf("name      value     size info other shndx\n");

	while (sym < end) {
		printf("%4u 0x%08X %8u %4u %5u %5u ", 
		       sym->name, sym->value, sym->size, sym->info, sym->other, sym->shndx);
		if (str)
			printf("%s", str->get(sym->name).c_str());
		printf("\n");
		sym++;
	}
}

void e_type(int type) {
#define X(x) do { if (type == x) { printf(#x "\n"); return; } } while (0)
	X(ET_NONE);	// No file type
	X(ET_REL);	// Relocatable file
	X(ET_EXEC);	// Executable file
	X(ET_DYN);	// Shared object file
	X(ET_CORE);	// Core file
	printf("Unknown\n");
#undef X
}

void sh_type(int type) {
#define X(x) if (type == x) printf(#x "\n")
	X(SHT_NULL);	/* inactive */
	X(SHT_PROGBITS);	/* program defined information */
	X(SHT_SYMTAB);	/* symbol table section */
	X(SHT_STRTAB);	/* string table section */
	X(SHT_RELA);	/* relocation section with addends */
	X(SHT_HASH);	/* symbol hash table section */
	X(SHT_DYNAMIC);	/* dynamic section */
	X(SHT_NOTE);	/* note section */
	X(SHT_NOBITS);	/* no space section */
	X(SHT_REL);	/* relocation section - no addends */
	X(SHT_SHLIB);	/* reserved - purpose unknown */
	X(SHT_DYNSYM);	/* dynamic symbol table section */
	X(SHT_INIT_ARRAY);	/* Initialization function pointers. */
	X(SHT_FINI_ARRAY);	/* Termination function pointers. */
	X(SHT_PREINIT_ARRAY);	/* Pre-initialization function ptrs. */
	X(SHT_GROUP);	/* Section group. */
	X(SHT_SYMTAB_SHNDX);	/* Section indexes (see SHN_XINDEX). */
	X(SHT_LOOS);	/* First of OS specific semantics */
	X(SHT_HIOS);	/* Last of OS specific semantics */
	X(SHT_GNU_VERDEF);
	X(SHT_GNU_VERNEED);
	X(SHT_GNU_VERSYM);
	X(SHT_LOPROC);	/* reserved range for processor */
	X(SHT_HIPROC);	/* specific section header types */
	X(SHT_LOUSER);	/* reserved range for application */
	X(SHT_HIUSER);	/* specific indexes */
#undef X
}

void sh_flags(int type) {
#define X(x) if (type & x) printf(" " #x)
	X(SHF_WRITE); /* Section contains writable data. */
	X(SHF_ALLOC); /* Section occupies memory. */
	X(SHF_EXECINSTR); /* Section contains instructions. */
	X(SHF_MERGE); /* Section may be merged. */
	X(SHF_STRINGS); /* Section contains strings. */
	X(SHF_INFO_LINK); /* sh_info holds section index. */
	X(SHF_LINK_ORDER); /* Special ordering requirements. */
	X(SHF_OS_NONCONFORMING); /* OS-specific processing required. */
	X(SHF_GROUP); /* Member of section group. */
	X(SHF_TLS); /* Section contains TLS data. */
	X(SHF_MASKOS); /* OS-specific semantics. */
	X(SHF_MASKPROC); /* Processor-specific semantics. */
	printf("\n");
#undef X
}

void ph_type(int type) {
#define X(x) if (type == x) printf(#x "\n")
	X(PT_NULL);/* Unused entry. */
	X(PT_LOAD);/* Loadable segment. */
	X(PT_DYNAMIC);/* Dynamic linking information segment. */
	X(PT_INTERP);/* Pathname of interpreter. */
	X(PT_NOTE);/* Auxiliary information. */
	X(PT_SHLIB);/* Reserved (not used). */
	X(PT_PHDR);/* Location of program header itself. */
	X(PT_TLS);/* Thread local storage segment */
#undef X
}

void ph_flags(int type) {
#define X(x) if (type & x) printf(" " #x);
	X(PF_X); /* Executable. */
	X(PF_W); /* Writable. */
	X(PF_R); /* Readable. */
	printf("\n");
#undef X
}

Elf::Elf(std::filesystem::path path)
	: file(path, std::ios::binary)
{
	if (!file.is_open())
		throw String(_T("File open error."));

	file.seekg(0, std::ios_base::end);
	file_size = file.tellg();

	read_header();
	read_sections();

	update_section_names();

	read_programs();
}



void Elf::read(void* buf, std::streamsize size) {
	file.read(static_cast<char*>(buf), size);

	if (file.fail())
		throw String(_T("File read error."));
}

void Elf::read_header() {
	// ELFMAG ELFCLASS32 ELFDATA2LSB EV_CURRENT
	constexpr const char supported_header[] = "\177ELF\x01\x01\x01";

	file.seekg(0, std::ios_base::beg);

	// Read file header
	read(&file_header, sizeof(file_header));
	if (std::strncmp(reinterpret_cast<const char*>(file_header.ident), supported_header, sizeof(supported_header) - 1))
		throw Exception("Unsupported elf file.");

	if (file_header.version != EV_CURRENT)
		throw Exception("Unsupported file version.");

	if (file_header.ehsize < sizeof(Elf32_Ehdr))
		throw Exception("Invalid file header size.");

	if (file_header.phoff >= file_size)
		throw Exception("Invalid program header file offset.");

	if (file_header.phentsize < sizeof(Elf32_Phdr))
		throw Exception("Invalid program header size.");

	if (file_header.phoff + file_header.phnum * sizeof(Elf32_Phdr) > file_size)
		throw Exception("Invalid number of program header entries"); //off + count*size

	if (file_header.shoff >= file_size)
		throw Exception("Invalid section header file offset.");

	if (file_header.shentsize < sizeof(Elf32_Shdr))
		throw Exception("Invalid section header size.");

	if (file_header.shoff + file_header.shnum * sizeof(Elf32_Shdr) > file_size)
		throw Exception("Invalid number of section header entries");

	if (file_header.shstrndx >= file_header.shnum)
		throw Exception("Invalid section name strings section index.");
}

void Elf::read_programs() {
	programs.resize(file_header.phnum);

	off_t pos = file_header.phoff;
	for (auto idx = 0; idx < file_header.phnum; idx++) {
		file.seekg(pos, std::ios_base::beg);

		read(&programs[idx], sizeof(programs[0]));

		if ((programs[idx].filesz > programs[idx].memsz) ||
			(programs[idx].off && programs[idx].filesz && (programs[idx].off + programs[idx].filesz > file_size)))
			throw Exception("Invalid program header.");

		pos += file_header.phentsize;
	}
}

void Elf::read_sections() {
	sections.reserve(file_header.shnum);

	off_t pos = file_header.shoff;
	for (auto idx = 0; idx < file_header.shnum; idx++) {
		file.seekg(pos, std::ios_base::beg);

		sections.emplace_back(&file, file_size);

		pos += file_header.shentsize;
	}
}

void Elf::update_section_names() {
	StringsTable strings;

	read_section(strings, file_header.shstrndx);

	for (auto idx = 0; idx < sections.size(); idx++)
		sections[idx].update_name(strings);
}

int Elf::find_section(const std::string name) {
	for (auto idx = 0; idx < sections.size(); idx++)
		if (sections[idx].name_str == name)
			return idx;

	return -1;
}

void Elf::read_section(Section& section, unsigned int index) {
	if (index >= sections.size())
		throw String(_T("Invalid section index."));

	section.read(&file, &sections[index], file_size);
}

void Elf::read_section(Section &section, const std::string name) {
	int index = find_section(name);
	if (index < 0)
		throw Exception("Section not found");

	read_section(section, index);
}



#if 0
// Read firmware image from elf file based on Program headers
void Elf::read_image(ImageInterface& image) {
	for (const Elf32_Phdr &hdr : programs) {
		// Use only load headers with content in file
		if ((hdr.type != PT_LOAD) || !hdr.filesz)
			continue;

		auto buf = image.process(hdr.paddr, hdr.memsz);
		file.seekg(hdr.off, std::ios_base::beg);
		read(buf.data(), hdr.filesz);

		//printf("0x%0X - 0x%0X; 0x%0X bytes (0x%0X in from file)\n",
		//	   hdr.paddr, hdr.paddr + hdr.memsz - 1, hdr.memsz, hdr.filesz);
	}
}

// Read firmware image from elf file based on Section headers
void Elf::read_image2(ImageInterface& image) {
	for (const Elf32_Shdr& hdr : sections) {
		// Use only load headers with content in file
		if (hdr.type != SHT_PROGBITS)
			continue;

		assert(hdr.flags & SHF_ALLOC);

		auto buf = image.process(hdr.vaddr, hdr.size);
		file.seekg(hdr.off, std::ios_base::beg);
		read(buf.data(), hdr.size);

		//printf("0x%0X - 0x%0X; 0x%0X bytes (%s)\n",
		//	hdr.vaddr, hdr.vaddr + hdr.size - 1, hdr.size, strings.get(hdr.name).c_str());
	}
}
#endif

void Elf::print() {
	printf("File type: 0x%04x ", file_header.type);
	e_type(file_header.type);
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

	for (auto idx = 0; idx < file_header.shnum; idx++) {
		SectionHeader& sect = sections[idx];
		printf("\nSection %u (%s [%u])\n", idx, sect.name_str.c_str(), sect.name);
		printf("\tSection name index: 0x%04x\n", sect.name);
		printf("\tSection type: 0x%04x ", sect.type);
		sh_type(sect.type);
		printf("\tSection flags: 0x%04x\n", sect.flags);
		sh_flags(sect.flags);
		printf("\tAddress in memory image: 0x%04x\n", sect.vaddr);
		printf("\tOffset in file: 0x%04x\n", sect.off);
		printf("\tSize in bytes: 0x%04x\n", sect.size);
		printf("\tIndex of a related section: 0x%04x\n", sect.link);
		printf("\tDepends on section type: 0x%04x\n", sect.info);
		printf("\tAlignment in bytes: 0x%04x\n", sect.addralign);
		printf("\tSize of each entry in section: 0x%04x\n", sect.entsize);
#if 0
		if (sect.type == SHT_STRTAB) {
			StringSection str;
			read_section(str, idx);
			str.print();
		}
#endif
	}

	for (auto idx = 0; idx < file_header.phnum; idx++) {
		Elf32_Phdr& prog = programs[idx];

		// TODO: Program header validation
		printf("\nProgram header %d:\n", idx);
		printf("\tEntry type: 0x%0x\n", prog.type);
		ph_type(prog.type);
		printf("\tFile offset of contents: 0x%0x\n", prog.off);
		printf("\tVirtual address in memory image: 0x%0x\n", prog.vaddr);
		printf("\tPhysical address (not used): 0x%0x\n", prog.paddr);
		printf("\tSize of contents in file: 0x%0x\n", prog.filesz);
		printf("\tSize of contents in memory: 0x%0x\n", prog.memsz);
		printf("\tAccess permission flags: 0x%0x\n", prog.flags);
		ph_flags(prog.flags);
		printf("\tAlignment in memory and file: 0x%0x\n", prog.align);

		if (!(prog.off && prog.filesz)) {
			if (prog.off)
				printf("Warning! Non zero file offset (%u) when filesz = 0.\n", prog.off);

			if (prog.filesz)
				printf("Warning! Non zero file size (%u) when offset = 0.\n", prog.filesz);
		}
	}
}





