import sys
import imapclient
import email
from email.header import decode_header
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QWidget, QListWidget, QTextEdit,
    QSplitter, QLineEdit, QPushButton, QLabel, QDialog, QFormLayout
)
from PyQt6.QtCore import Qt


class LoginDialog(QDialog):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Login - ZoraMail")
        self.setGeometry(300, 300, 400, 200)

        layout = QFormLayout()

        self.email_input = QLineEdit()
        self.email_input.setPlaceholderText("Enter your email")
        layout.addRow("Email:", self.email_input)

        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.EchoMode.Password)
        self.password_input.setPlaceholderText("Enter your password")
        layout.addRow("Password:", self.password_input)

        self.server_input = QLineEdit()
        self.server_input.setPlaceholderText("Enter IMAP server (e.g., imap.example.com)")
        layout.addRow("IMAP Server:", self.server_input)

        self.login_button = QPushButton("Login")
        self.login_button.clicked.connect(self.accept)
        layout.addWidget(self.login_button)

        self.setLayout(layout)

    def get_credentials(self):
        return self.email_input.text(), self.password_input.text(), self.server_input.text()


class ZoraMail(QMainWindow):
    def __init__(self, email, password, server):
        super().__init__()
        self.setWindowTitle("ZoraMail - GNUmail Style")
        self.setGeometry(100, 100, 1024, 768)

        self.email = email
        self.password = password
        self.server = server

        # Split view (left: email list, right: email content)
        splitter = QSplitter(Qt.Orientation.Horizontal)
        self.email_list = QListWidget()
        self.email_list.itemClicked.connect(self.load_email)

        self.email_view = QTextEdit()
        self.email_view.setReadOnly(True)

        splitter.addWidget(self.email_list)
        splitter.addWidget(self.email_view)
        splitter.setSizes([300, 700])

        container = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(splitter)
        container.setLayout(layout)

        self.setCentralWidget(container)

        # Fetch emails
        self.load_emails()

    def load_emails(self):
        """Fetch emails from an IMAP server and populate the list."""
        try:
            server = imapclient.IMAPClient(self.server, ssl=True)
            server.login(self.email, self.password)
            server.select_folder("INBOX")

            messages = server.search("ALL")
            for msg_id in messages[:10]:  # Load latest 10 emails
                msg_data = server.fetch(msg_id, ["RFC822"])[msg_id][b"RFC822"]
                msg = email.message_from_bytes(msg_data)

                subject, encoding = decode_header(msg["Subject"])[0]
                if isinstance(subject, bytes):
                    subject = subject.decode(encoding or "utf-8")

                self.email_list.addItem(subject)

            server.logout()

        except Exception as e:
            self.email_list.addItem(f"Error: {str(e)}")

    def load_email(self, item):
        """Display selected email in the text view."""
        selected_index = self.email_list.row(item)

        # Simulated email content
        email_content = f"""
        Subject: {item.text()}

        This is a sample email body for demonstration purposes.
        """
        self.email_view.setPlainText(email_content)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    # Force Windows Classic theme
    app.setStyle("windows")

    # Apply custom styling
    app.setStyleSheet("""
        QWidget {
            font: 9pt "MS Shell Dlg 2";
            background-color: rgb(212, 208, 200);
            color: black;
        }
        
        QMainWindow {
            border: 2px solid gray;
        }

        QListWidget {
            background-color: white;
            border: 1px solid gray;
            color: black;
        }

        QTextEdit {
            background-color: white;
            border: 1px solid gray;
            color: black;
        }

        QPushButton {
            background-color: rgb(236, 233, 216);
            border: 1px solid gray;
            padding: 2px;
            color: black;
        }

        QPushButton:hover {
            background-color: rgb(255, 251, 240);
        }

        QLineEdit {
            background-color: white;
            border: 1px inset gray;
            color: black;
        }
        
        QDialog {
            border: 1px solid gray;
            color: black;
        }
    """)

    # Show login dialog
    login_dialog = LoginDialog()
    if login_dialog.exec() == QDialog.DialogCode.Accepted:
        email, password, server = login_dialog.get_credentials()

        # Show main window
        window = ZoraMail(email, password, server)
        window.show()
        sys.exit(app.exec())
