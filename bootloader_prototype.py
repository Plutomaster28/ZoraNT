import tkinter as tk
from tkinter import messagebox, simpledialog
import subprocess
import os

class BootloaderUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Zora NT Bootloader")
        self.root.geometry("800x600")
        self.root.configure(bg='black')

        # Bootloader title (header)
        self.header = tk.Label(root, text="Zora NT Boot Manager", font=("Courier", 16), fg="white", bg="gray")
        self.header.pack(fill=tk.X, pady=(10, 20))

        # Instruction text
        self.instructions = tk.Label(root, text="Choose an operating system to start, or press TAB to select a tool:\n(Use the arrow keys to highlight your choice, then press ENTER.)", font=("Courier", 12), fg="white", bg="black")
        self.instructions.pack()

        # Options list frame
        self.options_frame = tk.Frame(root, bg="black")
        self.options_frame.pack(pady=20)

        # Boot options
        self.options = ["Zora NT", "Previous Version of Zora NT", "Zora NT Recovery Environment"]
        self.selected_option = 0
        self.option_labels = []

        # Creating option labels
        for idx, option in enumerate(self.options):
            label = tk.Label(self.options_frame, text=option, font=("Courier", 14), fg="white", bg="black", anchor="w", width=50, padx=10, pady=5)
            label.pack(anchor="w")
            self.option_labels.append(label)

        self.update_selection()

        # Advanced options and tools
        self.advanced_instructions = tk.Label(root, text="To specify an advanced option for this choice, press F8.", font=("Courier", 12), fg="white", bg="black")
        self.advanced_instructions.pack(pady=(20, 10))

        self.tools_label = tk.Label(root, text="Tools:\n  Memory Diagnostic", font=("Courier", 12), fg="white", bg="black", anchor="w")
        self.tools_label.pack(fill=tk.X, padx=10)

        # Footer (key instructions)
        self.footer = tk.Label(root, text="ENTER=Choose    TAB=Menu    ESC=Cancel    F8=Advanced Options", font=("Courier", 12), fg="white", bg="gray")
        self.footer.pack(fill=tk.X, side=tk.BOTTOM, pady=(20, 10))

        # Bind keys
        root.bind("<Up>", self.up_key)
        root.bind("<Down>", self.down_key)
        root.bind("<Return>", self.select_option)
        root.bind("<F8>", self.show_advanced_options)

    def update_selection(self):
        # Update the visual appearance of selected option
        for idx, label in enumerate(self.option_labels):
            if idx == self.selected_option:
                label.configure(bg="gray", fg="black")
            else:
                label.configure(bg="black", fg="white")

    def up_key(self, event):
        # Navigate up
        if self.selected_option > 0:
            self.selected_option -= 1
        self.update_selection()

    def down_key(self, event):
        # Navigate down
        if self.selected_option < len(self.options) - 1:
            self.selected_option += 1
        self.update_selection()

    def select_option(self, event):
        # Confirm selection
        selected_os = self.options[self.selected_option]
        if selected_os == "Zora NT":
            self.boot_zora_nt()
        else:
            messagebox.showinfo("Bootloader", "Feature still being worked on!")

    def boot_zora_nt(self):
        # Function to boot Zora NT by running the startup file
        startup_file = "ZoraNT_Quick_run.py"
        if os.path.exists(startup_file):
            messagebox.showinfo("Bootloader", "Booting into Zora NT...")
            subprocess.run(["python", startup_file])
            self.root.quit()  # Quit the bootloader after starting Zora NT
        else:
            messagebox.showerror("Error", f"Startup file '{startup_file}' not found.")

    def show_advanced_options(self, event):
        # Show advanced options when F8 is pressed
        advanced_options = [
            "Safe Mode",
            "Command Prompt",
            "Last Known Good Configuration",
            "Boot Logging",
            "Enable Debugging",
        ]

        selected_option = simpledialog.askstring("Advanced Options", "Choose an advanced option:\n" + "\n".join(advanced_options))
        
        if selected_option in advanced_options:
            messagebox.showinfo("Bootloader", f"{selected_option} selected! (Feature still being worked on!)")
        else:
            messagebox.showinfo("Bootloader", "Invalid selection or canceled.")

# Main function to run the UI
def main():
    root = tk.Tk()
    app = BootloaderUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
