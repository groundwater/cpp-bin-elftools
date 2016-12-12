const {test} = require('tap')

const {
  load,
  readHeader
} = require('../build/Release/elftool.node')

test('test', t => {
  try {
    load('SOME_FAKE_FILE')
    t.fail("Should have thrown")
  } catch(e) {
    t.equals("Bad File Name", e.message)
    t.end()
  }
})

test('test good read', t => {
  let buff = load('build/Release/obj.target/elftool/src/elftool.o')
  
  t.assert(buff.length > 0)
  t.end()
})

test('test elf headers', t => {
  let buff = new Buffer('test')

  t.throws(function(){
    readHeader(buff)
  })
  t.end()
})

test('check header', t => {
  let buff = load('build/Release/obj.target/elftool/src/elftool.o')
  let header = readHeader(buff)

  t.equal(header.ident, 'ELFCLASS64')
  t.equal(header.data, 'ELFDATA2LSB')
  t.equal(header.EI_VERSION, 'EV_CURRENT')
  t.equal(header.EI_OSABI, 'ELFOSABI_NONE')
  t.equal(header.e_type, 'ET_REL')
  t.equal(header.e_machine, 'EM_X86_64')

  t.assert(header.e_shoff > 0, `shoff ${header.e_shoff} > 0`)

  // object file has no program header
  t.equal(header.e_phoff, 0, `phoff ${header.e_phoff} == 0`)
  
  t.end()
})
