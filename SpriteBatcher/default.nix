{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  name = "SpriteBatcher";
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
