# AYED2-P2-huffman

This project is an academic implementation of the Huffman compression algorithm, written in C (C90). It supports both compression and decompression of files through command-line interaction.

## 📁 Project Structure

All source files are located in the `src` directory.

**Main files:**
- `main.c`: Entry point and command-line interface.
- `huffman.c`: Contains the core Huffman compression and decompression logic.

## ⚙️ Compilation

To compile the project, simply run:

```bash
gcc src/*.c -o huffman
```
This requires only gcc, with no additional libraries.

## 🚀 Usage

```bash
./huffman comprimir input_file.txt output_file.huff
./huffman descomprimir output_file.huff output_decompressed.txt
```

The program supports two commands:
- `comprimir`: Compresses the input file.
- `descomprimir`: Decompresses a previously compressed file.

## ⚠️ Known Issues

When decompressing, an extra character may appear at the end of the output file. This is likely leftover buffer garbage and should be ignored. It does not affect the correctness of the decompressed data otherwise.

## 📄 Documentation

For more information and related documentation, visit:
https://agusquartz.github.io/
