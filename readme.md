# Final Project: Geevoi A. Plaza’s Seat Reservation System

░░██████╗░░█████╗░██████╗░░░░░░░░░░██████╗██████╗░░██████╗░
░██╔════╝░██╔══██╗██╔══██╗░░░░░░░░██╔════╝██╔══██╗██╔════╝░
░██║░░██╗░███████║██████╔╝░█████╗░╚█████╗░██████╔╝░╚████╗░░
░██║░░╚██╗██╔══██║██╔═══╝░░╚════╝░░╚═══██╗██╔══██╗░╚═══██╗░
░╚██████╔╝██║░░██║██║░░░░░░░░░░░░░██████╔╝██║░░██║██████╔╝░
░░╚═════╝░╚═╝░░╚═╝╚═╝░░░░░░░░░░░░░╚═════╝░╚═╝░░╚═╝╚═════╝░░

## 1. Brief Overview of the Project

Geevoi A. Plaza’s Seat Reservation System—or GAP-SRS—is designed to demonstrate concepts such as:

-   Branching (decision-making: if-else, switch-case)
-   Looping (handling repetitive tasks)
-   Functions (reusable, modular code)
-   Arrays (storing and organizing data)

The system integrates CRUD operations (Create, Read, Update, Delete) using a two-dimensional array as the primary data structure to manage seat reservations.

## 2. Features and Functionality

### CRUD Features

1. **Create | `createReservation()`**

    - Reserve a specific seat by specifying its row and column.
    - Prompts for reservation details (name and description) if the seat is available.

2. **Read | `readReservation()`**

    - View details (name and description) of a specific reserved seat.

3. **Update | `updateReservation()`**

    - Modify reservation details for an existing seat.

4. **Delete | `deleteReservation()`**
    - Cancel a reservation, marking the seat as available.

### Miscellaneous Features

1. **Main Menu | `mainMenu()`**

    - Central navigation system for accessing CRUD operations and configuration settings.

2. **Display Seat Layout | `showSeatLayout()`**

    - Visual representation of the current seating arrangement in a grid format.

3. **Edit Seat Layout Dimensions | `optionsSetDimensions()`**

    - Adjust the number of rows and columns in the layout.

4. **Edit HUD Length | `optionsSetHUDLength()`**
    - Modify the width of the Heads-Up Display (HUD) for better visual alignment.

## 3. Instructions for Using the Program

### 3.1 Start the Program

-   Launch the program to display the **Main Menu**, which lists all available options.
-   Choose an option by typing the corresponding number and pressing `Enter`.

### 3.2 Navigation Options

1. **Display Seat Layout**

    - View all seats. Reserved seats are marked as `X`, available seats as `O`.

2. **Create Seat Reservation**

    - Reserve a seat by specifying its row and column and providing a name and description.

3. **Read/Display Seat Reservation**

    - View details of a reserved seat by entering its row and column.

4. **Update Seat Reservation**

    - Modify details for an existing reservation.

5. **Delete/Cancel Seat Reservation**

    - Cancel a reservation, making the seat available again.

6. **Settings**
    - Access additional configuration options:
        - **Edit Seat Layout Dimensions**  
          Adjust the number of rows and columns in the layout.
        - **Edit HUD Length**  
          Change the HUD width for better display alignment.

### 3.3 Input Guidelines

-   **Integer Inputs**: Ensure numeric inputs (e.g., row, column) are within valid ranges.
-   **Text Inputs**: Use meaningful and clear names/descriptions.
-   **Error Handling**: Invalid inputs trigger error messages with prompts to retry.

### 3.4 Returning to the Menu

After completing any action, options are provided to:

-   Return to the **Main Menu**.
-   Repeat the current task.

## 4. Notes

-   Ensure the program is run in an environment that supports console-based interaction.
-   For best performance, adhere to the predefined limits for rows, columns, and HUD length.

---

**Enjoy using GAP-SRS!**
