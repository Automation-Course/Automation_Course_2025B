import tkinter as tk
from tkinter import ttk, messagebox
import re

def binary_to_hex(binary_str):
    """
    המרה מבינארי להקסדצימלי.
    
    Args:
        binary_str (str): מחרוזת בינארית (צריכה להכיל רק 0 ו-1)
    
    Returns:
        str: ערך הקסדצימלי מקביל
    
    Raises:
        ValueError: אם הקלט אינו מחרוזת בינארית תקפה
    """
    # בדיקת תקינות הקלט
    if not binary_str:
        raise ValueError("הקלט ריק")
    
    # בדיקה שהקלט מכיל רק 0 ו-1
    if not all(bit in '01' for bit in binary_str):
        raise ValueError("הקלט חייב להכיל רק 0 ו-1")
    
    # הוספת אפסים בהתחלה כדי שהאורך יתחלק ב-4 (כל 4 ביטים = ספרה הקסדצימלית)
    padding = (4 - len(binary_str) % 4) % 4
    padded_binary = '0' * padding + binary_str
    
    # המרה מבינארי להקסדצימלי
    hex_result = ''
    for i in range(0, len(padded_binary), 4):
        # לוקחים 4 ביטים בכל פעם ומחשבים את הערך הדצימלי
        chunk = padded_binary[i:i+4]
        decimal_value = int(chunk, 2)
        
        # ממירים מדצימלי לתו הקסדצימלי
        hex_char = format(decimal_value, 'x').upper()
        hex_result += hex_char
    
    return hex_result


def hex_to_binary(hex_str):
    """
    המרה מהקסדצימלי לבינארי.
    
    Args:
        hex_str (str): מחרוזת הקסדצימלית (צריכה להכיל רק תווים תקפים של בסיס 16)
    
    Returns:
        str: ערך בינארי מקביל
    
    Raises:
        ValueError: אם הקלט אינו מחרוזת הקסדצימלית תקפה
    """
    # בדיקת תקינות הקלט
    if not hex_str:
        raise ValueError("הקלט ריק")
    
    # בדיקה שהקלט מכיל רק תווים תקפים של בסיס 16
    valid_hex_chars = set('0123456789ABCDEFabcdef')
    if not all(char in valid_hex_chars for char in hex_str):
        raise ValueError("הקלט חייב להכיל רק תווים הקסדצימליים תקפים (0-9, A-F)")
    
    # המרה מהקסדצימלי לבינארי
    binary_result = ''
    for char in hex_str:
        # ממירים תו הקסדצימלי לערך דצימלי
        decimal_value = int(char, 16)
        
        # ממירים מדצימלי לבינארי (4 ביטים)
        binary_value = format(decimal_value, '04b')
        binary_result += binary_value
    
    # הסרת אפסים מובילים (אופציונלי, תלוי בדרישות)
    # binary_result = binary_result.lstrip('0') or '0'  # אם כל התוצאה היא אפסים, החזר '0'
    
    return binary_result


