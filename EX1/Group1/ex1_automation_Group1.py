def binary_to_hex(binary_str):
    """Convert a binary string to a hexadecimal string without using bin or hex."""
    try:
        if not binary_str:
            return "Invalid binary input"
        # Convert binary to decimal
        decimal_value = 0
        for digit in binary_str:
            if digit not in {'0', '1'}:
                return "Invalid binary input"
            decimal_value = decimal_value * 2 + int(digit)

        # Convert decimal to hex manually
        hex_chars = "0123456789ABCDEF"
        hex_value = ""
        while decimal_value > 0:
            remainder = decimal_value % 16
            hex_value = hex_chars[remainder] + hex_value
            decimal_value //= 16

        return hex_value
    except ValueError:
        return "Invalid binary input"


def hex_to_binary(hex_str):
    """Convert a hexadecimal string to a binary string without using bin or hex."""
    try:
        if not hex_str:
            return "Invalid hexadecimal input"
        # Convert hex to decimal manually
        hex_chars = "0123456789ABCDEF"
        decimal_value = 0
        hex_str = hex_str.upper()
        for digit in hex_str:
            if digit not in hex_chars or not hex_str:
                return "Invalid hexadecimal input"
            decimal_value = decimal_value * 16 + hex_chars.index(digit)

        # Convert decimal to binary manually
        binary_value = ""
        while decimal_value > 0:
            remainder = decimal_value % 2
            binary_value = str(remainder) + binary_value
            decimal_value //= 2

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
        choice = input("Choose an option (1/2/3): ").strip()

        if choice == '1':
            binary_str = input("Enter a binary number: ").strip()
            print("Hexadecimal:", binary_to_hex(binary_str))
        elif choice == '2':
            hex_str = input("Enter a hexadecimal number: ").strip()
            print("Binary:", hex_to_binary(hex_str))
        elif choice == '3':
            print("Exiting...")
            break
        else:
            print("Invalid choice. Please try again.")


if __name__ == "__main__":
    main()