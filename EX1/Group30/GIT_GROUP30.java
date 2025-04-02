import java.util.Scanner;

public class GIT_GROUP30 {

	// Converts a binary string to a hexadecimal string
	public static String binaryToHex(String binaryStr) {
		try {
			int decimal = Integer.parseInt(binaryStr, 2); // Convert binary to decimal
			return Integer.toHexString(decimal).toUpperCase();// Convert decimal to hex (uppercase)
		} catch (NumberFormatException e) {
			return "שגיאה: הכנס מספר בינארי חוקי (רק 0 ו-1)";
		}
	}

	// Converts a hexadecimal string to a binary string
	public static String hexToBinary(String hexStr) {
		try {
			int decimal = Integer.parseInt(hexStr, 16);// Convert hex to decimal
			return Integer.toBinaryString(decimal);// Convert decimal to binary
		} catch (NumberFormatException e) {
			return "שגיאה: הכנס מספר הקסדצימלי חוקי (0-9, A-F)";
		}
	}


	// Main loop - handles user interface and input
	public static boolean mainLoop(Scanner scanner) {
		// Display menu options
		System.out.println("\nמחשבון המרות בסיסים:");
		System.out.println("1. הקסדצימלי → בינארי");
		System.out.println("2. בינארי → הקסדצימלי");
		System.out.println("3. יציאה");
		// Get user choice
		System.out.println("בחר אופציה (1-3): ");
		String choice = scanner.nextLine();
		// Handle each menu option using switch-case
		if (choice.equals("1")) {
			System.out.println("הכנס מספר בינארי:");
			System.out.println("הקסדצימלי:\n" + binaryToHex(scanner.nextLine()));
			return true;
		} else if (choice.equals("2")) {
			System.out.println("הכנס מספר הקסדצימלי:");
			System.out.println("בינארי:\n" + hexToBinary(scanner.nextLine()));
			return true;
		} else if (choice.equals("3")) {
			System.out.println("להתראות!");
			scanner.close();
			return false;
		} else {
			System.out.println("אופציה לא חוקית, נסה שוב.");
			return true;
		}

	}

	// Main method - runs the loop
	public static void main(String[] args) {
		Scanner scanner = new Scanner(System.in);
		boolean shouldContinue = true;
		while (shouldContinue == true) {
			shouldContinue = mainLoop(scanner);
		}
	}
}