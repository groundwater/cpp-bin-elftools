#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "elf.h"

#define fmt(word) printf("%s\n", #word);
#define C(header) case header: printf("- %s\n", #header); break;
#define D(err) default: printf("%s\n", #err);
#define M(X,V) if (X & V) { printf("%s\n", #X); }

int main(int argc, char *argv[]) {
  char * fName = argv[1];
  FILE * fd;

  fd = fopen(fName, "r");
  if (fd < 0) {
    printf("FAIL\n");
    exit(-1);
  }

  struct stat sb;
  if (stat(fName, &sb) == -1) {
    printf("Stat fail\n");
    exit(-1);
  }

  void * buffer = malloc(sizeof(char)*sb.st_size);

  if(fread(buffer,1,sb.st_size,fd) == -1) {
    printf("Fail Read\n");
    exit(-1);
  }

  Elf64_Ehdr * eh = (Elf64_Ehdr *) buffer;

  printf("Testing Magic Bytes: 0x%x %c%c%c\n",
      eh->e_ident[EI_MAG0], 
      eh->e_ident[EI_MAG1],
      eh->e_ident[EI_MAG2],
      eh->e_ident[EI_MAG3]);

  switch(eh->e_ident[EI_CLASS]) {
    case ELFCLASSNONE:
      printf("Invalid Class\n");
      break;
    case ELFCLASS32:
      printf("32 Bit Class\n");
      break;
    case ELFCLASS64:
      printf("64 Bit Class\n");
      break;
    D(Unknown Class)
  }

  switch(eh->e_ident[EI_DATA]) {
    case ELFDATANONE:
      printf("Invalid Encoding\n");
      break;
    case ELFDATA2LSB:
      printf("Little Endian\n");
      break;
    case ELFDATA2MSB:
      printf("Big Endian\n");
      break;
    D(Unknown Encoding)
  }

  switch(eh->e_ident[EI_VERSION]) {
    C(EV_NONE)
    C(EV_CURRENT)
    D(Unknown Version)
  }

  switch(eh->e_ident[EI_OSABI]) {
    C(ELFOSABI_LINUX)
    D(Unknown ABI)
  }

  printf("Current Type: ");
  switch(eh->e_type) {
    C(ET_EXEC)
    C(ET_REL)
    C(ET_DYN)
    C(ET_NONE)
    D(Unknown Type)
  }

  // https://github.com/torvalds/linux/blob/5924bbecd0267d87c24110cbe2041b5075173a25/include/uapi/linux/elf-em.h#L20-L45
  switch(eh->e_machine) {
    C(EM_860)
    C(EM_X86_64)
    D(Unknown Architecture)
  }

  Elf64_Off phoff = eh->e_phoff;
  Elf64_Half pnum = eh->e_phnum;
  Elf64_Half psize = eh->e_phentsize;

  printf("PH Offset: %llu\n", phoff);
  printf("PH Size: %d\n", psize);

  char * ph0 = ((char*)eh + phoff);
  
  for(int i=0; i<pnum; i++) {
    Elf64_Phdr * ph = (Elf64_Phdr*) (ph0 + i*psize);

    printf("PH(%d): ", i);
    switch(ph->p_type) {
      C(PT_NULL)
      C(PT_LOAD)
      C(PT_DYNAMIC)
      C(PT_INTERP)
      C(PT_NOTE)
      C(PT_SHLIB)
      C(PT_PHDR)
      C(PT_LOPROC)
      C(PT_HIPROC)
      default:
        printf("Unknown (%x)\n", ph->p_type);
    }

    printf("PH/Flags: ");
    if (ph->p_flags & PF_W) {
      printf("W");
    }
    if (ph->p_flags & PF_R) {
      printf("R");
    }
    if (ph->p_flags & PF_X) {
      printf("X");
    }
    printf("\n");
  }

  printf("Section Header String Index: %d\n", eh->e_shstrndx);

  Elf64_Off shoff = eh->e_shoff;
  Elf64_Half shsize = eh->e_shentsize;
  Elf64_Half shnum = eh->e_shnum;
  Elf64_Half shidx = eh->e_shstrndx;

  char * sh0 = ((char*)eh + shoff);

  Elf64_Shdr * strSect = (Elf64_Shdr*) (sh0 + eh->e_shstrndx*shsize);
  printf("Str Index %x\n", eh->e_shstrndx);

  Elf64_Shdr * stringTable;
  Elf64_Shdr * symTable;
  Elf64_Shdr * relaTable;
  Elf64_Shdr * relTable;

  for(int i=0; i<shnum; i++) {
    Elf64_Shdr * sh = (Elf64_Shdr*) (sh0 + i*shsize);
    
    char* strPtr = (char*)eh + strSect->sh_offset + sh->sh_name;

    printf("Section Name Index: %s\n", strPtr);

    switch(sh->sh_type) {
      C(SHT_NULL)
      C(SHT_PROGBITS)
      C(SHT_SYMTAB)
      C(SHT_STRTAB)
      C(SHT_RELA)
      C(SHT_HASH)
      C(SHT_DYNAMIC)
      C(SHT_NOTE)
      C(SHT_NOBITS)
      C(SHT_REL    )
      C(SHT_SHLIB)
      C(SHT_DYNSYM)
      C(SHT_NUM    )
      C(SHT_LOPROC)
      C(SHT_HIPROC)
      C(SHT_LOUSER)
      C(SHT_HIUSER)
      D(Unknown SH Type)
    }

    if (sh->sh_type == SHT_RELA) {
      relaTable = sh;
    }

    if (sh->sh_type == SHT_REL) {
      relTable = sh;
    }

    if (sh->sh_type == SHT_SYMTAB) {
      symTable = sh;
    }

    if (strcmp(strPtr, ".strtab") == 0) {
      stringTable = sh;
    }

    M(SHF_WRITE, sh->sh_flags)
    M(SHF_ALLOC, sh->sh_flags)
    M(SHF_EXECINSTR, sh->sh_flags)
    M(SHF_MASKPROC, sh->sh_flags)
  }

  printf("Found %d Symbols\n", symTable->sh_entsize);

  for(int i=0; i<symTable->sh_entsize; i++) {
    printf("Symbol %d\n", i);

    // the symbol table is an array of Elf${arch}_Sym structs
    Elf64_Sym * st = (Elf64_Sym*) ((char*)eh + symTable->sh_offset + sizeof(Elf64_Sym)*i);

    switch(ELF64_ST_TYPE(st->st_info)) {
      C(STT_NOTYPE )
      C(STT_OBJECT )
      C(STT_FUNC   )
      C(STT_SECTION)
      C(STT_FILE   )
      C(STT_COMMON )
      C(STT_TLS    )
    }

    switch(ELF64_ST_BIND(st->st_info)) {
      C(STB_LOCAL)
      C(STB_GLOBAL)
      C(STB_WEAK)
    }

    printf("- Section Index: ");
    switch(st->st_shndx) {
      C(SHN_UNDEF)
      C(SHN_LOPROC)
      C(SHN_HIRESERVE)
      C(SHN_ABS)
      C(SHN_HIPROC)
      C(SHN_COMMON)
      default:
        printf("%x\n", st->st_shndx);
    }

    printf("- Value: %x\n", st->st_value);
    printf("- Size: %x\n", st->st_size);

    bool probablyHasName = (
      ELF64_ST_BIND(st->st_info) == STB_GLOBAL ||
      ELF64_ST_BIND(st->st_info) == STB_LOCAL
    );

    if (st->st_name != 0 && probablyHasName) {
      char* strPtr = (char*)eh + stringTable->sh_offset + st->st_name;

      printf("- Name (%s)\n", strPtr);
    } else {
      printf("- No Name\n");
    }
  }

  if (relTable) {
    printf("Found %d Relocation Entries\n", relTable->sh_entsize);
    for(int i=0; i<relTable->sh_entsize; i++) {
      Elf64_Rel * r = (Elf64_Rel*) ((char*)eh + relTable->sh_offset + sizeof(Elf64_Rel)*i);

      int j = ELF64_R_SYM(r->r_info);
      int t = ELF64_R_TYPE(r->r_info);

      printf("Relocation\n");

      // Elf64_Sym * st = (Elf64_Sym*) ((char*)eh + symTable->sh_offset + sizeof(Elf64_Sym)*j);
      // char* strPtr = (char*)eh + stringTable->sh_offset + st->st_name;

      switch(ELF64_R_TYPE(r->r_info)) {
        C(R_X86_64_NONE)
        C(R_X86_64_64)
        C(R_X86_64_PC32)
        C(R_X86_64_GOT32)
        C(R_X86_64_PLT32)
        C(R_X86_64_COPY)
        C(R_X86_64_GLOB_DAT)
        C(R_X86_64_JUMP_SLOT)
        C(R_X86_64_RELATIVE)
        C(R_X86_64_GOTPCREL)
        C(R_X86_64_32)
        C(R_X86_64_32S)
        C(R_X86_64_16)
        C(R_X86_64_PC16)
        C(R_X86_64_8)
        C(R_X86_64_PC8)
        C(R_X86_64_NUM)
      }

      // https://github.com/torvalds/linux/blob/5924bbecd0267d87c24110cbe2041b5075173a25/arch/x86/um/asm/elf.h#L101

      printf("- Symbol %d\n", j);
    }
  } else {
    printf("No relocation table\n");
  }

  return 0;
}
