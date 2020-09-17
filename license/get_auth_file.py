
import requests
import json
import argparse


def parser_auth_key(auth_key):
    try:
        st = json.loads(auth_key)
    except:
        return "", "", False
    else:
        if st.has_key("serial") and st.has_key("key"):
            return st["serial"], st["key"], True
        else:
            return "", "", False
    

def get_auth_file(fingerprint, serial, key, sdk_type_name, active_code, output='authFile.v2c'):
    url = 'https://srp.megvii.com/sdk/api/v5/authdev'
    data = {
            "AuthType": 1,
            "Product_id": "1",
            "Feature_id": "1",
            "Expire_time": "0",
            "Concurrent_proc_num": 0,
            "Custom_memory": "",
            "serial": serial,
            "key": key,
            "active_code": active_code,
            "AuthFiles": [{"FileNo": 1, "File": fingerprint}],
            "sdk_type_name": sdk_type_name
    }
    data_json = json.dumps(data)
    files = {
        "params": (None, data_json)
    }

    resp = requests.post(url=url, files=files)
    print resp.text
    try:
        resp_json = json.loads(resp.text)
    except:
        print "false to get auth file"
        print "detail: ", resp.text
        return
    with open(output, 'w') as f:
        try:
            authFile = resp_json["files"][0]["AuthFile"]
            f.write(resp_json["files"][0]["AuthFile"])
        except:
            print "false to get auth file"
            print "url ret:", resp.text
        else:
            print "get auth file ok"


if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--key", required=True, help="key")
    parser.add_argument("--fp", required=True, help="fingerprint")
    parser.add_argument("--active", required=False, help="active code")
    parser.add_argument("--sdk_type_name", required=False, help="active code")
    args = parser.parse_args()
    key = args.key
    fp = args.fp
    activefile = args.active
    sdk_type_name = args.sdk_type_name

    active_code = ""
    if activefile == None :
        active_code = ""
    else:
        with open(activefile, 'r') as f:
            for line in f:
                active_code = active_code + line[:-1]

    auth_key = ""
    with open(key, 'r') as f:
        for line in f:
            auth_key = auth_key + line[:-1]
        auth_key = auth_key.replace("\"\"", "\"")
    auth_key = auth_key[1:len(auth_key)-1]

    finger = ""
    with open(fp, 'r') as f:
        for line in f:
            finger = finger + line[:-1]

    serial, key, ok = parser_auth_key(auth_key)
    if ok != True:
        print "--key input invalid"
    else:
        get_auth_file(finger, serial, key, sdk_type_name, active_code)

 
