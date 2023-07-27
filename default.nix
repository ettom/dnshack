{ lib, stdenv, cmake, fetchurl, callPackage, symlinkJoin }:

let
  dnshackresolver = stdenv.mkDerivation rec {
    pname = "dnshackresolver";
    version = "0.1.0";

    src = fetchurl {
      url = "https://github.com/ettom/dnshack/releases/download/v${version}/${pname}-bin-v${version}";
      sha256 = "sha256-EIE2dMK/EFGPIeBi43VPwlMb19NUcDhtj6OxbSlaJoI=";
    };

    dontUnpack = true;

    installPhase = ''
      install -m755 -D ${src} $out/bin/${pname}
    '';
  };

  dnshackbridge = callPackage ./dnshackbridge { };
in symlinkJoin {
  name = "dnshack";
  paths = [ dnshackresolver dnshackbridge ];
}
