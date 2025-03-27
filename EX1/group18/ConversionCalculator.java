import java.util.Scanner;

public class ConversionCalculator  {

    // Binary ---> Hexadecimal (Manual Calculation)
    public static String binaryToHex(String binaryStr) {
        try {
            int decimalValue = 0;
            for (int i = 0; i < binaryStr.length(); i++) {
                char bit = binaryStr.charAt(binaryStr.length() - 1 - i);
                if (bit == '1') {
                    decimalValue += Math.pow(2, i);
                } else if (bit != '0') {
                    return "cannot be displayed - Invalid input.";
                }
            }
            return Integer.toHexString(decimalValue).toUpperCase();
        } catch (Exception e) {
            return "cannot be displayed - Invalid input.";
        }
    }

    // Hexadecimal ---> Binary (Manual Calculation)
    public static String hexToBinary(String hexStr) {
        try {
            int decimalValue = 0;
            for (int i = 0; i < hexStr.length(); i++) {
                char digit = hexStr.charAt(hexStr.length() - 1 - i);
                int hexValue;
                if (Character.isDigit(digit)) {
                    hexValue = digit - '0';
                } else if (Character.isLetter(digit) && digit >= 'A' && digit <= 'F') {
                    hexValue = 10 + (digit - 'A');
                } else {
                    return "cannot be displayed - Invalid input.";
                }
                decimalValue += hexValue * Math.pow(16, i);
            }
            return Integer.toBinaryString(decimalValue);
        } catch (Exception e) {
            return "cannot be displayed - Invalid input.";
        }
    }

    public static void main(String[] args) {
       
    	//for doing tests please call to the function testing ()
        // Interactive Calculator
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\nBase Conversion Calculator");
            System.out.println("1. Convert from Binary to Hexadecimal");
            System.out.println("2. Convert from Hexadecimal to Binary");
            System.out.println("3. Exit");
            System.out.print("Select an option: ");

            // Choosing an option
            String choice = scanner.nextLine();

            switch (choice) {
                case "1":
                    System.out.print("Insert a Binary number: ");
                    String binaryStr = scanner.nextLine();
                    System.out.println("The Hexadecimal number: " + binaryToHex(binaryStr));
                    break;
                case "2":
                    System.out.print("Insert a Hexadecimal number: ");
                    String hexStr = scanner.nextLine();
                    System.out.println("The Binary number: " + hexToBinary(hexStr));
                    break;
                case "3":
                    System.out.println("Bye bye!");
                    scanner.close();
                    return;
                default:
                    System.out.println("Invalid - try again please");
            }
        }
    }

    // Simple testing method
    public static void test(String testName, String result, String expected) {
        if (result.equals(expected)) {
            System.out.println(testName + " - ✅ PASSED");
        } else {
            System.out.println(testName + " - ❌ FAILED");
            System.out.println("  Expected: " + expected);
            System.out.println("  Got: " + result);
        }
    }


	public static void testing () {
		
        // Start of Tests
        System.out.println("Starting Tests...\n");

        // Positive Tests (Valid Inputs)
        test("Binary to Hex - 1010", ConversionCalculator.binaryToHex("1010"), "A");
        test("Binary to Hex - 11111111", ConversionCalculator.binaryToHex("11111111"), "FF");
        test("Hex to Binary - A", ConversionCalculator.hexToBinary("A"), "1010");
        test("Hex to Binary - 1F", ConversionCalculator.hexToBinary("1F"), "11111");

        // Edge Cases
        test("Binary to Hex - 0", ConversionCalculator.binaryToHex("0"), "0");
        test("Hex to Binary - 0", ConversionCalculator.hexToBinary("0"), "0");
        test("Binary to Hex - Maximum 16-bit", ConversionCalculator.binaryToHex("1111111111111111"), "FFFF");
        test("Hex to Binary - FFFF", ConversionCalculator.hexToBinary("FFFF"), "1111111111111111");

        // Negative Tests (Invalid Inputs)
        test("Binary to Hex - Invalid Binary 10201", ConversionCalculator.binaryToHex("10201"), "cannot be displayed - Invalid input.");
        test("Hex to Binary - Invalid Hex G1H", ConversionCalculator.hexToBinary("G1H"), "cannot be displayed - Invalid input.");

        // Functional Tests
        test("Binary to Hex - Leading Zeros", ConversionCalculator.binaryToHex("0001010"), "A");
        test("Hex to Binary - Leading Zeros", ConversionCalculator.hexToBinary("00F"), "1111");

	}
}