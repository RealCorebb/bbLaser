{
  'targets': [
    {
      'target_name': 'native_tests',
      'sources': [ 'native_tests.cc' ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")'
      ],
    }
  ]
}
