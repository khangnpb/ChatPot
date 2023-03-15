import json
import openai
import os

openai.api_key = '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'

messages = [
    # system message first, it helps set the behavior of the assistant
    {"role": "system", "content": "You are a research assistant."},
]

while True:
    message = input("You 👨‍⚕️: ")
    if message:
        if message == "q": break
        messages.append(
            {"role": "user", "content": message},
        )
        chatCompletion = openai.ChatCompletion.create(
            model="gpt-3.5-turbo", messages=messages
        )
    
    reply = chatCompletion.choices[0].message.content
    print(f"Chatbot 🧙: {reply}")
    messages.append({"role": "assistant", "content": reply})