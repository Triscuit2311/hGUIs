def read_png_to_byte_array(image_path):
    with open(image_path, 'rb') as img_file:
        byte_array = img_file.read()
    return byte_array

def format_byte_array(byte_array):
    formatted_array = ', '.join(f'0x{byte:02X}' for byte in byte_array)
    return f"static const uint8_t IMAGE_DATA[] = {{\n    {formatted_array}\n}};\n"

def main(image_path, output_path):
    byte_array = read_png_to_byte_array(image_path)
    formatted_array = format_byte_array(byte_array)
    
    with open(output_path, 'w') as output_file:
        output_file.write(f"constexpr size_t IMAGE_DATA_SIZE = {len(byte_array)};\n")
        output_file.write(formatted_array)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_png_path> <output_cpp_path>")
        sys.exit(1)

    input_png_path = sys.argv[1]
    output_cpp_path = sys.argv[2]
    
    main(input_png_path, output_cpp_path)
