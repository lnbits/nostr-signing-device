const serialConfig = {
    selectedPort: null,
    writableStreamClosed: null,
    readableStreamClosed: null,
    writer: null,
    reader: null,
    serialDataValue: null,
    data: null
}

const sleep = ms => new Promise(r => setTimeout(r, ms))

async function openSerialPort(config = { baudRate: 115200 }) {
    if (!checkSerialPortSupported()) return false
    if (serialConfig.selectedPort) {
        showMessage('Already connected. Disconnect first!')
        return true
    }

    try {
        serialConfig.selectedPort = await navigator.serial.requestPort()
        serialConfig.selectedPort.addEventListener('connect', event => {
            console.log('### Connected to Serial Port!')
        })

        serialConfig.selectedPort.addEventListener('disconnect', () => {
            serialConfig.selectedPort = null
            showMessage('Disconnected from Serial Port!')
        })

        // Wait for the serial port to open.
        await serialConfig.selectedPort.open(config)
        // do not await
        startSerialPortReading2()
        // wait to init
        await sleep(1000)

        const textEncoder = new TextEncoderStream()
        serialConfig.writableStreamClosed = textEncoder.readable.pipeTo(
            serialConfig.selectedPort.writable
        )

        serialConfig.writer = textEncoder.writable.getWriter()
        showMessage('Connected to serial port!')
        return true
    } catch (error) {
        console.error(error)
        serialConfig.selectedPort = null
        showMessage('Cannot open serial port!')
        return false
    }
}

function checkSerialPortSupported() {
    if (!navigator.serial) {
        showMessage('Serial port communication not supported!')
        return false
    }
    return true
}

async function startSerialPortReading2() {
    const port = serialConfig.selectedPort

    while (port && port.readable) {
        const textDecoder = new TextDecoderStream()
        serialConfig.selectedPort.readableStreamClosed = port.readable.pipeTo(textDecoder.writable)
        serialConfig.selectedPort.reader = textDecoder.readable.getReader()
        const readStringUntil = readFromSerialPort(serialConfig.selectedPort.reader)

        try {
            while (true) {
                const { value, done } = await readStringUntil('\n')
                if (value) {
                    const [command, ...values] = value.split(" ")
                    handleSerialPortResponse(command, values.join(' '))
                }


                if (done) return
            }
        } catch (error) {
            console.error(error)
            console.log("### Serial port communication error!")
        }
    }
}

async function sendSerialData(data) {
    if (!serialConfig.writer) {
        showMessage("Not connected to device")
        throw new Error("Not connected to device")
    }
    await serialConfig.writer.write(data + '\n')
}

const readFromSerialPort = reader => {
    let partialChunk
    let fulliness = []

    const readStringUntil = async (separator = '\n') => {
        if (fulliness.length) {
            return { value: fulliness.shift().trim(), done: false }
        }
        const chunks = []
        if (partialChunk) {
            // leftovers from previous read
            chunks.push(partialChunk)
            partialChunk = undefined
        }
        while (true) {
            const { value, done } = await reader.read()
            if (value) {
                const values = value.split(separator)
                // found one or more separators
                if (values.length > 1) {
                    chunks.push(values.shift()) // first element
                    partialChunk = values.pop() // last element
                    fulliness = values // full lines
                    return { value: chunks.join('').trim(), done: false }
                }
                chunks.push(value)
            }
            if (done) return { value: chunks.join('').trim(), done: true }
        }
    }
    return readStringUntil
}

function handleSerialPortResponse(command, data = '') {
    if (command === '/file-read') {
        serialConfig.data += data + '\n'
    }
    if (command === '/file-done') {
        updateConfigUI(serialConfig.data)
        serialConfig.data = ''
    }
}