class CalculatorApp:
    """
    מחלקה ליישום מחשבון המרת בסיסים עם ממשק משתמש גרפי
    """
    def __init__(self, root):
        self.root = root
        self.root.title("מחשבון המרת בסיסים")
        
        # הגדרת כיוון RTL למסך הראשי
        self.root.tk.call('tk', 'scaling', 1.3)  # הגדלת פונט לנראות טובה יותר
        
        # קביעת כיוון RTL וסגנון
        self.configure_style()
        
        # הגדרת גודל מסך ומיקום
        window_width = 600
        window_height = 400
        screen_width = root.winfo_screenwidth()
        screen_height = root.winfo_screenheight()
        center_x = int(screen_width/2 - window_width/2)
        center_y = int(screen_height/2 - window_height/2)
        root.geometry(f'{window_width}x{window_height}+{center_x}+{center_y}')
        
        # יצירת פריסה עם כרטיסיות
        self.create_tabs()
        
        # יצירת המרכיבים בכל כרטיסייה
        self.create_binary_to_hex_tab()
        self.create_hex_to_binary_tab()
        
        # ואליזציה והגדרת ערכים
        self.configure_validation()
    
    def configure_style(self):
        """הגדרת סגנון לממשק"""
        style = ttk.Style()
        style.configure('TFrame', background='#f0f0f0')
        style.configure('TButton', font=('Arial', 12), padding=5)
        style.configure('TLabel', font=('Arial', 12), background='#f0f0f0')
        style.configure('TEntry', font=('Arial', 12))
        style.configure('TNotebook', background='#f0f0f0')
        style.configure('TNotebook.Tab', font=('Arial', 12, 'bold'), padding=[10, 5])
        
        # הגדר תמיכה ב-RTL לכל המרכיבים
        self.root.option_add('*TEXT*direction', tk.RIGHT)
        self.root.option_add('*TEXT*justify', tk.RIGHT)
        self.root.option_add('*Entry*justify', tk.RIGHT)
    
    def create_tabs(self):
        """יצירת כרטיסיות"""
        self.notebook = ttk.Notebook(self.root)
        
        # כרטיסייה ראשונה: בינארי להקסדצימלי
        self.tab1 = ttk.Frame(self.notebook)
        self.notebook.add(self.tab1, text='בינארי להקסדצימלי')
        
        # כרטיסייה שנייה: הקסדצימלי לבינארי
        self.tab2 = ttk.Frame(self.notebook)
        self.notebook.add(self.tab2, text='הקסדצימלי לבינארי')
        
        self.notebook.pack(expand=1, fill="both", padx=10, pady=10)
    
    def create_binary_to_hex_tab(self):
        """יצירת הממשק לכרטיסיית בינארי להקסדצימלי"""
        # מסגרת לכל תוכן הכרטיסייה
        main_frame = ttk.Frame(self.tab1, padding=20)
        main_frame.pack(fill="both", expand=True)
        
        # כותרת
        ttk.Label(main_frame, text="המרה מבינארי להקסדצימלי", font=('Arial', 16, 'bold')).pack(pady=(0, 20))
        
        # מסגרת לקלט
        input_frame = ttk.Frame(main_frame)
        input_frame.pack(fill="x", pady=10)
        
        ttk.Label(input_frame, text=":הכנס מספר בינארי", font=('Arial', 12)).pack(side=tk.RIGHT, padx=5)
        
        # שדה קלט
        self.binary_input = ttk.Entry(input_frame, width=40, justify=tk.RIGHT)
        self.binary_input.pack(side=tk.RIGHT, padx=5, expand=True, fill="x")
        
        # תוית למידע על בדיקת תקינות
        self.binary_validation_label = ttk.Label(main_frame, text="", foreground="red")
        self.binary_validation_label.pack(fill="x", pady=5)
        
        # כפתור המרה
        convert_frame = ttk.Frame(main_frame)
        convert_frame.pack(fill="x", pady=10)
        
        ttk.Button(convert_frame, text="המר לבסיס הקסדצימלי", command=self.convert_binary_to_hex).pack(side=tk.RIGHT, padx=5)
        ttk.Button(convert_frame, text="נקה", command=self.clear_binary_fields).pack(side=tk.RIGHT, padx=5)
        
        # מסגרת לפלט
        output_frame = ttk.Frame(main_frame)
        output_frame.pack(fill="x", pady=10)
        
        ttk.Label(output_frame, text=":תוצאה בבסיס הקסדצימלי", font=('Arial', 12)).pack(side=tk.RIGHT, padx=5)
        
        # שדה פלט (קריאה בלבד)
        self.hex_output = ttk.Entry(output_frame, width=40, state="readonly", justify=tk.RIGHT)
        self.hex_output.pack(side=tk.RIGHT, padx=5, expand=True, fill="x")
        
        # כפתור העתקה
        ttk.Button(output_frame, text="העתק", command=lambda: self.copy_to_clipboard(self.hex_output)).pack(side=tk.LEFT, padx=5)
    
    def create_hex_to_binary_tab(self):
        """יצירת הממשק לכרטיסיית הקסדצימלי לבינארי"""
        # מסגרת לכל תוכן הכרטיסייה
        main_frame = ttk.Frame(self.tab2, padding=20)
        main_frame.pack(fill="both", expand=True)
        
        # כותרת
        ttk.Label(main_frame, text="המרה מהקסדצימלי לבינארי", font=('Arial', 16, 'bold')).pack(pady=(0, 20))
        
        # מסגרת לקלט
        input_frame = ttk.Frame(main_frame)
        input_frame.pack(fill="x", pady=10)
        
        ttk.Label(input_frame, text=":הכנס מספר הקסדצימלי", font=('Arial', 12)).pack(side=tk.RIGHT, padx=5)
        
        # שדה קלט
        self.hex_input = ttk.Entry(input_frame, width=40, justify=tk.RIGHT)
        self.hex_input.pack(side=tk.RIGHT, padx=5, expand=True, fill="x")
        
        # תוית למידע על בדיקת תקינות
        self.hex_validation_label = ttk.Label(main_frame, text="", foreground="red")
        self.hex_validation_label.pack(fill="x", pady=5)
        
        # כפתור המרה
        convert_frame = ttk.Frame(main_frame)
        convert_frame.pack(fill="x", pady=10)
        
        ttk.Button(convert_frame, text="המר לבסיס בינארי", command=self.convert_hex_to_binary).pack(side=tk.RIGHT, padx=5)
        ttk.Button(convert_frame, text="נקה", command=self.clear_hex_fields).pack(side=tk.RIGHT, padx=5)
        
        # מסגרת לפלט
        output_frame = ttk.Frame(main_frame)
        output_frame.pack(fill="x", pady=10)
        
        ttk.Label(output_frame, text=":תוצאה בבסיס בינארי", font=('Arial', 12)).pack(side=tk.RIGHT, padx=5)
        
        # שדה פלט (קריאה בלבד)
        self.binary_output = ttk.Entry(output_frame, width=40, state="readonly", justify=tk.RIGHT)
        self.binary_output.pack(side=tk.RIGHT, padx=5, expand=True, fill="x")
        
        # כפתור העתקה
        ttk.Button(output_frame, text="העתק", command=lambda: self.copy_to_clipboard(self.binary_output)).pack(side=tk.LEFT, padx=5)
    
    def configure_validation(self):
        """הגדרת ולידציה בזמן אמת לשדות הקלט"""
        # ולידציה לשדה בינארי
        self.binary_input.bind("<KeyRelease>", self.validate_binary_input)
        
        # ולידציה לשדה הקסדצימלי
        self.hex_input.bind("<KeyRelease>", self.validate_hex_input)
    
    def validate_binary_input(self, event=None):
        """בדיקת תקינות קלט בינארי בזמן אמת"""
        binary_text = self.binary_input.get().strip()
        
        if not binary_text:
            self.binary_validation_label.config(text="")
            return
        
        if not re.match(r'^[01]+$', binary_text):
            self.binary_validation_label.config(text="שגיאה: הקלט חייב להכיל רק 0 ו-1")
        else:
            self.binary_validation_label.config(text="")
    
    def validate_hex_input(self, event=None):
        """בדיקת תקינות קלט הקסדצימלי בזמן אמת"""
        hex_text = self.hex_input.get().strip()
        
        if not hex_text:
            self.hex_validation_label.config(text="")
            return
        
        if not re.match(r'^[0-9A-Fa-f]+$', hex_text):
            self.hex_validation_label.config(text="שגיאה: הקלט חייב להכיל רק תווים הקסדצימליים תקפים (0-9, A-F)")
        else:
            self.hex_validation_label.config(text="")
    
    def convert_binary_to_hex(self):
        """המרה מבינארי להקסדצימלי והצגת התוצאה"""
        binary_text = self.binary_input.get().strip()
        
        # בדיקת תקינות הקלט
        if not binary_text:
            messagebox.showerror("שגיאה", "הקלט ריק")
            return
        
        if not re.match(r'^[01]+$', binary_text):
            messagebox.showerror("שגיאה", "הקלט חייב להכיל רק 0 ו-1")
            return
        
        try:
            # המרה
            hex_result = binary_to_hex(binary_text)
            
            # הצגת התוצאה
            self.hex_output.config(state="normal")
            self.hex_output.delete(0, tk.END)
            self.hex_output.insert(0, hex_result)
            self.hex_output.config(state="readonly")
            
        except Exception as e:
            messagebox.showerror("שגיאה", str(e))
    
    def convert_hex_to_binary(self):
        """המרה מהקסדצימלי לבינארי והצגת התוצאה"""
        hex_text = self.hex_input.get().strip()
        
        # בדיקת תקינות הקלט
        if not hex_text:
            messagebox.showerror("שגיאה", "הקלט ריק")
            return
        
        if not re.match(r'^[0-9A-Fa-f]+$', hex_text):
            messagebox.showerror("שגיאה", "הקלט חייב להכיל רק תווים הקסדצימליים תקפים (0-9, A-F)")
            return
        
        try:
            # המרה
            binary_result = hex_to_binary(hex_text.upper())
            
            # הצגת התוצאה
            self.binary_output.config(state="normal")
            self.binary_output.delete(0, tk.END)
            self.binary_output.insert(0, binary_result)
            self.binary_output.config(state="readonly")
            
        except Exception as e:
            messagebox.showerror("שגיאה", str(e))
    
    def clear_binary_fields(self):
        """ניקוי שדות בכרטיסיית בינארי להקסדצימלי"""
        self.binary_input.delete(0, tk.END)
        self.hex_output.config(state="normal")
        self.hex_output.delete(0, tk.END)
        self.hex_output.config(state="readonly")
        self.binary_validation_label.config(text="")
    
    def clear_hex_fields(self):
        """ניקוי שדות בכרטיסיית הקסדצימלי לבינארי"""
        self.hex_input.delete(0, tk.END)
        self.binary_output.config(state="normal")
        self.binary_output.delete(0, tk.END)
        self.binary_output.config(state="readonly")
        self.hex_validation_label.config(text="")
    
    def copy_to_clipboard(self, entry_widget):
        """העתקת תוכן שדה ללוח"""
        self.root.clipboard_clear()
        self.root.clipboard_append(entry_widget.get())
        messagebox.showinfo("העתקה", "התוכן הועתק ללוח")


def main():
    """פונקציה ראשית להפעלת האפליקציה"""
    root = tk.Tk()
    app = CalculatorApp(root)
    root.mainloop()


# כאשר מריצים את הקובץ ישירות
if __name__ == "__main__":
    main()
