import re

# Prints the calculator menu
def print_menu():
    print("\n🧮 Base Conversion Calculator (Binary ↔ Hexadecimal)")
    print("1. Convert Binary to Hexadecimal")
    print("2. Convert Hexadecimal to Binary")
    print("3. Exit")

# Checks if a string is a valid bin number (only contains 0 or 1)
def is_valid_bin(s):
    return all(c in '01' for c in s) and len(s) > 0

# Checks if a string is a valid hexadecimal number
def is_valid_hex(s):
    try:
        int(s, 16)
        return True
    except ValueError:
        return False

# Converts bin string to hexadecimal string
def bin_to_hex(bin_num):
    decimal = int(bin_num, 2)
    return hex(decimal)[2:].upper()  # Remove '0x' and capitalize letters

# Converts hexadecimal string to bin string
def hex_to_bin(hex_num):
    decimal = int(hex_num, 16)
    return bin(decimal)[2:]  # Remove '0b'

def main():
    while True:
        print_menu()
        mode = input("Choose an option (1 or 2 or 3): ").strip()

        if mode == '1':
            # Removes all whitespace from the input
            bin_input = re.sub(r"\s", "", input("Enter a bin number: "))
            if not is_valid_bin(bin_input):
                print("🚫 Error: That’s not a valid bin number! Only 0 and 1 are allowed.")
                continue
            hex_result = bin_to_hex(bin_input)
            print(f"✅ Hexadecimal result: {hex_result}")

        elif mode == '2':
            hex_input = re.sub(r"\s", "", input("Enter a hexadecimal number: "))
            if not is_valid_hex(hex_input):
                print("🚫 Error: Invalid hexadecimal input! Use digits 0-9 and letters A-F.")
                continue
            bin_result = hex_to_bin(hex_input)
            print(f"✅ Binary result: {bin_result}")

        elif mode == '3':
            print("👋 Goodbye!")
            break

        else:
            print("⚠️ Invalid choice. Please select 1, 2, or 3.")

if __name__ == "__main__":
    main()
