def binary_to_hex(binary_str):
    try:
        if binary_str.startswith('-'):
            return None  # Negative numbers are not supported
        if '.' in binary_str:
            int_part, frac_part = binary_str.split('.')
        else:
            int_part, frac_part = binary_str, ''

        decimal_int = int(int_part, 2)
        decimal_frac = sum(int(bit) * 2 ** -(i + 1) for i, bit in
enumerate(frac_part))

        total_decimal = decimal_int + decimal_frac
        # חלק שלם להקס
        hex_int = hex(int(total_decimal))[2:].upper()
        # חלק שברירי להקס
        hex_frac = ''
        frac = total_decimal - int(total_decimal)
        count = 0
        while frac > 0 and count < 10:
            frac *= 16
            digit = int(frac)
            hex_frac += hex(digit)[2:].upper()
            frac -= digit
            count += 1

        return hex_int + ('.' + hex_frac if hex_frac else '')

    except ValueError:
        return None


def hex_to_binary(hex_str):
    try:
        if hex_str.startswith('-'):
            return None
        if '.' in hex_str:
            int_part, frac_part = hex_str.split('.')
        else:
            int_part, frac_part = hex_str, ''

        decimal_int = int(int_part, 16)
        decimal_frac = sum(int(char, 16) * 16 ** -(i + 1) for i, char
in enumerate(frac_part))

        total_decimal = decimal_int + decimal_frac
        # חלק שלם לבינארי
        binary_int = bin(int(total_decimal))[2:]
        # חלק שברירי לבינארי
        binary_frac = ''
        frac = total_decimal - int(total_decimal)
        count = 0
        while frac > 0 and count < 10:
            frac *= 2
            bit = int(frac)
            binary_frac += str(bit)
            frac -= bit
            count += 1

        return binary_int + ('.' + binary_frac if binary_frac else '')

    except ValueError:
        return None


# ממשק המשתמש
if __name__ == "__main__":
    print("Base Conversion Calculator")
    while True:
        print("\nChoose an option:")
        print("1 - Convert binary to hexadecimal")
        print("2 - Convert hexadecimal to binary")
        print("0 - Exit")
        choice = input("Enter your choice (0, 1, or 2): ")

        if choice == "1":
            binary_input = input("Enter a binary number (can include a
fractional part): ")
            result = binary_to_hex(binary_input)
            print("Hexadecimal result:" if result else "Invalid binary
input.", result if result else "")
        elif choice == "2":
            hex_input = input("Enter a hexadecimal number (can include
a fractional part): ")
            result = hex_to_binary(hex_input)
            print("Binary result:" if result else "Invalid hexadecimal
input.", result if result else "")
        elif choice == "0":
            print("Goodbye!")
            break
        else:
            print("Invalid choice. Please try again.")