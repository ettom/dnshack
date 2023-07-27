{ pkgs ? import <nixpkgs> { } }:

let
  androidPkgs = import pkgs.path {
    crossSystem = {
      config = "aarch64-unknown-linux-android";
      sdkVer = "28";
      ndkVer = "24";
      useAndroidPrebuilt = true;
    };
  };

in androidPkgs.stdenv.mkDerivation rec {

  src = with pkgs.lib;
    cleanSourceWith {
      filter = name: type:
        let baseName = baseNameOf name;
        in !((type == "directory") && (baseName == "build"));
      src = sources.sourceFilesBySuffices ./.. [ ".cpp" ".h" ".hpp" "CMakeLists.txt" ".nix" ];
    };

  name = "dnshackresolver";
  sourceRoot = "source/${name}";

  nativeBuildInputs = with pkgs; [ cmake ];
  buildInputs = with androidPkgs.buildPackages; [ cereal ];

  fixupPhase = ''
    runHook preFixup
    patchelf --set-interpreter /android/system/bin/linker64 --set-rpath /android/system/lib64 $out/bin/${name}
    runHook postFixup
  '';

}
