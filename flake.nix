{
  description = "Inky Frame Display Server and Firmware";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {
      packages = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.python3.withPackages (ps: with ps; [
          numpy
          pillow
          qrcode
          requests
          ipdb
          ipython
        ]);
      });

      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell {
          name = "inky-frame-dev";
          packages = [
            self.packages.${pkgs.system}.default
          ];
        };
      });
    };
}
