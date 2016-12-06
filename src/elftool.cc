#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "linux/elf.h"

#define fmt(word) printf("word\n");
#define C(header) case header: fmt(header); break;

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

  Elf32_Ehdr * eh = (Elf32_Ehdr *) buffer;

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
  }

  switch(eh->e_ident[EI_VERSION]) {
    case EV_NONE:
      printf("Unknown Version\n");
      break;
    case EV_CURRENT:
      printf("Current Version\n");
      break;
  }

  switch(eh->e_ident[EI_OSABI]) {
    case ELFOSABI_LINUX:
      printf("Linux ABI\n");
      break;
    default:
      printf("Unknown: %x\n", eh->e_ident[EI_OSABI]);
  }

  printf("Current Type: ");
  switch(eh->e_type) {
    C(ET_EXEC)
    C(ET_REL)
    C(ET_DYN)
    C(ET_NONE)
  }

  // https://github.com/torvalds/linux/blob/5924bbecd0267d87c24110cbe2041b5075173a25/include/uapi/linux/elf-em.h#L20-L45
  switch(eh->e_machine) {
    case EM_860:
      printf("x86 arch\n");
      break;
    case EM_X86_64:
      printf("AMD 64\n");
      break;
    default:
      printf("Unknown: %x", eh->e_machine);
      break;
  }

  long long phoff = eh->e_phoff;
  Elf32_Half psize = eh->e_phentsize;

  printf("PH Offset: %x\n", phoff);

  Elf32_Phdr * ph0 = (Elf32_Phdr*) (&eh[phoff]);

  printf("%x\n", ph0->p_type);

  switch(ph0->p_type) {
    C(PT_NULL)
    C(PT_LOAD)
    C(PT_INTERP)
    C(PT_DYNAMIC)
    C(PT_NOTE)
    C(PT_LOPROC)
    C(PT_HIPROC)
    C(PT_PHDR)
  }

  return 0;
}
