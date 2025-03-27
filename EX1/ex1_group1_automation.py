def binary_to_hex(binary_str):
    """Convert a binary string to a hexadecimal string."""
    try:
        decimal_value = int(binary_str, 2)  # Convert binary to decimal
        hex_value = hex(decimal_value)[2:].upper()  # Convert decimal to hex and format
        return hex_value
    except ValueError:
        return "Invalid binary input"


def hex_to_binary(hex_str):
    """Convert a hexadecimal string to a binary string."""
    try:
        decimal_value = int(hex_str, 16)  # Convert hex to decimal
        binary_value = bin(decimal_value)[2:]  # Convert decimal to binary
        return binary_value
    except ValueError:
        return "Invalid hexadecimal input"


def main():
    """Simple user interface for the converter."""
    while True:
        print("\nConversion Options:")
        print("1. Binary to Hexadecimal")
        print("2. Hexadecimal to Binary")
        print("3. Exit")
        choice = input("Choose an option (1/2/3): ")

        if choice == '1':
            binary_str = input("Enter a binary number: ")
            print("Hexadecimal:", binary_to_hex(binary_str))
        elif choice == '2':
            hex_str = input("Enter a hexadecimal number: ")
            print("Binary:", hex_to_binary(hex_str))
        elif choice == '3':
            print("Exiting...")
            break
        else:
            print("Invalid choice. Please try again.")


if __name__ == "__main__":
    main()

