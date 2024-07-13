#!/usr/bin/python3

import subprocess
import pexpect

def connect_to_server():
    print("Connecting to server")
    p = subprocess.Popen(["getent", "hosts", "ctf_netconf_oran"], stdout=subprocess.PIPE)
    host = p.stdout.read().decode().split()[0]
    print(f"Host is: {host}")

    child = pexpect.spawn("netopeer2-cli")
    child.expect(">")
    print("netopeer2-cli spawned")
    child.send(f"connect -s -l ctf_netconf --host {host}\r")
    try:
        child.expect(".*continue connecting.*", timeout=1)
        print("continue connecting")
        child.send("yes\r")
    except pexpect.exceptions.TIMEOUT:
        pass
    child.expect(".*password:.*")
    print("Sending password")
    child.send("netconf\r")
    child.expect(">")
    print("Connected to server!")
    print(">")
    try:
        child.interact()
    except OSError:
        pass
    child.close()


if __name__ == "__main__":
    connect_to_server()