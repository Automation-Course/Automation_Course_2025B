def binary_to_hex(binary_str: str) -> str:
    """Convert a binary string to a hexadecimal string."""
    try:
        return hex(int(binary_str, 2))[2:].upper()
    except ValueError:
        return "Invalid binary input"

def hex_to_binary(hex_str: str) -> str:
    """Convert a hexadecimal string to a binary string."""
    try:
        return bin(int(hex_str, 16))[2:]
    except ValueError:
        return "Invalid hexadecimal input"


def main():
    """Prompt the user to choose a conversion type and input a number. Loops until 'n' is entered."""
    while True:
        choice = input("Choose conversion: 1 for Binary to Hex, 2 for Hex to Binary, n to exit: ")

        if choice == "1":
            binary_input = input("Enter a binary number: ")
            print("Hexadecimal output:", binary_to_hex(binary_input))
        elif choice == "2":
            hex_input = input("Enter a hexadecimal number: ")
            print("Binary output:", hex_to_binary(hex_input))
        elif choice.lower() == "n":
            print("Exiting the program. Thank you for using.")
            break
        else:
            print("Invalid choice. Please enter 1, 2, or n.")


# Run the main function
if __name__ == "__main__":
    main()


