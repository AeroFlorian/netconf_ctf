#!/usr/bin/python3
import os
import sys
import subprocess

challenge_number = int(os.getenv("CHALLENGE_NUMBER", "1"))

print(f"Running challenge {challenge_number}")

chall_number_to_name = {
    1: "1_intro_get",
    2: "2_get_filter",
    3: "3_edit_config",
    4: "4_rpc",
    5: "5_notif",
    6: "6_get_out",
    7: "7_update",
    8: "8_set_full_object",
    9: "9_leafref",
    10: "10_delete",
    11: "11_notif_schema",
    12: "12_nacm"
}

chall_name = chall_number_to_name[challenge_number]

os.system(f"/opt/dev/challenges/{chall_name}/install_modules_and_data.sh")

os.system(f"/usr/bin/supervisord -c /opt/dev/challenges/{chall_name}/supervisord.conf")
