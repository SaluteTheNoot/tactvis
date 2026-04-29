"""
TactVis Sensor Simulator
Broadcasts fake entity packets over UDP to localhost:5005 at 10 Hz.
Each packet is a minimal JSON object matching the SensorReceiver parser.

Usage: python3 sim/sensor_sim.py
"""
import socket
import json
import time
import math
import random

HOST = "127.0.0.1"
PORT = 5005
HZ   = 10

ENTITIES = [
    {"id": 1,  "callsign": "EAGLE-1",  "iff": "friendly", "type": "aircraft",
     "x": -500.0, "y": -300.0, "alt": 800.0, "heading": 45.0,  "speed": 90.0},
    {"id": 2,  "callsign": "EAGLE-2",  "iff": "friendly", "type": "aircraft",
     "x":  200.0, "y":  150.0, "alt": 600.0, "heading": 120.0, "speed": 75.0},
    {"id": 3,  "callsign": "VIPER-1",  "iff": "hostile",  "type": "aircraft",
     "x":  800.0, "y":  500.0, "alt": 700.0, "heading": 220.0, "speed": 110.0},
    {"id": 4,  "callsign": "VIPER-2",  "iff": "hostile",  "type": "aircraft",
     "x": -100.0, "y":  700.0, "alt": 500.0, "heading": 270.0, "speed": 95.0},
    {"id": 5,  "callsign": "GHOST-1",  "iff": "unknown",  "type": "aircraft",
     "x":  300.0, "y": -600.0, "alt": 400.0, "heading": 350.0, "speed": 60.0},
    {"id": 10, "callsign": "BRAVO-1",  "iff": "friendly", "type": "vehicle",
     "x": -200.0, "y":  100.0, "alt":   5.0, "heading":  90.0, "speed": 15.0},
    {"id": 11, "callsign": "BRAVO-2",  "iff": "friendly", "type": "vehicle",
     "x":  100.0, "y": -200.0, "alt":   5.0, "heading": 180.0, "speed": 12.0},
    {"id": 12, "callsign": "TANGO-1",  "iff": "hostile",  "type": "vehicle",
     "x":  600.0, "y":  300.0, "alt":   5.0, "heading": 240.0, "speed": 20.0},
]

def step(entity, dt):
    rad = math.radians(entity["heading"])
    entity["x"]   += math.sin(rad) * entity["speed"] * dt
    entity["y"]   -= math.cos(rad) * entity["speed"] * dt
    # Gentle heading drift so entities orbit the map
    entity["heading"] = (entity["heading"] + random.uniform(-1, 1)) % 360
    # Bounce off a loose boundary
    bound = 1500.0
    if abs(entity["x"]) > bound or abs(entity["y"]) > bound:
        entity["heading"] = (entity["heading"] + 150 + random.uniform(-20, 20)) % 360

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print(f"Simulator broadcasting {len(ENTITIES)} entities to {HOST}:{PORT} at {HZ} Hz")
print("Ctrl+C to stop")

interval = 1.0 / HZ
while True:
    t0 = time.monotonic()
    for e in ENTITIES:
        step(e, interval)
        pkt = json.dumps({k: e[k] for k in e}).encode()
        sock.sendto(pkt, (HOST, PORT))
    elapsed = time.monotonic() - t0
    time.sleep(max(0, interval - elapsed))
