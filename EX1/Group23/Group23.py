import tkinter as tk
from tkinter import messagebox

def binary_to_hex():
    binary_str = input_entry.get()
    if not all(bit in '01' for bit in binary_str):
        messagebox.showerror("שגיאה", "הקלט חייב להכיל רק 0 ו-1 (בינארי)")
        return
    try:
        decimal = int(binary_str, 2)
        hex_value = hex(decimal)[2:].upper()
        result_var.set(f"הקסדצימלי: {hex_value}")
    except ValueError:
        messagebox.showerror("שגיאה", "התרחשה שגיאה בהמרה")

def hex_to_binary():
    hex_str = input_entry.get().upper()
    try:
        decimal = int(hex_str, 16)
        binary_value = bin(decimal)[2:]
        result_var.set(f"בינארי: {binary_value}")
    except ValueError:
        messagebox.showerror("שגיאה", "הקלט חייב להיות מספר הקסדצימלי חוקי")

# יצירת חלון
root = tk.Tk()
root.title("מחשבון המרה בסיסים")
root.geometry("420x240")
root.resizable(False, False)

# כותרת
title_label = tk.Label(root, text="מחשבון המרה: בינארי ⇄ הקסדצימלי", font=("Arial", 14))
title_label.pack(pady=10)

# שדה קלט
input_entry = tk.Entry(root, font=("Arial", 12), width=30, justify='center')
input_entry.pack(pady=5)

# כפתורים
buttons_frame = tk.Frame(root)
buttons_frame.pack()

btn_bin_to_hex = tk.Button(
    buttons_frame,
    text="בינארי ← הקסדצימלי",  # אתה מזין בינארי → מקבל הקס
    command=binary_to_hex,
    width=26,
    font=("Arial", 11)
)
btn_bin_to_hex.grid(row=0, column=0, padx=5, pady=10)

btn_hex_to_bin = tk.Button(
    buttons_frame,
    text="הקסדצימלי ← בינארי",  # אתה מזין הקס → מקבל בינארי
    command=hex_to_binary,
    width=26,
    font=("Arial", 11)
)
btn_hex_to_bin.grid(row=0, column=1, padx=5)

# תוצאה
result_var = tk.StringVar()
result_label = tk.Label(root, textvariable=result_var, font=("Arial", 12), fg="blue")
result_label.pack(pady=10)

# הפעלת GUI
root.mainloop()