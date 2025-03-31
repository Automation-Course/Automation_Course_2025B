import java.util.Scanner; //import scanner to recieve input 
import java.math.BigInteger;

public class ex1 {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int baseType; // the base that the user chooses
        String number; // the input number
        boolean play = true;

        while (play) {
            System.out.println("Welcome! Choose your base:");
            System.out.println("1. Hexadecimal (Base 16)");
            System.out.println("2. Binary (Base 2)");
            baseType = sc.nextInt(); 
            System.out.println("Please enter your number:");
            number = sc.next();

            if (baseType == 1) { // Hexadecimal to Binary
                if (isValidHex(number)) { //checks if the input number is within Hex range
                    String binary = hexToBinary(number); //call the hex2b function
                    System.out.println("Binary: " + binary);
                }
				else //if invalid input
				System.out.println("Invalid base, please pick a number between 0-9 and a letter between A-F");
            } else if (baseType == 2) { // Binary to Hexadecimal
                if (isValidBinary(number)) {//checks if the input number is binary
                    String hex = binaryToHex(number); //calls the b2hex function
                    System.out.println("Hexadecimal: " + hex);
                }
				else // if the input is invalid 
				System.out.println("Invalid input, please only enter 0 or 1");
            } else {
                System.out.println("Invalid base selection.");
            }

            System.out.println("Would you like to play again? (1: Yes, 2: No)");
            int userAnswer = sc.nextInt();
            if (userAnswer == 2) {
                System.out.println("Goodbye!");
                play = false;
            }
        }
    }

    public static boolean isValidBinary(String number) {
        return number.matches("[01]+(\\.[01]+)?");
    }

    public static boolean isValidHex(String number) {
        return number.matches("[0-9A-Fa-f]+(\\.[0-9A-Fa-f]+)?");//support lower case letters
    }

    public static String hexToBinary(String hex) { 
        if (hex.contains(".")) {
            String[] parts = hex.split("\\."); //if decimal point, split into two numbers using the fraction function
            return new BigInteger(parts[0], 16).toString(2) + "." + hexFractionToBinary(parts[1]); //returns the numbers with decimal point after base change
        }
        return new BigInteger(hex, 16).toString(2);
    }

    public static String binaryToHex(String binary) {
        if (binary.contains(".")) {
            String[] parts = binary.split("\\."); //if decimal point, split into two numbers using the fraction function
            return new BigInteger(parts[0], 2).toString(16).toUpperCase() + "." + binaryFractionToHex(parts[1]);
        }
        return new BigInteger(binary, 2).toString(16).toUpperCase();
    }

    public static String hexFractionToBinary(String hexFraction) { //hex fraction to binary 
        double fractionValue = 0;
        for (int i = 0; i < hexFraction.length(); i++) {
            int digit = Character.digit(hexFraction.charAt(i), 16);
            fractionValue += digit / Math.pow(16, i + 1);
        }
        return fractionToBinary(fractionValue);
    }

    public static String binaryFractionToHex(String binaryFraction) { //binary fraction to hex 
        double fractionValue = 0;
        for (int i = 0; i < binaryFraction.length(); i++) {
            int digit = Character.digit(binaryFraction.charAt(i), 2);
            fractionValue += digit / Math.pow(2, i + 1);
        }
        return fractionToHex(fractionValue);
    }

    public static String fractionToBinary(double fraction) { //decimal fraction to binary
        StringBuilder binary = new StringBuilder();
        while (fraction > 0 && binary.length() < 10) { 
            fraction *= 2;
            if (fraction >= 1) {
                binary.append("1");
                fraction -= 1;
            } else {
                binary.append("0");
            }
        }
        return binary.toString();
    }

    public static String fractionToHex(double fraction) { //decimal fraction to hex
        StringBuilder hex = new StringBuilder();
        while (fraction > 0 && hex.length() < 10) { 
            fraction *= 16;
            int digit = (int) fraction;
            hex.append(Integer.toHexString(digit).toUpperCase());
            fraction -= digit;
        }
        return hex.toString();
    }
}