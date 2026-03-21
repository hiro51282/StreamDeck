from flask import Flask, request
import os
import subprocess
import psutil
import time
from config import TOKEN

app = Flask(__name__)

# 共通認証
def check_token():
    return request.args.get("token") == TOKEN


@app.route("/shutdown")
def shutdown():
    if not check_token():
        return "NG", 403

    print("Shutdown called")
    os.system("sudo /usr/sbin/shutdown now")
    return "OK"


@app.route("/status")
def status():
    if not check_token():
        return "NG", 403

    cpu = psutil.cpu_percent(interval=0.3)
    return f"CPU {cpu:.1f}%"


@app.route("/temp")
def temp():
    if not check_token():
        return "NG", 403

    try:
        with open("/sys/class/thermal/thermal_zone0/temp") as f:
            temp = int(f.read()) / 1000
        return f"TEMP {temp:.1f}C"
    except:
        return "TEMP ERR"


@app.route("/memory")
def memory():
    if not check_token():
        return "NG", 403

    mem = psutil.virtual_memory()
    return f"MEM {mem.percent:.1f}%"


@app.route("/ping")
def ping():
    if not check_token():
        return "NG", 403

    return "ONLINE"


@app.route("/uptime")
def uptime():
    if not check_token():
        return "NG", 403

    uptime_sec = int(time.time() - psutil.boot_time())
    return f"UP {uptime_sec}s"


app.run(host="0.0.0.0", port=5000)