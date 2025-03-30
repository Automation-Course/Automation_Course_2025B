import java.util.Scanner;
public class EX1_Automation {

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		int choice=1;
		while (choice!=3){
			menu();
			try {
				choice = sc.nextInt();
				sc.nextLine(); 
			} catch (Exception e) {
				System.out.println("Invalid input. Please enter a number between 1 and 3.");
				sc.nextLine(); // Clear the invalid input
				continue;
			}
			switch (choice) {
			case 1:
				//binary to hexdecimal
				System.out.println("please enter a binary number:");
				String binary_str = sc.nextLine();
				binary_to_hexdecimal_check(binary_str);
				break;			

			case 2:
				//hexdecimal to binary
				System.out.println("please enter a hexadecimal number:");
				String hex_str = sc.nextLine();
				hexdecimal_to_binary_check(hex_str);
				break;

			case 3:
				System.out.println("Exiting program...");
				break;
				
			default:
				System.out.println("Invalid choice. Please enter 1, 2, or 3.");
			}
		}
	}


	private static void menu() {
		System.out.println("\n");
		System.out.println("~ Conversion calculator ~");
		System.out.println("Please select the desired action:");
		System.out.println("1. Binary to Hexadecimal");
		System.out.println("2. Hexadecimal to binary");
		System.out.println("3. Exit");
	}



	private static void binary_to_hexdecimal_check(String binaryStr) {
		//handle choice 1 - binary to hexdecimal
		if (binaryStr.isEmpty()) 
		{
			System.out.println("Error: Input cannot be empty."); //handle empty string
			return;
		}
		boolean flag = true; //the number is positive
		if(isNegative(binaryStr)) {
			flag = false; // the number is negative
			binaryStr = binaryStr.substring(1);

		}
		// Validate input (only 0 and 1 allowed)
		for (int i = 0; i < binaryStr.length(); i++) {
			char c = binaryStr.charAt(i);
			if (c != '0' && c != '1') {
				System.out.println("Invalid input, try again");
				return;
			}
		}

		String hexResult = binaryToHex(binaryStr);
		//if the number is negative - also the result
		if (!flag) {
			hexResult = '-' + hexResult;
		}
		System.out.println("Hexadecimal: " + hexResult);
		return;
	}



	
	public static String binaryToHex(String binaryStr) {
		// Converts a binary number to hexadecimal

		// Pad the binary string to make its length a multiple of 4
		while (binaryStr.length() % 4 != 0) {
			binaryStr = "0" + binaryStr;
		}

		// Define hexadecimal mapping
		String hexDigits = "0123456789ABCDEF";
		String hexResult = "";

		// Convert each group of 4 binary digits to a hexadecimal digit
		for (int i = 0; i < binaryStr.length(); i += 4) {
			String binarySegment = binaryStr.substring(i, i + 4);
			int decimalValue = 0;
			// every 4 chars: 
			for (int index = 0; index < 4; index++) {
				decimalValue += (binarySegment.charAt(3 - index) - '0') * Math.pow(2, index);;
			}
			hexResult += hexDigits.charAt(decimalValue);  // adding the result to the string
		}
		// Remove all leading zeros
		while (hexResult.startsWith("0") && hexResult.length() > 1) {
			hexResult = hexResult.substring(1);  // Remove the leading zero
		}
		return hexResult;
	}


	private static void hexdecimal_to_binary_check(String hexStr) {
		//handle choice 2 - hexdecimal to binary
		if (hexStr.isEmpty()) {
			System.out.println("Error: Input cannot be empty.");
			return;
		}
		boolean flag = true; //the number is positive
		if(isNegative(hexStr)) {
			flag = false; // the number is negative
			hexStr = hexStr.substring(1);
		}
		// Make the letters into uppercase
		String upperHex = "";
		for (int i = 0; i < hexStr.length(); i++) {
			char c = hexStr.charAt(i);
			if (c >= 'a' && c <= 'f') {
				c -= 32; } // Convert lowercase to uppercase
			upperHex += c;
		}
		// Validate input (only digits 0-9 and letters A-F are allowed)
		for (int i = 0; i < upperHex.length(); i++) {
			char c = upperHex.charAt(i);
			if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F')) {
				System.out.println("Invalid input, try again");
				return;
			}
		}
		String binaryResult = hexToBinary(upperHex);
		System.out.println(binaryResult);
		return;
	}


	// Converts a hexadecimal number to binary
	public static String hexToBinary(String hexStr) {

		// Map hexadecimal digits to their corresponding 4-bit binary representation
		String binaryResult = "";
		for (int i = 0; i < hexStr.length(); i++) {
			char digit = hexStr.charAt(i);
			String binarySegment = "";

			if (digit == '0') binarySegment = "0000";
			else if (digit == '1') binarySegment = "0001";
			else if (digit == '2') binarySegment = "0010";
			else if (digit == '3') binarySegment = "0011";
			else if (digit == '4') binarySegment = "0100";
			else if (digit == '5') binarySegment = "0101";
			else if (digit == '6') binarySegment = "0110";
			else if (digit == '7') binarySegment = "0111";
			else if (digit == '8') binarySegment = "1000";
			else if (digit == '9') binarySegment = "1001";
			else if (digit == 'A') binarySegment = "1010";
			else if (digit == 'B') binarySegment = "1011";
			else if (digit == 'C') binarySegment = "1100";
			else if (digit == 'D') binarySegment = "1101";
			else if (digit == 'E') binarySegment = "1110";
			else if (digit == 'F') binarySegment = "1111";

			binaryResult += binarySegment;
		}

		return "Binary: " + binaryResult;
	}


	private static boolean isNegative(String binaryStr) {
		// check if the number is with "-" sign
		return (binaryStr.charAt(0) == '-');
	}


}
