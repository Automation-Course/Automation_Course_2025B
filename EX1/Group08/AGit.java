package automation;


import java.util.Scanner;

public class AGit {

    // Convert binary to hexadecimal (with fractional part support)
    public static String binaryToHex(String binary) {
        String[] parts = binary.split("\\.");
        String intPart = parts[0];
        String fracPart = parts.length > 1 ? parts[1] : "";

        // Pad integer part from left
        while (intPart.length() % 4 != 0) {
            intPart = "0" + intPart;
        }

        StringBuilder hexInt = new StringBuilder();
        for (int i = 0; i < intPart.length(); i += 4) {
            String nibble = intPart.substring(i, i + 4);
            int decimal = Integer.parseInt(nibble, 2);
            hexInt.append(Integer.toHexString(decimal).toUpperCase());
        }

        StringBuilder hexFrac = new StringBuilder();
        if (!fracPart.isEmpty()) {
            while (fracPart.length() % 4 != 0) {
                fracPart += "0";
            }
            for (int i = 0; i < fracPart.length(); i += 4) {
                String nibble = fracPart.substring(i, i + 4);
                int decimal = Integer.parseInt(nibble, 2);
                hexFrac.append(Integer.toHexString(decimal).toUpperCase());
            }
        }

        return hexFrac.length() > 0 ? hexInt + "." + hexFrac : hexInt.toString();
    }

    // Convert hexadecimal to binary (with fractional part support)
    public static String hexToBinary(String hex) {
        String[] parts = hex.split("\\.");
        String intPart = parts[0];
        String fracPart = parts.length > 1 ? parts[1] : "";

        StringBuilder binInt = new StringBuilder();
        for (char c : intPart.toCharArray()) {
            binInt.append(hexCharToBinary(c));
        }

        StringBuilder binFrac = new StringBuilder();
        for (char c : fracPart.toCharArray()) {
            binFrac.append(hexCharToBinary(c));
        }

        return binFrac.length() > 0 ? binInt + "." + binFrac : binInt.toString();
    }

    // Convert hex character to 4-bit binary string
    private static String hexCharToBinary(char c) {
        int val;
        if (c >= '0' && c <= '9') {
            val = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            val = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            val = c - 'a' + 10;
        } else {
            throw new IllegalArgumentException("Invalid hexadecimal character: " + c);
        }

        String binary = Integer.toBinaryString(val);
        while (binary.length() < 4) {
            binary = "0" + binary;
        }
        return binary;
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Base Conversion Calculator: Binary ↔ Hexadecimal");

        while (true) {
            System.out.println("\nChoose an option:");
            System.out.println("1. Binary to Hexadecimal");
            System.out.println("2. Hexadecimal to Binary");
            System.out.println("3. Exit");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    System.out.print("Enter a binary number: ");
                    String binary = scanner.nextLine().trim();
                    if (!binary.matches("[01.]+")) {
                        System.out.println("Invalid binary input.");
                        break;
                    }
                    System.out.println("Hexadecimal conversion: " + binaryToHex(binary));
                    break;

                case "2":
                    System.out.print("Enter a hexadecimal number: ");
                    String hex = scanner.nextLine().trim();
                    try {
                        System.out.println("Binary conversion: " + hexToBinary(hex));
                    } catch (IllegalArgumentException e) {
                        System.out.println("Invalid input: " + e.getMessage());
                    }
                    break;

                case "3":
                    System.out.println("Bye Bye");
                    scanner.close();
                    return;

                default:
                    System.out.println("Invalid choice, please choose again.");
            }
        }
    }
}
