# ELF Tool

My goal is to build an ELF file from hand.
I am allowed to use the `elf.h` headers to determine the size and structure of the file contents,
but everything else should be done by hand.

I may first make tools to dissect an ELF file to help understand what's going on.

## Status

- [ ] build an ELF by hand
- [ ] read an ELF
  - [x] read ELF header
  - [ ] read program headers
  - [ ] read section headers
  - [ ] read string table
  - [ ] read symbol table
  - [ ] read relocation table

## Setup

- The `START` script is all you need
  
  ```
  ./START doctor init setup build test
  ```

## Links

- http://bq9.blogspot.com/2012/02/gyp-and-ninja-hello-world.html
- https://www.freebsd.org/cgi/man.cgi?elf(5)
- http://wiki.osdev.org/ELF_Tutorial#Accessing_Section_Headers

