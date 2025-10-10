{
  "targets": [
    {
      "target_name": "fbx_loader",
      "sources": [
        "src/fbx_loader.cc",
        "src/fbx_parser.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "include"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags_cc": ["-fexceptions", "-std=c++17"],
      "conditions": [
        ["OS=='win'", {
          "defines": [
            "FBXSDK_SHARED"
          ],
          "libraries": [
            "-llibfbxsdk-md.lib"
          ],
          "library_dirs": [
            "third_party/fbxsdk/lib/vs2022/x64/release"
          ],
          "include_dirs": [
            "third_party/fbxsdk/include"
          ]
        }],
        ["OS=='linux'", {
          "defines": [
            "FBXSDK_SHARED"
          ],
          "libraries": [
            "-lfbxsdk"
          ],
          "library_dirs": [
            "third_party/fbxsdk/lib/gcc4/x64/release"
          ],
          "include_dirs": [
            "third_party/fbxsdk/include"
          ]
        }],
        ["OS=='mac'", {
          "defines": [
            "FBXSDK_SHARED"
          ],
          "libraries": [
            "-lfbxsdk"
          ],
          "library_dirs": [
            "third_party/fbxsdk/lib/clang/release"
          ],
          "include_dirs": [
            "third_party/fbxsdk/include"
          ]
        }]
      ]
    }
  ]
}
