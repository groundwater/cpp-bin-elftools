#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "linux/elf.h"

#define fmt(word) printf("%s\n", #word);
#define C(header) case header: printf("%s\n", #header); break;
#define D(err) default: printf("%s\n", #err);

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

  for(int i=0; i<shnum; i++) {
    Elf64_Shdr * sh = (Elf64_Shdr*) (sh0 + i*shsize);

    printf("Section Name Index: %d\n", sh->sh_name);

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

  }

  return 0;
}
