{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  name = "SpriteBatcher";
  src = ./.;
  nativeBuildInputs = [ pkgs.cmake ];
  buildInputs = [ pkgs.sdl3 ];
}
