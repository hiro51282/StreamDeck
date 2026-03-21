from flask import Flask, request
import os
import subprocess
import psutil
from config import TOKEN 

app = Flask(__name__)

@app.route("/shutdown")
def shutdown():
    if request.args.get("token") != TOKEN:
        return "NG", 403

    print("Shutdown called")
    os.system("sudo /usr/sbin/shutdown now")
    return "OK"

@app.route("/status")
def status():
    cpu = psutil.cpu_percent(interval=0.5)
    return f"{cpu:.1f}%"

@app.route("/temp")
def temp():
    try:
        with open("/sys/class/thermal/thermal_zone0/temp") as f:
            temp = int(f.read()) / 1000
        return f"{temp:.1f}C"
    except:
        return "TEMP ERR"

@app.route("/memory")
def memory():
    import psutil
    mem = psutil.virtual_memory()
    return f"{mem.percent:.1f}%"

@app.route("/ping")
def ping():
    return "ONLINE"

@app.route("/uptime")
def uptime():
    import time
    return f"{int(time.time() - psutil.boot_time())}s"

app.run(host="0.0.0.0", port=5000)

