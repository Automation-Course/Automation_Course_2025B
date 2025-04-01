def main():
    # תפריט ראשי להרצת המרת בסיסים
    while True:
        print("\nBase Converter (Binary ↔ Hexadecimal)")
        print("1. Binary to Hexadecimal")
        print("2. Hexadecimal to Binary")
        print("3. Exit")

        choice = input("Please choose option (1/2/3): ")

        if choice == '1':
            # המרה מבינארי להקסדצימלי
            while True:
                binary_input = input("Enter binary number: ")
                if len(binary_input) > 128:
                    print("The input should be smaller than 128 bits")
                    continue
                # בודק שהקלט חוקי
                if all(c in '01.' for c in binary_input) and binary_input.count('.') <= 1:
                    result = binary_to_hex(binary_input)
                    print("Hexadecimal:", result)
                    break
                else:
                    print("Invalid input: Only 0 and 1 are allowed.")

        elif choice == '2':
            # המרה מהקסדצימלי לבינארי
            while True:
                hex_input = input("Enter hexadecimal number: ")
                if len(hex_input) > 32:
                    print("The input should be smaller than 32 bits")
                    continue
                hex_input = hex_input.upper()
                if all(c in '0123456789ABCDEF.' for c in hex_input) and hex_input.count('.') <= 1:
                    result = hex_to_binary(hex_input)
                    print("Binary:", result)
                    break
                else:
                    print("Invalid hexadecimal input: Only 0–9 and A–F are allowed.")

        elif choice == '3':
            print("👋 Goodbye!")
            break

        else:
            print("Invalid choice. Please enter 1, 2, or 3.")


def binary_to_hex(binary_input):
    # ממיר מחרוזת בינארית להקסדצימלית
    hex_map = {
        0: '0', 1: '1', 2: '2', 3: '3',
        4: '4', 5: '5', 6: '6', 7: '7',
        8: '8', 9: '9', 10: 'A', 11: 'B',
        12: 'C', 13: 'D', 14: 'E', 15: 'F'
    }

    # מפריד בין חלק שלם לשבר
    if '.' in binary_input:
        int_part, frac_part = binary_input.split('.')
    else:
        int_part = binary_input
        frac_part = ''

    # משלים אפסים אם צריך
    int_mod = len(int_part) % 4
    if int_mod != 0:
        int_part = '0' * (4 - int_mod) + int_part

    if frac_part:
        frac_mod = len(frac_part) % 4
        if frac_mod != 0:
            frac_part += '0' * (4 - frac_mod)

    # המרה של החלק השלם
    int_hex = ''
    for i in range(0, len(int_part), 4):
        group = int_part[i:i + 4]
        decimal = int(group[0]) * 8 + int(group[1]) * 4 + int(group[2]) * 2 + int(group[3])
        int_hex += hex_map[decimal]

    # המרה של השבר
    frac_hex = ''
    for i in range(0, len(frac_part), 4):
        group = frac_part[i:i + 4]
        decimal = int(group[0]) * 8 + int(group[1]) * 4 + int(group[2]) * 2 + int(group[3])
        frac_hex += hex_map[decimal]

    return f"{int_hex}.{frac_hex}" if frac_hex else int_hex


def hex_to_binary(binary_input):
    # ממיר מחרוזת הקסדצימלית לבינארית
    binary_map = {
        '0': '0000', '1': '0001', '2': '0010', '3': '0011',
        '4': '0100', '5': '0101', '6': '0110', '7': '0111',
        '8': '1000', '9': '1001', 'A': '1010', 'B': '1011',
        'C': '1100', 'D': '1101', 'E': '1110', 'F': '1111',
    }

    # מפריד בין חלק שלם לשבר
    if '.' in binary_input:
        int_part, frac_part = binary_input.split('.')
    else:
        int_part = binary_input
        frac_part = ''

    # המרה של החלק השלם והשבר
    int_binary = ''.join(binary_map[c] for c in int_part)
    frac_binary = ''.join(binary_map[c] for c in frac_part)

    return f"{int_binary}.{frac_binary}" if frac_binary else int_binary


def run():
    # מפעיל את התוכנית
    main()


if __name__ == "__main__":
    run()
