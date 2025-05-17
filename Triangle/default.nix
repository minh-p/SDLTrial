{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  name = "Triangle";
  src = ./.;
  nativeBuildInputs = [ pkgs.cmake ];
  buildInputs = [
    pkgs.sdl3
    pkgs.vulkan-headers
    pkgs.vulkan-loader
    pkgs.vulkan-validation-layers
    pkgs.shaderc
    pkgs.glslang
  ];
}
