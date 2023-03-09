const stringifyNice = (data) => JSON.stringify(data, null, 2)
const templates = Object.freeze({
    elements: {
        fileName: "elements.json",
        value: stringifyNice([
            {
                "name": "privatekey",
                "type": "ACInput",
                "label": "Nostr privatekey",
                "pattern": "",
                "placeholder": "",
                "style": "",
                "apply": "text"
            }
        ])
    }
})

