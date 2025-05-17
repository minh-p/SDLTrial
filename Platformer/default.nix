{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  name = "Platformer";
  src = ./src;
  buildInputs = [ pkgs.gcc pkgs.sdl3 ];
  buildPhase = "c++ -o Platformer main.cpp -lSDL3";

  installPhase = ''
    mkdir -p $out/bin
    cp Platformer $out/bin/
  '';
}
