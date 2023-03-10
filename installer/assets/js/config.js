

async function saveToDevice(btn) {

    if (!serialConfig.writer) {
        showMessage("Not connected to device")
        return
    }
    const privateKey = document.getElementById("private-key").value
    if (!privateKey) {
        showMessage('No private key specified!')
        return
    }
    var re = /[0-9A-Fa-f]{6}/g;

    if (privateKey.length != 64 || !re.test(privateKey)) {
        showMessage('The private key is not a valid 32 bytes hex string!')
        return
    }

    const progressBar = document.getElementById("progress-bar-top")
    const progressBarValue = document.getElementById("progress-bar-value")

    progressBarValue.style.width = '0%'
    progressBar.classList.remove('d-none')
    btn.classList.add('d-none')
    try {

        await sendSerialData(`/restore ${privateKey}`)
    } catch (error) {
        console.error(error)
        showMessage('Cannot update private key!')
    } finally {
        progressBar.classList.add('d-none')
        btn.classList.remove('d-none')
    }


}

async function connectToDevice(btn) {
    const baudRate = document.getElementById("baud-rate").value || 115200
    const connected = await openSerialPort({ baudRate })
    if (connected === true) {
        btn.value = 'Connected'
    }
}

function updateBinVersion(versionDropdown) {
    const espInstallButton = document.getElementById("espInstallButton")
    espInstallButton.manifest = `./firmware/esp32/${versionDropdown.value}/manifest.json`;
}

