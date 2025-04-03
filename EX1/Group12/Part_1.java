import java.util.Scanner;

public class Part_1 {
	static Scanner sc = new Scanner(System.in);
	static boolean minus = false;

	//Get number between 0 to 15 and return the hexadecimal number
	public static String Binary_Hexadecimal_Dictionary(int decimal)
	{
		if(decimal>=0 && decimal<=9)
			return String.valueOf(decimal);
		else
		{
			switch(decimal)
			{
			case 10:
				return "A";
			case 11:
				return "B";
			case 12:
				return "C";
			case 13:
				return "D";
			case 14:
				return "E";
			case 15:
				return "F";
			default:
				return "-1";
			}
		}
	}

	//Get hexadecimal number and return the binary number
	public static String Hexadecimal_Binary_Dictionary(char hexadecimal)
	{
		String[] hexadecimal_To_Binary_Map = {
				"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
				"1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
		if(hexadecimal >= '0' && hexadecimal <= '9')
			return hexadecimal_To_Binary_Map[hexadecimal - '0'];
		else
		{
			switch(hexadecimal)
			{
			case 'A':
				return hexadecimal_To_Binary_Map[10];
			case 'B':
				return hexadecimal_To_Binary_Map[11];
			case 'C':
				return hexadecimal_To_Binary_Map[12];
			case 'D':
				return hexadecimal_To_Binary_Map[13];
			case 'E':
				return hexadecimal_To_Binary_Map[14];
			case 'F':
				return hexadecimal_To_Binary_Map[15];
			default:
				return "-1";
			}
		}
	}

	//Check if the given binary number is acceptable
	public static String Check_Binary_Number(String binary)
	{
		int point_counter = 0;
		boolean flag;
		do
		{
			flag = true;
			point_counter = 0;
			for(int i = 0; i<binary.length(); i++)
			{
				if(binary.charAt(0) == '.' || binary.charAt(binary.length()-1) == '.')
					flag = false;
				if(binary.charAt(i) == '.')
					point_counter++;
				if((binary.charAt(i)!='0' && binary.charAt(i)!='1' && binary.charAt(i)!='.')
						|| point_counter > 1)
					flag = false;

				if(!flag || binary.length() == 0)
				{
					System.out.println("The number you entered is not a binary number, try again");
					binary = "";
					while(binary.length() == 0)//get string with spaces
					{
						binary = sc.nextLine();
					}
					
					if(binary.charAt(0) == '-')
					{
						minus = true;
						binary = binary.substring(1);
					}
					else
						minus = false;
					
					break;
				}
			}
		} while(!flag);

		return binary;
	}
	
	//Check if the given hexadecimal number is acceptable
		public static String Check_Hexadecimal_Number(String hexadecimal)
		{
			int point_counter = 0;
			boolean flag;
			do
			{
				flag = true;
				point_counter = 0;
				for(int i = 0; i<hexadecimal.length(); i++)
				{
					if(hexadecimal.charAt(0) == '.' || hexadecimal.charAt(hexadecimal.length()-1) == '.')
						flag = false;
					if(hexadecimal.charAt(i) == '.')
						point_counter++;
					if(!(hexadecimal.charAt(i) >= '0' && hexadecimal.charAt(i) <= '9'))
						if(!(hexadecimal.charAt(i) >= 'A' && hexadecimal.charAt(i) <= 'F'))
							if(hexadecimal.charAt(i) != '.' || point_counter > 1)
								flag = false;

					if(!flag || hexadecimal.length() == 0)
					{
						System.out.println("The number you entered is not a hexadecimal number, try again");
						hexadecimal = "";
						while(hexadecimal.length() == 0)
						{
							hexadecimal = sc.nextLine();
						}
						
						if(hexadecimal.charAt(0) == '-')
						{
							minus = true;
							hexadecimal = hexadecimal.substring(1);
						}
						else
							minus = false;
						
						break;
					}
				}
			} while(!flag);

			return hexadecimal;
		}

	//Completing a binary integer so will be divisible by 4
	public static String Completing_A_Binary_Integer(String binary_integer)
	{
		while(binary_integer.length()%4!=0)
		{
			binary_integer = "0" + binary_integer;
		}
		return binary_integer;
	}

	//Completing a decimal integer so will be divisible by 4
	public static String Completing_A_Decimal_Integer(String binary_decimal)
	{
		while(binary_decimal.length()%4!=0)
		{
			binary_decimal = binary_decimal + "0";
		}
		return binary_decimal;
	}

	//Delete all the zero on the right side of the number
	public static String Delete_Zero_On_Right(String str)
	{
		while(!str.isEmpty())
		{
			if(str.indexOf(".") == -1)
				return str;
			if(str.charAt(str.length()-1) == '.')
				return str.substring(0, str.length()-1);
			if(str.charAt(str.length()-1) == '0')
			{
				str = str.substring(0, str.length()-1);
				continue;
			}
			return str;
		}
		return str;
	}
	
	//Delete all the zero on the left side of the number
	public static String Delete_Zero_On_Left(String str)
	{
		while(!str.isEmpty())
		{
			if(str.length() == 1)
				return str;
			if(str.charAt(0)=='0' && str.charAt(1)=='.')
				return str;
			if(str.charAt(0)=='0')
			{
				str = str.substring(1);
				continue;
			}
			return str;
		}
		return str;
	}

	

	//Get parts of binary number and the hexadecimal number
	public static String Get_Hexadecimal_By_Binary_Parts(String binary)
	{
		int exponential = 0;
		int power = 0;
		int decimal = 0;
		int index = 0;
		String hexadecimal = "";

		for(int j = 0; j<binary.length(); j++)
		{			
			index = binary.length()-1-j;
			power = binary.charAt(j)-48;
			exponential = index%4;
			decimal += Math.pow(2, exponential)*power;

			if((j+1)%4 == 0)
			{
				hexadecimal += Binary_Hexadecimal_Dictionary(decimal);
				decimal = 0;
			}
		}
		return hexadecimal;
	}

	//Convert binary number to hexadecimal number
	public static String Convert_Binary_To_Hexadecimal()
	{
		String binary = "";
		String hexadecimal = "";
		String integer_binary = "";
		String decimal_binary = "";

		System.out.println("Enter binary number");

		while(binary.length() == 0)
		{
			binary = sc.nextLine();
		}
		
		if(binary.charAt(0) == '-')
		{
			minus = true;
			binary = binary.substring(1);
		}
		else
			minus = false;
		
		binary = Check_Binary_Number(binary);

		if(binary.indexOf(".") != -1)
		{
			integer_binary = binary.substring(0, binary.indexOf("."));
			decimal_binary = binary.substring(binary.indexOf(".")+1);
		}
		else
			integer_binary = binary;

		integer_binary = Completing_A_Binary_Integer(integer_binary);
		decimal_binary = Completing_A_Decimal_Integer(decimal_binary);

		hexadecimal += Get_Hexadecimal_By_Binary_Parts(integer_binary);
		if(binary.indexOf(".") != -1)
			hexadecimal += ".";
		hexadecimal +=Get_Hexadecimal_By_Binary_Parts(decimal_binary);
		
		hexadecimal = Delete_Zero_On_Right(hexadecimal);
		hexadecimal = Delete_Zero_On_Left(hexadecimal);

		if(minus)
			return "-" + hexadecimal;
		return hexadecimal;
	}

	//Convert hexadecimal number to binary number
	public static String Convert_Hexadecimal_To_Binary()
	{
		String binary = "";
		String hexadecimal = "";
		String integer_hexadecimal = "";
		String decimal_hexadecimal = "";

		System.out.println("Enter hexadecimal number");

		while(hexadecimal.length() == 0)
		{
			hexadecimal = sc.nextLine();
		}
		
		if(hexadecimal.charAt(0) == '-')
		{
			minus = true;
			hexadecimal = hexadecimal.substring(1);
		}
		else
			minus = false;
		
		hexadecimal = Check_Hexadecimal_Number(hexadecimal);

		if(hexadecimal.indexOf(".") != -1)
		{
			integer_hexadecimal = hexadecimal.substring(0, hexadecimal.indexOf("."));
			decimal_hexadecimal = hexadecimal.substring(hexadecimal.indexOf(".")+1);
		}
		else
			integer_hexadecimal = hexadecimal;

		for(int i = 0; i<integer_hexadecimal.length(); i++)
		{
			binary += Hexadecimal_Binary_Dictionary(integer_hexadecimal.charAt(i));
		}

		if(hexadecimal.indexOf(".") != -1)
			binary += ".";

		for(int i = 0; i<decimal_hexadecimal.length(); i++)
		{
			binary += Hexadecimal_Binary_Dictionary(decimal_hexadecimal.charAt(i));
		}
		
		binary = Delete_Zero_On_Right(binary);
		binary = Delete_Zero_On_Left(binary);

		if(minus)
			return "-" + binary;
		return binary;
	}

	public static void main(String[] args)
	{
		String choice;
		System.out.println("Welcome to the magic calculator!");
		do
		{
			System.out.println();
			System.out.println("To exit the calculator, press 0");
			System.out.println("To convert a binary number to hexadecimal, press 1");
			System.out.println("To convert a hexadecimal number to binary, press 2");

			choice = "";
			while(choice.length() == 0)
			{
				choice = sc.nextLine();
			}

			switch(choice)
			{
			case "0":
				System.out.println("Goodbye");
				break;
			case "1":
				System.out.println(Convert_Binary_To_Hexadecimal());
				break;
			case "2":
				System.out.println(Convert_Hexadecimal_To_Binary());
				break;
			default:
				System.out.println("Wrong choise, try again");
				break;
			}
		}
		while(!choice.equals("0"));
	}
}
