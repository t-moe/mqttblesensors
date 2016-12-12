# API

## General

Status object sent from rasp -> client on a regular basis

```javascript
{"status": {
    "scan": true, /* True if we're scanning for devices */
    "connected": [ /* The connected devices addresses */
       "12:1F:13:2A:12",
       "23:23:E8:C2:9A"
    ],
    "devices" : [ /* The devices in range. empty if scan=false */
      {"addr": "12:1F:13:2A:12", "desc": "Johns Txw 51 Sensor" },
      {"addr": "2A:12:12:1F:13", "desc": "Texas Instrument 78C8 Temperature Sensor" }
    ]
  }
}
```

## 1. Scan

To start a scan:

```javascript
{"scan": true}
```

To stop a scan:

```javascript
{"scan": false}
```

## 2. Connect

To connect to a device:

```javascript
{"connect": "12:1F:13:2A:12"}
```


To disconnect from a device:

```javascript
{"disconnect": "12:1F:13:2A:12"}
```


## 3. Measure

The server will send data:

```javascript
{"data": {
    "device": "12:1F:13:2A:12",
    "type": "temperature"
    "raw": 32.0
  }
}
```

```javascript
{"data": {
    "device": "12:1F:13:2A:12",
    "type": "accelerate"
    "raw": {
       "x": -0.11,
       "y": -0.1,
       "z": 1.01
    }
  }
}
```

```javascript
{"data": {
    "device": "12:1F:13:2A:12",
    "type": "gyro"
    "raw": {
        "x": -55.4,
        "y": -49.17,
        "z": 156.26
    }
  }
}
```
