{
    description = "atom.editor";

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
        atom_core_pkg = inputs.atom_core.packages.${system}.default;
        atom_logging_pkg = inputs.atom_logging.packages.${system}.default;
        atom_engine_pkg = inputs.atom_engine.packages.${system}.default;

        clang_scan_deps_include_paths =
            inputs.atom_core.clang_scan_deps_include_paths + 
            inputs.atom_logging.clang_scan_deps_include_paths + 
            inputs.atom_engine.clang_scan_deps_include_paths;

        derivation = stdenv.mkDerivation rec {

            name = "atom.editor";

            src = ./.;

            propogatedNativeBuildInputs = with pkgs; [
                atom_core_pkg
                atom_logging_pkg
                atom_engine_pkg
            ];

            nativeBuildInputs = with pkgs; [
                cmake
                cmake-format
                ninja
                git
            ];

            configurePhase = ''
                cmake -S . -B build
            '';

            buildPhase = ''
                cmake --build build --target atom.editor
            '';

            installPhase = ''
                cmake --install build --prefix $out
            '';

            CXXFLAGS = clang_scan_deps_include_paths;
            CMAKE_GENERATOR = "Ninja";
            CMAKE_BUILD_TYPE = "Debug";
            CMAKE_EXPORT_COMPILE_COMMANDS = "true";
        };
    in
    {
        inherit clang_scan_deps_include_paths;

        devShells.${system}.default = derivation;

        packages.${system}.default = derivation;
    };
}
