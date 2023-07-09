# Build/Run Instructions
## Prerequisites
### Dependencies
* A Linux environment (WSL will work, but WSL2 is preferred because it's much faster)
* LLVM
* NASM
* Meson + Ninja
* QEMU
* XORRISO

### Installing Dependencies
Visit LLVM's [website](https://llvm.org) for distribution wise instructions for installing llvm tools.

Visit [here](https://mesonbuild.com) for distribution wise instructions for installing `meson`.

* Ubuntu/Debian: 
```shell
apt install build-essential nasm xorriso qemu-system-x86 qemu-utils qemu-system-gui
```
* Arch:
```shell
pacman -S base-devel qemu qemu-arch-extra qemu-ui-gtk nasm xorriso 
```
* Fedora:
```shell
dnf install @development-tools qemu qemu-system-x86 nasm xorriso
dnf group install "C Development Tools and Libraries"
```

## Configuring meson
1. Go to project root directory
2. From that directory, run
```shell
meson setup build --cross-file cross-files/meson-kernel-clang-x86_64.cross-file [-Doptions=...]
```

Visit [meson_options.txt](meson_options.txt) for various options while configuring.

## Building and running
1. In the `build` directory, run `meson compile` to build the kernel and run qemu with the image.
2. Enjoy!

##### Note:
The iso image for the operating system is available under `build` directory after running `meson compile` command.
