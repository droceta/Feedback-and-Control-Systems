import serial
import time

# Set the port name (e.g., 'COM4' on Windows or '/dev/ttyACM0' on Linux/Mac)
# Set the baud rate to 9600 to match the Arduino code
PORT = 'COM4' 
BAUD_RATE = 9600

try:
    # Initialize the serial connection
    arduino = serial.Serial(port=PORT, baudrate=BAUD_RATE, timeout=1)
    time.sleep(2)  # Wait for the Arduino to reset after connection
    print("Connection established!")
except Exception as e:
    print(f"Error: Could not connect to {PORT}. Check your connection.")
    exit()

while True:
    user_input = input("Enter servo angle (0-180) or 'q' to quit: ")

    # Allow user to exit the program
    if user_input.lower() == 'q':
        break

    # ERROR PROOFING: Check if the input is a valid number
    try:
        angle = int(user_input)
        
        # ERROR PROOFING: Check if the number is within range
        if 0 <= angle <= 180:
            # Send the angle to Arduino as a string followed by a newline
            arduino.write(bytes(str(angle), 'utf-8'))
            time.sleep(0.05)
            
            # Read the response from Arduino
            if arduino.in_waiting > 0:
                response = arduino.readline().decode('utf-8').strip()
                print(f"Arduino says: {response}")
        else:
            print("Error: Please enter a number between 0 and 180.")
            
    except ValueError:
        print("Error: Invalid input. Please enter a whole number.")

# Close the connection when the loop ends
arduino.close()
print("Program closed.")