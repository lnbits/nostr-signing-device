async function downloadFromDevice(btn) {
    const configFile = document.getElementById("config-file-path").value
    if (!serialConfig.writer) {
        showMessage("Not connected to device")
        return
    }
    if (!configFile) {
        showMessage('No file path specified!')
        return
    }
    try {
        btn.value = 'Downloading...'
        await sleep(500)
        serialConfig.data = ''
        await sendSerialData(`/file-read ${configFile}`)
    } catch (error) {
        console.error(error)
        showMessage('Cannot download file: ' + configFile)
    } finally {
        await sleep(500)
        btn.value = 'Download from Device'
    }

}

async function saveToDevice(btn) {
    const configFile = document.getElementById("config-file-path").value
    if (!serialConfig.writer) {
        showMessage("Not connected to device")
    }
    if (!configFile) {
        showMessage('No file path specified!')
        return
    }

    const progressBar = document.getElementById("progress-bar-top")
    const progressBarValue = document.getElementById("progress-bar-value")

    progressBarValue.style.width = '0%'
    progressBar.classList.remove('d-none')
    btn.classList.add('d-none')
    try {


        refreshTextConfig()
        await sendSerialData(`/file-remove ${configFile}`)
        const lines = configText.value.split('\n')

        let i = 0;
        for (const line of lines) {
            await sendSerialData(`/file-append ${configFile} ${line}`)
            await sleep(200)
            progressBarValue.style.width = Math.trunc((i * 100) / lines.length) + '%'
            i++
        }
    } catch (error) {
        console.error(error)
        showMessage('Cannot update file!')
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

