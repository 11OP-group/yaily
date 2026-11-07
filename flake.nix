{
  description = "Yaily programming language";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
  in {
    devShells = forAllSystems (system: let
      pkgs = import nixpkgs {inherit system;};

      ryaily-script = pkgs.writeShellScriptBin "ryaily" ''
        exec ${pkgs.rlwrap}/bin/rlwrap ./zig-out/bin/yaily "$@"
      '';
    in {
      default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          zig
          zls
          clang-tools

          rlwrap

          ryaily-script
        ];

        shellHook = ''
        '';
      };
    });
  };
}
