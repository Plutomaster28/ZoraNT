import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QPushButton, QLineEdit
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QUrl

class ZoraBrowser(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ZoraNT Web Browser")
        self.setGeometry(100, 100, 1024, 768)

        # WebView
        self.browser = QWebEngineView()
        self.browser.setUrl(QUrl.fromUserInput("https://www.microsoft.com"))

        # Navigation Bar
        self.url_bar = QLineEdit()
        self.url_bar.setPlaceholderText("Enter URL or search query and press Enter...")
        self.url_bar.returnPressed.connect(self.navigate)

        self.back_button = QPushButton("←")
        self.back_button.clicked.connect(self.browser.back)

        self.forward_button = QPushButton("→")
        self.forward_button.clicked.connect(self.browser.forward)

        self.reload_button = QPushButton("⟳")
        self.reload_button.clicked.connect(self.browser.reload)

        # Navigation Layout (Horizontal)
        nav_layout = QHBoxLayout()
        nav_layout.addWidget(self.back_button)
        nav_layout.addWidget(self.forward_button)
        nav_layout.addWidget(self.reload_button)
        nav_layout.addWidget(self.url_bar)

        # Main Layout
        container = QWidget()
        main_layout = QVBoxLayout()
        main_layout.addLayout(nav_layout)
        main_layout.addWidget(self.browser)
        container.setLayout(main_layout)

        self.setCentralWidget(container)

        # Apply Windows 2000 Style
        self.apply_windows_2000_style()

    def navigate(self):
        url = self.url_bar.text()
        if "." in url and not url.startswith("http"):
            # If the input contains a dot, assume it's a URL and prepend "http://"
            url = "http://" + url
        elif "." not in url:
            # If the input doesn't contain a dot, treat it as a search query
            search_engine = "https://www.google.com/search?q="
            url = search_engine + QUrl.toPercentEncoding(url).data().decode()
        self.browser.setUrl(QUrl.fromUserInput(url))

    def apply_windows_2000_style(self):
        # Define a Windows 2000-inspired style using Qt Stylesheets
        self.setStyleSheet("""
            QMainWindow {
                background-color: #C0C0C0; /* Light gray background */
                color: #000000; /* Black text */
            }
            QLineEdit {
                background-color: #FFFFFF; /* White background */
                color: #000000; /* Black text */
                border: 1px solid #808080; /* Simple gray border */
                padding: 2px;
            }
            QPushButton {
                background-color: #E0E0E0; /* Light gray button */
                color: #000000; /* Black text */
                border: 1px solid #808080; /* Simple gray border */
                padding: 2px 5px;
            }
            QPushButton:hover {
                background-color: #D0D0D0; /* Slightly darker gray on hover */
            }
            QPushButton:pressed {
                background-color: #C0C0C0; /* Same as background when pressed */
            }
            QWebEngineView {
                border: 1px solid #808080; /* Simple gray border around the web view */
            }
        """)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ZoraBrowser()
    window.show()
    sys.exit(app.exec())
