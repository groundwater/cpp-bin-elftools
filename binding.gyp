{
  'targets': [
    {
      'target_name': 'elftool',
      'dependencies': [
      ],
      'defines': [
      ],
      'include_dirs': [
        "vendor/linux/include",
        "vendor/linux/arch/arm/include",
        "<!(node -e \"require('nan')\")",
      ],
      'sources': [
        'src/elftool.cc',
      ],
      'clfags': [
        '-w',
      ],
    },
  ],
}
