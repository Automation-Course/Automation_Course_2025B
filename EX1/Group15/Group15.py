import time
def binary_to_hex(binary_str):
    try:
        decimal_value = int(binary_str, 2)  # Binary to decimal
        hex_value = hex(decimal_value)[2:].upper()  # Decimal to Hexadecimal
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

def main(): # Show menu to user
    print("Welcome to the Base Conversion Calculator - Group 15!")
    print("Choose an option:")
    print("1 - Convert Binary to Hexadecimal")
    print("2 - Convert Hexadecimal to Binary")
    print("3 - Exit Calculator")

    while (True): # While user didn't exit the program
        choice = input("Enter your choice: ")

        if choice == "1":
            binary_input = input("Enter a Binary number: ")  # binary input
            print("Hexadecimal conversion is:", binary_to_hex(binary_input))
        elif choice == "2":
            hex_input = input("Enter a Hexadecimal number: ")  # Hexadecimal input
            print("Binary conversion is:", hex_to_binary(hex_input))
        elif choice =="3":
            print("Thank you for using our calculator! Goodbye :)")
            time.sleep(2)
            break
        else:
            print("Error: Please select a valid option (1, 2 or 3).")

if __name__ == "__main__":
    main()

