{
  "variables": {
    "boost_root%": "C:/Users/danie/CLionProjects/corona_school_matching/boost_includes",
    "nlohmann_root%": "C:/Users/danie/CLionProjects/corona_school_matching/json-develop"

  },
    'targets': [
      {
        'target_name': 'corona_school_matching',
        'defines': [
          'DEFINE_FOO',
          'DEFINE_A_VALUE=value',
          'NAPI_DISABLE_CPP_EXCEPTIONS',
        ],
        'include_dirs': [
          "<!@(node -p \"require('node-addon-api').include\")",
          "<@(nlohmann_root)/include",
        ],
        'sources': [
          'index.cpp',
          'src/graph_creator.cpp',
        ],
        "cflags!": [ "-fno-exceptions",],
        "cflags_cc!": [ "-fno-exceptions",  ],
        'cflags_cc': [
          '-std=c++17' , # use c++17
        ],
        'linkflags': [
        '-lstdc++fs -lc++fs -ld',
                  ],
        'conditions': [
          ['OS=="linux"', {
            'defines': [
              'LINUX_DEFINE',
            ],
            'include_dirs': [
              'include/linux',
            ],
          }],
          ['OS=="mac"', {
            'defines': [
              'MAC_DEFINE',
            ],
            'xcode_settings': { # define special things for macOS (in particular, without that, c++17 wouldn't work on macOS)
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'MACOSX_DEPLOYMENT_TARGET': '10.15',
              'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',
              'CLANG_CXX_LIBRARY': 'libc++',
            },
          }],
          ['OS=="win"', {
            'defines': [
              'WINDOWS_SPECIFIC_DEFINE',
            ],
             'msvs_settings': {
                'VCCLCompilerTool': {
                  'AdditionalOptions': [ '-std:c++17', '-IC:/Users/danie/CLionProjects/corona_school_matching/boost_includes' ],
                },
              },
          }, { # OS != "win",
            'defines': [
              'NON_WINDOWS_DEFINE',
            ],
          }]
        ],
      },
    ],
  }