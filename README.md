# Project 3 — Candy Shop Cashier
name : Samah Ahmed Mahmoud Ahmed
email : sammahmedzz50@gmail.com
## Description
A point-of-sale system for a candy shop that manages inventory, processes customer shopping baskets, merges duplicate item selections, and computes exact change breakdown.

## How to Build and Run
1. Open terminal in the project directory.
2. Compile with warnings enabled:
   ```bash
   gcc -Wall -Wextra main.c -o candy_shop
Run the executable:

On Windows: .\candy_shop.exe

On Linux/Mac: ./candy_shop

Explain Why
How does the program handle duplicate item additions in the shopping basket?
The search routine checks if the item already exists in the basket; if found, it updates the quantity on the existing line item instead of creating a redundant entry, optimizing memory and cleanliness.
