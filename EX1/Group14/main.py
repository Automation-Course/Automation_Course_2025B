def binary_to_hex(binary_str):
    binary_str = binary_str.strip()
    # Check for negative sign
    is_negative = binary_str.startswith('-')
    if is_negative:
        binary_str = binary_str[1:]
    # Check for decimal point
    if '.' in binary_str:
        int_part, frac_part = binary_str.split('.')
    else:
        int_part, frac_part = binary_str, ''
    # Validate characters
    if not all(c in '01' for c in int_part + frac_part):
        return None
    # Convert integer part
    while len(int_part) % 4 != 0:
        int_part = '0' + int_part
    int_decimal = int(int_part, 2)
    int_hex = hex(int_decimal)[2:].upper()
    # Convert fractional part
    frac_hex = ''
    if frac_part:
        frac_decimal = 0
        for i, bit in enumerate(frac_part, start=1):
            frac_decimal += int(bit) * (1 / (2 ** i))
        # Convert fractional decimal to hex
        count = 0
        while frac_decimal > 0 and count < 10:
            frac_decimal *= 16
            digit = int(frac_decimal)
            frac_hex += hex(digit)[2:].upper()
            frac_decimal -= digit
            count += 1
    result = int_hex
    if frac_hex:
        result += '.' + frac_hex
    if is_negative:
        result = '-' + result
    return result


def hex_to_binary(hex_str):
    hex_str = hex_str.strip().upper()
    # Check for negative sign
    is_negative = hex_str.startswith('-')
    if is_negative:
        hex_str = hex_str[1:]
    # Check for decimal point
    if '.' in hex_str:
        int_part, frac_part = hex_str.split('.')
    else:
        int_part, frac_part = hex_str, ''
    valid_hex = "0123456789ABCDEF"
    if not all(c in valid_hex for c in int_part + frac_part):
        return None
    # Convert integer part
    int_decimal = int(int_part, 16)
    int_binary = bin(int_decimal)[2:]
    # Convert fractional part
    frac_binary = ''
    if frac_part:
        frac_decimal = 0
        for i, digit in enumerate(frac_part):
            frac_decimal += int(valid_hex.index(digit)) * (1 / (16 ** (i + 1)))
        # Convert fractional decimal to binary
        count = 0
        while frac_decimal > 0 and count < 20:
            frac_decimal *= 2
            bit = int(frac_decimal)
            frac_binary += str(bit)
            frac_decimal -= bit
            count += 1
    result = int_binary
    if frac_binary:
        result += '.' + frac_binary
    if is_negative:
        result = '-' + result
    return result


def run_calculator():
    print("Welcome to the Lazy People Base Converter!")
    print("We do the hard math, you just type stuff. Fair deal?\n")
    while True:
        print("Choose a conversion:")
        print("1. Binary - Hexadecimal")
        print("2. Hexadecimal - Binary")
        choice = input("Enter 1 or 2: ").strip()
        if choice not in ['1', '2']:
            print("Invalid choice. Let's try again.\n")
            continue
        if choice == '1':
            while True:
                binary_input = input("Enter a binary number: ").strip()
                result = binary_to_hex(binary_input)
                if result is not None:
                    print(f"Hexadecimal: {result}\n")
                    break
                else:
                    print("Not a valid binary number. Try again.\n")
        elif choice == '2':
            while True:
                hex_input = input("Enter a hexadecimal number (0–9, A–F): ").strip()
                result = hex_to_binary(hex_input)
                if result is not None:
                    print(f"Binary: {result}\n")
                    break
                else:
                    print("Not a valid hexadecimal number. Try again.\n")
        while True:
            again = input("Do you want to convert another number? (yes/no): ").strip().lower()
            if again == 'no':
                print("\nThanks for using the Lazy People Base Converter! Bye")
                break
            elif again != 'yes':
                print("Invalid input. Try again.\n")
            else:
                break
        if again == 'no':
            break


# Run the calculator
run_calculator()
