const stringifyNice = (data) => JSON.stringify(data, null, 2)
const templates = Object.freeze({
    elements: {
        fileName: "elements.json",
        value: stringifyNice([
            {
                "name": "password",
                "type": "ACInput",
                "value": "ToTheMoon1",
                "label": "Password for PoS AP WiFi",
                "pattern": "",
                "placeholder": "",
                "style": "",
                "apply": "text"
            },
            {
                "name": "ssid",
                "type": "ACInput",
                "value": "",
                "label": "WiFi SSID",
                "pattern": "",
                "placeholder": "",
                "style": "",
                "apply": "text"
            },
            {
                "name": "wifipassword",
                "type": "ACInput",
                "value": "",
                "label": "WiFi Password",
                "pattern": "",
                "placeholder": "",
                "style": "",
                "apply": "text"
            },
            {
                "name": "socket",
                "type": "ACInput",
                "value": "",
                "label": "Copy link from LNURLDevices extension in LNbits",
                "pattern": "",
                "placeholder": "",
                "style": "",
                "apply": "text"
            },
            {
                "name": "lnurl",
                "type": "ACInput",
                "value": "true",
                "label": "LNURL",
                "pattern": "",
                "placeholder": "",
                "style": "",
                "apply": "text"
            }
        ])
    }
})

