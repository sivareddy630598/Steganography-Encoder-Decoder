# 🧠 Steganography Encoder & Decoder (C Project)

This project implements **image steganography** using the **Least Significant Bit (LSB)** technique — allowing you to hide and extract secret data within BMP images securely.  
Developed in **C language**, this project demonstrates bit-level manipulation, file handling, and data encoding concepts.

---

## 🧩 Project Overview

The **Steganography Encoder & Decoder** works by modifying the least significant bits of the image pixels to store secret information.  
The process is completely reversible, allowing the hidden data to be retrieved accurately.

### 🔹 Encoding Process
1. Reads a source BMP image.
2. Embeds the secret file’s data (text or code) into the image’s pixel bits.
3. Generates a new stego image containing the hidden data.

### 🔹 Decoding Process
1. Reads the stego image.
2. Extracts the hidden bits from the pixel data.
3. Reconstructs and saves the original secret file.

---

## ⚙️ Features

✅ Encode any text or source file (e.g., `.txt`, `.c`, `.pdf`) into an image.  
✅ Decode and retrieve the exact hidden file from the image.  
✅ Uses **LSB technique** for efficient and undetectable data hiding.  
✅ Modular code design with separate encode and decode modules.  
✅ Simple command-line interface.

---

## 🧰 Technologies Used

- **Language:** C  
- **Compiler:** GCC / XC8  
- **Image Format:** BMP  

---

## 📁 Project Structure
## 📦 Steganography-Encoder-Decoder
## ┣ 📜 main.c
## ┣ 📜 encode.c
## ┣ 📜 decode.c
## ┣ 📜 encode.h
## ┣ 📜 decode.h
## ┣ 📜 types.h
## ┣ 📜 types1.h
## ┣ 📜 common.h
## ┣ 📜 Makefile
## ┣ 🖼️ sample.bmp
## ┣ 🖼️ stego.bmp
## ┗ 📜 README.md


---

## 🚀 How to Run

### 🖋️ **Encoding**

bash
  
    $ ./stego -e beautiful.bmp secret.txt stego.bmp


Explanation:

 
    -e → encode mode

    beautiful.bmp → source image

    secret.txt → file to hide

    stego.bmp → output stego image

🧩 Decoding

    $ ./stego -d stego.bmp output.txt


Explanation:

    -d → decode mode

    stego.bmp → image containing hidden data

    output.txt → extracted file

🧪 Example

Input Image:

      input.bmp

Secret File:

    message.txt (contains “Hello, this is hidden!”)

Output Image:

    stego.bmp

Decoded Output:

    decoded_message.txt → “Hello, this is hidden!”

🧑‍💻 Author

👤 Siva Prasad
💼 Embedded Systems & C Developer

