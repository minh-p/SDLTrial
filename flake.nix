{
  description = "A Nix-flake-based C/C++ development environment";

  inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0.1";

  outputs = inputs:
    let
      supportedSystems =
        [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f:
        inputs.nixpkgs.lib.genAttrs supportedSystems
        (system: f { pkgs = import inputs.nixpkgs { inherit system; }; });
    in {
      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell.override {
          # Override stdenv in order to change compiler:
          # stdenv = pkgs.clangStdenv;
        } {
          packages = with pkgs;
            [
              clang-tools
              cmake
              codespell
              conan
              cppcheck
              doxygen
              gtest
              lcov
              vcpkg
              vcpkg-tool
              sdl3
              bear
              pkg-config
              shaderc
            ] ++ (if system == "aarch64-darwin" then [ ] else [ gdb ]);
          shellHook = ''
            FLAKE_ROOT="$(git rev-parse --show-toplevel)"
            cd $FLAKE_ROOT
            echo -e "CompileFlags:\n  Add: [" > .clangd
            pkg-config --cflags sdl3 | sed 's/ \?-\([^ ]*\)/  "-\1",\n/g' >> .clangd
            echo "  ]" >> .clangd
          '';
        };
      });
    };
}

