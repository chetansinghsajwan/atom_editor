{
    description = "atom-editor";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

        atom_core = {
            url = "github:shifu-dev/atom.core";
            inputs.nixpkgs.follows = "nixpkgs";
        };

        atom_logging = {
            url = "github:shifu-dev/atom.logging";
            inputs.nixpkgs.follows = "nixpkgs";
            inputs.atom_core.follows = "atom_core";
        };

        atom_engine = {
            url = "github:shifu-dev/atom.engine";
            inputs.nixpkgs.follows = "nixpkgs";
            inputs.atom_core.follows = "atom_core";
            inputs.atom_logging.follows = "atom_logging";
        };
    };

    outputs = inputs:
    let
        system = "x86_64-linux";
        pkgs = inputs.nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;
        stdenv = pkgs.llvmPackages_18.libcxxStdenv;
        atom_core_env = inputs.atom_core.env.${system}.default;
        atom_core_pkg = inputs.atom_core.packages.${system}.default;
        atom_logging_env = inputs.atom_logging.env.${system}.default;
        atom_logging_pkg = inputs.atom_logging.packages.${system}.default;
        atom_engine_env = inputs.atom_engine.env.${system}.default;
        atom_engine_pkg = inputs.atom_engine.packages.${system}.default;
    in rec
    {
        env.${system}.default = rec {

            name = "atom-editor";

            src = ./.;

            nativeBuildInputs = with pkgs; [
                atom_core_pkg
                atom_logging_pkg
                atom_engine_pkg

                cmake
                cmake-format
                ninja
                git
            ];

            configurePhase = ''
                cmake -S . -B build \
                    -D atom_core_DIR:PATH=${atom_core_pkg} \
                    -D atom_core_DIR:PATH=${atom_logging_pkg} \
                    -D atom_core_DIR:PATH=${atom_engine_pkg};
            '';

            buildPhase = ''
                cmake --build build --target atom.editor;
            '';

            installPhase = ''
                cmake --install build --prefix $out;
            '';

            clang_scan_deps_include_paths = [
                "${atom_engine_pkg}/include"
            ];

            envVars = {
                CXXFLAGS = lib.strings.concatMapStrings (v: " -I " + v) (
                    atom_core_env.clang_scan_deps_include_paths ++
                    atom_logging_env.clang_scan_deps_include_paths ++
                    atom_engine_env.clang_scan_deps_include_paths ++
                    clang_scan_deps_include_paths);

                CMAKE_GENERATOR = "Ninja";
                CMAKE_BUILD_TYPE = "Debug";
                CMAKE_EXPORT_COMPILE_COMMANDS = "true";
            };
        };

        devShells.${system}.default = with env.${system}.default; stdenv.mkDerivation ({
            inherit name;
            inherit src;
            inherit nativeBuildInputs;
        } // envVars);

        packages.${system}.default = with env.${system}.default; stdenv.mkDerivation ({
            inherit name;
            inherit src;
            inherit nativeBuildInputs;
            inherit configurePhase;
            inherit buildPhase;
            inherit installPhase;
        } // envVars);
    };
}
