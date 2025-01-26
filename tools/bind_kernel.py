#!/usr/bin/python3

import argparse
import os
import struct
import subprocess

def pad_to_sector(file, size):
    position = file.tell()
    padding_needed = (size - (position % size)) % size
    if padding_needed:
        file.write(b'\x00' * padding_needed)

def main():
    parser = argparse.ArgumentParser(description="Binds the kernel image to the bootloader")
    parser.add_argument('--bootloader', required=True, help="Path of the bootloader")
    parser.add_argument('--kernel', required=True, help="Path of the kernel image")
    parser.add_argument('--entry', required=True, help="Base and entry address")
    parser.add_argument('--output', required=True, help="Path to the output file")
    parser.add_argument('--cc', required=True, help="Compiler driver")
    parser.add_argument('--ls', required=True, help="Linker script")
    parser.add_argument('--align', help="Sector size for alignment", default=0x200, type=int)
    args = parser.parse_args()

    if isinstance(args.entry, str):
        args.entry = int(args.entry, 0)

    try:
        os.remove("/tmp/kaolin.bin")
    except:
        pass

    subprocess.run([args.cc, "-nostdlib", "-T", args.ls, args.kernel, "-o", "/tmp/kaolin.bin"])
    args.kernel = "/tmp/kaolin.bin"

    with open(args.output, 'wb') as output:
        # write bootloader
        with open(args.bootloader, 'rb') as f0:
            output.write(f0.read())

        pad_to_sector(output, args.align)

        # write bootloader info sector
        #  uintptr_t entry;
        #  uint16_t sectors;
        output.write(struct.pack('<IH',
                                 args.entry,
                                 (os.path.getsize(args.kernel) + args.align - 1) // args.align))
        pad_to_sector(output, args.align)

        # write kernel
        with open(args.kernel, 'rb') as f1:
            output.write(f1.read())

        pad_to_sector(output, args.align)

if __name__ == '__main__':
    main()
