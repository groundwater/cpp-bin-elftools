{
  'targets': [
    {
      'target_name': 'elftool',
      'type': 'executable',
      'dependencies': [
      ],
      'defines': [
      ],
      'include_dirs': [
        "vendor/linux/include",
        "vendor/linux/arch/arm/include",
      ],
      'sources': [
        'src/elftool.cc',
      ],
      'conditions': [
        ['OS=="linux"', {
          'defines': [
          ],
          'include_dirs': [
          ],
        }],
      ],
    },
  ],
}
