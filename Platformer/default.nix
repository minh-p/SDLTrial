{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  name = "Platformer";
  src = ./.;
  nativeBuildInputs = [ pkgs.cmake pkgs.pkg-config ];
  buildInputs = [ pkgs.sdl3 ];
}
