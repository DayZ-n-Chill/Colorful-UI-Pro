import tkinter as tk
from tkinter import ttk

# Initialize the main window
root = tk.Tk()
root.title("UI Elements Showcase")
root.geometry("400x600")

# Label
label = tk.Label(root, text="This is a Label", font=("Arial", 12))
label.pack(pady=10)

# Entry (text input)
entry = tk.Entry(root)
entry.insert(0, "This is an Entry")
entry.pack(pady=10)

# Button
button = tk.Button(root, text="This is a Button")
button.pack(pady=10)

# Checkbutton (checkbox)
check_var = tk.BooleanVar()
checkbutton = tk.Checkbutton(root, text="This is a Checkbutton", variable=check_var)
checkbutton.pack(pady=10)

# Radiobuttons (radio buttons)
radio_var = tk.StringVar(value="Option1")
radiobutton1 = tk.Radiobutton(root, text="Option 1", variable=radio_var, value="Option1")
radiobutton2 = tk.Radiobutton(root, text="Option 2", variable=radio_var, value="Option2")
radiobutton1.pack(pady=2)
radiobutton2.pack(pady=2)

# Combobox (drop-down menu)
combo = ttk.Combobox(root, values=["Option 1", "Option 2", "Option 3"])
combo.set("Select an option")
combo.pack(pady=10)

# Progress bar
progress = ttk.Progressbar(root, length=200, mode="determinate")
progress["value"] = 50  # Set to 50% for demo
progress.pack(pady=10)

# Scale (slider)
scale = tk.Scale(root, from_=0, to=100, orient="horizontal")
scale.set(50)  # Set to 50 for demo
scale.pack(pady=10)

# Listbox
listbox = tk.Listbox(root)
listbox.insert(1, "Item 1")
listbox.insert(2, "Item 2")
listbox.insert(3, "Item 3")
listbox.pack(pady=10)

# Spinbox
spinbox = tk.Spinbox(root, from_=0, to=10)
spinbox.pack(pady=10)

# Separator
separator = ttk.Separator(root, orient="horizontal")
separator.pack(fill="x", pady=20)

# Notebook (tabbed interface)
notebook = ttk.Notebook(root)
frame1 = ttk.Frame(notebook)
frame2 = ttk.Frame(notebook)
notebook.add(frame1, text="Tab 1")
notebook.add(frame2, text="Tab 2")
notebook.pack(pady=10)

# Status label at the bottom
status = tk.Label(root, text="This is a Status Label", relief="sunken", anchor="w")
status.pack(fill="x", side="bottom", pady=5)

# Run the main application loop
root.mainloop()
