def binary_to_hex(binary_str):
    try:
        decimal_value = int(binary_str, 2)  # binary to decimal
        hex_value = hex(decimal_value)[2:].upper()  # decimal to Hexadecimal
        return hex_value
    except ValueError:
        return "Error: Please enter a valid binary number (only 0 and 1)."


def hex_to_binary(hex_str):
    try:
        decimal_value = int(hex_str, 16)  # Hexadecimal to decimal
        binary_value = bin(decimal_value)[2:]  # Decimal to binary
        return binary_value
    except ValueError:
        return "Error: Please enter a valid hexadecimal number (0-9, A-F)."


def main():
    print("Welcome to the Base Conversion Calculator - Group 15!")
    print("Choose an option:")
    print("1 - Convert Binary to Hexadecimal")
    print("2 - Convert Hexadecimal to Binary")

    choice = input("Enter your choice: ")

    if choice == "1":
        binary_input = input("Enter a Binary number: ")  # binary input
        print("Hexadecimal conversion is:", binary_to_hex(binary_input))
    elif choice == "2":
        hex_input = input("Enter a Hexadecimal number: ")  # Hexadecimal input
        print("Binary conversion is:", hex_to_binary(hex_input))
    else:
        print("Error: Please select a valid option (1 or 2).")


if __name__ == "__main__":  # Changed from "_main_" to "__main__"
    main()