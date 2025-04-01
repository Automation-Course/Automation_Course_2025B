class BinaryHexConverter:
    hex_to_dec = {
        '0': 0, '1': 1, '2': 2, '3': 3, '4': 4,
        '5': 5, '6': 6, '7': 7, '8': 8, '9': 9,
        'A': 10, 'B': 11, 'C': 12, 'D': 13, 'E': 14, 'F': 15
    }

    dec_to_hex = {
        '0': 0, '1': 1, '2': 2, '3': 3, '4': 4,
        '5': 5, '6': 6, '7': 7, '8': 8, '9': 9,
        '10' : 'A' , '11' : 'B', '12' : 'C', '13' : 'D', '14' : 'E', '15' : 'F'
    }

    @staticmethod
    def is_binary(s):
        return set(s) <= {"0", "1"}  # Checks if all characters are either '0' or '1'

    @staticmethod
    def is_hex(s):
        return set(s.upper()) <= set("0123456789ABCDEF")  # Convert to uppercase & check subset


    @staticmethod
    def convert(origin_base, requested_base,input_number):
        decimal_value = 0
        for i, char in enumerate(input_number[::-1]):    # reverses the string and iterates over it
            decimal_value += BinaryHexConverter.hex_to_dec[char.upper()] * origin_base**i

        res = 1
        power = 0
        while res < decimal_value:
            power+=1
            res=requested_base**power
        output = ''
        while decimal_value >= 0 and res >=1:

            if res <= decimal_value:
                digit = int(decimal_value//res)
                output += str(BinaryHexConverter.dec_to_hex[str(digit)])
                decimal_value -= digit*res
            else:
                output += "0"
            res = res/requested_base


        if len(output)==1:
            return output
        else:
            return output[1:]



while True:
    print("\nChoose conversion type:")
    print("1. Hex to Binary")
    print("2. Binary to Hex")
    print("3. Exit")

    choice = input("Enter choice (1/2/3): ")

    if choice == "1":
        hex_num = input("Enter a hexadecimal number: ")
        if not BinaryHexConverter.is_hex(hex_num) or not hex_num.strip():
            print("Invalid input!")
            continue
        print("Binary:", BinaryHexConverter.convert(16,2,hex_num))
    elif choice == "2":
        binary_num = input("Enter a binary number: ")
        if not BinaryHexConverter.is_binary(binary_num) or not binary_num.strip():
            print("Invalid input!")
            continue
        print("Hexadecimal:", BinaryHexConverter.convert(2,16,binary_num))
    elif choice == "3":
        print("Goodbye!")
        break
    else:
        print("Invalid choice! Please enter 1, 2, or 3.")
