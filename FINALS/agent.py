import requests
import serial
import time
import re

print("🔥 AI STUDY COACH STARTED")

# ================= ARDUINO =================
try:
    arduino = serial.Serial('COM4', 9600, timeout=1)
    time.sleep(2)
    print("✅ Arduino connected")
except:
    arduino = None
    print("⚠️ Arduino not connected")

# ================= LM STUDIO =================
URL = "http://localhost:1234/v1/chat/completions"

# ================= TIMER RULES =================
def get_timer(rating):
    if rating == "1":
        return 20, 10, "LOW"
    elif rating == "2":
        return 25, 5, "MID"
    elif rating == "3":
        return 40, 5, "DEEP"
    return 25, 5, "MID"

# ================= AI =================
def send_to_ai(state):

    prompt = f"""
You are a Pomodoro assistant.

User rating: {state}

Return ONLY ONE number:
1 = low energy
2 = medium energy
3 = high focus
"""

    try:
        response = requests.post(
            URL,
            json={
                "model": "tinyllama-1.1b-chat-v1.0",
                "messages": [{"role": "user", "content": prompt}],
                "temperature": 0.2
            },
            timeout=30
        )

        result = response.json()
        output = result["choices"][0]["message"]["content"].strip()

        match = re.search(r"[1-3]", output)
        return match.group() if match else "2"

    except:
        return "2"


# ================= MAIN LOOP =================
while True:

    print("\n==============================")
    print("🧠 FOCUS CHECK")
    print("1 = low energy 😴")
    print("2 = mid 🙂")
    print("3 = deep focus 😌")
    print("==============================")

    user_input = input("Enter rating: ").strip()

    if user_input not in ["1", "2", "3"]:
        print("Invalid input")
        continue

    ai_result = send_to_ai(user_input)

    study, brk, mode = get_timer(user_input)

    print(f"\n🤖 AI: {ai_result}")
    print(f"⏱ MODE: {mode}")
    print(f"STUDY: {study}s | BREAK: {brk}s")

    if arduino:
        try:
            arduino.write(f"{ai_result}\n".encode())
            time.sleep(0.1)
            arduino.write(f"T{study},{brk},{mode}\n".encode())
            print("🔌 Sent to Arduino")
        except:
            print("⚠️ Arduino error")
