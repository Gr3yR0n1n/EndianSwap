# EndianSwap

Simple C utlity for changing the endianess of a file. Input file's length must be a multiple of 2 or 4 bytes.

## Compiling

```
make
```

## Usage

32 bits endianness swap :

```
bin/endianswap /my/input/file.bin /my/output/file.bin
```

16 bits endianness swap : 

```
bin/endianswap -n 16 input.bin output.bin
```
