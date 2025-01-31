import tkinter as tk
from tkinter import font, simpledialog
import threading
import main
import json
import time
import datetime


def format_timestamp(timestamp):
    message_time = datetime.datetime.fromtimestamp(timestamp)
    now = datetime.datetime.now()

    if message_time.date() == now.date():
        return message_time.strftime("%H:%M")
    elif message_time.date() == (now - datetime.timedelta(days=1)).date():
        return message_time.strftime("Wczoraj, %H:%M")
    else:
        return message_time.strftime("%A, %H:%M")

class ChatApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Yapu Yapu")
        self.root.geometry("500x500")
        self.root.config(bg="#f2f2f2")

        self.custom_font = font.Font(family="Helvetica", size=12)
        self.w_conn = None
        self.r_conn = None
        self.user_pid = None
        self.recipient_pid = None
        self.available_pids = ['1560221111', '1560221112', '1560221113']
        self.unread_messages = {}
        self.running = True
        self.chat_history = {}
        self.create_login_screen()


        self.root.protocol("WM_DELETE_WINDOW", self.close_connection)

    def create_login_screen(self):
        self.login_frame = tk.Frame(self.root, bg="#f2f2f2")
        self.login_frame.pack(expand=True)

        tk.Label(self.login_frame, text="Wpisz PID do logowania", font=("Helvetica", 14, "bold"), bg="#f2f2f2",
                 fg="#333").pack(pady=(50, 10))

        self.entry_pid = tk.Entry(self.login_frame, font=self.custom_font, width=30, bd=2, relief="solid", bg="#fff",
                                  fg="#333")
        self.entry_pid.pack(pady=10, padx=20)

        tk.Label(self.login_frame, text="Wpisz adres IP serwera", font=("Helvetica", 12), bg="#f2f2f2", fg="#333").pack(
            pady=(10, 5))

        self.entry_ip = tk.Entry(self.login_frame, font=self.custom_font, width=30, bd=2, relief="solid", bg="#fff",
                                 fg="#333")
        self.entry_ip.pack(pady=5, padx=20)

        tk.Button(self.login_frame, text="Zaloguj", font=self.custom_font, command=self.login, bg="#4CAF50", fg="white",
                  bd=0, relief="solid", padx=20, pady=10).pack(pady=10)

    def login(self):
        pid = self.entry_pid.get()
        ip = self.entry_ip.get()

        if pid and ip:
            self.user_pid = pid
            self.login_frame.pack_forget()
            self.create_main_screen(ip)

    def create_main_screen(self, ip):
        self.w_conn, self.r_conn = main.create_connection(ip, 1234, self.user_pid)
        self.main_frame = tk.Frame(self.root, bg="#f2f2f2")
        self.main_frame.pack(fill=tk.BOTH, expand=True)

        self.navbar = tk.Frame(self.main_frame, width=120, bg="#d9d9d9")
        self.navbar.pack(side=tk.LEFT, fill=tk.Y)

        tk.Label(self.navbar, text="Kontakty", font=("Helvetica", 12, "bold"), bg="#d9d9d9").pack(pady=10)

        self.contacts_frame = tk.Frame(self.navbar, bg="#d9d9d9")
        self.contacts_frame.pack()

        for pid in self.available_pids:
            self.add_contact_button(pid)

        tk.Button(self.navbar, text="Dodaj", font=self.custom_font, command=self.add_contact, bg="#4CAF50", fg="white",
                  bd=0, relief="solid", padx=10, pady=5).pack(pady=5)

        self.chat_frame = tk.Frame(self.main_frame, bg="#f2f2f2")
        self.chat_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

        self.program_title = tk.Label(self.chat_frame, text="⭐ Yapu Yapu 呀普呀普 ⭐", font=("Helvetica", 16, "bold"),
                                      bg="#f2f2f2", fg="#333")
        self.program_title.pack(pady=(10, 5))

        self.title_label = tk.Label(self.chat_frame, text="Wybierz odbiorcę", font=("Helvetica", 14, "bold"),
                                    bg="#f2f2f2", fg="#333")
        self.title_label.pack(pady=(10, 10))

        self.text_area = tk.Text(self.chat_frame, height=15, width=40, font=self.custom_font, bg="#e6e6e6", fg="#333",
                                 state=tk.DISABLED, wrap=tk.WORD)
        self.text_area.pack(pady=10, padx=10)

        self.entry = tk.Entry(self.chat_frame, font=self.custom_font, width=30, bd=2, relief="solid", bg="#fff",
                              fg="#333")
        self.entry.pack(pady=10, padx=20)

        # self.send_button = tk.Button(self.chat_frame, text=":-)", font=self.custom_font, command=self.send_message,
        #                              bg="#4CAF50", fg="white", bd=0, relief="solid", padx=20, pady=10)
        self.send_button = tk.Button(self.chat_frame, text="Wyślij", font=self.custom_font, command=self.send_message,
                                     bg="#4CAF50", fg="white", bd=0, relief="solid", padx=20, pady=10)
        self.send_button.pack(pady=10)

        threading.Thread(target=self.receive_messages, daemon=True).start()

    def add_contact_button(self, pid):
        btn_text = pid
        if pid in self.unread_messages and self.unread_messages[pid]:
            btn_text += " 🔴"
        btn = tk.Button(self.contacts_frame, text=btn_text, font=self.custom_font, width=12, bg="#bfbfbf",
                        command=lambda p=pid: self.select_recipient(p))
        btn.pack(pady=5, padx=5)

    def add_contact(self):
        new_pid = simpledialog.askstring("Dodaj kontakt", "Wpisz PID nowego kontaktu:")
        if new_pid and new_pid not in self.available_pids:
            self.available_pids.append(new_pid)
            self.add_contact_button(new_pid)

    def select_recipient(self, pid):
        self.recipient_pid = pid
        self.title_label.config(text=f"Czat z {pid}")
        self.text_area.config(state=tk.NORMAL)
        self.text_area.delete(1.0, tk.END)

        if pid in self.chat_history:
            for msg in self.chat_history[pid]:
                self.text_area.insert(tk.END, f"{msg}\n")

        self.text_area.config(state=tk.DISABLED)

        if pid in self.unread_messages:
            for msg in self.unread_messages[pid]:
                self.text_area.config(state=tk.NORMAL)
                # niepotrzebne juz bo historia dziala
                # self.text_area.insert(tk.END, f"{msg}\n")
            self.text_area.config(state=tk.DISABLED)
            self.unread_messages[pid] = []

        for btn in self.contacts_frame.winfo_children():
            if btn.cget("text").startswith(pid):
                btn.config(text=pid)

    def send_message(self):
        message = self.entry.get()
        if message and self.recipient_pid:
            timestamp = time.time()
            formatted_time = format_timestamp(timestamp)
            self.text_area.config(state=tk.NORMAL)
            self.text_area.insert(tk.END, f"Ty ({formatted_time}): {message}\n")
            self.text_area.config(state=tk.DISABLED)
            self.entry.delete(0, tk.END)

            if self.recipient_pid not in self.chat_history:
                self.chat_history[self.recipient_pid] = []
            self.chat_history[self.recipient_pid].append(f"Ty ({formatted_time}): {message}")


            if self.w_conn is not None:
                try:
                    threading.Thread(target=self._send_message, args=(message, self.user_pid), daemon=True).start()
                except Exception as e:
                    print(f"błąd: {e}")
            else:
                print("Coś z połączeniem nie styka")

    def _send_message(self, message, user_pid):
        try:
            main.send_message(self.w_conn, self.recipient_pid, message, user_pid)
        except Exception as e:
            print(f"Error in _send_message: {e}")

    def receive_messages(self):
        while self.running:
            try:
                message = self.r_conn.recv(1000)

                if message:
                    message_str = message.decode("utf-8")
                    message_dict = json.loads(message_str)
                    content = message_dict["content"]
                    sender = message_dict["from"]
                    timestamp = message_dict.get("timestamp", time.time())
                    formatted_time = format_timestamp(timestamp)

                    if sender == self.recipient_pid:
                        self.text_area.config(state=tk.NORMAL)
                        self.text_area.insert(tk.END, f"{sender} ({formatted_time}): {content}\n")
                        self.text_area.config(state=tk.DISABLED)

                        if sender not in self.chat_history:
                            self.chat_history[sender] = []
                        self.chat_history[sender].append(f"{sender} ({formatted_time}): {content}")

                    else:
                        if sender not in self.unread_messages:
                            self.unread_messages[sender] = []
                        # self.unread_messages[sender].append(content)
                        self.unread_messages[sender].append(f"{sender} ({formatted_time}): {content}")
                        self.chat_history[sender].append(f"{sender} ({formatted_time}): {content}")
                        for btn in self.contacts_frame.winfo_children():
                            if btn.cget("text").startswith(sender):
                                btn.config(text=f"{sender} 🔴")

            except Exception as e:
                if self.running:
                    print(f"Error receiving message: {e}")
                break

    def close_connection(self):
        try:
            self.running = False
            if self.w_conn:
                self.w_conn.close()
            if self.r_conn:
                self.r_conn.close()
            print("Połączenie zostało zamknięte.")
        except Exception as e:
            print(f"Błąd podczas zamykania połączenia: {e}")
        self.root.quit()


root = tk.Tk()
app = ChatApp(root)
root.mainloop()
