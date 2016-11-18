# API

## General

Status object sent from rasp -> client on a regular basis

```javascript
{"status": {
    "scan": true, /* True if we're scanning for devices */
    "devices": [ /* The connected devices addresses */
       "12:1F:13:2A:12", 
       "23:23:E8:C2:9A"
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

The server will respond with the following periodically:
```javascript
{"devices": [ /* devices found in range */ 
     {"addr": "12:1F:13:2A:12", "desc": "Johns Txw 51 Sensor" },
     {"addr": "2A:12:12:1F:13", "desc": "Texas Instrument 78C8 Temperature Sensor" }
  ]
}
```

## 2. Connect

To connect to a device:

```javascript
{"connect": "12:1F:13:2A:12"}
```

The server will respond with:

```javascript
{"connected": "12:1F:13:2A:12"}
```

or

```javascript
{"disconnected": "12:1F:13:2A:12", "err": "The device is no longer ...."}
```


To disconnect from a device:

```javascript
{"disconnect": "12:1F:13:2A:12"}
```

The server will respond with:

```javascript
{"disconnected": "12:1F:13:2A:12"}
```


## 3. Measure

*TODO*