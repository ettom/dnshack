{ lib, stdenv, cmake, cereal}:

stdenv.mkDerivation rec {
  src = with lib;
    cleanSourceWith {
      filter = name: type:
        let baseName = baseNameOf name;
        in !((type == "directory") && (baseName == "build"));
      src = sources.sourceFilesBySuffices ./.. [ ".cpp" ".h" ".hpp" "CMakeLists.txt" ];
    };

  name = "dnshackbridge";
  sourceRoot = "source/${name}";

  nativeBuildInputs = [ cmake ];
  buildInputs = [ cereal ];
}
