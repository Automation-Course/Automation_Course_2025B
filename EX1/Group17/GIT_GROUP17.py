
def binary_to_decimal(binary_str):
    decimal = 0
    index= len(binary_str)-1
    for digit in binary_str:
        if digit not in ('0', '1'):  # בדיקת תקינות הקלט - רק 0 או 1 מותרים
            return "קלט לא חוקי!"
        decimal +=  (2**index) * int(digit)  # המרת בינארי לדצימלי
        index-=1
    return decimal

def decimal_to_hex(decimal_str):
    hex_digits = "0123456789ABCDEF"  # ערכים הקסדצימליים
    hex_value = ""
    while decimal_str > 0:
        remainder = decimal_str % 16  # חישוב שארית (הסיפרה ההקסדצימלית)
        hex_value = hex_digits[remainder] + hex_value  # הוספת הסיפרה לתוצאה
        decimal_str //= 16  # חלוקה שלמה ב-16 להמשך ההמרה

    return hex_value if hex_value else "0"  # אם המספר הוא 0, מחזירים "0"

def binary_to_hex(binary_str):
    decimal=binary_to_decimal(binary_str)
    if decimal!="קלט לא חוקי!":
        return decimal_to_hex(decimal)
    return "קלט לא חוקי!"

def hex_to_decimal(hex_str):
    decimal = 0
    index = len(hex_str) - 1
    for digit in hex_str:
        if digit not in ('0', '1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'):  # בדיקת תקינות הקלט
            return "קלט לא חוקי!"
        if digit not in ('A','B','C','D','E','F'):
            decimal += (16 ** index) * int(digit)  # המרת בינארי לדצימלי
        else:
            decimal += (16 ** index) * (ord(digit)-55)  # המרת בינארי לדצימלי- שימוש בהסקי קוד (A=65)
        index -= 1
    return decimal

def decimal_to_binary(decimal_str):
    bin_digits = "01"  # ערכים בינאריים
    bin_value = ""
    while decimal_str > 0:
        remainder = decimal_str % 2  # חישוב שארית (הסיפרה הבינארית)
        bin_value = bin_digits[remainder] + bin_value  # הוספת הסיפרה לתוצאה
        decimal_str //= 2  # חלוקה שלמה ב-2 להמשך ההמרה

    return bin_value if bin_value else "0"  # אם המספר הוא 0, מחזירים "0"

def hex_to_binary(hex_str):
    decimal = hex_to_decimal(hex_str)
    if decimal != "קלט לא חוקי!":
        return decimal_to_binary(decimal)
    return "קלט לא חוקי!"

def main():
    """
    פונקציה ראשית להרצת התפריט וקבלת קלט מהמשתמש.
    """
    while True:
        print("\nמחשבון המרת בסיסים:")
        print("1. המרה מבינארי להקסדצימלי")
        print("2. המרה מהקסדצימלי לבינארי")
        print("3. יציאה")

        choice = input("בחר אפשרות מהתפריט: ")

        if choice == '1':
            binary_input = input("הכנס מספר בינארי: ")
            print("הערך בהקסדצימלי: ", binary_to_hex(binary_input))
        elif choice == '2':
            hex_input = input("הכנס מספר הקסדצימלי (אותיות גדולות בלבד): ")
            print("הערך בבינארי: ", hex_to_binary(hex_input))
        elif choice == '3':
            print("יציאה...")
            break
        else:
            print("נא לבחור אפשרות תקינה!")


if __name__ == "__main__":
    main()