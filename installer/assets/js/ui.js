
const configText = document.getElementById('config-file-text');
let configFormat = ''

function updateConfigUI(data) {
    configText.value = data
    updateConfigFormat(data)
    // todo add propeties check; add text config
    updateInputFields(data)
}

function updateConfigFormat(data) {
    try {
        JSON.parse(data)
        configFormat = 'json'
    } catch (error) {
        console.error(error)
        configFormat = 'properties'
    }
}

if (window.FileList && window.File) {
    document.getElementById('config-file-selector').addEventListener('change', event => {
        configText.innerHTML = '';
        for (const file of event.target.files) {
            const configFilePath = document.getElementById('config-file-path');
            configFilePath.value = configFilePath.value || file.name || ""


            let reader = new FileReader()
            reader.readAsText(file, "UTF-8");

            reader.onload = function (evt) {
                updateConfigUI(evt.target.result)
            }
        }
    });

}

function updateInputFields(data) {
    if (configFormat === 'json') {
        return updateInputFieldsFromJson(data)
    }
    if (configFormat === 'properties') {
        return updateInputFieldsFromProperties(data)
    }
}

function updateInputFieldsFromJson(dataJson) {
    const configFieldsGroup = document.getElementById('config-file-fields-group');
    configFieldsGroup.innerHTML = ""

    const data = JSON.parse(dataJson)
    data.forEach(cP => configFieldsGroup.innerHTML += createConfigField(cP))
}

function updateInputFieldsFromProperties(data) {
    const configFieldsGroup = document.getElementById('config-file-fields-group');
    configFieldsGroup.innerHTML = ""

    const configProperties = propsToField(data)
    configProperties.forEach(cP => configFieldsGroup.innerHTML += createConfigField(cP))
}

function updateConfigTextFromInputFields() {
    if (configFormat === 'properties') {
        return updateConfigPropertiesFromInputFields()
    }
    if (configFormat === 'json') {
        return updateConfigJsonFromInputFields()
    }
}

function updateConfigPropertiesFromInputFields() {
    const inputFields = document.getElementsByClassName("serial-config-input-field");
    const props = Array.from(inputFields).map(field => `${field.id}=${field.value || ''}`)
    configText.value = props.join('\n')
}

function updateConfigJsonFromInputFields() {
    const inputFields = document.getElementsByClassName("serial-config-input-field");

    const uiData = Array.from(inputFields).map(field => ({ name: field.id, value: field.value }))
    const data = JSON.parse(configText.value)
    data.map(item => {
        const updatedItem = uiData.find(uiEl => uiEl.name === item.name) || {}
        return Object.assign(item, updatedItem)
    })
    configText.value = JSON.stringify(data, null, 2)
}

function propsToField(text = '') {
    const lines = text.split('\n').filter(line => line.length)
    return lines.map(l => {
        const [name, value] = l.split('=')
        return { name, value, label: name, type: 'ACInput', apply: 'text', placeholder: 'No value' }
    })
}

function createConfigField(configField) {
    const elements = createConfigElement(configField)
    return `<div class="ud-form-group">${elements}</div>`
}

function createConfigElement(configField) {
    if (configField.type === 'ACInput') {
        return `<label for="${configField.name}">${configField.label}</label>
                        <input type="${configField.apply}" 
                            name="${configField.name}" 
                            id="${configField.name}" 
                            value="${configField.value || ''}" 
                            placeholder="${configField.placeholder || ''}" 
                            class="serial-config-input-field" />`
    }
    if (configField.type === 'ACText' && configField.value) {
        return `<h4 class="ud-feature-title">${configField.value}</h4>`
    }
    return ""
}

function onDisplayModeChamge() {
    const showText = document.getElementById("display-mode").checked;
    if (showText === false) {
        updateConfigFormat(configText.value)
        updateInputFields(configText.value)
        document.getElementById("config-file-fields-group").style.display = "block";
        document.getElementById("config-file-text-group").style.display = "none";
    } else {
        updateConfigTextFromInputFields()
        document.getElementById("config-file-fields-group").style.display = "none";
        document.getElementById("config-file-text-group").style.display = "block";
    }
}

function refreshTextConfig() {
    const showText = document.getElementById("display-mode").checked;
    if (showText === false) {
        updateConfigTextFromInputFields()
    }
}

function onToggleConfig(cb) {
    if (cb.checked) {
        showMessage("To enter config mode: reboot the device holding boot pin'")
    }
    const configDivs = document.getElementsByClassName("bitcoinswitch-config-div");
    Array.from(configDivs).forEach(div => {
        if (cb.checked) {
            div.classList.add('d-flex')
            div.classList.remove('d-none')
        } else {
            div.classList.add('d-none')
            div.classList.remove('d-flex')
        }

    })
}

function updateTemplate(templateDropwdown) {
    const templateName = templateDropwdown.value
    const template = templates[templateName]
    if (!template) {
        showMessage('No template found!')
        return
    }
    updateConfigUI(template.value + "") // make a copy
    const configFilePath = document.getElementById('config-file-path');
    configFilePath.value = template.fileName || ""
}


function showMessage(message) {
    document.getElementById("show-modal-button").click()
    document.getElementById("modal-text").innerText = message

}
