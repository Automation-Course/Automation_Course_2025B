def binary_to_hex(binary_str):
    binary_str = binary_str.strip()
    # Check if input is only 0s and 1s
    if not all(c in '01' for c in binary_str):
        return None  # Invalid input
    # Pad with zeros to make length a multiple of 4
    while len(binary_str) % 4 != 0:
        binary_str = '0' + binary_str
    # Convert binary to decimal, then to hex
    decimal_value = int(binary_str, 2)
    hex_value = hex(decimal_value)[2:].upper()
    return hex_value


def hex_to_binary(hex_str):
    hex_str = hex_str.strip().upper()
    hex_digits = "0123456789ABCDEF"
    # Check if input contains only valid hex characters
    if not all(c in hex_digits for c in hex_str):
        return None  # Invalid input
    # Convert hex to decimal, then to binary
    decimal_value = int(hex_str, 16)
    binary_value = bin(decimal_value)[2:]
    return binary_value


def run_calculator():
    print("Welcome to the Lazy People Base Converter!")
    print("We do the hard math, you just type stuff. Fair deal?\n")

    while True:
        # Ask the user to choose conversion type
        print("Choose a conversion:")
        print("1. Binary - Hexadecimal")
        print("2. Hexadecimal - Binary")
        choice = input("Enter 1 or 2: ").strip()

        # If input is not valid, ask again
        if choice not in ['1', '2']:
            print("Invalid choice. Let's try again.\n")
            continue

        if choice == '1':
            while True:
                # Get binary input from user
                binary_input = input("Enter a binary number (only 0s and 1s): ").strip()
                result = binary_to_hex(binary_input)
                if result is not None:
                    print(f"Hexadecimal: {result}\n")
                    break
                else:
                    print("Not a valid binary number. Try again.\n")

        elif choice == '2':
            while True:
                # Get hexadecimal input from user
                hex_input = input("Enter a hexadecimal number (0–9, A–F): ").strip()
                result = hex_to_binary(hex_input)
                if result is not None:
                    print(f"Binary: {result}\n")
                    break
                else:
                    print("Not a valid hexadecimal number. Try again.\n")

        # Ask if the user wants to do another conversion
        while True:
            again = input("Do you want to convert another number? (yes/no): ").strip().lower()
            if again == 'no':
                print("\nThanks for using the Lazy People Base Converter! Bye")
                break
            elif again != 'yes':
                print("invalid input. Try again.\n")
            else:
                break
        if again == 'no':
            break


# Run the calculator
run_calculator()
