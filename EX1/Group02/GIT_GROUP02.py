class Main:
    def __init__(self):
        self.running=True

    def run(self):
        while self.running:
            choice = self.print_menu()

            if choice == '1':
                bin_input = input("please enter your bin number to convert")
                self.binary_to_hex(bin_input)
            elif choice == '2':
                hex_input = input("please enter your hex number to convert")
                self.hex_to_binary(hex_input)
            elif choice == '0':
                print("Exiting the program. Goodbye!")
                self.running = False
            else:
                print("Invalid choice. Please select 1, 2, or 0.")

            if self.running:
                print("\n" + "-" * 50 + "\n")  # Print a separator line between operations

    def print_menu(self):
        print("Choose which base conversion you want to perform:")
        print("For Binary to Hexadecimal conversion, press 1")
        print("For Hexadecimal to Binary conversion, press 2")
        print("For Exit, press 0")

        choice = input("Enter your choice (1 or 2): ")
        return choice

    def hex_to_binary (self, hex_input):
            hex_input =self.validate_hex(hex_input)
            if hex_input:
                hex_to_bin_dict  = {
                                    '0': '0000',
                                    '1': '0001',
                                    '2': '0010',
                                    '3': '0011',
                                    '4': '0100',
                                    '5': '0101',
                                    '6': '0110',
                                    '7': '0111',
                                    '8': '1000',
                                    '9': '1001',
                                    'A': '1010',
                                    'B': '1011',
                                    'C': '1100',
                                    'D': '1101',
                                    'E': '1110',
                                    'F': '1111',
                                    '.': '.'
                                }
                output = ""
                for char in hex_input:
                        if char in hex_to_bin_dict:
                            output += hex_to_bin_dict[char]

                print ("binary output: ", output)

    def validate_hex(self, hexadecimal):
        # Remove spaces if any
        hexadecimal = hexadecimal.replace(" ", "")

        # Check if empty
        if not hexadecimal:
            print("Error: Input cannot be empty.")
            return None

        # Check for decimal point
        if '.' in hexadecimal:
            parts = hexadecimal.split('.')
            # Check if there's exactly one decimal point and it's not at the edges
            if len(parts) != 2 or not parts[0] or not parts[1]:
                print("Error: Decimal point must appear exactly once and not at the edges.")
                return None

            # Check if all characters are valid hexadecimal digits
            if not all(c in '0123456789ABCDEF' for c in parts[0] + parts[1]):
                print("Error: Hexadecimal numbers can only contain digits 0-9 and letters A-F.")
                return None
        else:
            # Check if all characters are valid hexadecimal digits
            if not all(c in '0123456789ABCDEF' for c in hexadecimal):
                print("Error: Hexadecimal numbers can only contain digits 0-9 and letters A-F.")
                return None

        return hexadecimal

    # Binary to Hexadecimal Conversion Map


    def binary_to_hex(self, binary_str):
        binary_to_hex_map = {
            "0000": "0", "0001": "1", "0010": "2", "0011": "3",
            "0100": "4", "0101": "5", "0110": "6", "0111": "7",
            "1000": "8", "1001": "9", "1010": "A", "1011": "B",
            "1100": "C", "1101": "D", "1110": "E", "1111": "F"
        }
        # Input validation
        binary_str = binary_str.replace(" ", "")  # remove spaces
        if binary_str.count('.') > 1:  # NOTE for decimal points when there is more than one.
            print("Invalid number - more than one decimal point")
            return None
        if not all(c in "01." for c in binary_str):  # note for invalid binary num
            print("Invalid number - contains non-binary characters")
            return None
        if binary_str == "":  # note for null
            print("Invalid number")
            return None

        if '.' in binary_str:  # if the num is decimal split the num for two parts
            integer_part, fractional_part = binary_str.split('.')  # part 1- decimal
        else:
            integer_part, fractional_part = binary_str, ''  # part 2- integet

        # Pad the integer part with leading zeros to make its length a multiple of 4
        while len(integer_part) % 4 != 0:
            integer_part = "0" + integer_part

        # Pad the fractional part with trailing zeros to make its length a multiple of 4
        while len(fractional_part) % 4 != 0:
            fractional_part += "0"

        hex_result = ""

        # Convert the integer part
        for i in range(0, len(integer_part), 4):
            four_bits = integer_part[i:i + 4]
            hex_result += binary_to_hex_map[four_bits]

        # Convert the fractional part
        if fractional_part:
            hex_result += '.'
            for i in range(0, len(fractional_part), 4):
                four_bits = fractional_part[i:i + 4]
                hex_result += binary_to_hex_map[four_bits]

        print ("hex output: ", hex_result)



# Create and run the main application
if __name__ == "__main__":
    app = Main()
    app.run()


